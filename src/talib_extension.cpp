#define DUCKDB_EXTENSION_MAIN

#include "talib_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/main/extension/extension_loader.hpp"

namespace duckdb {

// Defined in talib_scalar.cpp
void RegisterTalibScalarFunctions(ExtensionLoader &loader);
// Defined in talib_aggregate.cpp
void RegisterTalibAggregateFunctions(ExtensionLoader &loader);
// Defined in talib_multi_output.cpp
void RegisterTalibMultiOutputFunctions(ExtensionLoader &loader);
// Defined in talib_multi_output_agg.cpp
void RegisterTalibMultiOutputAggFunctions(ExtensionLoader &loader);

static void LoadInternal(ExtensionLoader &loader) {
    RegisterTalibScalarFunctions(loader);
    RegisterTalibAggregateFunctions(loader);
    RegisterTalibMultiOutputFunctions(loader);
    RegisterTalibMultiOutputAggFunctions(loader);
}

void TalibExtension::Load(ExtensionLoader &loader) {
    LoadInternal(loader);
}

std::string TalibExtension::Name() {
    return "talib";
}

std::string TalibExtension::Version() const {
#ifdef EXT_VERSION_TALIB
    return EXT_VERSION_TALIB;
#else
    return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_CPP_EXTENSION_ENTRY(talib, loader) {
    duckdb::LoadInternal(loader);
}

}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
