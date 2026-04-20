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
// State structs
// ============================================================

// Single series + one integer period
struct MultiAggState1 {
    std::vector<double> *values;
    int period;
};

// Two series (HL) + one integer period
struct MultiAggStateHL {
    std::vector<double> *high;
    std::vector<double> *low;
    int period;
};

// Single series, no period
struct MultiAggStateNoPeriod {
    std::vector<double> *values;
};

// Single series + two double params (MAMA)
struct MultiAggStateMama {
    std::vector<double> *values;
    double fast_limit;
    double slow_limit;
};

// Single series + three integer params (MACD)
struct MultiAggStateMacd {
    std::vector<double> *values;
    int fast_period;
    int slow_period;
    int signal_period;
};

// Single series + BBANDS params
struct MultiAggStateBbands {
    std::vector<double> *values;
    int time_period;
    double nb_dev_up;
    double nb_dev_dn;
    int ma_type;
};

// HLC + stochastic params
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
        UnifiedVectorFormat vdata, fdata, sldata, sdata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fdata);
        inputs[2].ToUnifiedFormat(count, sldata);
        states_vec.ToUnifiedFormat(count, sdata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_limits = UnifiedVectorFormat::GetData<double>(fdata);
        auto slow_limits = UnifiedVectorFormat::GetData<double>(sldata);
        auto states = (MultiAggStateMama **)sdata.data;

        for (idx_t i = 0; i < count; i++) {
            auto sidx = sdata.sel->get_index(i);
            auto vidx = vdata.sel->get_index(i);
            auto fidx = fdata.sel->get_index(i);
            auto slidx = sldata.sel->get_index(i);
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

        UnifiedVectorFormat vdata, fdata, sldata;
        inputs[0].ToUnifiedFormat(count, vdata);
        inputs[1].ToUnifiedFormat(count, fdata);
        inputs[2].ToUnifiedFormat(count, sldata);

        auto values = UnifiedVectorFormat::GetData<double>(vdata);
        auto fast_limits = UnifiedVectorFormat::GetData<double>(fdata);
        auto slow_limits = UnifiedVectorFormat::GetData<double>(sldata);

        if (!state->values) {
            state->values = new std::vector<double>();
        }

        for (idx_t i = 0; i < count; i++) {
            auto vidx = vdata.sel->get_index(i);
            auto fidx = fdata.sel->get_index(i);
            auto slidx = sldata.sel->get_index(i);
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

// ============================================================
// Registration
// ============================================================
void RegisterTalibMultiOutputAggFunctions(ExtensionLoader &loader) {

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
}

} // namespace duckdb
