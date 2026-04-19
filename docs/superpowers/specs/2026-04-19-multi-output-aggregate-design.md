# Multi-Output Aggregate Window Functions

## Summary

Add `ta_*` aggregate (window-compatible) forms for the 8 multi-output functions that currently only exist as `t_*` scalar functions. Each returns a `STRUCT` per row instead of `LIST<STRUCT>`.

Existing `t_*` scalar functions remain unchanged.

## Motivation

Currently, multi-output functions like MACD require a verbose CTE + GROUP BY + UNNEST pattern:

```sql
WITH raw AS (
    SELECT symbol,
           list(date ORDER BY date) AS dates,
           list(close ORDER BY date) AS closes,
           t_macd(list(close ORDER BY date), 12, 26, 9) AS macd_arr
    FROM t_bar_1d
    WHERE symbol = 'MSFT'
    GROUP BY symbol
)
SELECT symbol,
       UNNEST(dates) AS date,
       UNNEST(closes) AS close,
       UNNEST(macd_arr).macd AS macd,
       UNNEST(macd_arr).signal AS signal,
       UNNEST(macd_arr).hist AS hist
FROM raw;
```

With aggregate forms, this becomes:

```sql
SELECT symbol, date, close,
       m.macd, m.signal, m.hist
FROM (
    SELECT *, ta_macd(close, 12, 26, 9) OVER (
        PARTITION BY symbol ORDER BY date
        ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM t_bar_1d
    WHERE symbol = 'MSFT'
);
```

## Functions

All 8 multi-output functions get aggregate forms:

| Function | Inputs | Return type |
|----------|--------|-------------|
| `ta_macd` | (DOUBLE, INT, INT, INT) | STRUCT(macd, signal, hist) |
| `ta_bbands` | (DOUBLE, INT, DOUBLE, DOUBLE, INT) | STRUCT(upper, middle, lower) |
| `ta_stoch` | (DOUBLE, DOUBLE, DOUBLE, INT, INT, INT, INT, INT) | STRUCT(slowk, slowd) |
| `ta_aroon` | (DOUBLE, DOUBLE, INT) | STRUCT(aroon_down, aroon_up) |
| `ta_minmax` | (DOUBLE, INT) | STRUCT(min, max) |
| `ta_mama` | (DOUBLE, DOUBLE, DOUBLE) | STRUCT(mama, fama) |
| `ta_ht_phasor` | (DOUBLE) | STRUCT(inphase, quadrature) |
| `ta_ht_sine` | (DOUBLE) | STRUCT(sine, leadsine) |

## Architecture

### File structure

- New file: `src/talib_aggregate_multi.cpp`
- New registration function: `RegisterTalibAggregateMultiOutputFunctions(ExtensionLoader &loader)`
- Called from `talib_extension.cpp` alongside existing registration functions

### State structs

The shared state structs (`TalibAggState1`, `TalibAggState8`) are currently defined in `talib_aggregate.cpp`. Extract them to `src/include/talib_aggregate_state.hpp` so both files can use them.

Reuse existing state structs where the input pattern matches:

| State struct | Reused by |
|-------------|-----------|
| `TalibAggState1` (values + period) | ta_minmax, ta_ht_phasor, ta_ht_sine |
| `TalibAggState8` (high + low + period) | ta_aroon |

New state structs needed for unique parameter combinations:

| New state struct | Used by | Fields |
|-----------------|---------|--------|
| `TalibAggStateMacd` | ta_macd | values, fast_period, slow_period, signal_period |
| `TalibAggStateBbands` | ta_bbands | values, time_period, nb_dev_up, nb_dev_dn, ma_type |
| `TalibAggStateStoch` | ta_stoch | high, low, close, fastk_period, slowk_period, slowk_matype, slowd_period, slowd_matype |
| `TalibAggStateMama` | ta_mama | values, fast_limit, slow_limit |

### Finalize pattern

All functions follow the same Finalize pattern, returning a STRUCT instead of a scalar:

```cpp
static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result,
                     idx_t count, idx_t offset) {
    auto states = FlatVector::GetData<StateType *>(states_vec);
    auto &entries = StructVector::GetEntries(result);
    auto d0 = FlatVector::GetData<double>(*entries[0]);
    auto d1 = FlatVector::GetData<double>(*entries[1]);
    // d2 for 3-output functions
    auto &v0 = FlatVector::Validity(*entries[0]);
    auto &v1 = FlatVector::Validity(*entries[1]);

    for (idx_t i = 0; i < count; i++) {
        auto state = states[i];
        idx_t ridx = i + offset;

        // ... null checks, call TA-Lib ...

        if (rc != TA_SUCCESS || outNb == 0) {
            v0.SetInvalid(ridx);
            v1.SetInvalid(ridx);
        } else {
            d0[ridx] = out1[outNb - 1];
            d1[ridx] = out2[outNb - 1];
        }
    }
}
```

Update/Combine/Destroy are identical to the corresponding single-output aggregate templates.

### Registration

Each function registered individually (no X-macro) since each has a unique TA-Lib C signature:

```cpp
AggregateFunction func(
    "ta_macd",
    {LogicalType::DOUBLE, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
    LogicalType::STRUCT({{"macd", LogicalType::DOUBLE},
                         {"signal", LogicalType::DOUBLE},
                         {"hist", LogicalType::DOUBLE}}),
    StateSize, Initialize, Update, Combine, Finalize,
    FunctionNullHandling::DEFAULT_NULL_HANDLING,
    SimpleUpdate, nullptr, Destroy);
```

## Performance

No new performance concerns beyond what existing `ta_*` aggregates already have:

- Same O(n^2) per-partition cost (TA-Lib recomputes full buffer each window position)
- 2-3 temporary output arrays instead of 1 in Finalize (negligible)
- Users accessing multiple STRUCT fields via repeated `(ta_macd(...) OVER w).field` may cause DuckDB to evaluate the window expression multiple times if CSE doesn't deduplicate. Workaround: use a subquery to materialize the STRUCT once.

## Testing

New test file: `test/sql/test_multi_output_aggregate.test`

- Basic window usage for all 8 functions
- Correctness: compare last row value against scalar `t_*` output
- PARTITION BY with multiple groups
- NULL handling in input data
- Lookback period behavior (early rows return NULL struct fields)

## Documentation

Update `README.md`, `cookbook.md`, and `index.md` to document the new aggregate forms with window syntax examples.
