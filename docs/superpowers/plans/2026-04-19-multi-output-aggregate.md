# Multi-Output Aggregate Window Functions Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add `ta_*` aggregate (window-compatible) forms for 8 multi-output functions (MACD, BBANDS, STOCH, AROON, MINMAX, MAMA, HT_PHASOR, HT_SINE) that return STRUCT per row.

**Architecture:** Extract shared state structs to a header, create `src/talib_aggregate_multi.cpp` with per-function aggregate implementations that reuse existing state structs where possible and define 4 new ones. Each function accumulates rows, calls TA-Lib in Finalize, and returns a STRUCT with the last computed values.

**Tech Stack:** C++, DuckDB Extension API (AggregateFunction, StructVector), TA-Lib C API

---

## File Map

| File | Action | Responsibility |
|------|--------|---------------|
| `src/include/talib_aggregate_state.hpp` | Create | Shared state structs (extracted from talib_aggregate.cpp + 4 new ones) |
| `src/talib_aggregate.cpp` | Modify | Remove inline state struct definitions, `#include "talib_aggregate_state.hpp"` |
| `src/talib_aggregate_multi.cpp` | Create | 8 multi-output aggregate functions |
| `src/talib_extension.cpp` | Modify | Add forward declaration + call to RegisterTalibAggregateMultiOutputFunctions |
| `CMakeLists.txt` | Modify | Add `src/talib_aggregate_multi.cpp` to EXTENSION_SOURCES |
| `test/sql/test_multi_output_aggregate.test` | Create | Tests for all 8 functions |
| `README.md` | Modify | Document new aggregate forms |
| `cookbook.md` | Modify | Add window usage examples |
| `index.md` | Modify | Add function reference entries |

---

### Task 1: Extract state structs to shared header

**Files:**
- Create: `src/include/talib_aggregate_state.hpp`
- Modify: `src/talib_aggregate.cpp:17-69`

- [ ] **Step 1: Create the shared header**

Create `src/include/talib_aggregate_state.hpp`:

```cpp
#pragma once

#include <vector>

namespace duckdb {

// ============================================================
// Shared state structs for aggregate TA-Lib functions
// ============================================================

// P1/P2: single value series (+ optional period)
struct TalibAggState1 {
    std::vector<double> *values;
    int period; // -1 means no period (P2)
};

// P3: HLC + period
struct TalibAggState3 {
    std::vector<double> *high;
    std::vector<double> *low;
    std::vector<double> *close;
    int period;
};

// P4: HLCV (no period)
struct TalibAggState4 {
    std::vector<double> *high;
    std::vector<double> *low;
    std::vector<double> *close;
    std::vector<double> *volume;
};

// P5: OHLC (no period)
struct TalibAggState5 {
    std::vector<double> *open_;
    std::vector<double> *high;
    std::vector<double> *low;
    std::vector<double> *close;
};

// P6: HL (no period)
struct TalibAggState6 {
    std::vector<double> *high;
    std::vector<double> *low;
};

// P7: HLC (no period)
struct TalibAggState7 {
    std::vector<double> *high;
    std::vector<double> *low;
    std::vector<double> *close;
};

// P8: HL + period
struct TalibAggState8 {
    std::vector<double> *high;
    std::vector<double> *low;
    int period;
};

// ============================================================
// Multi-output state structs
// ============================================================

// MACD: close + 3 integer periods
struct TalibAggStateMacd {
    std::vector<double> *values;
    int fast_period;
    int slow_period;
    int signal_period;
};

// BBANDS: close + int period + 2 double deviations + int ma_type
struct TalibAggStateBbands {
    std::vector<double> *values;
    int time_period;
    double nb_dev_up;
    double nb_dev_dn;
    int ma_type;
};

// STOCH: HLC + 5 integer params
struct TalibAggStateStoch {
    std::vector<double> *high;
    std::vector<double> *low;
    std::vector<double> *close;
    int fastk_period;
    int slowk_period;
    int slowk_matype;
    int slowd_period;
    int slowd_matype;
};

// MAMA: close + 2 double limits
struct TalibAggStateMama {
    std::vector<double> *values;
    double fast_limit;
    double slow_limit;
};

} // namespace duckdb
```

- [ ] **Step 2: Update talib_aggregate.cpp to use the header**

In `src/talib_aggregate.cpp`, replace the state struct definitions (lines 17-69) with:

```cpp
#include "talib_aggregate_state.hpp"
```

Remove these lines (the entire block from `// ============================================================` / `// State structs for each pattern` through the closing `};` of `TalibAggState8`). Keep all other includes and code intact.

The file's existing includes block becomes:

```cpp
#include "duckdb.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/function/function_set.hpp"
#include "duckdb/main/extension/extension_loader.hpp"
// flat_vector included via duckdb.hpp

#include "talib_aggregate_state.hpp"

extern "C" {
#include "ta_libc.h"
}

#include <vector>
#include <cstring>
#include <new>
```

- [ ] **Step 3: Build and verify no regressions**

Run:
```bash
make clean && make -j
```
Expected: Compiles successfully with no errors.

Run:
```bash
make test
```
Expected: All existing tests pass.

- [ ] **Step 4: Commit**

```bash
git add src/include/talib_aggregate_state.hpp src/talib_aggregate.cpp
git commit -m "refactor: extract aggregate state structs to shared header

Add talib_aggregate_state.hpp with all existing state structs plus
4 new multi-output state structs (Macd, Bbands, Stoch, Mama).
Prepares for multi-output aggregate functions."
```

---

### Task 2: Add talib_aggregate_multi.cpp with MACD and MINMAX

Start with one 3-output function (MACD) and one 2-output function (MINMAX) to validate the STRUCT return pattern.

**Files:**
- Create: `src/talib_aggregate_multi.cpp`
- Modify: `src/talib_extension.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Write the test file**

Create `test/sql/test_multi_output_aggregate.test`:

```
# name: test/sql/test_multi_output_aggregate.test
# description: Test multi-output aggregate (window) TA-Lib functions
# group: [talib]

require talib

# ============================================================
# Test data table
# ============================================================

statement ok
CREATE TABLE test_ohlcv AS
SELECT * FROM (VALUES
    ('2024-01-02'::DATE, 'MSFT', 376.0, 377.5, 374.0, 375.3, 100000.0),
    ('2024-01-03'::DATE, 'MSFT', 375.0, 378.0, 373.5, 377.2, 110000.0),
    ('2024-01-04'::DATE, 'MSFT', 377.0, 380.0, 376.0, 379.1, 105000.0),
    ('2024-01-05'::DATE, 'MSFT', 379.0, 381.5, 378.0, 380.5, 120000.0),
    ('2024-01-08'::DATE, 'MSFT', 380.0, 383.0, 379.5, 382.0, 115000.0),
    ('2024-01-09'::DATE, 'MSFT', 382.0, 384.0, 381.0, 383.5, 125000.0),
    ('2024-01-10'::DATE, 'MSFT', 383.0, 385.5, 382.5, 384.8, 130000.0),
    ('2024-01-11'::DATE, 'MSFT', 384.0, 386.0, 383.0, 385.2, 118000.0),
    ('2024-01-12'::DATE, 'MSFT', 385.0, 387.0, 384.0, 386.0, 122000.0),
    ('2024-01-16'::DATE, 'MSFT', 386.0, 388.0, 385.0, 387.5, 128000.0)
) AS t(date, symbol, open, high, low, close, volume);

# ============================================================
# MINMAX: 2-output, reuses TalibAggState1
# ============================================================

# Basic window usage - returns STRUCT
query I
SELECT (ta_minmax(close, 3) OVER (
    ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
)).min IS NOT NULL
FROM test_ohlcv
ORDER BY date
LIMIT 1 OFFSET 2;
----
true

# Early rows should have NULL fields (lookback)
query I
SELECT (ta_minmax(close, 3) OVER (
    ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
)).min IS NULL
FROM test_ohlcv
ORDER BY date
LIMIT 1;
----
true

# Both fields accessible
query II
SELECT m.min IS NOT NULL, m.max IS NOT NULL
FROM (
    SELECT ta_minmax(close, 3) OVER (
        ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_ohlcv
) sub
ORDER BY rowid
LIMIT 1 OFFSET 9;
----
true	true

# ============================================================
# MACD: 3-output, uses TalibAggStateMacd
# ============================================================

# Basic window usage - use small periods so we get output with 10 rows
# fast=3, slow=5, signal=2 -> lookback = slow-1 + signal-1 = 5
query I
SELECT (ta_macd(close, 3, 5, 2) OVER (
    ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
)).macd IS NOT NULL
FROM test_ohlcv
ORDER BY date
LIMIT 1 OFFSET 9;
----
true

# All 3 fields accessible via subquery
query III
SELECT m.macd IS NOT NULL, m.signal IS NOT NULL, m.hist IS NOT NULL
FROM (
    SELECT ta_macd(close, 3, 5, 2) OVER (
        ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_ohlcv
) sub
ORDER BY rowid
LIMIT 1 OFFSET 9;
----
true	true	true

# Early rows NULL (lookback)
query I
SELECT (ta_macd(close, 3, 5, 2) OVER (
    ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
)).macd IS NULL
FROM test_ohlcv
ORDER BY date
LIMIT 1;
----
true
```

- [ ] **Step 2: Create talib_aggregate_multi.cpp**

Create `src/talib_aggregate_multi.cpp`:

```cpp
#include "duckdb.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/main/extension/extension_loader.hpp"

#include "talib_aggregate_state.hpp"

extern "C" {
#include "ta_libc.h"
}

#include <vector>
#include <new>

namespace duckdb {

// ============================================================
// MINMAX: (DOUBLE, INT) -> STRUCT(min, max)
// Reuses TalibAggState1
// ============================================================

struct TalibAggMinMax {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState1); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState1 *>(state);
        s->values = nullptr;
        s->period = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, pdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, pdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);
        auto states = (TalibAggState1 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->period = periods[pidx];

            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState1 *>(state_p);

        UnifiedVectorFormat vdata, pdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, pdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            state->period = periods[pidx];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState1 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState1 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
                tgt[i]->period = src[i]->period;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_min = FlatVector::GetData<double>(*entries[0]);
        auto d_max = FlatVector::GetData<double>(*entries[1]);
        auto &v_min = FlatVector::Validity(*entries[0]);
        auto &v_max = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_min.SetInvalid(ridx);
                v_max.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outMin(size), outMax(size);

            TA_RetCode rc = TA_MINMAX(0, size - 1, state->values->data(),
                                      state->period, &outBeg, &outNb,
                                      outMin.data(), outMax.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_min.SetInvalid(ridx);
                v_max.SetInvalid(ridx);
            } else {
                d_min[ridx] = outMin[outNb - 1];
                d_max[ridx] = outMax[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};

// ============================================================
// MACD: (DOUBLE, INT, INT, INT) -> STRUCT(macd, signal, hist)
// Uses TalibAggStateMacd
// ============================================================

struct TalibAggMacd {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggStateMacd); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggStateMacd *>(state);
        s->values = nullptr;
        s->fast_period = 0;
        s->slow_period = 0;
        s->signal_period = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, fpdata, spdata, sigdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fpdata);
        inputs[2].ToUnifiedFormat(count, spdata);
        inputs[3].ToUnifiedFormat(count, sigdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_p = UnifiedVectorFormat::GetData<int32_t>(fpdata);
        auto slow_p = UnifiedVectorFormat::GetData<int32_t>(spdata);
        auto sig_p = UnifiedVectorFormat::GetData<int32_t>(sigdata);
        auto states = (TalibAggStateMacd **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->fast_period = fast_p[fpdata.sel->get_index(i)];
            state->slow_period = slow_p[spdata.sel->get_index(i)];
            state->signal_period = sig_p[sigdata.sel->get_index(i)];

            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggStateMacd *>(state_p);

        UnifiedVectorFormat vdata, fpdata, spdata, sigdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fpdata);
        inputs[2].ToUnifiedFormat(count, spdata);
        inputs[3].ToUnifiedFormat(count, sigdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_p = UnifiedVectorFormat::GetData<int32_t>(fpdata);
        auto slow_p = UnifiedVectorFormat::GetData<int32_t>(spdata);
        auto sig_p = UnifiedVectorFormat::GetData<int32_t>(sigdata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            state->fast_period = fast_p[fpdata.sel->get_index(i)];
            state->slow_period = slow_p[spdata.sel->get_index(i)];
            state->signal_period = sig_p[sigdata.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggStateMacd *>(source);
        auto tgt = FlatVector::GetData<TalibAggStateMacd *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
                tgt[i]->fast_period = src[i]->fast_period;
                tgt[i]->slow_period = src[i]->slow_period;
                tgt[i]->signal_period = src[i]->signal_period;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggStateMacd *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_macd = FlatVector::GetData<double>(*entries[0]);
        auto d_signal = FlatVector::GetData<double>(*entries[1]);
        auto d_hist = FlatVector::GetData<double>(*entries[2]);
        auto &v_macd = FlatVector::Validity(*entries[0]);
        auto &v_signal = FlatVector::Validity(*entries[1]);
        auto &v_hist = FlatVector::Validity(*entries[2]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_macd.SetInvalid(ridx);
                v_signal.SetInvalid(ridx);
                v_hist.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outMACD(size), outSignal(size), outHist(size);

            TA_RetCode rc = TA_MACD(0, size - 1, state->values->data(),
                                    state->fast_period, state->slow_period, state->signal_period,
                                    &outBeg, &outNb,
                                    outMACD.data(), outSignal.data(), outHist.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_macd.SetInvalid(ridx);
                v_signal.SetInvalid(ridx);
                v_hist.SetInvalid(ridx);
            } else {
                d_macd[ridx] = outMACD[outNb - 1];
                d_signal[ridx] = outSignal[outNb - 1];
                d_hist[ridx] = outHist[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggStateMacd *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};

// ============================================================
// Registration
// ============================================================

void RegisterTalibAggregateMultiOutputFunctions(ExtensionLoader &loader) {

    // MINMAX: (DOUBLE, INT) -> STRUCT(min, max)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("min", LogicalType::DOUBLE));
        fields.push_back(make_pair("max", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_minmax",
            {LogicalType::DOUBLE, LogicalType::INTEGER},
            LogicalType::STRUCT(fields),
            TalibAggMinMax::StateSize, TalibAggMinMax::Initialize,
            TalibAggMinMax::Update, TalibAggMinMax::Combine, TalibAggMinMax::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggMinMax::SimpleUpdate, nullptr, TalibAggMinMax::Destroy);
        loader.RegisterFunction(func);
    }

    // MACD: (DOUBLE, INT, INT, INT) -> STRUCT(macd, signal, hist)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("macd", LogicalType::DOUBLE));
        fields.push_back(make_pair("signal", LogicalType::DOUBLE));
        fields.push_back(make_pair("hist", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_macd",
            {LogicalType::DOUBLE, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
            LogicalType::STRUCT(fields),
            TalibAggMacd::StateSize, TalibAggMacd::Initialize,
            TalibAggMacd::Update, TalibAggMacd::Combine, TalibAggMacd::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggMacd::SimpleUpdate, nullptr, TalibAggMacd::Destroy);
        loader.RegisterFunction(func);
    }
}

} // namespace duckdb
```

- [ ] **Step 3: Update talib_extension.cpp**

In `src/talib_extension.cpp`, add the forward declaration and call. After line 14 (`void RegisterTalibMultiOutputFunctions(ExtensionLoader &loader);`), add:

```cpp
// Defined in talib_aggregate_multi.cpp
void RegisterTalibAggregateMultiOutputFunctions(ExtensionLoader &loader);
```

In `LoadInternal`, after `RegisterTalibMultiOutputFunctions(loader);` (line 19), add:

```cpp
    RegisterTalibAggregateMultiOutputFunctions(loader);
```

- [ ] **Step 4: Update CMakeLists.txt**

In `CMakeLists.txt`, add `src/talib_aggregate_multi.cpp` to `EXTENSION_SOURCES`. Change:

```cmake
set(EXTENSION_SOURCES
    src/talib_extension.cpp
    src/talib_adapter.cpp
    src/talib_scalar.cpp
    src/talib_aggregate.cpp
    src/talib_multi_output.cpp
)
```

To:

```cmake
set(EXTENSION_SOURCES
    src/talib_extension.cpp
    src/talib_adapter.cpp
    src/talib_scalar.cpp
    src/talib_aggregate.cpp
    src/talib_aggregate_multi.cpp
    src/talib_multi_output.cpp
)
```

- [ ] **Step 5: Build and run tests**

Run:
```bash
make clean && make -j
```
Expected: Compiles successfully.

Run:
```bash
make test
```
Expected: All tests pass, including the new `test_multi_output_aggregate.test`.

- [ ] **Step 6: Commit**

```bash
git add src/talib_aggregate_multi.cpp src/talib_extension.cpp CMakeLists.txt test/sql/test_multi_output_aggregate.test
git commit -m "feat: add ta_macd and ta_minmax aggregate window functions

Return STRUCT per row instead of scalar. MACD returns
STRUCT(macd, signal, hist), MINMAX returns STRUCT(min, max).
Used with OVER(...) window frames."
```

---

### Task 3: Add BBANDS, AROON, STOCH

**Files:**
- Modify: `src/talib_aggregate_multi.cpp`
- Modify: `test/sql/test_multi_output_aggregate.test`

- [ ] **Step 1: Add tests**

Append to `test/sql/test_multi_output_aggregate.test`:

```
# ============================================================
# BBANDS: 3-output, uses TalibAggStateBbands
# ============================================================

query I
SELECT (ta_bbands(close, 5, 2.0, 2.0, 0) OVER (
    ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
)).upper IS NOT NULL
FROM test_ohlcv
ORDER BY date
LIMIT 1 OFFSET 9;
----
true

# All 3 fields via subquery
query III
SELECT m.upper IS NOT NULL, m.middle IS NOT NULL, m.lower IS NOT NULL
FROM (
    SELECT ta_bbands(close, 5, 2.0, 2.0, 0) OVER (
        ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_ohlcv
) sub
ORDER BY rowid
LIMIT 1 OFFSET 9;
----
true	true	true

# ============================================================
# AROON: 2-output, reuses TalibAggState8
# ============================================================

query II
SELECT m.aroon_down IS NOT NULL, m.aroon_up IS NOT NULL
FROM (
    SELECT ta_aroon(high, low, 3) OVER (
        ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_ohlcv
) sub
ORDER BY rowid
LIMIT 1 OFFSET 9;
----
true	true

# ============================================================
# STOCH: 2-output, uses TalibAggStateStoch
# ============================================================

query II
SELECT m.slowk IS NOT NULL, m.slowd IS NOT NULL
FROM (
    SELECT ta_stoch(high, low, close, 5, 3, 0, 3, 0) OVER (
        ORDER BY date ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_ohlcv
) sub
ORDER BY rowid
LIMIT 1 OFFSET 9;
----
true	true
```

- [ ] **Step 2: Add BBANDS implementation**

In `src/talib_aggregate_multi.cpp`, before the `// Registration` section, add:

```cpp
// ============================================================
// BBANDS: (DOUBLE, INT, DOUBLE, DOUBLE, INT) -> STRUCT(upper, middle, lower)
// Uses TalibAggStateBbands
// ============================================================

struct TalibAggBbands {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggStateBbands); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggStateBbands *>(state);
        s->values = nullptr;
        s->time_period = 0;
        s->nb_dev_up = 0.0;
        s->nb_dev_dn = 0.0;
        s->ma_type = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, tpdata, dudata, dddata, mtdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, tpdata);
        inputs[2].ToUnifiedFormat(count, dudata);
        inputs[3].ToUnifiedFormat(count, dddata);
        inputs[4].ToUnifiedFormat(count, mtdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto tp = UnifiedVectorFormat::GetData<int32_t>(tpdata);
        auto du = UnifiedVectorFormat::GetData<double>(dudata);
        auto dd = UnifiedVectorFormat::GetData<double>(dddata);
        auto mt = UnifiedVectorFormat::GetData<int32_t>(mtdata);
        auto states = (TalibAggStateBbands **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->time_period = tp[tpdata.sel->get_index(i)];
            state->nb_dev_up = du[dudata.sel->get_index(i)];
            state->nb_dev_dn = dd[dddata.sel->get_index(i)];
            state->ma_type = mt[mtdata.sel->get_index(i)];

            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggStateBbands *>(state_p);

        UnifiedVectorFormat vdata, tpdata, dudata, dddata, mtdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, tpdata);
        inputs[2].ToUnifiedFormat(count, dudata);
        inputs[3].ToUnifiedFormat(count, dddata);
        inputs[4].ToUnifiedFormat(count, mtdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto tp = UnifiedVectorFormat::GetData<int32_t>(tpdata);
        auto du = UnifiedVectorFormat::GetData<double>(dudata);
        auto dd = UnifiedVectorFormat::GetData<double>(dddata);
        auto mt = UnifiedVectorFormat::GetData<int32_t>(mtdata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            state->time_period = tp[tpdata.sel->get_index(i)];
            state->nb_dev_up = du[dudata.sel->get_index(i)];
            state->nb_dev_dn = dd[dddata.sel->get_index(i)];
            state->ma_type = mt[mtdata.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggStateBbands *>(source);
        auto tgt = FlatVector::GetData<TalibAggStateBbands *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
                tgt[i]->time_period = src[i]->time_period;
                tgt[i]->nb_dev_up = src[i]->nb_dev_up;
                tgt[i]->nb_dev_dn = src[i]->nb_dev_dn;
                tgt[i]->ma_type = src[i]->ma_type;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggStateBbands *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_upper = FlatVector::GetData<double>(*entries[0]);
        auto d_middle = FlatVector::GetData<double>(*entries[1]);
        auto d_lower = FlatVector::GetData<double>(*entries[2]);
        auto &v_upper = FlatVector::Validity(*entries[0]);
        auto &v_middle = FlatVector::Validity(*entries[1]);
        auto &v_lower = FlatVector::Validity(*entries[2]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_upper.SetInvalid(ridx);
                v_middle.SetInvalid(ridx);
                v_lower.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outUpper(size), outMiddle(size), outLower(size);

            TA_RetCode rc = TA_BBANDS(0, size - 1, state->values->data(),
                                      state->time_period, state->nb_dev_up, state->nb_dev_dn,
                                      (TA_MAType)state->ma_type,
                                      &outBeg, &outNb,
                                      outUpper.data(), outMiddle.data(), outLower.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_upper.SetInvalid(ridx);
                v_middle.SetInvalid(ridx);
                v_lower.SetInvalid(ridx);
            } else {
                d_upper[ridx] = outUpper[outNb - 1];
                d_middle[ridx] = outMiddle[outNb - 1];
                d_lower[ridx] = outLower[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggStateBbands *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};

// ============================================================
// AROON: (DOUBLE, DOUBLE, INT) -> STRUCT(aroon_down, aroon_up)
// Reuses TalibAggState8
// ============================================================

struct TalibAggAroon {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState8); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState8 *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->period = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, pdata, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, pdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);
        auto states = (TalibAggState8 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->high) {
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
            }
            state->period = periods[pidx];

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState8 *>(state_p);

        UnifiedVectorFormat hdata, ldata, pdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, pdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            state->period = periods[pidx];
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState8 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState8 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->high && !src[i]->high->empty()) {
                if (!tgt[i]->high) {
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                }
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
                tgt[i]->period = src[i]->period;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState8 *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_down = FlatVector::GetData<double>(*entries[0]);
        auto d_up = FlatVector::GetData<double>(*entries[1]);
        auto &v_down = FlatVector::Validity(*entries[0]);
        auto &v_up = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->high || state->high->empty()) {
                v_down.SetInvalid(ridx);
                v_up.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->high->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outDown(size), outUp(size);

            TA_RetCode rc = TA_AROON(0, size - 1, state->high->data(), state->low->data(),
                                     state->period, &outBeg, &outNb,
                                     outDown.data(), outUp.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_down.SetInvalid(ridx);
                v_up.SetInvalid(ridx);
            } else {
                d_down[ridx] = outDown[outNb - 1];
                d_up[ridx] = outUp[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState8 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->high;
            delete states[i]->low;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
        }
    }
};

// ============================================================
// STOCH: (DOUBLE x3, INT x5) -> STRUCT(slowk, slowd)
// Uses TalibAggStateStoch
// ============================================================

struct TalibAggStoch {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggStateStoch); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggStateStoch *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
        s->fastk_period = 0;
        s->slowk_period = 0;
        s->slowk_matype = 0;
        s->slowd_period = 0;
        s->slowd_matype = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, cdata, sdata;
        UnifiedVectorFormat fkdata, skdata, skmdata, sddata, sdmdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, fkdata);
        inputs[4].ToUnifiedFormat(count, skdata);
        inputs[5].ToUnifiedFormat(count, skmdata);
        inputs[6].ToUnifiedFormat(count, sddata);
        inputs[7].ToUnifiedFormat(count, sdmdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto fk = UnifiedVectorFormat::GetData<int32_t>(fkdata);
        auto sk = UnifiedVectorFormat::GetData<int32_t>(skdata);
        auto skm = UnifiedVectorFormat::GetData<int32_t>(skmdata);
        auto sd = UnifiedVectorFormat::GetData<int32_t>(sddata);
        auto sdm = UnifiedVectorFormat::GetData<int32_t>(sdmdata);
        auto states = (TalibAggStateStoch **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->high) {
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
                state->close = new std::vector<double>();
            }
            state->fastk_period = fk[fkdata.sel->get_index(i)];
            state->slowk_period = sk[skdata.sel->get_index(i)];
            state->slowk_matype = skm[skmdata.sel->get_index(i)];
            state->slowd_period = sd[sddata.sel->get_index(i)];
            state->slowd_matype = sdm[sdmdata.sel->get_index(i)];

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggStateStoch *>(state_p);

        UnifiedVectorFormat hdata, ldata, cdata;
        UnifiedVectorFormat fkdata, skdata, skmdata, sddata, sdmdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, fkdata);
        inputs[4].ToUnifiedFormat(count, skdata);
        inputs[5].ToUnifiedFormat(count, skmdata);
        inputs[6].ToUnifiedFormat(count, sddata);
        inputs[7].ToUnifiedFormat(count, sdmdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto fk = UnifiedVectorFormat::GetData<int32_t>(fkdata);
        auto sk = UnifiedVectorFormat::GetData<int32_t>(skdata);
        auto skm = UnifiedVectorFormat::GetData<int32_t>(skmdata);
        auto sd = UnifiedVectorFormat::GetData<int32_t>(sddata);
        auto sdm = UnifiedVectorFormat::GetData<int32_t>(sdmdata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            state->fastk_period = fk[fkdata.sel->get_index(i)];
            state->slowk_period = sk[skdata.sel->get_index(i)];
            state->slowk_matype = skm[skmdata.sel->get_index(i)];
            state->slowd_period = sd[sddata.sel->get_index(i)];
            state->slowd_matype = sdm[sdmdata.sel->get_index(i)];
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggStateStoch *>(source);
        auto tgt = FlatVector::GetData<TalibAggStateStoch *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->high && !src[i]->high->empty()) {
                if (!tgt[i]->high) {
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                    tgt[i]->close = new std::vector<double>();
                }
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
                tgt[i]->close->insert(tgt[i]->close->end(), src[i]->close->begin(), src[i]->close->end());
                tgt[i]->fastk_period = src[i]->fastk_period;
                tgt[i]->slowk_period = src[i]->slowk_period;
                tgt[i]->slowk_matype = src[i]->slowk_matype;
                tgt[i]->slowd_period = src[i]->slowd_period;
                tgt[i]->slowd_matype = src[i]->slowd_matype;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggStateStoch *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_slowk = FlatVector::GetData<double>(*entries[0]);
        auto d_slowd = FlatVector::GetData<double>(*entries[1]);
        auto &v_slowk = FlatVector::Validity(*entries[0]);
        auto &v_slowd = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->high || state->high->empty()) {
                v_slowk.SetInvalid(ridx);
                v_slowd.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->high->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outSlowK(size), outSlowD(size);

            TA_RetCode rc = TA_STOCH(0, size - 1,
                                     state->high->data(), state->low->data(), state->close->data(),
                                     state->fastk_period, state->slowk_period, (TA_MAType)state->slowk_matype,
                                     state->slowd_period, (TA_MAType)state->slowd_matype,
                                     &outBeg, &outNb,
                                     outSlowK.data(), outSlowD.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_slowk.SetInvalid(ridx);
                v_slowd.SetInvalid(ridx);
            } else {
                d_slowk[ridx] = outSlowK[outNb - 1];
                d_slowd[ridx] = outSlowD[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggStateStoch *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->high;
            delete states[i]->low;
            delete states[i]->close;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
            states[i]->close = nullptr;
        }
    }
};
```

- [ ] **Step 3: Add registrations**

In `RegisterTalibAggregateMultiOutputFunctions`, add after the MACD registration:

```cpp
    // BBANDS: (DOUBLE, INT, DOUBLE, DOUBLE, INT) -> STRUCT(upper, middle, lower)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("upper", LogicalType::DOUBLE));
        fields.push_back(make_pair("middle", LogicalType::DOUBLE));
        fields.push_back(make_pair("lower", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_bbands",
            {LogicalType::DOUBLE, LogicalType::INTEGER, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER},
            LogicalType::STRUCT(fields),
            TalibAggBbands::StateSize, TalibAggBbands::Initialize,
            TalibAggBbands::Update, TalibAggBbands::Combine, TalibAggBbands::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggBbands::SimpleUpdate, nullptr, TalibAggBbands::Destroy);
        loader.RegisterFunction(func);
    }

    // AROON: (DOUBLE, DOUBLE, INT) -> STRUCT(aroon_down, aroon_up)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("aroon_down", LogicalType::DOUBLE));
        fields.push_back(make_pair("aroon_up", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_aroon",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER},
            LogicalType::STRUCT(fields),
            TalibAggAroon::StateSize, TalibAggAroon::Initialize,
            TalibAggAroon::Update, TalibAggAroon::Combine, TalibAggAroon::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggAroon::SimpleUpdate, nullptr, TalibAggAroon::Destroy);
        loader.RegisterFunction(func);
    }

    // STOCH: (DOUBLE x3, INT x5) -> STRUCT(slowk, slowd)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("slowk", LogicalType::DOUBLE));
        fields.push_back(make_pair("slowd", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_stoch",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE,
             LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER,
             LogicalType::INTEGER, LogicalType::INTEGER},
            LogicalType::STRUCT(fields),
            TalibAggStoch::StateSize, TalibAggStoch::Initialize,
            TalibAggStoch::Update, TalibAggStoch::Combine, TalibAggStoch::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggStoch::SimpleUpdate, nullptr, TalibAggStoch::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and test**

Run:
```bash
make clean && make -j && make test
```
Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_aggregate_multi.cpp test/sql/test_multi_output_aggregate.test
git commit -m "feat: add ta_bbands, ta_aroon, ta_stoch aggregate window functions

BBANDS returns STRUCT(upper, middle, lower).
AROON returns STRUCT(aroon_down, aroon_up).
STOCH returns STRUCT(slowk, slowd)."
```

---

### Task 4: Add MAMA, HT_PHASOR, HT_SINE

**Files:**
- Modify: `src/talib_aggregate_multi.cpp`
- Modify: `test/sql/test_multi_output_aggregate.test`

- [ ] **Step 1: Add tests**

Append to `test/sql/test_multi_output_aggregate.test`:

```
# ============================================================
# Larger dataset for cycle indicators (need ~40 points)
# ============================================================

statement ok
CREATE TABLE test_cycle AS
SELECT row_number() OVER () AS idx,
       100.0 + 5.0 * sin(row_number() OVER () * 0.3) +
       2.0 * sin(row_number() OVER () * 0.7) AS val
FROM range(50);

# ============================================================
# MAMA: 2-output, uses TalibAggStateMama
# ============================================================

query II
SELECT m.mama IS NOT NULL, m.fama IS NOT NULL
FROM (
    SELECT ta_mama(val, 0.5, 0.05) OVER (
        ORDER BY idx ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_cycle
) sub
ORDER BY rowid
LIMIT 1 OFFSET 49;
----
true	true

# ============================================================
# HT_PHASOR: 2-output, reuses TalibAggState1 (no period)
# ============================================================

query II
SELECT m.inphase IS NOT NULL, m.quadrature IS NOT NULL
FROM (
    SELECT ta_ht_phasor(val) OVER (
        ORDER BY idx ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_cycle
) sub
ORDER BY rowid
LIMIT 1 OFFSET 49;
----
true	true

# ============================================================
# HT_SINE: 2-output, reuses TalibAggState1 (no period)
# ============================================================

query II
SELECT m.sine IS NOT NULL, m.leadsine IS NOT NULL
FROM (
    SELECT ta_ht_sine(val) OVER (
        ORDER BY idx ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) AS m
    FROM test_cycle
) sub
ORDER BY rowid
LIMIT 1 OFFSET 49;
----
true	true

# ============================================================
# PARTITION BY test (ta_minmax with 2 groups)
# ============================================================

statement ok
CREATE TABLE multi_stock AS
SELECT * FROM (VALUES
    ('2024-01-01'::DATE, 'A', 10.0, 12.0, 9.0, 11.0),
    ('2024-01-02'::DATE, 'A', 11.0, 13.0, 10.0, 12.0),
    ('2024-01-03'::DATE, 'A', 12.0, 14.0, 11.0, 13.0),
    ('2024-01-04'::DATE, 'A', 13.0, 15.0, 12.0, 14.0),
    ('2024-01-01'::DATE, 'B', 50.0, 52.0, 49.0, 51.0),
    ('2024-01-02'::DATE, 'B', 51.0, 53.0, 50.0, 52.0),
    ('2024-01-03'::DATE, 'B', 52.0, 54.0, 51.0, 53.0),
    ('2024-01-04'::DATE, 'B', 53.0, 55.0, 52.0, 54.0)
) AS t(date, symbol, open, high, low, close);

query TII
SELECT symbol, m.min IS NOT NULL, m.max IS NOT NULL
FROM (
    SELECT symbol, date,
           ta_minmax(close, 2) OVER (
               PARTITION BY symbol ORDER BY date
               ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
           ) AS m
    FROM multi_stock
) sub
WHERE date = '2024-01-04'
ORDER BY symbol;
----
A	true	true
B	true	true
```

- [ ] **Step 2: Add MAMA implementation**

In `src/talib_aggregate_multi.cpp`, before the `// Registration` section, add:

```cpp
// ============================================================
// MAMA: (DOUBLE, DOUBLE, DOUBLE) -> STRUCT(mama, fama)
// Uses TalibAggStateMama
// ============================================================

struct TalibAggMama {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggStateMama); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggStateMama *>(state);
        s->values = nullptr;
        s->fast_limit = 0.0;
        s->slow_limit = 0.0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, fldata, sldata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fldata);
        inputs[2].ToUnifiedFormat(count, sldata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fl = UnifiedVectorFormat::GetData<double>(fldata);
        auto sl = UnifiedVectorFormat::GetData<double>(sldata);
        auto states = (TalibAggStateMama **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->fast_limit = fl[fldata.sel->get_index(i)];
            state->slow_limit = sl[sldata.sel->get_index(i)];

            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggStateMama *>(state_p);

        UnifiedVectorFormat vdata, fldata, sldata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fldata);
        inputs[2].ToUnifiedFormat(count, sldata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fl = UnifiedVectorFormat::GetData<double>(fldata);
        auto sl = UnifiedVectorFormat::GetData<double>(sldata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            state->fast_limit = fl[fldata.sel->get_index(i)];
            state->slow_limit = sl[sldata.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggStateMama *>(source);
        auto tgt = FlatVector::GetData<TalibAggStateMama *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
                tgt[i]->fast_limit = src[i]->fast_limit;
                tgt[i]->slow_limit = src[i]->slow_limit;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggStateMama *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_mama = FlatVector::GetData<double>(*entries[0]);
        auto d_fama = FlatVector::GetData<double>(*entries[1]);
        auto &v_mama = FlatVector::Validity(*entries[0]);
        auto &v_fama = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_mama.SetInvalid(ridx);
                v_fama.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outMAMA(size), outFAMA(size);

            TA_RetCode rc = TA_MAMA(0, size - 1, state->values->data(),
                                    state->fast_limit, state->slow_limit,
                                    &outBeg, &outNb,
                                    outMAMA.data(), outFAMA.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_mama.SetInvalid(ridx);
                v_fama.SetInvalid(ridx);
            } else {
                d_mama[ridx] = outMAMA[outNb - 1];
                d_fama[ridx] = outFAMA[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggStateMama *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};

// ============================================================
// HT_PHASOR: (DOUBLE) -> STRUCT(inphase, quadrature)
// Reuses TalibAggState1 (period unused, set to -1)
// ============================================================

struct TalibAggHtPhasor {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState1); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState1 *>(state);
        s->values = nullptr;
        s->period = -1;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto states = (TalibAggState1 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState1 *>(state_p);

        UnifiedVectorFormat vdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        auto values = UnifiedVectorFormat::GetData<double>(vdata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState1 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState1 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_inphase = FlatVector::GetData<double>(*entries[0]);
        auto d_quad = FlatVector::GetData<double>(*entries[1]);
        auto &v_inphase = FlatVector::Validity(*entries[0]);
        auto &v_quad = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_inphase.SetInvalid(ridx);
                v_quad.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outInPhase(size), outQuadrature(size);

            TA_RetCode rc = TA_HT_PHASOR(0, size - 1, state->values->data(),
                                          &outBeg, &outNb,
                                          outInPhase.data(), outQuadrature.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_inphase.SetInvalid(ridx);
                v_quad.SetInvalid(ridx);
            } else {
                d_inphase[ridx] = outInPhase[outNb - 1];
                d_quad[ridx] = outQuadrature[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};

// ============================================================
// HT_SINE: (DOUBLE) -> STRUCT(sine, leadsine)
// Reuses TalibAggState1 (period unused, set to -1)
// ============================================================

struct TalibAggHtSine {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState1); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState1 *>(state);
        s->values = nullptr;
        s->period = -1;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto states = (TalibAggState1 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState1 *>(state_p);

        UnifiedVectorFormat vdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        auto values = UnifiedVectorFormat::GetData<double>(vdata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState1 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState1 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->values && !src[i]->values->empty()) {
                if (!tgt[i]->values) {
                    tgt[i]->values = new std::vector<double>();
                }
                tgt[i]->values->insert(tgt[i]->values->end(), src[i]->values->begin(), src[i]->values->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto d_sine = FlatVector::GetData<double>(*entries[0]);
        auto d_lead = FlatVector::GetData<double>(*entries[1]);
        auto &v_sine = FlatVector::Validity(*entries[0]);
        auto &v_lead = FlatVector::Validity(*entries[1]);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                v_sine.SetInvalid(ridx);
                v_lead.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outSine(size), outLeadSine(size);

            TA_RetCode rc = TA_HT_SINE(0, size - 1, state->values->data(),
                                        &outBeg, &outNb,
                                        outSine.data(), outLeadSine.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                v_sine.SetInvalid(ridx);
                v_lead.SetInvalid(ridx);
            } else {
                d_sine[ridx] = outSine[outNb - 1];
                d_lead[ridx] = outLeadSine[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState1 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            if (states[i]->values) {
                delete states[i]->values;
                states[i]->values = nullptr;
            }
        }
    }
};
```

- [ ] **Step 3: Add registrations**

In `RegisterTalibAggregateMultiOutputFunctions`, add after the STOCH registration:

```cpp
    // MAMA: (DOUBLE, DOUBLE, DOUBLE) -> STRUCT(mama, fama)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("mama", LogicalType::DOUBLE));
        fields.push_back(make_pair("fama", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_mama",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE},
            LogicalType::STRUCT(fields),
            TalibAggMama::StateSize, TalibAggMama::Initialize,
            TalibAggMama::Update, TalibAggMama::Combine, TalibAggMama::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggMama::SimpleUpdate, nullptr, TalibAggMama::Destroy);
        loader.RegisterFunction(func);
    }

    // HT_PHASOR: (DOUBLE) -> STRUCT(inphase, quadrature)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("inphase", LogicalType::DOUBLE));
        fields.push_back(make_pair("quadrature", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_ht_phasor",
            {LogicalType::DOUBLE},
            LogicalType::STRUCT(fields),
            TalibAggHtPhasor::StateSize, TalibAggHtPhasor::Initialize,
            TalibAggHtPhasor::Update, TalibAggHtPhasor::Combine, TalibAggHtPhasor::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggHtPhasor::SimpleUpdate, nullptr, TalibAggHtPhasor::Destroy);
        loader.RegisterFunction(func);
    }

    // HT_SINE: (DOUBLE) -> STRUCT(sine, leadsine)
    {
        child_list_t<LogicalType> fields;
        fields.push_back(make_pair("sine", LogicalType::DOUBLE));
        fields.push_back(make_pair("leadsine", LogicalType::DOUBLE));

        AggregateFunction func(
            "ta_ht_sine",
            {LogicalType::DOUBLE},
            LogicalType::STRUCT(fields),
            TalibAggHtSine::StateSize, TalibAggHtSine::Initialize,
            TalibAggHtSine::Update, TalibAggHtSine::Combine, TalibAggHtSine::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            TalibAggHtSine::SimpleUpdate, nullptr, TalibAggHtSine::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and test**

Run:
```bash
make clean && make -j && make test
```
Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_aggregate_multi.cpp test/sql/test_multi_output_aggregate.test
git commit -m "feat: add ta_mama, ta_ht_phasor, ta_ht_sine aggregate window functions

MAMA returns STRUCT(mama, fama).
HT_PHASOR returns STRUCT(inphase, quadrature).
HT_SINE returns STRUCT(sine, leadsine).
All 8 multi-output aggregate functions now complete."
```

---

### Task 5: Update documentation

**Files:**
- Modify: `README.md`
- Modify: `cookbook.md`
- Modify: `index.md`

- [ ] **Step 1: Read current docs**

Read `README.md`, `cookbook.md`, and `index.md` to understand current structure and where to insert new content.

- [ ] **Step 2: Update README.md**

Add a section about multi-output aggregate functions after the existing aggregate function documentation. Include the subquery pattern example:

```markdown
### Multi-Output Window Functions

Multi-output functions (MACD, BBANDS, etc.) also work as window aggregates, returning a `STRUCT` per row:

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

Available: `ta_macd`, `ta_bbands`, `ta_stoch`, `ta_aroon`, `ta_minmax`, `ta_mama`, `ta_ht_phasor`, `ta_ht_sine`.
```

- [ ] **Step 3: Update cookbook.md**

Add examples for at least `ta_macd` and `ta_bbands` window usage with the subquery pattern.

- [ ] **Step 4: Update index.md**

Add entries for all 8 functions with their input/output signatures.

- [ ] **Step 5: Commit**

```bash
git add README.md cookbook.md index.md
git commit -m "docs: add multi-output aggregate window function documentation"
```
