# Multi-Output Window Functions Design

## Summary

Extend the 8 multi-output functions (currently scalar-only `t_` prefix) to also support window/aggregate forms (`ta_` prefix) that return a `STRUCT` per row, enabling dot-notation field access.

## Current State

Multi-output functions exist only as scalar functions returning `LIST<STRUCT>`:
- `t_macd(prices, fast, slow, signal)` -> `LIST<STRUCT(macd, signal, hist)>`
- `t_bbands(prices, period, devup, devdn, matype)` -> `LIST<STRUCT(upper, middle, lower)>`
- `t_stoch(high, low, close, fastK, slowK, slowKMA, slowD, slowDMA)` -> `LIST<STRUCT(slowk, slowd)>`
- `t_aroon(high, low, period)` -> `LIST<STRUCT(aroon_down, aroon_up)>`
- `t_minmax(prices, period)` -> `LIST<STRUCT(min, max)>`
- `t_mama(prices, fast_limit, slow_limit)` -> `LIST<STRUCT(mama, fama)>`
- `t_ht_phasor(prices)` -> `LIST<STRUCT(inphase, quadrature)>`
- `t_ht_sine(prices)` -> `LIST<STRUCT(sine, leadsine)>`

## Target Usage

```sql
-- Access struct fields directly via dot notation
SELECT ts, close,
       (ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts)).macd,
       (ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts)).signal
FROM prices;

-- Or assign alias first
SELECT ts, close, m.macd, m.signal, m.hist
FROM (
    SELECT *, ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts) AS m
    FROM prices
);
```

## Design

### New Window Functions

| Function | Inputs | Return Type |
|----------|--------|-------------|
| `ta_macd` | (close DOUBLE, fast INT, slow INT, signal INT) | STRUCT(macd, signal, hist) |
| `ta_bbands` | (close DOUBLE, period INT, devup DOUBLE, devdn DOUBLE, matype INT) | STRUCT(upper, middle, lower) |
| `ta_stoch` | (high DOUBLE, low DOUBLE, close DOUBLE, fastK INT, slowK INT, slowKMA INT, slowD INT, slowDMA INT) | STRUCT(slowk, slowd) |
| `ta_aroon` | (high DOUBLE, low DOUBLE, period INT) | STRUCT(aroon_down, aroon_up) |
| `ta_minmax` | (close DOUBLE, period INT) | STRUCT(min, max) |
| `ta_mama` | (close DOUBLE, fast_limit DOUBLE, slow_limit DOUBLE) | STRUCT(mama, fama) |
| `ta_ht_phasor` | (close DOUBLE) | STRUCT(inphase, quadrature) |
| `ta_ht_sine` | (close DOUBLE) | STRUCT(sine, leadsine) |

### Implementation Approach

Add a new file `src/talib_multi_output_agg.cpp` containing:

1. **State structs** - Reuse existing `TalibAggState1` (single series + params) and `TalibAggState3` (HLC + params) patterns. Add a new state for MACD/BBANDS/STOCH that stores additional parameters (multiple int/double params beyond a single period).

2. **Aggregate operator templates** - Two new templates:
   - `TalibAggMulti2` - For 2-output functions (aroon, minmax, mama, ht_phasor, ht_sine, stoch)
   - `TalibAggMulti3` - For 3-output functions (macd, bbands)

3. **Finalize into STRUCT** - Instead of writing a single double to `result`, finalize writes into the struct's child vectors:
   ```cpp
   static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
       auto states = FlatVector::GetData<State *>(states_vec);
       auto &entries = StructVector::GetEntries(result);
       auto data0 = FlatVector::GetData<double>(*entries[0]);
       auto data1 = FlatVector::GetData<double>(*entries[1]);
       auto &validity = FlatVector::Validity(result);

       for (idx_t i = 0; i < count; i++) {
           auto state = states[i];
           idx_t ridx = i + offset;
           // Call TA-Lib, take last values
           if (rc != TA_SUCCESS || outNb == 0) {
               validity.SetInvalid(ridx);
           } else {
               data0[ridx] = out1[outNb - 1];
               data1[ridx] = out2[outNb - 1];
           }
       }
   }
   ```

4. **State structs for multi-param functions** - Functions like MACD need 3 integer params (fast, slow, signal) rather than a single period. New state structs:
   ```cpp
   // MACD state: single series + 3 int params
   struct TalibAggStateMACD {
       std::vector<double> *values;
       int fast_period;
       int slow_period;
       int signal_period;
   };

   // BBANDS state: single series + 2 int + 2 double + 1 int params
   struct TalibAggStateBBANDS {
       std::vector<double> *values;
       int time_period;
       double nb_dev_up;
       double nb_dev_dn;
       int ma_type;
   };

   // STOCH state: HLC + 5 params
   struct TalibAggStateSTOCH {
       std::vector<double> *high;
       std::vector<double> *low;
       std::vector<double> *close;
       int fastk_period;
       int slowk_period;
       int slowk_matype;
       int slowd_period;
       int slowd_matype;
   };
   ```

5. **Registration** - Each function is registered individually (not via x-macro) since they have unique signatures. Register in a new `RegisterTalibMultiOutputAggFunctions(ExtensionLoader &loader)` called from `talib_extension.cpp`.

### Return Type Construction

```cpp
static LogicalType MakeStruct(const vector<pair<string, LogicalType>> &fields) {
    child_list_t<LogicalType> children;
    for (auto &f : fields) {
        children.push_back(make_pair(f.first, f.second));
    }
    return LogicalType::STRUCT(children);
}
```

### File Changes

| File | Change |
|------|--------|
| `src/talib_multi_output_agg.cpp` | **New file** - all 8 window aggregate implementations |
| `src/talib_extension.cpp` | Add `RegisterTalibMultiOutputAggFunctions` call |
| `CMakeLists.txt` | Add new source file (if not auto-globbed) |
| `test/sql/test_multi_output_agg.test` | **New file** - tests for all 8 window functions |
| `index.md` | Update docs to show `ta_` forms for multi-output functions |

### Grouping by Complexity

**Simple (reuse TalibAggState1 pattern, single series + 1 int param):**
- `ta_minmax` - (DOUBLE, INT) -> STRUCT(min, max)

**Medium (single series, no period or double params):**
- `ta_ht_phasor` - (DOUBLE) -> STRUCT(inphase, quadrature)
- `ta_ht_sine` - (DOUBLE) -> STRUCT(sine, leadsine)
- `ta_mama` - (DOUBLE, DOUBLE, DOUBLE) -> STRUCT(mama, fama)

**Complex (HLC or multi-param):**
- `ta_aroon` - (DOUBLE, DOUBLE, INT) -> STRUCT(aroon_down, aroon_up)
- `ta_macd` - (DOUBLE, INT, INT, INT) -> STRUCT(macd, signal, hist)
- `ta_bbands` - (DOUBLE, INT, DOUBLE, DOUBLE, INT) -> STRUCT(upper, middle, lower)
- `ta_stoch` - (DOUBLE, DOUBLE, DOUBLE, INT, INT, INT, INT, INT) -> STRUCT(slowk, slowd)

### Testing Strategy

Each function gets a test that:
1. Verifies the window function returns correct struct values matching the scalar form
2. Tests dot-notation field access works
3. Tests NULL handling (insufficient data in window)

Example test:
```sql
# ta_macd window function
statement ok
CREATE TABLE test_prices AS SELECT unnest(range(1,101))::DOUBLE AS close;

query III
SELECT (ta_macd(close, 12, 26, 9) OVER (ORDER BY rowid)).macd IS NOT NULL AS has_macd,
       (ta_macd(close, 12, 26, 9) OVER (ORDER BY rowid)).signal IS NOT NULL AS has_signal,
       (ta_macd(close, 12, 26, 9) OVER (ORDER BY rowid)).hist IS NOT NULL AS has_hist
FROM test_prices
ORDER BY rowid DESC LIMIT 1;
----
true	true	true
```
