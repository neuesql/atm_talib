#include "talib_adapter.hpp"
// list_vector included via duckdb.hpp

#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension/extension_loader.hpp"
#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"
#include "talib_function_desc.hpp"

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
  return ListVector::GetChildMutable(args.data[col]);
}

// ============================================================
// Helper: pack multi-output results into LIST<STRUCT>
// Each output array has the same outBeg/outNb; we build
// a list of structs where lookback positions have NULL fields.
// ============================================================

// Pack 2-output result
static void PackStruct2Result(Vector &result, idx_t idx, int input_size,
                              int out_beg, int out_nb, const double *out1,
                              const double *out2, const string &name1,
                              const string &name2) {
  // Set list entry
  auto list_data = FlatVector::GetDataMutable<list_entry_t>(result);
  auto offset = ListVector::GetListSize(result);
  list_data[idx].offset = offset;
  list_data[idx].length = input_size;

  ListVector::Reserve(result, offset + input_size);
  ListVector::SetListSize(result, offset + input_size);

  // Get the struct child vector (the entry of the LIST)
  auto &struct_vec = ListVector::GetChildMutable(result);
  auto &entries = StructVector::GetEntries(struct_vec);

  // entries[0] = first field, entries[1] = second field
  auto data0 = FlatVector::GetDataMutable<double>(entries[0]);
  auto data1 = FlatVector::GetDataMutable<double>(entries[1]);
  auto &validity0 = FlatVector::ValidityMutable(entries[0]);
  auto &validity1 = FlatVector::ValidityMutable(entries[1]);

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
                              int out_beg, int out_nb, const double *out1,
                              const double *out2, const double *out3) {
  auto list_data = FlatVector::GetDataMutable<list_entry_t>(result);
  auto offset = ListVector::GetListSize(result);
  list_data[idx].offset = offset;
  list_data[idx].length = input_size;

  ListVector::Reserve(result, offset + input_size);
  ListVector::SetListSize(result, offset + input_size);

  auto &struct_vec = ListVector::GetChildMutable(result);
  auto &entries = StructVector::GetEntries(struct_vec);

  auto data0 = FlatVector::GetDataMutable<double>(entries[0]);
  auto data1 = FlatVector::GetDataMutable<double>(entries[1]);
  auto data2 = FlatVector::GetDataMutable<double>(entries[2]);
  auto &validity0 = FlatVector::ValidityMutable(entries[0]);
  auto &validity1 = FlatVector::ValidityMutable(entries[1]);
  auto &validity2 = FlatVector::ValidityMutable(entries[2]);

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
static void TalibMacdScalar(DataChunk &args, ExpressionState &state,
                            Vector &result) {
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

    TA_RetCode rc = TA_MACD(0, size - 1, input.data(), fast_period, slow_period,
                            signal_period, &outBeg, &outNb, outMACD.data(),
                            outSignal.data(), outHist.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct3Result(result, i, size, outBeg, outNb, outMACD.data(),
                      outSignal.data(), outHist.data());
  }
}

// ============================================================
// BBANDS: (LIST<DOUBLE>, INT, DOUBLE, DOUBLE, INT) -> LIST<STRUCT(upper,
// middle, lower)>
// ============================================================
static void TalibBbandsScalar(DataChunk &args, ExpressionState &state,
                              Vector &result) {
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

    TA_RetCode rc =
        TA_BBANDS(0, size - 1, input.data(), time_period, nb_dev_up, nb_dev_dn,
                  (TA_MAType)ma_type, &outBeg, &outNb, outUpper.data(),
                  outMiddle.data(), outLower.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct3Result(result, i, size, outBeg, outNb, outUpper.data(),
                      outMiddle.data(), outLower.data());
  }
}

// ============================================================
// STOCH: (LIST<DOUBLE> x3, INT, INT, INT, INT, INT) -> LIST<STRUCT(slowk,
// slowd)>
// ============================================================
static void TalibStochScalar(DataChunk &args, ExpressionState &state,
                             Vector &result) {
  auto count = args.size();
  for (idx_t i = 0; i < count; i++) {
    auto list_h = GetListEntry(args, 0, i);
    auto list_l = GetListEntry(args, 1, i);
    auto list_c = GetListEntry(args, 2, i);
    auto high = ListToDoubleArray(list_h, GetListChild(args, 0));
    auto low = ListToDoubleArray(list_l, GetListChild(args, 1));
    auto close = ListToDoubleArray(list_c, GetListChild(args, 2));
    int size = (int)high.size();

    int fastk_period = GetIntParam(args, 3, i);
    int slowk_period = GetIntParam(args, 4, i);
    int slowk_matype = GetIntParam(args, 5, i);
    int slowd_period = GetIntParam(args, 6, i);
    int slowd_matype = GetIntParam(args, 7, i);

    int outBeg = 0, outNb = 0;
    std::vector<double> outSlowK(size), outSlowD(size);

    TA_RetCode rc =
        TA_STOCH(0, size - 1, high.data(), low.data(), close.data(),
                 fastk_period, slowk_period, (TA_MAType)slowk_matype,
                 slowd_period, (TA_MAType)slowd_matype, &outBeg, &outNb,
                 outSlowK.data(), outSlowD.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outSlowK.data(),
                      outSlowD.data(), "slowk", "slowd");
  }
}

// ============================================================
// AROON: (LIST<DOUBLE> x2, INT) -> LIST<STRUCT(aroon_down, aroon_up)>
// ============================================================
static void TalibAroonScalar(DataChunk &args, ExpressionState &state,
                             Vector &result) {
  auto count = args.size();
  for (idx_t i = 0; i < count; i++) {
    auto list_h = GetListEntry(args, 0, i);
    auto list_l = GetListEntry(args, 1, i);
    auto high = ListToDoubleArray(list_h, GetListChild(args, 0));
    auto low = ListToDoubleArray(list_l, GetListChild(args, 1));
    int size = (int)high.size();

    int time_period = GetIntParam(args, 2, i);

    int outBeg = 0, outNb = 0;
    std::vector<double> outDown(size), outUp(size);

    TA_RetCode rc = TA_AROON(0, size - 1, high.data(), low.data(), time_period,
                             &outBeg, &outNb, outDown.data(), outUp.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outDown.data(),
                      outUp.data(), "aroon_down", "aroon_up");
  }
}

// ============================================================
// MINMAX: (LIST<DOUBLE>, INT) -> LIST<STRUCT(min, max)>
// ============================================================
static void TalibMinMaxScalar(DataChunk &args, ExpressionState &state,
                              Vector &result) {
  auto count = args.size();
  for (idx_t i = 0; i < count; i++) {
    auto list = GetListEntry(args, 0, i);
    auto &child = GetListChild(args, 0);
    auto input = ListToDoubleArray(list, child);
    int size = (int)input.size();

    int time_period = GetIntParam(args, 1, i);

    int outBeg = 0, outNb = 0;
    std::vector<double> outMin(size), outMax(size);

    TA_RetCode rc = TA_MINMAX(0, size - 1, input.data(), time_period, &outBeg,
                              &outNb, outMin.data(), outMax.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outMin.data(),
                      outMax.data(), "min", "max");
  }
}

// ============================================================
// MAMA: (LIST<DOUBLE>, DOUBLE, DOUBLE) -> LIST<STRUCT(mama, fama)>
// ============================================================
static void TalibMamaScalar(DataChunk &args, ExpressionState &state,
                            Vector &result) {
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

    TA_RetCode rc = TA_MAMA(0, size - 1, input.data(), fast_limit, slow_limit,
                            &outBeg, &outNb, outMAMA.data(), outFAMA.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outMAMA.data(),
                      outFAMA.data(), "mama", "fama");
  }
}

// ============================================================
// HT_PHASOR: (LIST<DOUBLE>) -> LIST<STRUCT(inphase, quadrature)>
// ============================================================
static void TalibHtPhasorScalar(DataChunk &args, ExpressionState &state,
                                Vector &result) {
  auto count = args.size();
  for (idx_t i = 0; i < count; i++) {
    auto list = GetListEntry(args, 0, i);
    auto &child = GetListChild(args, 0);
    auto input = ListToDoubleArray(list, child);
    int size = (int)input.size();

    int outBeg = 0, outNb = 0;
    std::vector<double> outInPhase(size), outQuadrature(size);

    TA_RetCode rc = TA_HT_PHASOR(0, size - 1, input.data(), &outBeg, &outNb,
                                 outInPhase.data(), outQuadrature.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outInPhase.data(),
                      outQuadrature.data(), "inphase", "quadrature");
  }
}

// ============================================================
// HT_SINE: (LIST<DOUBLE>) -> LIST<STRUCT(sine, leadsine)>
// ============================================================
static void TalibHtSineScalar(DataChunk &args, ExpressionState &state,
                              Vector &result) {
  auto count = args.size();
  for (idx_t i = 0; i < count; i++) {
    auto list = GetListEntry(args, 0, i);
    auto &child = GetListChild(args, 0);
    auto input = ListToDoubleArray(list, child);
    int size = (int)input.size();

    int outBeg = 0, outNb = 0;
    std::vector<double> outSine(size), outLeadSine(size);

    TA_RetCode rc = TA_HT_SINE(0, size - 1, input.data(), &outBeg, &outNb,
                               outSine.data(), outLeadSine.data());
    if (rc != TA_SUCCESS) {
      outNb = 0;
      outBeg = 0;
    }
    PackStruct2Result(result, i, size, outBeg, outNb, outSine.data(),
                      outLeadSine.data(), "sine", "leadsine");
  }
}

// ============================================================
// Registration
// ============================================================

static const auto LIST_DOUBLE = LogicalType::LIST(LogicalType::DOUBLE);

static LogicalType
MakeStructList(const vector<pair<string, LogicalType>> &fields) {
  child_list_t<LogicalType> children;
  for (auto &f : fields) {
    children.emplace_back(Identifier(f.first), f.second);
  }
  return LogicalType::LIST(LogicalType::STRUCT(children));
}

// Registers a scalar function with a FunctionDescription attached: renames
// its own signature parameters (what actually drives
// duckdb_functions().parameters on DuckDB 2.0 — see talib_function_desc.hpp),
// attaches a FunctionDescription, and sets on_conflict to match what the bare
// RegisterFunction(ScalarFunction) overload does internally.
static void
RegisterDescribedScalar(ExtensionLoader &loader, ScalarFunction func,
                        std::initializer_list<const char *> param_names,
                        string description, string example, string category) {
  SetParamNames(func, param_names);
  CreateScalarFunctionInfo info(func);
  info.on_conflict = OnCreateConflict::ALTER_ON_CONFLICT;
  FunctionDescription desc;
  desc.description = std::move(description);
  desc.parameter_names = vector<string>(param_names.begin(), param_names.end());
  desc.examples = {std::move(example)};
  desc.categories = {std::move(category), "technical_analysis"};
  info.descriptions.push_back(std::move(desc));
  loader.RegisterFunction(std::move(info));
}

void RegisterTalibMultiOutputFunctions(ExtensionLoader &loader) {

  // MACD: (LIST<DOUBLE>, INT, INT, INT) -> LIST<STRUCT(macd, signal, hist)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_macd",
                     {LIST_DOUBLE, LogicalType::INTEGER, LogicalType::INTEGER,
                      LogicalType::INTEGER},
                     MakeStructList({{"macd", LogicalType::DOUBLE},
                                     {"signal", LogicalType::DOUBLE},
                                     {"hist", LogicalType::DOUBLE}}),
                     TalibMacdScalar),
      {"values", "fast_period", "slow_period", "signal_period"},
      "Moving Average Convergence/Divergence: the difference between a fast "
      "and slow EMA of the input, plus an EMA signal line and the histogram "
      "of their difference.",
      "t_macd([1.0, 2.0, 3.0, 4.0, 5.0], 12, 26, 9)", "Trend");

  // BBANDS: (LIST<DOUBLE>, INT, DOUBLE, DOUBLE, INT) -> LIST<STRUCT(upper,
  // middle, lower)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_bbands",
                     {LIST_DOUBLE, LogicalType::INTEGER, LogicalType::DOUBLE,
                      LogicalType::DOUBLE, LogicalType::INTEGER},
                     MakeStructList({{"upper", LogicalType::DOUBLE},
                                     {"middle", LogicalType::DOUBLE},
                                     {"lower", LogicalType::DOUBLE}}),
                     TalibBbandsScalar),
      {"values", "time_period", "nb_dev_up", "nb_dev_dn", "ma_type"},
      "Bollinger Bands: a moving-average midline with upper and lower bands "
      "offset by a multiple of the rolling standard deviation.",
      "t_bbands([1.0, 2.0, 3.0, 4.0, 5.0], 20, 2.0, 2.0, 0)", "Volatility");

  // STOCH: (LIST<DOUBLE> x3, INT, INT, INT, INT, INT) -> LIST<STRUCT(slowk,
  // slowd)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_stoch",
                     {LIST_DOUBLE, LIST_DOUBLE, LIST_DOUBLE,
                      LogicalType::INTEGER, LogicalType::INTEGER,
                      LogicalType::INTEGER, LogicalType::INTEGER,
                      LogicalType::INTEGER},
                     MakeStructList({{"slowk", LogicalType::DOUBLE},
                                     {"slowd", LogicalType::DOUBLE}}),
                     TalibStochScalar),
      {"high", "low", "close", "fastk_period", "slowk_period", "slowk_matype",
       "slowd_period", "slowd_matype"},
      "Stochastic Oscillator: compares the current close to its recent "
      "high-low range, smoothed into %K and %D lines.",
      "t_stoch([12.0, 13.0, 14.0], [10.0, 11.0, 12.0], [11.0, 12.0, 13.0], 5, "
      "3, 0, 3, 0)",
      "Momentum");

  // AROON: (LIST<DOUBLE> x2, INT) -> LIST<STRUCT(aroon_down, aroon_up)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_aroon",
                     {LIST_DOUBLE, LIST_DOUBLE, LogicalType::INTEGER},
                     MakeStructList({{"aroon_down", LogicalType::DOUBLE},
                                     {"aroon_up", LogicalType::DOUBLE}}),
                     TalibAroonScalar),
      {"high", "low", "time_period"},
      "Aroon Indicator: measures the time since the most recent highest high "
      "and lowest low within the period, as two 0-100 oscillators.",
      "t_aroon([12.0, 13.0, 14.0], [10.0, 11.0, 12.0], 14)", "Trend");

  // MINMAX: (LIST<DOUBLE>, INT) -> LIST<STRUCT(min, max)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_minmax", {LIST_DOUBLE, LogicalType::INTEGER},
                     MakeStructList({{"min", LogicalType::DOUBLE},
                                     {"max", LogicalType::DOUBLE}}),
                     TalibMinMaxScalar),
      {"values", "time_period"},
      "Computes the rolling minimum and maximum of the input over the given "
      "period in a single pass.",
      "t_minmax([1.0, 2.0, 3.0, 4.0, 5.0], 3)", "Volatility");

  // MAMA: (LIST<DOUBLE>, DOUBLE, DOUBLE) -> LIST<STRUCT(mama, fama)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_mama",
                     {LIST_DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE},
                     MakeStructList({{"mama", LogicalType::DOUBLE},
                                     {"fama", LogicalType::DOUBLE}}),
                     TalibMamaScalar),
      {"values", "fast_limit", "slow_limit"},
      "MESA Adaptive Moving Average: an EMA whose smoothing factor adapts to "
      "the dominant market cycle via the Hilbert Transform, paired with a "
      "slower following average (FAMA).",
      "t_mama([1.0, 2.0, 3.0, 4.0, 5.0], 0.5, 0.05)", "Trend");

  // HT_PHASOR: (LIST<DOUBLE>) -> LIST<STRUCT(inphase, quadrature)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_ht_phasor", {LIST_DOUBLE},
                     MakeStructList({{"inphase", LogicalType::DOUBLE},
                                     {"quadrature", LogicalType::DOUBLE}}),
                     TalibHtPhasorScalar),
      {"values"},
      "Hilbert Transform Phasor Components: the in-phase and quadrature "
      "components of the dominant price cycle.",
      "t_ht_phasor([1.0, 2.0, 3.0, 4.0, 5.0])", "Cycle");

  // HT_SINE: (LIST<DOUBLE>) -> LIST<STRUCT(sine, leadsine)>
  RegisterDescribedScalar(
      loader,
      ScalarFunction("t_ht_sine", {LIST_DOUBLE},
                     MakeStructList({{"sine", LogicalType::DOUBLE},
                                     {"leadsine", LogicalType::DOUBLE}}),
                     TalibHtSineScalar),
      {"values"},
      "Hilbert Transform SineWave: the sine and lead sine of the dominant "
      "cycle's phase angle.",
      "t_ht_sine([1.0, 2.0, 3.0, 4.0, 5.0])", "Cycle");
}

} // namespace duckdb
