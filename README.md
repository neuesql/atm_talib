# 📈 DuckDB TA-Lib Extension

> Bring 100+ [TA-Lib](https://ta-lib.org/) technical analysis functions to SQL — right inside DuckDB.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

---

## ⚡ Quick Install

```sql
INSTALL talib FROM community;
LOAD talib;

# OR latest version

INSTALL talib FROM 'https://neuesql.github.io/atm_talib';
LOAD talib;
```

## 🚀 Quick Start

Three ways to compute **SMA(14)** on `ohlc` — pick based on your needs:

```sql
-- 1️⃣ List form (shortest & fastest): whole-series, returns a LIST<DOUBLE>
SELECT t_sma(list(close ORDER BY date), 14)
FROM ohlc WHERE ticker = 'NVDA';

-- 2️⃣ Window, bounded frame (fast, one row per input): recommended for dashboards
SELECT date, close,
       ta_sma(close, 14) OVER (ORDER BY date
                               ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS sma_14
FROM ohlc WHERE ticker = 'NVDA';

-- 3️⃣ Window, default frame (shortest SQL, but O(N²) — avoid on large tables)
SELECT date, close,
       ta_sma(close, 14) OVER (ORDER BY date) AS sma_14
FROM ohlc WHERE ticker = 'NVDA';
```

More examples:

```sql
-- 📐 MACD with STRUCT output
SELECT t_macd(list(close ORDER BY date), 12, 26, 9) FROM ohlc WHERE ticker = 'NVDA';

-- 🕯️ Candlestick pattern detection
SELECT t_cdldoji(list(open ORDER BY date), list(high ORDER BY date),
                 list(low ORDER BY date), list(close ORDER BY date))
FROM ohlc WHERE ticker = 'NVDA';
```

---

## 🔧 Function Types

Every TA-Lib function is registered in **two forms**:

| Form | Prefix | Usage | Example |
|------|--------|-------|---------|
| **Scalar (list)** | `t_` | Pass pre-collected lists, returns list | `t_sma([1.0, 2.0, 3.0], 2)` |
| **Aggregate (window)** | `ta_` | Use with `OVER()` for row-by-row results | `ta_sma(close, 14) OVER (ORDER BY date ...)` |

### Which should I use?

| | 🏎️ Scalar `t_*` | 🧑‍💻 Aggregate `ta_*` |
|---|---|---|
| **Performance** | ⚡ Fast — one pass over the full series, O(N) | 🐢 Slower — recomputes per window frame, ~O(N × window) |
| **Ergonomics** | Requires `list(col ORDER BY date)` + `unnest` to rejoin rows | Natural SQL — plugs into `OVER (PARTITION BY … ORDER BY …)` |
| **Best for** | Backtests, full-history feature generation, large datasets | Dashboards, ad-hoc queries, mixing indicators with row-level columns |

> 💡 **Rule of thumb:** reach for `t_*` when speed matters or you're computing over a whole series; reach for `ta_*` when the query reads more clearly as a window function.
>
> **Why two prefixes?** DuckDB requires different SQL names for scalar and aggregate functions.

---

## 📚 Supported Functions (100+)

| Category | Count | Examples |
|----------|-------|---------|
| 🔀 Overlap Studies | 8+ | `t_sma`, `t_ema`, `t_wma`, `t_dema`, `t_tema` |
| 🏃 Momentum | 15+ | `t_rsi`, `t_macd`, `t_willr`, `t_cci`, `t_adx` |
| 🔊 Volume | 1+ | `t_ad` |
| 🌊 Volatility | 2+ | `t_atr`, `t_natr` |
| 🕯️ Pattern Recognition | 49+ | `t_cdldoji`, `t_cdlhammer`, `t_cdlengulfing` |
| 💰 Price Transform | 2+ | `t_avgprice`, `t_bop` |
| 🔄 Cycle Indicators | 4+ | `t_ht_dcperiod`, `t_ht_trendline`, `t_ht_trendmode` |
| 📏 Statistics | 10+ | `t_linearreg`, `t_tsf`, `t_max`, `t_min` |
| ➗ Math Transform | 15 | `t_sin`, `t_cos`, `t_ln`, `t_sqrt` |

---

## 🧩 Multi-Output Functions

Functions like MACD and Bollinger Bands return `LIST<STRUCT>` types:

```sql
-- 📐 MACD → LIST<STRUCT(macd, signal, hist)>
SELECT t_macd(list(close ORDER BY date), 12, 26, 9) FROM ohlc;

-- 📊 BBANDS → LIST<STRUCT(upper, middle, lower)>
SELECT t_bbands(list(close ORDER BY date), 20, 2.0, 2.0, 0) FROM ohlc;

-- 📈 STOCH → LIST<STRUCT(slowk, slowd)>
SELECT t_stoch(list(high ORDER BY date), list(low ORDER BY date),
                list(close ORDER BY date), 5, 3, 0, 3, 0) FROM ohlc;

-- 🏹 AROON → LIST<STRUCT(aroon_down, aroon_up)>
SELECT t_aroon(list(high ORDER BY date), list(low ORDER BY date), 14) FROM ohlc;
```

---

## 🗺️ Input Patterns

| Pattern | Scalar Signature | Example |
|---------|-----------------|---------|
| P1 | `(LIST<DOUBLE>, INTEGER)` | `t_sma(values, period)` |
| P2 | `(LIST<DOUBLE>)` | `t_sin(values)` |
| P3 | `(LIST<DOUBLE> x3, INTEGER)` | `t_willr(high, low, close, period)` |
| P4 | `(LIST<DOUBLE> x4)` | `t_ad(high, low, close, volume)` |
| P5 | `(LIST<DOUBLE> x4)` | `t_cdldoji(open, high, low, close)` |
| P6 | `(LIST<DOUBLE> x2)` | `t_medprice(high, low)` |
| P7 | `(LIST<DOUBLE> x3)` | `t_typprice(high, low, close)` |
| P8 | `(LIST<DOUBLE> x2, INTEGER)` | `t_midprice(high, low, period)` |

---

## 🖥️ Platforms

> ⚠️ **DuckDB version:** Only **v1.5.2** is supported.

| Platform | Architectures |
|----------|--------------|
| 🐧 Linux | x86_64, aarch64 |
| 🍎 macOS | x86_64, arm64 |

---

## 🛠️ Build from Source

```bash
git clone --recurse-submodules https://github.com/neuesql/atm_talib.git
cd atm_talib
GEN=ninja make        # build
make test             # run tests
```

**Prerequisites:** CMake 3.5+, C++17 compiler, ninja (recommended)

---

## 📖 Documentation

- [Function Reference](index.md) — all functions with parameters and types
- [SQL Cookbook](cookbook.md) — examples for every function

---

## 📄 License

MIT
