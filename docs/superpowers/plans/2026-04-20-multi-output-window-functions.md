# Multi-Output Window Functions Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add window/aggregate forms (`ta_` prefix) for the 8 multi-output functions, returning STRUCT per row with dot-notation field access.

**Architecture:** New file `src/talib_multi_output_agg.cpp` with per-function state structs and aggregate operators that finalize into STRUCT vectors. Each function is registered individually since signatures vary.

**Tech Stack:** C++, DuckDB extension API (AggregateFunction, StructVector), TA-Lib C API.

---

## File Structure

| File | Role |
|------|------|
| `src/talib_multi_output_agg.cpp` | **Create** - All 8 window aggregate implementations + registration |
| `src/talib_extension.cpp` | **Modify** - Add forward declaration + call to new registration function |
| `CMakeLists.txt` | **Modify** - Add new source file to EXTENSION_SOURCES |
| `test/sql/test_multi_output_agg.test` | **Create** - Tests for all 8 window functions |

---

### Task 1: Scaffold and build infrastructure

**Files:**
- Create: `src/talib_multi_output_agg.cpp`
- Modify: `src/talib_extension.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create empty implementation file**

Create `src/talib_multi_output_agg.cpp`:

```cpp
#include "duckdb.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/function/function_set.hpp"
#include "duckdb/main/extension/extension_loader.hpp"

extern "C" {
#include "ta_libc.h"
}

#include <vector>
#include <new>

namespace duckdb {

// ============================================================
// Helper: construct STRUCT type
// ============================================================
static LogicalType MakeStruct(const vector<pair<string, LogicalType>> &fields) {
    child_list_t<LogicalType> children;
    for (auto &f : fields) {
        children.push_back(make_pair(f.first, f.second));
    }
    return LogicalType::STRUCT(children);
}

// ============================================================
// Registration
// ============================================================
void RegisterTalibMultiOutputAggFunctions(ExtensionLoader &loader) {
    // Functions will be added here
}

} // namespace duckdb
```

- [ ] **Step 2: Add to CMakeLists.txt**

Add `src/talib_multi_output_agg.cpp` to the `EXTENSION_SOURCES` list in `CMakeLists.txt`, after the existing `src/talib_multi_output.cpp` line:

```cmake
set(EXTENSION_SOURCES
    src/talib_extension.cpp
    src/talib_adapter.cpp
    src/talib_scalar.cpp
    src/talib_aggregate.cpp
    src/talib_multi_output.cpp
    src/talib_multi_output_agg.cpp
)
```

- [ ] **Step 3: Wire up in talib_extension.cpp**

Add forward declaration and call. In `src/talib_extension.cpp`, add after line 14:

```cpp
// Defined in talib_multi_output_agg.cpp
void RegisterTalibMultiOutputAggFunctions(ExtensionLoader &loader);
```

And in `LoadInternal`, add after `RegisterTalibMultiOutputFunctions(loader);`:

```cpp
    RegisterTalibMultiOutputAggFunctions(loader);
```

- [ ] **Step 4: Verify it compiles**

Run: `cd /Users/qunfei/Projects/atm_talib && make`

Expected: Build succeeds with no errors.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp src/talib_extension.cpp CMakeLists.txt
git commit -m "feat: scaffold multi-output aggregate functions file"
```

---

### Task 2: Implement ta_minmax (simplest: single series + period, 2 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Create: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Write the test file**

Create `test/sql/test_multi_output_agg.test`:

```
# name: test/sql/test_multi_output_agg.test
# description: Test multi-output window/aggregate TA-Lib functions
# group: [talib]

require talib

# ---------- ta_minmax ----------

statement ok
CREATE TABLE minmax_data AS
SELECT unnest(range(1, 21))::DOUBLE AS price, unnest(range(1, 21)) AS rn;

# Basic: returns struct with min/max fields
query II
SELECT (ta_minmax(price, 5) OVER (ORDER BY rn)).min,
       (ta_minmax(price, 5) OVER (ORDER BY rn)).max
FROM minmax_data
WHERE rn = 20;
----
16.0	20.0

# NULL for insufficient data (first 4 rows with period=5)
query I
SELECT (ta_minmax(price, 5) OVER (ORDER BY rn)).min IS NULL
FROM minmax_data
WHERE rn = 1;
----
true

statement ok
DROP TABLE minmax_data;
```

- [ ] **Step 2: Implement ta_minmax**

In `src/talib_multi_output_agg.cpp`, add between the `MakeStruct` helper and the registration function:

```cpp
// ============================================================
// State: single series + one integer period
// ============================================================
struct MultiAggState1 {
    std::vector<double> *values;
    int period;
};

// ============================================================
// ta_minmax: (DOUBLE, INT) -> STRUCT(min, max)
// ============================================================
struct TalibAggMinMax {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggState1); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggState1 *>(state);
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
        auto states = (MultiAggState1 **)sdata.data;

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
        auto state = reinterpret_cast<MultiAggState1 *>(state_p);

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
        auto src = FlatVector::GetData<MultiAggState1 *>(source);
        auto tgt = FlatVector::GetData<MultiAggState1 *>(target);
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
        auto states = FlatVector::GetData<MultiAggState1 *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_min = FlatVector::GetData<double>(*entries[0]);
        auto data_max = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outMin(size), outMax(size);

            TA_RetCode rc = TA_MINMAX(0, size - 1, state->values->data(),
                                      state->period, &outBeg, &outNb,
                                      outMin.data(), outMax.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                data_min[ridx] = outMin[outNb - 1];
                data_max[ridx] = outMax[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggState1 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 3: Register ta_minmax**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_minmax: (DOUBLE, INT) -> STRUCT(min, max)
    {
        using OP = TalibAggMinMax;
        AggregateFunction func(
            "ta_minmax",
            {LogicalType::DOUBLE, LogicalType::INTEGER},
            MakeStruct({{"min", LogicalType::DOUBLE}, {"max", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: Build succeeds, `test_multi_output_agg` passes.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_minmax window function returning STRUCT(min, max)"
```

---

### Task 3: Implement ta_aroon (HL + period, 2 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add test**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_aroon ----------

statement ok
CREATE TABLE aroon_data AS
SELECT unnest(generate_series(1, 20))::DOUBLE + (random() * 5)::DOUBLE AS high,
       unnest(generate_series(1, 20))::DOUBLE - (random() * 5)::DOUBLE AS low,
       unnest(range(1, 21)) AS rn;

# Basic: returns struct with aroon_down/aroon_up fields
query I
SELECT (ta_aroon(high, low, 14) OVER (ORDER BY rn)).aroon_up IS NOT NULL
FROM aroon_data
WHERE rn = 20;
----
true

# NULL for insufficient data
query I
SELECT (ta_aroon(high, low, 14) OVER (ORDER BY rn)).aroon_up IS NULL
FROM aroon_data
WHERE rn = 1;
----
true

statement ok
DROP TABLE aroon_data;
```

- [ ] **Step 2: Add state struct and implement TalibAggAroon**

In `src/talib_multi_output_agg.cpp`, add a new state struct (after `MultiAggState1`):

```cpp
// ============================================================
// State: two series (HL) + one integer period
// ============================================================
struct MultiAggStateHL {
    std::vector<double> *high;
    std::vector<double> *low;
    int period;
};
```

Then add the operator struct (after `TalibAggMinMax`):

```cpp
// ============================================================
// ta_aroon: (DOUBLE, DOUBLE, INT) -> STRUCT(aroon_down, aroon_up)
// ============================================================
struct TalibAggAroon {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateHL); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateHL *>(state);
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
        auto states = (MultiAggStateHL **)sdata.data;

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
        auto state = reinterpret_cast<MultiAggStateHL *>(state_p);

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
        auto src = FlatVector::GetData<MultiAggStateHL *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateHL *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateHL *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_down = FlatVector::GetData<double>(*entries[0]);
        auto data_up = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->high || state->high->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->high->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outDown(size), outUp(size);

            TA_RetCode rc = TA_AROON(0, size - 1, state->high->data(), state->low->data(),
                                     state->period, &outBeg, &outNb,
                                     outDown.data(), outUp.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                data_down[ridx] = outDown[outNb - 1];
                data_up[ridx] = outUp[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateHL *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->high;
            delete states[i]->low;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
        }
    }
};
```

- [ ] **Step 3: Register ta_aroon**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_aroon: (DOUBLE, DOUBLE, INT) -> STRUCT(aroon_down, aroon_up)
    {
        using OP = TalibAggAroon;
        AggregateFunction func(
            "ta_aroon",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER},
            MakeStruct({{"aroon_down", LogicalType::DOUBLE}, {"aroon_up", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_aroon window function returning STRUCT(aroon_down, aroon_up)"
```

---

### Task 4: Implement ta_ht_phasor and ta_ht_sine (single series, no period, 2 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add tests**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_ht_phasor ----------

statement ok
CREATE TABLE ht_data AS
SELECT sin(unnest(range(1, 101))::DOUBLE * 0.1) * 50 + 100 AS price,
       unnest(range(1, 101)) AS rn;

# Basic: returns struct with inphase/quadrature fields
query I
SELECT (ta_ht_phasor(price) OVER (ORDER BY rn)).inphase IS NOT NULL
FROM ht_data
WHERE rn = 100;
----
true

query I
SELECT (ta_ht_phasor(price) OVER (ORDER BY rn)).quadrature IS NOT NULL
FROM ht_data
WHERE rn = 100;
----
true

# ---------- ta_ht_sine ----------

# Basic: returns struct with sine/leadsine fields
query I
SELECT (ta_ht_sine(price) OVER (ORDER BY rn)).sine IS NOT NULL
FROM ht_data
WHERE rn = 100;
----
true

query I
SELECT (ta_ht_sine(price) OVER (ORDER BY rn)).leadsine IS NOT NULL
FROM ht_data
WHERE rn = 100;
----
true

statement ok
DROP TABLE ht_data;
```

- [ ] **Step 2: Add state struct for no-period single series**

In `src/talib_multi_output_agg.cpp`, add (after `MultiAggStateHL`):

```cpp
// ============================================================
// State: single series, no period
// ============================================================
struct MultiAggStateNoPeriod {
    std::vector<double> *values;
};
```

- [ ] **Step 3: Implement TalibAggHtPhasor**

```cpp
// ============================================================
// ta_ht_phasor: (DOUBLE) -> STRUCT(inphase, quadrature)
// ============================================================
struct TalibAggHtPhasor {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateNoPeriod); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateNoPeriod *>(state);
        s->values = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto states = (MultiAggStateNoPeriod **)sdata.data;

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
        auto state = reinterpret_cast<MultiAggStateNoPeriod *>(state_p);

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
        auto src = FlatVector::GetData<MultiAggStateNoPeriod *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateNoPeriod *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateNoPeriod *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_inphase = FlatVector::GetData<double>(*entries[0]);
        auto data_quadrature = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outInPhase(size), outQuadrature(size);

            TA_RetCode rc = TA_HT_PHASOR(0, size - 1, state->values->data(),
                                          &outBeg, &outNb,
                                          outInPhase.data(), outQuadrature.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                data_inphase[ridx] = outInPhase[outNb - 1];
                data_quadrature[ridx] = outQuadrature[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateNoPeriod *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 4: Implement TalibAggHtSine**

```cpp
// ============================================================
// ta_ht_sine: (DOUBLE) -> STRUCT(sine, leadsine)
// ============================================================
struct TalibAggHtSine {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateNoPeriod); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateNoPeriod *>(state);
        s->values = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto states = (MultiAggStateNoPeriod **)sdata.data;

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
        auto state = reinterpret_cast<MultiAggStateNoPeriod *>(state_p);

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
        auto src = FlatVector::GetData<MultiAggStateNoPeriod *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateNoPeriod *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateNoPeriod *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_sine = FlatVector::GetData<double>(*entries[0]);
        auto data_leadsine = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->values->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outSine(size), outLeadSine(size);

            TA_RetCode rc = TA_HT_SINE(0, size - 1, state->values->data(),
                                        &outBeg, &outNb,
                                        outSine.data(), outLeadSine.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                data_sine[ridx] = outSine[outNb - 1];
                data_leadsine[ridx] = outLeadSine[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateNoPeriod *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 5: Register both functions**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_ht_phasor: (DOUBLE) -> STRUCT(inphase, quadrature)
    {
        using OP = TalibAggHtPhasor;
        AggregateFunction func(
            "ta_ht_phasor",
            {LogicalType::DOUBLE},
            MakeStruct({{"inphase", LogicalType::DOUBLE}, {"quadrature", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }

    // ta_ht_sine: (DOUBLE) -> STRUCT(sine, leadsine)
    {
        using OP = TalibAggHtSine;
        AggregateFunction func(
            "ta_ht_sine",
            {LogicalType::DOUBLE},
            MakeStruct({{"sine", LogicalType::DOUBLE}, {"leadsine", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 6: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 7: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_ht_phasor and ta_ht_sine window functions"
```

---

### Task 5: Implement ta_mama (single series + 2 double params, 2 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add test**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_mama ----------

statement ok
CREATE TABLE mama_data AS
SELECT sin(unnest(range(1, 101))::DOUBLE * 0.1) * 50 + 100 AS price,
       unnest(range(1, 101)) AS rn;

# Basic: returns struct with mama/fama fields
query I
SELECT (ta_mama(price, 0.5, 0.05) OVER (ORDER BY rn)).mama IS NOT NULL
FROM mama_data
WHERE rn = 100;
----
true

query I
SELECT (ta_mama(price, 0.5, 0.05) OVER (ORDER BY rn)).fama IS NOT NULL
FROM mama_data
WHERE rn = 100;
----
true

statement ok
DROP TABLE mama_data;
```

- [ ] **Step 2: Add state struct and implement TalibAggMama**

Add state struct (after `MultiAggStateNoPeriod`):

```cpp
// ============================================================
// State: single series + two double params
// ============================================================
struct MultiAggStateMama {
    std::vector<double> *values;
    double fast_limit;
    double slow_limit;
};
```

Then add the operator:

```cpp
// ============================================================
// ta_mama: (DOUBLE, DOUBLE, DOUBLE) -> STRUCT(mama, fama)
// ============================================================
struct TalibAggMama {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateMama); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateMama *>(state);
        s->values = nullptr;
        s->fast_limit = 0.5;
        s->slow_limit = 0.05;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, fdata, sdata_fmt, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fdata);
        inputs[2].ToUnifiedFormat(count, sdata_fmt);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_limits = UnifiedVectorFormat::GetData<double>(fdata);
        auto slow_limits = UnifiedVectorFormat::GetData<double>(sdata_fmt);
        auto states = (MultiAggStateMama **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto fidx = fdata.sel->get_index(i);
            auto slidx = sdata_fmt.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->fast_limit = fast_limits[fidx];
            state->slow_limit = slow_limits[slidx];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<MultiAggStateMama *>(state_p);

        UnifiedVectorFormat vdata, fdata, sdata_fmt;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fdata);
        inputs[2].ToUnifiedFormat(count, sdata_fmt);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_limits = UnifiedVectorFormat::GetData<double>(fdata);
        auto slow_limits = UnifiedVectorFormat::GetData<double>(sdata_fmt);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            auto fidx = fdata.sel->get_index(i);
            auto slidx = sdata_fmt.sel->get_index(i);
            state->fast_limit = fast_limits[fidx];
            state->slow_limit = slow_limits[slidx];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<MultiAggStateMama *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateMama *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateMama *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_mama = FlatVector::GetData<double>(*entries[0]);
        auto data_fama = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
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
                rmask.SetInvalid(ridx);
            } else {
                data_mama[ridx] = outMAMA[outNb - 1];
                data_fama[ridx] = outFAMA[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateMama *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 3: Register ta_mama**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_mama: (DOUBLE, DOUBLE, DOUBLE) -> STRUCT(mama, fama)
    {
        using OP = TalibAggMama;
        AggregateFunction func(
            "ta_mama",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE},
            MakeStruct({{"mama", LogicalType::DOUBLE}, {"fama", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_mama window function returning STRUCT(mama, fama)"
```

---

### Task 6: Implement ta_macd (single series + 3 int params, 3 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add test**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_macd ----------

statement ok
CREATE TABLE macd_data AS
SELECT (100.0 + unnest(range(1, 51))::DOUBLE * 0.5 + sin(unnest(range(1, 51))::DOUBLE * 0.3) * 5) AS price,
       unnest(range(1, 51)) AS rn;

# Basic: returns struct with macd/signal/hist fields
query I
SELECT (ta_macd(price, 12, 26, 9) OVER (ORDER BY rn)).macd IS NOT NULL
FROM macd_data
WHERE rn = 50;
----
true

query I
SELECT (ta_macd(price, 12, 26, 9) OVER (ORDER BY rn)).signal IS NOT NULL
FROM macd_data
WHERE rn = 50;
----
true

query I
SELECT (ta_macd(price, 12, 26, 9) OVER (ORDER BY rn)).hist IS NOT NULL
FROM macd_data
WHERE rn = 50;
----
true

# NULL for insufficient data
query I
SELECT (ta_macd(price, 12, 26, 9) OVER (ORDER BY rn)).macd IS NULL
FROM macd_data
WHERE rn = 1;
----
true

statement ok
DROP TABLE macd_data;
```

- [ ] **Step 2: Add state struct and implement TalibAggMacd**

Add state struct:

```cpp
// ============================================================
// State: single series + three integer params (MACD)
// ============================================================
struct MultiAggStateMacd {
    std::vector<double> *values;
    int fast_period;
    int slow_period;
    int signal_period;
};
```

Then add the operator:

```cpp
// ============================================================
// ta_macd: (DOUBLE, INT, INT, INT) -> STRUCT(macd, signal, hist)
// ============================================================
struct TalibAggMacd {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateMacd); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateMacd *>(state);
        s->values = nullptr;
        s->fast_period = 12;
        s->slow_period = 26;
        s->signal_period = 9;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, fp, sp, sigp, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fp);
        inputs[2].ToUnifiedFormat(count, sp);
        inputs[3].ToUnifiedFormat(count, sigp);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_periods = UnifiedVectorFormat::GetData<int32_t>(fp);
        auto slow_periods = UnifiedVectorFormat::GetData<int32_t>(sp);
        auto signal_periods = UnifiedVectorFormat::GetData<int32_t>(sigp);
        auto states = (MultiAggStateMacd **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->fast_period = fast_periods[fp.sel->get_index(i)];
            state->slow_period = slow_periods[sp.sel->get_index(i)];
            state->signal_period = signal_periods[sigp.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<MultiAggStateMacd *>(state_p);

        UnifiedVectorFormat vdata, fp, sp, sigp;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fp);
        inputs[2].ToUnifiedFormat(count, sp);
        inputs[3].ToUnifiedFormat(count, sigp);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_periods = UnifiedVectorFormat::GetData<int32_t>(fp);
        auto slow_periods = UnifiedVectorFormat::GetData<int32_t>(sp);
        auto signal_periods = UnifiedVectorFormat::GetData<int32_t>(sigp);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            state->fast_period = fast_periods[fp.sel->get_index(i)];
            state->slow_period = slow_periods[sp.sel->get_index(i)];
            state->signal_period = signal_periods[sigp.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<MultiAggStateMacd *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateMacd *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateMacd *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_macd = FlatVector::GetData<double>(*entries[0]);
        auto data_signal = FlatVector::GetData<double>(*entries[1]);
        auto data_hist = FlatVector::GetData<double>(*entries[2]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
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
                rmask.SetInvalid(ridx);
            } else {
                data_macd[ridx] = outMACD[outNb - 1];
                data_signal[ridx] = outSignal[outNb - 1];
                data_hist[ridx] = outHist[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateMacd *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 3: Register ta_macd**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_macd: (DOUBLE, INT, INT, INT) -> STRUCT(macd, signal, hist)
    {
        using OP = TalibAggMacd;
        AggregateFunction func(
            "ta_macd",
            {LogicalType::DOUBLE, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
            MakeStruct({{"macd", LogicalType::DOUBLE}, {"signal", LogicalType::DOUBLE}, {"hist", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_macd window function returning STRUCT(macd, signal, hist)"
```

---

### Task 7: Implement ta_bbands (single series + 2 int + 2 double params, 3 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add test**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_bbands ----------

statement ok
CREATE TABLE bbands_data AS
SELECT (100.0 + sin(unnest(range(1, 31))::DOUBLE * 0.2) * 10) AS price,
       unnest(range(1, 31)) AS rn;

# Basic: returns struct with upper/middle/lower fields
query I
SELECT (ta_bbands(price, 20, 2.0, 2.0, 0) OVER (ORDER BY rn)).upper IS NOT NULL
FROM bbands_data
WHERE rn = 30;
----
true

query I
SELECT (ta_bbands(price, 20, 2.0, 2.0, 0) OVER (ORDER BY rn)).middle IS NOT NULL
FROM bbands_data
WHERE rn = 30;
----
true

query I
SELECT (ta_bbands(price, 20, 2.0, 2.0, 0) OVER (ORDER BY rn)).lower IS NOT NULL
FROM bbands_data
WHERE rn = 30;
----
true

# NULL for insufficient data
query I
SELECT (ta_bbands(price, 20, 2.0, 2.0, 0) OVER (ORDER BY rn)).upper IS NULL
FROM bbands_data
WHERE rn = 1;
----
true

statement ok
DROP TABLE bbands_data;
```

- [ ] **Step 2: Add state struct and implement TalibAggBbands**

Add state struct:

```cpp
// ============================================================
// State: single series + BBANDS params (int, double, double, int)
// ============================================================
struct MultiAggStateBbands {
    std::vector<double> *values;
    int time_period;
    double nb_dev_up;
    double nb_dev_dn;
    int ma_type;
};
```

Then add the operator:

```cpp
// ============================================================
// ta_bbands: (DOUBLE, INT, DOUBLE, DOUBLE, INT) -> STRUCT(upper, middle, lower)
// ============================================================
struct TalibAggBbands {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateBbands); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateBbands *>(state);
        s->values = nullptr;
        s->time_period = 5;
        s->nb_dev_up = 2.0;
        s->nb_dev_dn = 2.0;
        s->ma_type = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat vdata, tp, du, dd, mt, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, tp);
        inputs[2].ToUnifiedFormat(count, du);
        inputs[3].ToUnifiedFormat(count, dd);
        inputs[4].ToUnifiedFormat(count, mt);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto time_periods = UnifiedVectorFormat::GetData<int32_t>(tp);
        auto dev_ups = UnifiedVectorFormat::GetData<double>(du);
        auto dev_dns = UnifiedVectorFormat::GetData<double>(dd);
        auto ma_types = UnifiedVectorFormat::GetData<int32_t>(mt);
        auto states = (MultiAggStateBbands **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->values) {
                state->values = new std::vector<double>();
            }
            state->time_period = time_periods[tp.sel->get_index(i)];
            state->nb_dev_up = dev_ups[du.sel->get_index(i)];
            state->nb_dev_dn = dev_dns[dd.sel->get_index(i)];
            state->ma_type = ma_types[mt.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<MultiAggStateBbands *>(state_p);

        UnifiedVectorFormat vdata, tp, du, dd, mt;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, tp);
        inputs[2].ToUnifiedFormat(count, du);
        inputs[3].ToUnifiedFormat(count, dd);
        inputs[4].ToUnifiedFormat(count, mt);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto time_periods = UnifiedVectorFormat::GetData<int32_t>(tp);
        auto dev_ups = UnifiedVectorFormat::GetData<double>(du);
        auto dev_dns = UnifiedVectorFormat::GetData<double>(dd);
        auto ma_types = UnifiedVectorFormat::GetData<int32_t>(mt);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            state->time_period = time_periods[tp.sel->get_index(i)];
            state->nb_dev_up = dev_ups[du.sel->get_index(i)];
            state->nb_dev_dn = dev_dns[dd.sel->get_index(i)];
            state->ma_type = ma_types[mt.sel->get_index(i)];
            if (vdata.validity.RowIsValid(vidx)) {
                state->values->push_back(values[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<MultiAggStateBbands *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateBbands *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateBbands *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_upper = FlatVector::GetData<double>(*entries[0]);
        auto data_middle = FlatVector::GetData<double>(*entries[1]);
        auto data_lower = FlatVector::GetData<double>(*entries[2]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->values || state->values->empty()) {
                rmask.SetInvalid(ridx);
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
                rmask.SetInvalid(ridx);
            } else {
                data_upper[ridx] = outUpper[outNb - 1];
                data_middle[ridx] = outMiddle[outNb - 1];
                data_lower[ridx] = outLower[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateBbands *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->values;
            states[i]->values = nullptr;
        }
    }
};
```

- [ ] **Step 3: Register ta_bbands**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_bbands: (DOUBLE, INT, DOUBLE, DOUBLE, INT) -> STRUCT(upper, middle, lower)
    {
        using OP = TalibAggBbands;
        AggregateFunction func(
            "ta_bbands",
            {LogicalType::DOUBLE, LogicalType::INTEGER, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER},
            MakeStruct({{"upper", LogicalType::DOUBLE}, {"middle", LogicalType::DOUBLE}, {"lower", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_bbands window function returning STRUCT(upper, middle, lower)"
```

---

### Task 8: Implement ta_stoch (HLC + 5 params, 2 outputs)

**Files:**
- Modify: `src/talib_multi_output_agg.cpp`
- Modify: `test/sql/test_multi_output_agg.test`

- [ ] **Step 1: Add test**

Append to `test/sql/test_multi_output_agg.test`:

```
# ---------- ta_stoch ----------

statement ok
CREATE TABLE stoch_data AS
SELECT (100.0 + unnest(range(1, 31))::DOUBLE * 0.3 + sin(unnest(range(1, 31))::DOUBLE) * 3) AS high,
       (95.0 + unnest(range(1, 31))::DOUBLE * 0.3 - sin(unnest(range(1, 31))::DOUBLE) * 3) AS low,
       (97.5 + unnest(range(1, 31))::DOUBLE * 0.3 + cos(unnest(range(1, 31))::DOUBLE) * 2) AS close,
       unnest(range(1, 31)) AS rn;

# Basic: returns struct with slowk/slowd fields
query I
SELECT (ta_stoch(high, low, close, 5, 3, 0, 3, 0) OVER (ORDER BY rn)).slowk IS NOT NULL
FROM stoch_data
WHERE rn = 30;
----
true

query I
SELECT (ta_stoch(high, low, close, 5, 3, 0, 3, 0) OVER (ORDER BY rn)).slowd IS NOT NULL
FROM stoch_data
WHERE rn = 30;
----
true

# NULL for insufficient data
query I
SELECT (ta_stoch(high, low, close, 5, 3, 0, 3, 0) OVER (ORDER BY rn)).slowk IS NULL
FROM stoch_data
WHERE rn = 1;
----
true

statement ok
DROP TABLE stoch_data;
```

- [ ] **Step 2: Add state struct and implement TalibAggStoch**

Add state struct:

```cpp
// ============================================================
// State: HLC + stochastic params
// ============================================================
struct MultiAggStateStoch {
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

Then add the operator:

```cpp
// ============================================================
// ta_stoch: (DOUBLE x3, INT x5) -> STRUCT(slowk, slowd)
// ============================================================
struct TalibAggStoch {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(MultiAggStateStoch); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<MultiAggStateStoch *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
        s->fastk_period = 5;
        s->slowk_period = 3;
        s->slowk_matype = 0;
        s->slowd_period = 3;
        s->slowd_matype = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, cdata, fk, sk, skm, sd, sdm, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, fk);
        inputs[4].ToUnifiedFormat(count, sk);
        inputs[5].ToUnifiedFormat(count, skm);
        inputs[6].ToUnifiedFormat(count, sd);
        inputs[7].ToUnifiedFormat(count, sdm);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto fk_vals = UnifiedVectorFormat::GetData<int32_t>(fk);
        auto sk_vals = UnifiedVectorFormat::GetData<int32_t>(sk);
        auto skm_vals = UnifiedVectorFormat::GetData<int32_t>(skm);
        auto sd_vals = UnifiedVectorFormat::GetData<int32_t>(sd);
        auto sdm_vals = UnifiedVectorFormat::GetData<int32_t>(sdm);
        auto states = (MultiAggStateStoch **)sdata.data;

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
            state->fastk_period = fk_vals[fk.sel->get_index(i)];
            state->slowk_period = sk_vals[sk.sel->get_index(i)];
            state->slowk_matype = skm_vals[skm.sel->get_index(i)];
            state->slowd_period = sd_vals[sd.sel->get_index(i)];
            state->slowd_matype = sdm_vals[sdm.sel->get_index(i)];

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<MultiAggStateStoch *>(state_p);

        UnifiedVectorFormat hdata, ldata, cdata, fk, sk, skm, sd, sdm;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, fk);
        inputs[4].ToUnifiedFormat(count, sk);
        inputs[5].ToUnifiedFormat(count, skm);
        inputs[6].ToUnifiedFormat(count, sd);
        inputs[7].ToUnifiedFormat(count, sdm);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto fk_vals = UnifiedVectorFormat::GetData<int32_t>(fk);
        auto sk_vals = UnifiedVectorFormat::GetData<int32_t>(sk);
        auto skm_vals = UnifiedVectorFormat::GetData<int32_t>(skm);
        auto sd_vals = UnifiedVectorFormat::GetData<int32_t>(sd);
        auto sdm_vals = UnifiedVectorFormat::GetData<int32_t>(sdm);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            state->fastk_period = fk_vals[fk.sel->get_index(i)];
            state->slowk_period = sk_vals[sk.sel->get_index(i)];
            state->slowk_matype = skm_vals[skm.sel->get_index(i)];
            state->slowd_period = sd_vals[sd.sel->get_index(i)];
            state->slowd_matype = sdm_vals[sdm.sel->get_index(i)];

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<MultiAggStateStoch *>(source);
        auto tgt = FlatVector::GetData<MultiAggStateStoch *>(target);
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
        auto states = FlatVector::GetData<MultiAggStateStoch *>(states_vec);
        auto &entries = StructVector::GetEntries(result);
        auto data_slowk = FlatVector::GetData<double>(*entries[0]);
        auto data_slowd = FlatVector::GetData<double>(*entries[1]);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->high || state->high->empty()) {
                rmask.SetInvalid(ridx);
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
                rmask.SetInvalid(ridx);
            } else {
                data_slowk[ridx] = outSlowK[outNb - 1];
                data_slowd[ridx] = outSlowD[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<MultiAggStateStoch *>(states_vec);
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

- [ ] **Step 3: Register ta_stoch**

In `RegisterTalibMultiOutputAggFunctions`, add:

```cpp
    // ta_stoch: (DOUBLE x3, INT x5) -> STRUCT(slowk, slowd)
    {
        using OP = TalibAggStoch;
        AggregateFunction func(
            "ta_stoch",
            {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE,
             LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER,
             LogicalType::INTEGER, LogicalType::INTEGER},
            MakeStruct({{"slowk", LogicalType::DOUBLE}, {"slowd", LogicalType::DOUBLE}}),
            OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize,
            FunctionNullHandling::DEFAULT_NULL_HANDLING,
            OP::SimpleUpdate, nullptr, OP::Destroy);
        loader.RegisterFunction(func);
    }
```

- [ ] **Step 4: Build and run tests**

Run: `cd /Users/qunfei/Projects/atm_talib && make && make test`

Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/talib_multi_output_agg.cpp test/sql/test_multi_output_agg.test
git commit -m "feat: add ta_stoch window function returning STRUCT(slowk, slowd)"
```

---

### Task 9: Update documentation

**Files:**
- Modify: `index.md`

- [ ] **Step 1: Update the multi-output section in index.md**

In the Function Index table (lines 149-157), update the multi-output entries to show both forms:

```markdown
| | **Multi-Output** (scalar returns `LIST<STRUCT>`, window returns `STRUCT`) | | | | |
| 119 | Multi | `t_macd` / `ta_macd` | MACD | (values, fast, slow, signal) | STRUCT(macd, signal, hist) |
| 120 | Multi | `t_bbands` / `ta_bbands` | Bollinger Bands | (values, period, devup, devdn, matype) | STRUCT(upper, middle, lower) |
| 121 | Multi | `t_stoch` / `ta_stoch` | Stochastic | (high, low, close, fastK, slowK, slowKMA, slowD, slowDMA) | STRUCT(slowk, slowd) |
| 122 | Multi | `t_aroon` / `ta_aroon` | Aroon | (high, low, period) | STRUCT(aroon_down, aroon_up) |
| 123 | Multi | `t_minmax` / `ta_minmax` | Min/Max over period | (values, period) | STRUCT(min, max) |
| 124 | Multi | `t_mama` / `ta_mama` | MESA Adaptive Moving Average | (values, fastlimit, slowlimit) | STRUCT(mama, fama) |
| 125 | Multi | `t_ht_phasor` / `ta_ht_phasor` | Hilbert Transform — Phasor | (values) | STRUCT(inphase, quadrature) |
| 126 | Multi | `t_ht_sine` / `ta_ht_sine` | Hilbert Transform — SineWave | (values) | STRUCT(sine, leadsine) |
```

- [ ] **Step 2: Add window form documentation in the Multi-Output section (section 10)**

After the existing scalar examples, add a window usage section:

```markdown
### Window form — row-by-row with OVER()

Multi-output window functions return a `STRUCT` per row. Access fields with dot notation:

```sql
-- MACD with dot-notation field access
SELECT
    symbol, ts, close,
    (ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts)).macd,
    (ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts)).signal,
    (ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts)).hist
FROM prices;

-- Or use a subquery to avoid repeating the window expression:
SELECT symbol, ts, close, m.macd, m.signal, m.hist
FROM (
    SELECT *, ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts) AS m
    FROM prices
);

-- Bollinger Bands
SELECT ts,
    (ta_bbands(close, 20, 2.0, 2.0, 0) OVER (ORDER BY ts)).upper,
    (ta_bbands(close, 20, 2.0, 2.0, 0) OVER (ORDER BY ts)).middle,
    (ta_bbands(close, 20, 2.0, 2.0, 0) OVER (ORDER BY ts)).lower
FROM prices;
```
```

- [ ] **Step 3: Commit**

```bash
git add index.md
git commit -m "docs: add ta_ window form documentation for multi-output functions"
```
