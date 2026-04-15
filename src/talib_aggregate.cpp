#include "duckdb.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/function/function_set.hpp"
#include "duckdb/main/extension/extension_loader.hpp"
#include "duckdb/common/vector/flat_vector.hpp"

extern "C" {
#include "ta_libc.h"
}

#include <vector>
#include <cstring>
#include <new>

namespace duckdb {

// ============================================================
// State structs for each pattern
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
// Generic callbacks for each pattern
// ============================================================

// --- P1 DOUBLE ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int, int*, int*, double[])>
struct TalibAggP1Double {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState1); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState1 *>(state);
        s->values = nullptr;
        s->period = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        // inputs[0] = DOUBLE value, inputs[1] = INTEGER period
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
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->values->data(), state->period, &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
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

// --- P1 INT ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int, int*, int*, int[])>
struct TalibAggP1Int {
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
        auto rdata = FlatVector::GetDataMutable<int32_t>(result);
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
            std::vector<int> outInt(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->values->data(), state->period, &outBeg, &outNb, outInt.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outInt[outNb - 1];
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

// --- P2 DOUBLE ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int*, int*, double[])>
struct TalibAggP2Double {
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
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->values->data(), &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
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

// --- P2 INT ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int*, int*, int[])>
struct TalibAggP2Int {
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
        auto rdata = FlatVector::GetDataMutable<int32_t>(result);
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
            std::vector<int> outInt(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->values->data(), &outBeg, &outNb, outInt.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outInt[outNb - 1];
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

// --- P3 DOUBLE: (high, low, close, period) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], int, int*, int*, double[])>
struct TalibAggP3 {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState3); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState3 *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
        s->period = 0;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, cdata, pdata, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, pdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);
        auto states = (TalibAggState3 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->high) {
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
                state->close = new std::vector<double>();
            }
            state->period = periods[pidx];

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState3 *>(state_p);

        UnifiedVectorFormat hdata, ldata, cdata, pdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, pdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(pdata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto pidx = pdata.sel->get_index(i);
            state->period = periods[pidx];
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState3 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState3 *>(target);
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
                tgt[i]->period = src[i]->period;
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState3 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->high->data(), state->low->data(), state->close->data(),
                                    state->period, &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState3 *>(states_vec);
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

// --- P4 DOUBLE: (high, low, close, volume) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, double[])>
struct TalibAggP4 {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState4); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState4 *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
        s->volume = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, cdata, vdata, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, vdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto vvals = UnifiedVectorFormat::GetData<double>(vdata);
        auto states = (TalibAggState4 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->high) {
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
                state->close = new std::vector<double>();
                state->volume = new std::vector<double>();
            }

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) &&
                cdata.validity.RowIsValid(cidx) && vdata.validity.RowIsValid(vidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
                state->volume->push_back(vvals[vidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState4 *>(state_p);

        UnifiedVectorFormat hdata, ldata, cdata, vdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        inputs[3].ToUnifiedFormat(count, vdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto vvals = UnifiedVectorFormat::GetData<double>(vdata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
            state->volume = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) &&
                cdata.validity.RowIsValid(cidx) && vdata.validity.RowIsValid(vidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
                state->volume->push_back(vvals[vidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState4 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState4 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->high && !src[i]->high->empty()) {
                if (!tgt[i]->high) {
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                    tgt[i]->close = new std::vector<double>();
                    tgt[i]->volume = new std::vector<double>();
                }
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
                tgt[i]->close->insert(tgt[i]->close->end(), src[i]->close->begin(), src[i]->close->end());
                tgt[i]->volume->insert(tgt[i]->volume->end(), src[i]->volume->begin(), src[i]->volume->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState4 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->high->data(), state->low->data(), state->close->data(),
                                    state->volume->data(), &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState4 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->high;
            delete states[i]->low;
            delete states[i]->close;
            delete states[i]->volume;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
            states[i]->close = nullptr;
            states[i]->volume = nullptr;
        }
    }
};

// --- P5 DOUBLE: (open, high, low, close) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, double[])>
struct TalibAggP5Double {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState5); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState5 *>(state);
        s->open_ = nullptr;
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat odata, hdata, ldata, cdata, sdata;
        inputs[0].ToUnifiedFormat(count, odata);
        inputs[1].ToUnifiedFormat(count, hdata);
        inputs[2].ToUnifiedFormat(count, ldata);
        inputs[3].ToUnifiedFormat(count, cdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto ovals = UnifiedVectorFormat::GetData<double>(odata);
        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto states = (TalibAggState5 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto oidx = odata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->open_) {
                state->open_ = new std::vector<double>();
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
                state->close = new std::vector<double>();
            }

            if (odata.validity.RowIsValid(oidx) && hdata.validity.RowIsValid(hidx) &&
                ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->open_->push_back(ovals[oidx]);
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState5 *>(state_p);

        UnifiedVectorFormat odata, hdata, ldata, cdata;
        inputs[0].ToUnifiedFormat(count, odata);
        inputs[1].ToUnifiedFormat(count, hdata);
        inputs[2].ToUnifiedFormat(count, ldata);
        inputs[3].ToUnifiedFormat(count, cdata);

        auto ovals = UnifiedVectorFormat::GetData<double>(odata);
        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);

        if (!state->open_) {
            state->open_ = new std::vector<double>();
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto oidx = odata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            if (odata.validity.RowIsValid(oidx) && hdata.validity.RowIsValid(hidx) &&
                ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->open_->push_back(ovals[oidx]);
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState5 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState5 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->open_ && !src[i]->open_->empty()) {
                if (!tgt[i]->open_) {
                    tgt[i]->open_ = new std::vector<double>();
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                    tgt[i]->close = new std::vector<double>();
                }
                tgt[i]->open_->insert(tgt[i]->open_->end(), src[i]->open_->begin(), src[i]->open_->end());
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
                tgt[i]->close->insert(tgt[i]->close->end(), src[i]->close->begin(), src[i]->close->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState5 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<double>(result);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->open_ || state->open_->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->open_->size();
            int outBeg = 0, outNb = 0;
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->open_->data(), state->high->data(),
                                    state->low->data(), state->close->data(), &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState5 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->open_;
            delete states[i]->high;
            delete states[i]->low;
            delete states[i]->close;
            states[i]->open_ = nullptr;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
            states[i]->close = nullptr;
        }
    }
};

// --- P5 INT: (open, high, low, close) -> int ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, int[])>
struct TalibAggP5Int {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState5); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState5 *>(state);
        s->open_ = nullptr;
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat odata, hdata, ldata, cdata, sdata;
        inputs[0].ToUnifiedFormat(count, odata);
        inputs[1].ToUnifiedFormat(count, hdata);
        inputs[2].ToUnifiedFormat(count, ldata);
        inputs[3].ToUnifiedFormat(count, cdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto ovals = UnifiedVectorFormat::GetData<double>(odata);
        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto states = (TalibAggState5 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto oidx = odata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->open_) {
                state->open_ = new std::vector<double>();
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
                state->close = new std::vector<double>();
            }

            if (odata.validity.RowIsValid(oidx) && hdata.validity.RowIsValid(hidx) &&
                ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->open_->push_back(ovals[oidx]);
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState5 *>(state_p);

        UnifiedVectorFormat odata, hdata, ldata, cdata;
        inputs[0].ToUnifiedFormat(count, odata);
        inputs[1].ToUnifiedFormat(count, hdata);
        inputs[2].ToUnifiedFormat(count, ldata);
        inputs[3].ToUnifiedFormat(count, cdata);

        auto ovals = UnifiedVectorFormat::GetData<double>(odata);
        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);

        if (!state->open_) {
            state->open_ = new std::vector<double>();
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto oidx = odata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            if (odata.validity.RowIsValid(oidx) && hdata.validity.RowIsValid(hidx) &&
                ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->open_->push_back(ovals[oidx]);
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState5 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState5 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->open_ && !src[i]->open_->empty()) {
                if (!tgt[i]->open_) {
                    tgt[i]->open_ = new std::vector<double>();
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                    tgt[i]->close = new std::vector<double>();
                }
                tgt[i]->open_->insert(tgt[i]->open_->end(), src[i]->open_->begin(), src[i]->open_->end());
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
                tgt[i]->close->insert(tgt[i]->close->end(), src[i]->close->begin(), src[i]->close->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState5 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<int32_t>(result);
        auto &rmask = FlatVector::Validity(result);

        for (idx_t i = 0; i < count; i++) {
            auto state = states[i];
            idx_t ridx = i + offset;

            if (!state->open_ || state->open_->empty()) {
                rmask.SetInvalid(ridx);
                continue;
            }

            int size = (int)state->open_->size();
            int outBeg = 0, outNb = 0;
            std::vector<int> outInt(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->open_->data(), state->high->data(),
                                    state->low->data(), state->close->data(), &outBeg, &outNb, outInt.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outInt[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState5 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->open_;
            delete states[i]->high;
            delete states[i]->low;
            delete states[i]->close;
            states[i]->open_ = nullptr;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
            states[i]->close = nullptr;
        }
    }
};

// --- P6 DOUBLE: (high, low) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], int*, int*, double[])>
struct TalibAggP6 {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState6); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState6 *>(state);
        s->high = nullptr;
        s->low = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto states = (TalibAggState6 **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto state = states[sidx];

            if (!state->high) {
                state->high = new std::vector<double>();
                state->low = new std::vector<double>();
            }

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState6 *>(state_p);

        UnifiedVectorFormat hdata, ldata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState6 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState6 *>(target);
        for (idx_t i = 0; i < count; i++) {
            if (src[i]->high && !src[i]->high->empty()) {
                if (!tgt[i]->high) {
                    tgt[i]->high = new std::vector<double>();
                    tgt[i]->low = new std::vector<double>();
                }
                tgt[i]->high->insert(tgt[i]->high->end(), src[i]->high->begin(), src[i]->high->end());
                tgt[i]->low->insert(tgt[i]->low->end(), src[i]->low->begin(), src[i]->low->end());
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState6 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->high->data(), state->low->data(), &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState6 *>(states_vec);
        for (idx_t i = 0; i < count; i++) {
            delete states[i]->high;
            delete states[i]->low;
            states[i]->high = nullptr;
            states[i]->low = nullptr;
        }
    }
};

// --- P7 DOUBLE: (high, low, close) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], int*, int*, double[])>
struct TalibAggP7 {
    static idx_t StateSize(const AggregateFunction &) { return sizeof(TalibAggState7); }

    static void Initialize(const AggregateFunction &, data_ptr_t state) {
        auto s = reinterpret_cast<TalibAggState7 *>(state);
        s->high = nullptr;
        s->low = nullptr;
        s->close = nullptr;
    }

    static void Update(Vector inputs[], AggregateInputData &, idx_t input_count, Vector &states_vec, idx_t count) {
        UnifiedVectorFormat hdata, ldata, cdata, sdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);
        auto states = (TalibAggState7 **)sdata.data;

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

            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void SimpleUpdate(Vector inputs[], AggregateInputData &, idx_t input_count, data_ptr_t state_p, idx_t count) {
        auto state = reinterpret_cast<TalibAggState7 *>(state_p);

        UnifiedVectorFormat hdata, ldata, cdata;
        inputs[0].ToUnifiedFormat(count, hdata);
        inputs[1].ToUnifiedFormat(count, ldata);
        inputs[2].ToUnifiedFormat(count, cdata);

        auto hvals = UnifiedVectorFormat::GetData<double>(hdata);
        auto lvals = UnifiedVectorFormat::GetData<double>(ldata);
        auto cvals = UnifiedVectorFormat::GetData<double>(cdata);

        if (!state->high) {
            state->high = new std::vector<double>();
            state->low = new std::vector<double>();
            state->close = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto hidx = hdata.sel->get_index(i);
            auto lidx = ldata.sel->get_index(i);
            auto cidx = cdata.sel->get_index(i);
            if (hdata.validity.RowIsValid(hidx) && ldata.validity.RowIsValid(lidx) && cdata.validity.RowIsValid(cidx)) {
                state->high->push_back(hvals[hidx]);
                state->low->push_back(lvals[lidx]);
                state->close->push_back(cvals[cidx]);
            }
        }
    }

    static void Combine(Vector &source, Vector &target, AggregateInputData &, idx_t count) {
        auto src = FlatVector::GetData<TalibAggState7 *>(source);
        auto tgt = FlatVector::GetData<TalibAggState7 *>(target);
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
            }
        }
    }

    static void Finalize(Vector &states_vec, AggregateInputData &, Vector &result, idx_t count, idx_t offset) {
        auto states = FlatVector::GetData<TalibAggState7 *>(states_vec);
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->high->data(), state->low->data(), state->close->data(),
                                    &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
            }
        }
    }

    static void Destroy(Vector &states_vec, AggregateInputData &, idx_t count) {
        auto states = FlatVector::GetData<TalibAggState7 *>(states_vec);
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

// --- P8 DOUBLE: (high, low, period) ---
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], int, int*, int*, double[])>
struct TalibAggP8 {
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
        auto rdata = FlatVector::GetDataMutable<double>(result);
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
            std::vector<double> outReal(size);

            TA_RetCode rc = TA_FUNC(0, size - 1, state->high->data(), state->low->data(),
                                    state->period, &outBeg, &outNb, outReal.data());

            if (rc != TA_SUCCESS || outNb == 0) {
                rmask.SetInvalid(ridx);
            } else {
                rdata[ridx] = outReal[outNb - 1];
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
// Registration
// ============================================================

void RegisterTalibAggregateFunctions(ExtensionLoader &loader) {

    // --- P1 DOUBLE: (DOUBLE, INTEGER) -> DOUBLE ---
    #define TALIB_AGG_P1_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP1Double<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::INTEGER}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P1 INT: (DOUBLE, INTEGER) -> INTEGER ---
    #define TALIB_AGG_P1_INT(sql_name, ta_func) \
        { \
            using OP = TalibAggP1Int<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::INTEGER}, \
                LogicalType::INTEGER, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P2 DOUBLE: (DOUBLE) -> DOUBLE ---
    #define TALIB_AGG_P2_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP2Double<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P2 INT: (DOUBLE) -> INTEGER ---
    #define TALIB_AGG_P2_INT(sql_name, ta_func) \
        { \
            using OP = TalibAggP2Int<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE}, \
                LogicalType::INTEGER, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P3 DOUBLE: (DOUBLE x3, INTEGER) -> DOUBLE ---
    #define TALIB_AGG_P3_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP3<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P4 DOUBLE: (DOUBLE x4) -> DOUBLE ---
    #define TALIB_AGG_P4_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP4<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P5 DOUBLE: (DOUBLE x4) -> DOUBLE ---
    #define TALIB_AGG_P5_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP5Double<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P5 INT: (DOUBLE x4) -> INTEGER ---
    #define TALIB_AGG_P5_INT(sql_name, ta_func) \
        { \
            using OP = TalibAggP5Int<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE}, \
                LogicalType::INTEGER, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P6 DOUBLE: (DOUBLE x2) -> DOUBLE ---
    #define TALIB_AGG_P6_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP6<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P7 DOUBLE: (DOUBLE x3) -> DOUBLE ---
    #define TALIB_AGG_P7_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP7<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // --- P8 DOUBLE: (DOUBLE x2, INTEGER) -> DOUBLE ---
    #define TALIB_AGG_P8_DOUBLE(sql_name, ta_func) \
        { \
            using OP = TalibAggP8<ta_func>; \
            AggregateFunction func( \
                "taw_" #sql_name, \
                {LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER}, \
                LogicalType::DOUBLE, \
                OP::StateSize, OP::Initialize, OP::Update, OP::Combine, OP::Finalize, \
                FunctionNullHandling::DEFAULT_NULL_HANDLING, \
                OP::SimpleUpdate, nullptr, OP::Destroy); \
            loader.RegisterFunction(func); \
        }

    // Dispatch macro
    #define TALIB_FUNC(sql_name, ta_func, ta_lookback, pattern, ret_type) \
        TALIB_AGG_##pattern##_##ret_type(sql_name, ta_func)

    #include "talib_functions.hpp"

    #undef TALIB_FUNC
    #undef TALIB_AGG_P1_DOUBLE
    #undef TALIB_AGG_P1_INT
    #undef TALIB_AGG_P2_DOUBLE
    #undef TALIB_AGG_P2_INT
    #undef TALIB_AGG_P3_DOUBLE
    #undef TALIB_AGG_P4_DOUBLE
    #undef TALIB_AGG_P5_DOUBLE
    #undef TALIB_AGG_P5_INT
    #undef TALIB_AGG_P6_DOUBLE
    #undef TALIB_AGG_P7_DOUBLE
    #undef TALIB_AGG_P8_DOUBLE
}

} // namespace duckdb
