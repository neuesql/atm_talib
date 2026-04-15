# DuckDB TA-Lib Extension

A DuckDB extension that brings [TA-Lib](https://ta-lib.org/) (Technical Analysis Library) to SQL. Over 100 TA-Lib functions are available as SQL functions for technical analysis of financial market data.

## Install

```sql
-- From community repository (after acceptance)
INSTALL talib FROM community;
LOAD talib;

-- From custom repository
INSTALL talib FROM 'https://your-repo-url';
LOAD talib;
```

## Quick Start

```sql
LOAD talib;

-- Simple Moving Average on a list
SELECT ta_sma(list(close ORDER BY date), 14) FROM ohlc WHERE ticker = 'NVDA';

-- SMA as a window function
SELECT date, close,
       taw_sma(close, 14) OVER (ORDER BY date ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS sma_14
FROM ohlc WHERE ticker = 'NVDA';

-- MACD with STRUCT output
SELECT ta_macd(list(close ORDER BY date), 12, 26, 9) FROM ohlc WHERE ticker = 'NVDA';

-- Candlestick pattern detection
SELECT ta_cdldoji(list(open ORDER BY date), list(high ORDER BY date),
                  list(low ORDER BY date), list(close ORDER BY date))
FROM ohlc WHERE ticker = 'NVDA';
```

## Function Types

Every TA-Lib function is registered in two forms:

| Form | Prefix | Usage | Example |
|------|--------|-------|---------|
| **Scalar (list)** | `ta_` | Pass pre-collected lists, returns list | `ta_sma([1.0, 2.0, 3.0], 2)` |
| **Aggregate (window)** | `taw_` | Use with `OVER()` for row-by-row results | `taw_sma(close, 14) OVER (ORDER BY date ...)` |

> **Note:** DuckDB requires different names for scalar and aggregate functions. Scalar functions use `ta_` prefix, aggregate/window functions use `taw_` prefix.

## Supported Functions

Over 100 functions across these categories:

| Category | Count | Examples |
|----------|-------|---------|
| Overlap Studies | 8+ | `ta_sma`, `ta_ema`, `ta_wma`, `ta_dema`, `ta_tema` |
| Momentum | 15+ | `ta_rsi`, `ta_macd`, `ta_willr`, `ta_cci`, `ta_adx` |
| Volume | 1+ | `ta_ad` |
| Volatility | 2+ | `ta_atr`, `ta_natr` |
| Pattern Recognition | 49+ | `ta_cdldoji`, `ta_cdlhammer`, `ta_cdlengulfing` |
| Price Transform | 2+ | `ta_avgprice`, `ta_bop` |
| Cycle Indicators | 4+ | `ta_ht_dcperiod`, `ta_ht_trendline`, `ta_ht_trendmode` |
| Statistics | 10+ | `ta_linearreg`, `ta_tsf`, `ta_max`, `ta_min` |
| Math Transform | 15 | `ta_sin`, `ta_cos`, `ta_ln`, `ta_sqrt` |

### Multi-Output Functions

Functions like MACD and Bollinger Bands return `LIST<STRUCT>` types:

```sql
-- MACD returns LIST<STRUCT(macd, signal, hist)>
SELECT ta_macd(list(close ORDER BY date), 12, 26, 9) FROM ohlc;

-- BBANDS returns LIST<STRUCT(upper, middle, lower)>
SELECT ta_bbands(list(close ORDER BY date), 20, 2.0, 2.0, 0) FROM ohlc;

-- STOCH returns LIST<STRUCT(slowk, slowd)>
SELECT ta_stoch(list(high ORDER BY date), list(low ORDER BY date),
                list(close ORDER BY date), 5, 3, 0, 3, 0) FROM ohlc;

-- AROON returns LIST<STRUCT(aroon_down, aroon_up)>
SELECT ta_aroon(list(high ORDER BY date), list(low ORDER BY date), 14) FROM ohlc;
```

## Input Patterns

| Pattern | Scalar Signature | Example |
|---------|-----------------|---------|
| P1 | `(LIST<DOUBLE>, INTEGER)` | `ta_sma(values, period)` |
| P2 | `(LIST<DOUBLE>)` | `ta_sin(values)` |
| P3 | `(LIST<DOUBLE> x3, INTEGER)` | `ta_willr(high, low, close, period)` |
| P4 | `(LIST<DOUBLE> x4)` | `ta_ad(high, low, close, volume)` |
| P5 | `(LIST<DOUBLE> x4)` | `ta_cdldoji(open, high, low, close)` |
| P6 | `(LIST<DOUBLE> x2)` | `ta_medprice(high, low)` |
| P7 | `(LIST<DOUBLE> x3)` | `ta_typprice(high, low, close)` |
| P8 | `(LIST<DOUBLE> x2, INTEGER)` | `ta_midprice(high, low, period)` |

## Platforms

- Linux x86_64, aarch64
- macOS x86_64, arm64

## Build from Source

```bash
git clone --recurse-submodules https://github.com/<username>/atm_talib.git
cd atm_talib
GEN=ninja make        # build
make test             # run tests
```

Requires: CMake 3.5+, C++17 compiler, ninja (recommended).

## Documentation

- [Function Reference](index.md) — all functions with parameters and types
- [SQL Cookbook](cookbook.md) — examples for every function

## License

MIT
