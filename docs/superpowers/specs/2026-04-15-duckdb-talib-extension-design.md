# DuckDB TA-Lib Extension Design Spec

## Overview

A DuckDB extension named `talib` that wraps all 158 TA-Lib (Technical Analysis Library) C functions, exposing them as SQL window functions and scalar/list functions. This enables SQL-native technical analysis for financial market data directly within DuckDB.

## Decisions

| Decision | Choice |
|----------|--------|
| Extension name | `talib` |
| DuckDB version | v1.x stable |
| TA-Lib linking | Static, via CMake FetchContent |
| Function coverage | All 158 functions |
| SQL naming | `ta_` prefix, lowercase (e.g. `ta_sma`, `ta_macd`) |
| Function types | Window functions + scalar/list functions |
| Build approach | Hybrid: code generation + manual overrides for multi-output |
| Multi-output returns | DuckDB STRUCT with named fields |
| Platforms | Linux (x86_64 + aarch64), macOS (x86_64 + arm64). Windows later. |
| Testing | SQLLogicTest, values validated against Python TA-Lib |
| Distribution | DuckDB community extensions repo |
| Docs | index.md (reference), cookbook.md (examples), README.md |

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│                  DuckDB Engine                   │
│                                                  │
│  SQL: ta_sma(close, 14) OVER (ORDER BY date)     │
│       ta_ad(high, low, close, volume)            │
│       ta_macd(close, 12, 26, 9)                  │
└──────────────┬──────────────────┬────────────────┘
               │ Window Function  │ Scalar (List)
               ▼                  ▼
┌─────────────────────────────────────────────────┐
│             talib Extension (C++)                │
│                                                  │
│  ┌──────────────────────────────────────────┐    │
│  │  Generated Registration Layer            │    │
│  │  (auto-gen from ta_func.h)               │    │
│  │  - 158 window functions (ta_xxx)         │    │
│  │  - 158 scalar/list functions (ta_xxx)    │    │
│  └──────────────────┬───────────────────────┘    │
│                     │                            │
│  ┌──────────────────▼───────────────────────┐    │
│  │  TA-Lib Adapter Layer                    │    │
│  │  - Input pattern handlers (5 patterns)   │    │
│  │  - Multi-output STRUCT builders          │    │
│  │  - NULL/NaN handling                     │    │
│  └──────────────────┬───────────────────────┘    │
│                     │                            │
│  ┌──────────────────▼───────────────────────┐    │
│  │  TA-Lib C Library (statically linked)    │    │
│  │  - 158 functions, compiled from source   │    │
│  └──────────────────────────────────────────┘    │
└─────────────────────────────────────────────────┘
```

Three layers:

1. **Generated Registration Layer** — auto-generated C++ code that registers all 158 functions as both DuckDB window functions and scalar/list functions. Produced by a Python code generator at build time.
2. **Adapter Layer** — hand-written C++ that bridges DuckDB vectors/frames to TA-Lib's array-based C API. Provides 5 templated pattern handlers and STRUCT builders for multi-output functions.
3. **TA-Lib** — the upstream C library, pulled via CMake FetchContent and statically linked.

---

## Input Patterns

TA-Lib's 158 functions are classified into 5 input patterns:

| Pattern | Inputs | Example Functions | Approx Count |
|---------|--------|-------------------|--------------|
| P1: Single array + timeperiod | `(inReal[], timePeriod)` | `ta_sma`, `ta_ema`, `ta_rsi` | ~45 |
| P2: Single array, no timeperiod | `(inReal[])` | `ta_sin`, `ta_cos`, `ta_ln` | ~25 |
| P3: HLC + timeperiod | `(high[], low[], close[], timePeriod)` | `ta_willr`, `ta_cci`, `ta_atr` | ~20 |
| P4: HLCV (+ optional timeperiod) | `(high[], low[], close[], volume[])` | `ta_ad`, `ta_obv`, `ta_mfi` | ~5 |
| P5: OHLC, no timeperiod | `(open[], high[], low[], close[])` | `ta_cdldoji`, `ta_cdlhammer` | ~61 |

### Multi-output functions (~15 functions)

These return a DuckDB STRUCT instead of a single value:

| Function | Return STRUCT |
|----------|---------------|
| `ta_macd` | `STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)` |
| `ta_bbands` | `STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)` |
| `ta_stoch` | `STRUCT(slowk DOUBLE, slowd DOUBLE)` |
| `ta_stochf` | `STRUCT(fastk DOUBLE, fastd DOUBLE)` |
| `ta_stochrsi` | `STRUCT(fastk DOUBLE, fastd DOUBLE)` |
| `ta_aroon` | `STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)` |
| `ta_macdext` | `STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)` |
| `ta_macdfix` | `STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)` |
| `ta_minmax` | `STRUCT(min DOUBLE, max DOUBLE)` |
| `ta_minmaxindex` | `STRUCT(minidx INTEGER, maxidx INTEGER)` |
| `ta_mama` | `STRUCT(mama DOUBLE, fama DOUBLE)` |
| `ta_ht_phasor` | `STRUCT(inphase DOUBLE, quadrature DOUBLE)` |
| `ta_ht_sine` | `STRUCT(sine DOUBLE, leadsine DOUBLE)` |

### Special parameters

Some functions have additional optional parameters beyond timeperiod:

- `ta_bbands`: `nbdevup DOUBLE DEFAULT 2.0`, `nbdevdn DOUBLE DEFAULT 2.0`, `matype INTEGER DEFAULT 0`
- `ta_macdext`: `fastmatype INTEGER DEFAULT 0`, `slowmatype INTEGER DEFAULT 0`, `signalmatype INTEGER DEFAULT 0`
- `ta_ma`: `matype INTEGER DEFAULT 0` (SMA=0, EMA=1, WMA=2, DEMA=3, TEMA=4, TRIMA=5, KAMA=6, MAMA=7, T3=8)

---

## SQL Interface

### Window function usage (primary)

```sql
-- P1: Single array + timeperiod
SELECT ta_sma(close, 14) OVER (
    PARTITION BY ticker ORDER BY date
    ROWS BETWEEN 13 PRECEDING AND CURRENT ROW
) FROM ohlc;

-- P3: HLC + timeperiod
SELECT ta_willr(high, low, close, 14) OVER (
    PARTITION BY ticker ORDER BY date
    ROWS BETWEEN 13 PRECEDING AND CURRENT ROW
) FROM ohlc;

-- P4: HLCV, no timeperiod
SELECT ta_ad(high, low, close, volume) OVER (
    PARTITION BY ticker ORDER BY date
    ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
) FROM ohlc;

-- P5: Candlestick pattern (returns INTEGER: -100, 0, or 100)
SELECT ta_cdldoji(open, high, low, close) OVER (
    PARTITION BY ticker ORDER BY date
    ROWS BETWEEN 10 PRECEDING AND CURRENT ROW
) FROM ohlc;

-- Multi-output: returns STRUCT, expandable with .*
SELECT (ta_macd(close, 12, 26, 9) OVER (
    PARTITION BY ticker ORDER BY date
    ROWS BETWEEN 33 PRECEDING AND CURRENT ROW
)).* FROM ohlc;
-- Expands to columns: macd, signal, hist
```

### Scalar/list function usage (alternative)

```sql
-- Operate on a pre-collected list
SELECT ta_sma(list(close ORDER BY date), 14)
FROM ohlc WHERE ticker = 'NVDA';

-- Use with array literals
SELECT ta_sma([1.0, 2.0, 3.0, 4.0, 5.0], 3);
```

### Return types

| Category | Return Type |
|----------|-------------|
| Most indicators | `DOUBLE` |
| Candlestick patterns | `INTEGER` (-100, 0, 100) |
| Multi-output functions | `STRUCT(...)` with named fields |
| Scalar/list variants | `LIST(DOUBLE)` or `LIST(INTEGER)` |

### NULL handling

- Input NULLs produce output NULL for that row
- TA-Lib lookback period: first N rows return NULL (e.g. `ta_sma(close, 14)` produces 13 NULLs at start)

---

## Project Structure

```
atm_talib/
├── CMakeLists.txt                    # Main build config
├── Makefile                          # DuckDB extension convenience wrapper
├── extension_config.cmake            # Extension metadata
├── vcpkg.json                        # Empty deps (TA-Lib via FetchContent)
├── scripts/
│   └── generate_functions.py         # Code generator: parses ta_func.h → C++
├── src/
│   ├── include/
│   │   └── talib_extension.hpp       # Extension header
│   ├── talib_extension.cpp           # Extension entry point (Load)
│   ├── talib_adapter.cpp             # Adapter: DuckDB vectors ↔ TA-Lib arrays
│   ├── talib_adapter.hpp             # Adapter types & helpers
│   ├── generated/
│   │   ├── window_functions.cpp      # Auto-generated window registrations
│   │   ├── scalar_functions.cpp      # Auto-generated scalar/list registrations
│   │   └── function_metadata.hpp     # Auto-generated function name/param tables
│   └── overrides/
│       ├── multi_output.cpp          # Hand-written: MACD, BBANDS, STOCH, etc.
│       └── multi_output.hpp          # STRUCT type definitions for multi-output
├── test/
│   └── sql/
│       ├── test_overlap.test         # SMA, EMA, BBANDS, DEMA, TEMA, WMA, etc.
│       ├── test_momentum.test        # RSI, MACD, WILLR, STOCH, CCI, ADX, etc.
│       ├── test_volume.test          # AD, OBV, ADOSC, MFI
│       ├── test_volatility.test      # ATR, NATR, STDDEV
│       ├── test_pattern.test         # Candlestick patterns (INTEGER return)
│       ├── test_scalar.test          # List-based scalar variants
│       ├── test_null_handling.test   # NULLs, empty tables, single-row
│       ├── test_multi_output.test    # STRUCT returns: MACD, BBANDS, STOCH
│       └── test_partition.test       # PARTITION BY ticker isolation
├── docs/
│   ├── index.md                      # Function reference (all 158 functions)
│   └── cookbook.md                    # SQL examples per function with OHLC table
├── README.md                         # Overview, install, quickstart, build
├── .github/
│   └── workflows/
│       └── main.yml                  # CI: build + test on Linux & macOS
└── description.yml                   # DuckDB community extension descriptor
```

---

## Build Flow

1. CMake `FetchContent` pulls TA-Lib source from GitHub and builds as static library
2. `scripts/generate_functions.py` runs as a CMake custom command, reads `ta_func.h`, outputs `src/generated/*.cpp`
3. Extension compiles: adapter layer + generated code + overrides → links with static TA-Lib → produces `talib.duckdb_extension`

### TA-Lib integration (CMakeLists.txt)

```cmake
FetchContent_Declare(
  ta_lib
  GIT_REPOSITORY https://github.com/ta-lib/ta-lib.git
  GIT_TAG v0.6.4
)
FetchContent_MakeAvailable(ta_lib)
target_link_libraries(${EXTENSION_NAME} ta_lib)
target_link_libraries(${LOADABLE_EXTENSION_NAME} ta_lib)
```

### Code generator (CMake custom command)

```cmake
add_custom_command(
    OUTPUT ${GENERATED_SOURCES}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/generate_functions.py
            --header ${ta_lib_SOURCE_DIR}/include/ta_func.h
            --output ${CMAKE_SOURCE_DIR}/src/generated/
    DEPENDS ${ta_lib_SOURCE_DIR}/include/ta_func.h
)
```

---

## Code Generator Design

**Input:** TA-Lib's `ta_func.h` header file.

**Process:**

1. Regex-parse each `TA_RetCode TA_XXX(...)` function signature
2. Extract: function name, input arrays (by name: `inReal`, `inHigh`, `inLow`, `inClose`, `inOpen`, `inVolume`), optional parameters (`optIn*`), output arrays (`out*`)
3. Classify into pattern P1-P5 based on input parameter names
4. Detect multi-output functions (>1 `out*` array) → add to skip-list (handled by overrides)
5. Generate C++ registration code for window and scalar variants

**Generated code delegates to templated adapter functions:**

```cpp
// Window function registration (generated)
registry.AddFunction(WindowFunction(
    "ta_sma",
    {LogicalType::DOUBLE, LogicalType::INTEGER},
    LogicalType::DOUBLE,
    TalibWindowBind<Pattern::P1>,
    TalibWindowInit<TA_SMA, Pattern::P1>,
    TalibWindowUpdate<TA_SMA, Pattern::P1>,
    TalibWindowFinalize<TA_SMA, Pattern::P1>
));

// Scalar/list function registration (generated)
registry.AddFunction(ScalarFunction(
    "ta_sma",
    {LogicalType::LIST(LogicalType::DOUBLE), LogicalType::INTEGER},
    LogicalType::LIST(LogicalType::DOUBLE),
    TalibScalarExecute<TA_SMA, Pattern::P1>
));
```

**Override mechanism:** Functions in `src/overrides/multi_output.cpp` are manually registered and excluded from code generation. These handle STRUCT return types with explicit field names.

---

## Testing Strategy

**Framework:** DuckDB SQLLogicTest (`.test` files in `test/sql/`).

**Validation:** Pre-compute expected values using the Python `ta-lib` package for a fixed dataset. Compare DuckDB output with rounding tolerance.

**Test categories:**

| Test File | Coverage |
|-----------|----------|
| `test_overlap.test` | SMA, EMA, BBANDS, DEMA, TEMA, WMA, KAMA, etc. |
| `test_momentum.test` | RSI, MACD, WILLR, STOCH, CCI, ADX, etc. |
| `test_volume.test` | AD, OBV, ADOSC, MFI |
| `test_volatility.test` | ATR, NATR, STDDEV |
| `test_pattern.test` | Candlestick patterns — verify INTEGER return |
| `test_scalar.test` | List-based scalar variants across all patterns |
| `test_null_handling.test` | NULL inputs, empty tables, single-row tables |
| `test_multi_output.test` | STRUCT returns: MACD, BBANDS, STOCH, AROON |
| `test_partition.test` | PARTITION BY ticker — verify isolation between groups |

**Edge cases tested:** lookback period > input size, all-NULL input, single row, empty partition.

**CI:** `make test` on Linux x86_64 and macOS arm64 via GitHub Actions.

---

## Documentation

### `docs/index.md` — Function Reference

All 158 functions listed by category. Each entry includes: function name, description, parameters with types and defaults, return type.

### `docs/cookbook.md` — SQL Cookbook

One working SQL example per function using a standard OHLC table. Organized by use case: Trend Following, Mean Reversion, Volume Analysis, Pattern Detection. Each example shows both window and scalar usage. Includes table setup DDL at top.

### `README.md`

Project description, install instructions (community repo + custom repo), quick start (5-line SQL), supported platforms, compact function table, build from source instructions, links to index.md and cookbook.md.

---

## Distribution

### Community extension submission (`description.yml`)

```yaml
extension:
  name: talib
  description: Technical Analysis Library (TA-Lib) functions for DuckDB
  version: 0.1.0
  language: C++
  requires_toolchains: python3
  repo: https://github.com/<username>/atm_talib
  ref: main
```

### Install paths

```sql
-- From community repo (after acceptance)
INSTALL talib FROM community;
LOAD talib;

-- From custom repo
INSTALL talib FROM 'https://your-repo-url';
LOAD talib;
```

### Platform support

- **Launch:** Linux (x86_64, aarch64), macOS (x86_64, arm64)
- **Later:** Windows (x86_64)
