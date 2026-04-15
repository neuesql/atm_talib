#include "talib_adapter.hpp"
// list_vector included via duckdb.hpp
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension/extension_loader.hpp"

namespace duckdb {

// ============================================================
// Helper: extract list_entry_t from a unified vector format
// ============================================================
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
// P1: (LIST<DOUBLE>, INTEGER) -> LIST<DOUBLE> or LIST<INTEGER>
// Signature: TA_FUNC(startIdx, endIdx, inReal[], optInTimePeriod, &outBegIdx, &outNBElement, outReal[])
// ============================================================

// P1 returning DOUBLE
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int, int*, int*, double[])>
static void TalibScalarP1Double(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        // Get period parameter
        UnifiedVectorFormat period_data;
        args.data[1].ToUnifiedFormat(count, period_data);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(period_data);
        auto period_idx = period_data.sel->get_index(i);
        int period = periods[period_idx];

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, input.data(), period, &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// P1 returning INT
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int, int*, int*, int[])>
static void TalibScalarP1Int(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        UnifiedVectorFormat period_data;
        args.data[1].ToUnifiedFormat(count, period_data);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(period_data);
        auto period_idx = period_data.sel->get_index(i);
        int period = periods[period_idx];

        int outBeg = 0, outNb = 0;
        std::vector<int> outInt(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, input.data(), period, &outBeg, &outNb, outInt.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackIntResult(result, i, size, outBeg, outNb, outInt.data());
    }
}

// ============================================================
// P2: (LIST<DOUBLE>) -> LIST<DOUBLE> or LIST<INTEGER>
// Signature: TA_FUNC(startIdx, endIdx, inReal[], &outBegIdx, &outNBElement, outReal[])
// ============================================================

// P2 returning DOUBLE
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int*, int*, double[])>
static void TalibScalarP2Double(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, input.data(), &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// P2 returning INT
template <TA_RetCode (*TA_FUNC)(int, int, const double[], int*, int*, int[])>
static void TalibScalarP2Int(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list = GetListEntry(args, 0, i);
        auto &child = GetListChild(args, 0);
        auto input = ListToDoubleArray(list, child);
        int size = (int)input.size();

        int outBeg = 0, outNb = 0;
        std::vector<int> outInt(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, input.data(), &outBeg, &outNb, outInt.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackIntResult(result, i, size, outBeg, outNb, outInt.data());
    }
}

// ============================================================
// P3: (LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>, INTEGER) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inHigh[], inLow[], inClose[], optInTimePeriod, &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], int, int*, int*, double[])>
static void TalibScalarP3(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto list_c = GetListEntry(args, 2, i);
        auto high  = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low   = ListToDoubleArray(list_l, GetListChild(args, 1));
        auto close = ListToDoubleArray(list_c, GetListChild(args, 2));
        int size = (int)high.size();

        UnifiedVectorFormat period_data;
        args.data[3].ToUnifiedFormat(count, period_data);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(period_data);
        auto period_idx = period_data.sel->get_index(i);
        int period = periods[period_idx];

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, high.data(), low.data(), close.data(), period, &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// P4: (LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inHigh[], inLow[], inClose[], inVolume[], &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, double[])>
static void TalibScalarP4(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto list_c = GetListEntry(args, 2, i);
        auto list_v = GetListEntry(args, 3, i);
        auto high   = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low    = ListToDoubleArray(list_l, GetListChild(args, 1));
        auto close  = ListToDoubleArray(list_c, GetListChild(args, 2));
        auto volume = ListToDoubleArray(list_v, GetListChild(args, 3));
        int size = (int)high.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, high.data(), low.data(), close.data(), volume.data(), &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// P5 DOUBLE: (LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inOpen[], inHigh[], inLow[], inClose[], &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, double[])>
static void TalibScalarP5Double(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_o = GetListEntry(args, 0, i);
        auto list_h = GetListEntry(args, 1, i);
        auto list_l = GetListEntry(args, 2, i);
        auto list_c = GetListEntry(args, 3, i);
        auto open_  = ListToDoubleArray(list_o, GetListChild(args, 0));
        auto high   = ListToDoubleArray(list_h, GetListChild(args, 1));
        auto low    = ListToDoubleArray(list_l, GetListChild(args, 2));
        auto close  = ListToDoubleArray(list_c, GetListChild(args, 3));
        int size = (int)open_.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, open_.data(), high.data(), low.data(), close.data(), &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// P5 INT: (LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>) -> LIST<INTEGER>
// Signature: TA_FUNC(startIdx, endIdx, inOpen[], inHigh[], inLow[], inClose[], &outBeg, &outNb, outInteger[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], const double[], int*, int*, int[])>
static void TalibScalarP5Int(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_o = GetListEntry(args, 0, i);
        auto list_h = GetListEntry(args, 1, i);
        auto list_l = GetListEntry(args, 2, i);
        auto list_c = GetListEntry(args, 3, i);
        auto open_  = ListToDoubleArray(list_o, GetListChild(args, 0));
        auto high   = ListToDoubleArray(list_h, GetListChild(args, 1));
        auto low    = ListToDoubleArray(list_l, GetListChild(args, 2));
        auto close  = ListToDoubleArray(list_c, GetListChild(args, 3));
        int size = (int)open_.size();

        int outBeg = 0, outNb = 0;
        std::vector<int> outInt(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, open_.data(), high.data(), low.data(), close.data(), &outBeg, &outNb, outInt.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackIntResult(result, i, size, outBeg, outNb, outInt.data());
    }
}

// ============================================================
// P6: (LIST<DOUBLE>, LIST<DOUBLE>) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inHigh[], inLow[], &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], int*, int*, double[])>
static void TalibScalarP6(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto high = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low  = ListToDoubleArray(list_l, GetListChild(args, 1));
        int size = (int)high.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, high.data(), low.data(), &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// P7: (LIST<DOUBLE>, LIST<DOUBLE>, LIST<DOUBLE>) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inHigh[], inLow[], inClose[], &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], const double[], int*, int*, double[])>
static void TalibScalarP7(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto list_c = GetListEntry(args, 2, i);
        auto high  = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low   = ListToDoubleArray(list_l, GetListChild(args, 1));
        auto close = ListToDoubleArray(list_c, GetListChild(args, 2));
        int size = (int)high.size();

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, high.data(), low.data(), close.data(), &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// P8: (LIST<DOUBLE>, LIST<DOUBLE>, INTEGER) -> LIST<DOUBLE>
// Signature: TA_FUNC(startIdx, endIdx, inHigh[], inLow[], optInTimePeriod, &outBeg, &outNb, outReal[])
// ============================================================
template <TA_RetCode (*TA_FUNC)(int, int, const double[], const double[], int, int*, int*, double[])>
static void TalibScalarP8(DataChunk &args, ExpressionState &state, Vector &result) {
    auto count = args.size();
    for (idx_t i = 0; i < count; i++) {
        auto list_h = GetListEntry(args, 0, i);
        auto list_l = GetListEntry(args, 1, i);
        auto high = ListToDoubleArray(list_h, GetListChild(args, 0));
        auto low  = ListToDoubleArray(list_l, GetListChild(args, 1));
        int size = (int)high.size();

        UnifiedVectorFormat period_data;
        args.data[2].ToUnifiedFormat(count, period_data);
        auto periods = UnifiedVectorFormat::GetData<int32_t>(period_data);
        auto period_idx = period_data.sel->get_index(i);
        int period = periods[period_idx];

        int outBeg = 0, outNb = 0;
        std::vector<double> outReal(size);
        TA_RetCode rc = TA_FUNC(0, size - 1, high.data(), low.data(), period, &outBeg, &outNb, outReal.data());
        if (rc != TA_SUCCESS) {
            outNb = 0;
            outBeg = 0;
        }
        PackDoubleResult(result, i, size, outBeg, outNb, outReal.data());
    }
}

// ============================================================
// Registration
// ============================================================

static const auto LIST_DOUBLE = LogicalType::LIST(LogicalType::DOUBLE);
static const auto LIST_INT    = LogicalType::LIST(LogicalType::INTEGER);

void RegisterTalibScalarFunctions(ExtensionLoader &loader) {

    // --- P1 DOUBLE: (LIST<DOUBLE>, INTEGER) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P1_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LogicalType::INTEGER}, \
            LIST_DOUBLE, \
            TalibScalarP1Double<ta_func>));

    // --- P1 INT: (LIST<DOUBLE>, INTEGER) -> LIST<INTEGER> ---
    #define TALIB_SCALAR_P1_INT(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LogicalType::INTEGER}, \
            LIST_INT, \
            TalibScalarP1Int<ta_func>));

    // --- P2 DOUBLE: (LIST<DOUBLE>) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P2_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE}, \
            LIST_DOUBLE, \
            TalibScalarP2Double<ta_func>));

    // --- P2 INT: (LIST<DOUBLE>) -> LIST<INTEGER> ---
    #define TALIB_SCALAR_P2_INT(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE}, \
            LIST_INT, \
            TalibScalarP2Int<ta_func>));

    // --- P3 DOUBLE: (LIST<DOUBLE> x3, INTEGER) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P3_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE, LogicalType::INTEGER}, \
            LIST_DOUBLE, \
            TalibScalarP3<ta_func>));

    // --- P4 DOUBLE: (LIST<DOUBLE> x4) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P4_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE}, \
            LIST_DOUBLE, \
            TalibScalarP4<ta_func>));

    // --- P5 DOUBLE: (LIST<DOUBLE> x4) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P5_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE}, \
            LIST_DOUBLE, \
            TalibScalarP5Double<ta_func>));

    // --- P5 INT: (LIST<DOUBLE> x4) -> LIST<INTEGER> ---
    #define TALIB_SCALAR_P5_INT(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE}, \
            LIST_INT, \
            TalibScalarP5Int<ta_func>));

    // --- P6 DOUBLE: (LIST<DOUBLE> x2) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P6_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE}, \
            LIST_DOUBLE, \
            TalibScalarP6<ta_func>));

    // --- P7 DOUBLE: (LIST<DOUBLE> x3) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P7_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE}, \
            LIST_DOUBLE, \
            TalibScalarP7<ta_func>));

    // --- P8 DOUBLE: (LIST<DOUBLE> x2, INTEGER) -> LIST<DOUBLE> ---
    #define TALIB_SCALAR_P8_DOUBLE(sql_name, ta_func) \
        loader.RegisterFunction(ScalarFunction( \
            "ta_" #sql_name, \
            {LIST_DOUBLE, LIST_DOUBLE, LogicalType::INTEGER}, \
            LIST_DOUBLE, \
            TalibScalarP8<ta_func>));

    // Dispatch macro: TALIB_FUNC -> TALIB_SCALAR_<pattern>_<ret_type>
    #define TALIB_FUNC(sql_name, ta_func, ta_lookback, pattern, ret_type) \
        TALIB_SCALAR_##pattern##_##ret_type(sql_name, ta_func)

    #include "talib_functions.hpp"

    #undef TALIB_FUNC
    #undef TALIB_SCALAR_P1_DOUBLE
    #undef TALIB_SCALAR_P1_INT
    #undef TALIB_SCALAR_P2_DOUBLE
    #undef TALIB_SCALAR_P2_INT
    #undef TALIB_SCALAR_P3_DOUBLE
    #undef TALIB_SCALAR_P4_DOUBLE
    #undef TALIB_SCALAR_P5_DOUBLE
    #undef TALIB_SCALAR_P5_INT
    #undef TALIB_SCALAR_P6_DOUBLE
    #undef TALIB_SCALAR_P7_DOUBLE
    #undef TALIB_SCALAR_P8_DOUBLE
}

} // namespace duckdb
