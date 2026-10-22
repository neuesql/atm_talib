#pragma once

#include "duckdb/function/function.hpp"

namespace duckdb {

// Renames a function's own signature parameters in place. This is what
// actually fixes the `parameters` column in duckdb_functions() on DuckDB
// 2.0+ — that column is read from FunctionSignature, not from
// FunctionDescription.parameter_names (which is only consulted when the
// parameter count doesn't match, which never happens for talib's
// fixed-arity functions).
inline void SetParamNames(SimpleFunction &fn,
                          std::initializer_list<const char *> names) {
  auto &sig = fn.GetSignature();
  idx_t i = 0;
  for (auto *name : names) {
    if (i >= sig.GetParameterCount()) {
      break;
    }
    sig.GetParameter(i++).SetName(name);
  }
}

} // namespace duckdb
