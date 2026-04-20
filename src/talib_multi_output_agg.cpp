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
}

} // namespace duckdb
