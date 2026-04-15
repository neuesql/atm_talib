#pragma once

#include "duckdb.hpp"
#include "duckdb/function/scalar_function.hpp"

extern "C" {
#include "ta_libc.h"
}

#include <vector>
#include <string>

namespace duckdb {

// Input pattern classification for TA-Lib functions
enum class TalibPattern {
    P1, // Single array + timeperiod: (inReal[], timePeriod)
    P2, // Single array, no timeperiod: (inReal[])
    P3, // HLC + timeperiod: (high[], low[], close[], timePeriod)
    P4, // HLCV + optional timeperiod: (high[], low[], close[], volume[])
    P5  // OHLC, no timeperiod: (open[], high[], low[], close[])
};

// Extract a double array from a DuckDB LIST vector entry
std::vector<double> ListToDoubleArray(const list_entry_t &list, const Vector &child);

// Pack a double output array from TA-Lib back into a DuckDB LIST, respecting outBegIdx
void PackDoubleResult(Vector &result, idx_t idx, int input_size,
                      int out_beg_idx, int out_nb_element, const double *out_array);

// Pack an integer output array (candlestick patterns)
void PackIntResult(Vector &result, idx_t idx, int input_size,
                   int out_beg_idx, int out_nb_element, const int *out_array);

} // namespace duckdb
