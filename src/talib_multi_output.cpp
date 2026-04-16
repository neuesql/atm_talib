#include "talib_adapter.hpp"
// list_vector included via duckdb.hpp

#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension/extension_loader.hpp"

namespace duckdb {

// ============================================================
// Helper: extract scalar parameter values
// ============================================================
static inline int32_t GetIntParam(DataChunk &args, idx_t col, idx_t row) {
    UnifiedVectorFormat vdata;
    args.data[col].ToUnifiedFormat(args.size(), vdata);
    auto data = UnifiedVectorFormat::GetData<int32_t>(vdata);
    return data[vdata.sel->get_index(row)];
}

static inline double GetDoubleParam(DataChunk &args, idx_t col, idx_t row) {
    UnifiedVectorFormat vdata;
    args.data[col].ToUnifiedFormat(args.size(), vdata);
    auto data = UnifiedVectorFormat::GetData<double>(vdata);
    return data[vdata.sel->get_index(row)];
}

static inline list_entry_t GetListEntry(DataChunk &args, idx_t col, idx_t row) {
    auto &vec = args.data[col];
    UnifiedVectorFormat vdata;
    vec.ToUnifiedFormat(args.size(), vdata);
    auto list_data = UnifiedVectorFormat::GetData<list_entry_t>(vdata);
    auto idx = vdata.sel->get_index(row);
    return list_data[idx];
}

static inline Vector &GetListChild(DataChunk &args, idx_t col) {
    return ListVector::GetEntry(args.data[col]);
}

// ============================================================
// Helper: pack multi-output results into LIST<STRUCT>
// Each output array has the same outBeg/outNb; we build
// a list of structs where lookback positions have NULL fields.
// ============================================================

// Pack 2-output result
static void PackStruct2Result(Vector &result, idx_t idx, int input_size,
                              int out_beg, int out_nb,
                              const double *out1, const double *out2,
                              const string &name1, const string &name2) {
    // Set list entry
    auto list_data = FlatVector::GetData<list_entry_t>(result);
    auto offset = ListVector::GetListSize(result);
    list_data[idx].offset = offset;
    list_data[idx].length = input_size;

    ListVector::Reserve(result, offset + input_size);
    ListVector::SetListSize(result, offset + input_size);

    // Get the struct child vector (the entry of the LIST)
    auto &struct_vec = ListVector::GetEntry(result);
    auto &entries = StructVector::GetEntries(struct_vec);

    // entries[0] = first field, entries[1] = second field
    auto data0 = FlatVector::GetData<double>(*entries[0]);
    auto data1 = FlatVector::GetData<double>(*entries[1]);
    auto &validity0 = FlatVector::Validity(*entries[0]);
    auto &validity1 = FlatVector::Validity(*entries[1]);

    // Fill lookback NULLs
    for (int i = 0; i < out_beg; i++) {
        validity0.SetInvalid(offset + i);
        validity1.SetInvalid(offset + i);
    }
    // Fill values
    for (int i = 0; i < out_nb; i++) {
        data0[offset + out_beg + i] = out1[i];
        data1[offset + out_beg + i] = out2[i];
    }
    // Fill trailing NULLs
    for (int i = out_beg + out_nb; i < input_size; i++) {
        validity0.SetInvalid(offset + i);
        validity1.SetInvalid(offset + i);
    }
}

// Pack 3-output result
static void PackStruct3Result(Vector &result, idx_t idx, int input_size,
                              int out_beg, int out_nb,
                              const double *out1, const double *out2, const double *out3) {
    auto list_data = FlatVector::GetData<list_entry_t>(result);
    auto offset = ListVector::GetListSize(result);
    list_data[idx].offset = offset;
    list_data[idx].length = input_size;

    ListVector::Reserve(result, offset + input_size);
    ListVector::SetListSize(result, offset + input_size);

    auto &struct_vec = ListVector::GetEntry(result);
    auto &entries = StructVector::GetEntries(struct_vec);

    auto data0 = FlatVector::GetData<double>(*entries[0]);
    auto data1 = FlatVector::GetData<double>(*entries[1]);
    auto data2 = FlatVector::GetData<double>(*entries[2]);
    auto &validity0 = FlatVector::Validity(*entries[0]);
    auto &validity1 = FlatVector::Validity(*entries[1]);
    auto &validity2 = FlatVector::Validity(*entries[2]);

    for (int i = 0; i < out_beg; i++) {
        validity0.SetInvalid(offset + i);
        validity1.SetInvalid(offset + i);
        validity2.SetInvalid(offset + i);
    }
    for (int i = 0; i < out_nb; i++) {
        data0[offset + out_beg + i] = out1[i];
        data1[offset + out_beg + i] = out2[i];
        data2[offset + out_beg + i] = out3[i];
    }
    for (int i = out_beg + out_nb; i < input_size; i++) {
        validity0.SetInvalid(offset + i);
        validity1.SetInvalid(offset + i);
        validity2.SetInvalid(offset + i);
    }
}

// ============================================================
// MACD: (LIST<DOUBLE>, INT, INT, INT) -> LIST<STRUCT(macd, signal, hist)>
// ============================================================
static void TalibMacdScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int fast_period = GetIntParam(args, 1, i);
        int slow_period = GetIntParam(args, 2, i);
        int signal_period = GetIntParam(args, 3, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outMACD(size), outSignal(size), outHist(size);

        TA_RetCode rc = TA_MACD(0, size - 1, input.data(),
                                fast_period, slow_period, signal_period,
                                &outBeg, &outNb,
                                outMACD.data(), outSignal.data(), outHist.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct3Result(result, i, size, outBeg, outNb,
                          outMACD.data(), outSignal.data(), outHist.data());
    }
}

// ============================================================
// BBANDS: (LIST<DOUBLE>, INT, DOUBLE, DOUBLE, INT) -> LIST<STRUCT(upper, middle, lower)>
// ============================================================
static void TalibBbandsScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int time_period = GetIntParam(args, 1, i);
        double nb_dev_up = GetDoubleParam(args, 2, i);
        double nb_dev_dn = GetDoubleParam(args, 3, i);
        int ma_type = GetIntParam(args, 4, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outUpper(size), outMiddle(size), outLower(size);

        TA_RetCode rc = TA_BBANDS(0, size - 1, input.data(),
                                  time_period, nb_dev_up, nb_dev_dn, (TA_MAType)ma_type,
                                  &outBeg, &outNb,
                                  outUpper.data(), outMiddle.data(), outLower.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct3Result(result, i, size, outBeg, outNb,
                          outUpper.data(), outMiddle.data(), outLower.data());
    }
}

// ============================================================
// STOCH: (LIST<DOUBLE> x3, INT, INT, INT, INT, INT) -> LIST<STRUCT(slowk, slowd)>
// ============================================================
static void TalibStochScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto list_c = GetListEntry(args, 2, i);
        auto high  = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low   = ListToDoubleArray(list_l, GetListChild(args, 1));
        auto close = ListToDoubleArray(list_c, GetListChild(args, 2));
        int size = (int)high.size();

        int fastk_period = GetIntParam(args, 3, i);
        int slowk_period = GetIntParam(args, 4, i);
        int slowk_matype = GetIntParam(args, 5, i);
        int slowd_period = GetIntParam(args, 6, i);
        int slowd_matype = GetIntParam(args, 7, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outSlowK(size), outSlowD(size);

        TA_RetCode rc = TA_STOCH(0, size - 1, high.data(), low.data(), close.data(),
                                 fastk_period, slowk_period, (TA_MAType)slowk_matype,
                                 slowd_period, (TA_MAType)slowd_matype,
                                 &outBeg, &outNb,
                                 outSlowK.data(), outSlowD.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outSlowK.data(), outSlowD.data(), "slowk", "slowd");
    }
}

// ============================================================
// AROON: (LIST<DOUBLE> x2, INT) -> LIST<STRUCT(aroon_down, aroon_up)>
// ============================================================
static void TalibAroonScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto high = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low  = ListToDoubleArray(list_l, GetListChild(args, 1));
        int size = (int)high.size();

        int time_period = GetIntParam(args, 2, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outDown(size), outUp(size);

        TA_RetCode rc = TA_AROON(0, size - 1, high.data(), low.data(),
                                 time_period,
                                 &outBeg, &outNb,
                                 outDown.data(), outUp.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outDown.data(), outUp.data(), "aroon_down", "aroon_up");
    }
}

// ============================================================
// MINMAX: (LIST<DOUBLE>, INT) -> LIST<STRUCT(min, max)>
// ============================================================
static void TalibMinMaxScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int time_period = GetIntParam(args, 1, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outMin(size), outMax(size);

        TA_RetCode rc = TA_MINMAX(0, size - 1, input.data(),
                                  time_period,
                                  &outBeg, &outNb,
                                  outMin.data(), outMax.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outMin.data(), outMax.data(), "min", "max");
    }
}

// ============================================================
// MAMA: (LIST<DOUBLE>, DOUBLE, DOUBLE) -> LIST<STRUCT(mama, fama)>
// ============================================================
static void TalibMamaScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        double fast_limit = GetDoubleParam(args, 1, i);
        double slow_limit = GetDoubleParam(args, 2, i);

        int outBeg = 0, outNb = 0;
        std::vector<double> outMAMA(size), outFAMA(size);

        TA_RetCode rc = TA_MAMA(0, size - 1, input.data(),
                                fast_limit, slow_limit,
                                &outBeg, &outNb,
                                outMAMA.data(), outFAMA.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outMAMA.data(), outFAMA.data(), "mama", "fama");
    }
}

// ============================================================
// HT_PHASOR: (LIST<DOUBLE>) -> LIST<STRUCT(inphase, quadrature)>
// ============================================================
static void TalibHtPhasorScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outInPhase(size), outQuadrature(size);

        TA_RetCode rc = TA_HT_PHASOR(0, size - 1, input.data(),
                                      &outBeg, &outNb,
                                      outInPhase.data(), outQuadrature.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outInPhase.data(), outQuadrature.data(), "inphase", "quadrature");
    }
}

// ============================================================
// HT_SINE: (LIST<DOUBLE>) -> LIST<STRUCT(sine, leadsine)>
// ============================================================
static void TalibHtSineScalar(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outSine(size), outLeadSine(size);

        TA_RetCode rc = TA_HT_SINE(0, size - 1, input.data(),
                                    &outBeg, &outNb,
                                    outSine.data(), outLeadSine.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackStruct2Result(result, i, size, outBeg, outNb,
                          outSine.data(), outLeadSine.data(), "sine", "leadsine");
    }
}

// ============================================================
// Registration
// ============================================================

static const auto LIST_DOUBLE = LogicalType::LIST(LogicalType::DOUBLE);

static LogicalType MakeStructList(const vector<pair<string, LogicalType>> &fields) {
    child_list_t<LogicalType> children;
    for (auto &f : fields) {
        children.push_back(make_pair(f.first, f.second));
    }
    return LogicalType::LIST(LogicalType::STRUCT(children));
}

void RegisterTalibMultiOutputFunctions(ExtensionLoader &loader) {

    // MACD: (LIST<DOUBLE>, INT, INT, INT) -> LIST<STRUCT(macd, signal, hist)>
    loader.RegisterFunction(ScalarFunction(
        "t_macd",
        {LIST_DOUBLE, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
        MakeStructList({{"macd", LogicalType::DOUBLE}, {"signal", LogicalType::DOUBLE}, {"hist", LogicalType::DOUBLE}}),
        TalibMacdScalar
    ));

    // BBANDS: (LIST<DOUBLE>, INT, DOUBLE, DOUBLE, INT) -> LIST<STRUCT(upper, middle, lower)>
    loader.RegisterFunction(ScalarFunction(
        "t_bbands",
        {LIST_DOUBLE, LogicalType::INTEGER, LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::INTEGER},
        MakeStructList({{"upper", LogicalType::DOUBLE}, {"middle", LogicalType::DOUBLE}, {"lower", LogicalType::DOUBLE}}),
        TalibBbandsScalar
    ));

    // STOCH: (LIST<DOUBLE> x3, INT, INT, INT, INT, INT) -> LIST<STRUCT(slowk, slowd)>
    loader.RegisterFunction(ScalarFunction(
        "t_stoch",
        {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE,
         LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER,
         LogicalType::INTEGER, LogicalType::INTEGER},
        MakeStructList({{"slowk", LogicalType::DOUBLE}, {"slowd", LogicalType::DOUBLE}}),
        TalibStochScalar
    ));

    // AROON: (LIST<DOUBLE> x2, INT) -> LIST<STRUCT(aroon_down, aroon_up)>
    loader.RegisterFunction(ScalarFunction(
        "t_aroon",
        {LIST_DOUBLE, LIST_DOUBLE, LogicalType::INTEGER},
        MakeStructList({{"aroon_down", LogicalType::DOUBLE}, {"aroon_up", LogicalType::DOUBLE}}),
        TalibAroonScalar
    ));

    // MINMAX: (LIST<DOUBLE>, INT) -> LIST<STRUCT(min, max)>
    loader.RegisterFunction(ScalarFunction(
        "t_minmax",
        {LIST_DOUBLE, LogicalType::INTEGER},
        MakeStructList({{"min", LogicalType::DOUBLE}, {"max", LogicalType::DOUBLE}}),
        TalibMinMaxScalar
    ));

    // MAMA: (LIST<DOUBLE>, DOUBLE, DOUBLE) -> LIST<STRUCT(mama, fama)>
    loader.RegisterFunction(ScalarFunction(
        "t_mama",
        {LIST_DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE},
        MakeStructList({{"mama", LogicalType::DOUBLE}, {"fama", LogicalType::DOUBLE}}),
        TalibMamaScalar
    ));

    // HT_PHASOR: (LIST<DOUBLE>) -> LIST<STRUCT(inphase, quadrature)>
    loader.RegisterFunction(ScalarFunction(
        "t_ht_phasor",
        {LIST_DOUBLE},
        MakeStructList({{"inphase", LogicalType::DOUBLE}, {"quadrature", LogicalType::DOUBLE}}),
        TalibHtPhasorScalar
    ));

    // HT_SINE: (LIST<DOUBLE>) -> LIST<STRUCT(sine, leadsine)>
    loader.RegisterFunction(ScalarFunction(
        "t_ht_sine",
        {LIST_DOUBLE},
        MakeStructList({{"sine", LogicalType::DOUBLE}, {"leadsine", LogicalType::DOUBLE}}),
        TalibHtSineScalar
    ));
}

} // namespace duckdb
