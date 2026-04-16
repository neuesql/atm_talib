# DuckDB TA-Lib Extension — Function Reference

This extension wraps [TA-Lib](https://ta-lib.org/) technical analysis functions for use inside DuckDB SQL queries.

## Function Forms

Every indicator is available in two forms:

- **Scalar** (`t_` prefix): accepts `LIST<DOUBLE>` inputs, returns a `LIST` of the same length (leading lookback positions are `NULL`).
- **Window / Aggregate** (`ta_` prefix): accepts plain scalar columns and is used with a window `OVER (...)` clause, returning one `DOUBLE` per row.

### Return-type note for pattern recognition

Candlestick pattern functions return `LIST<INTEGER>` (scalar) or `INTEGER` (window). Values are `100` (bullish), `-100` (bearish), or `0` (no pattern).

---

## Function Index

| # | Category | Function | Description | Input | Return |
|---|----------|----------|-------------|-------|--------|
| | **Overlap Studies** | | | | |
| 1 | Overlap | `t_sma` / `ta_sma` | Simple Moving Average | (values, period) | DOUBLE |
| 2 | Overlap | `t_ema` / `ta_ema` | Exponential Moving Average | (values, period) | DOUBLE |
| 3 | Overlap | `t_wma` / `ta_wma` | Weighted Moving Average | (values, period) | DOUBLE |
| 4 | Overlap | `t_dema` / `ta_dema` | Double Exponential Moving Average | (values, period) | DOUBLE |
| 5 | Overlap | `t_tema` / `ta_tema` | Triple Exponential Moving Average | (values, period) | DOUBLE |
| 6 | Overlap | `t_trima` / `ta_trima` | Triangular Moving Average | (values, period) | DOUBLE |
| 7 | Overlap | `t_kama` / `ta_kama` | Kaufman Adaptive Moving Average | (values, period) | DOUBLE |
| 8 | Overlap | `t_midpoint` / `ta_midpoint` | MidPoint over period | (values, period) | DOUBLE |
| 9 | Overlap | `t_midprice` / `ta_midprice` | Midpoint Price over period | (high, low, period) | DOUBLE |
| | **Momentum Indicators** | | | | |
| 10 | Momentum | `t_rsi` / `ta_rsi` | Relative Strength Index | (values, period) | DOUBLE |
| 11 | Momentum | `t_cmo` / `ta_cmo` | Chande Momentum Oscillator | (values, period) | DOUBLE |
| 12 | Momentum | `t_mom` / `ta_mom` | Momentum | (values, period) | DOUBLE |
| 13 | Momentum | `t_roc` / `ta_roc` | Rate of Change | (values, period) | DOUBLE |
| 14 | Momentum | `t_rocp` / `ta_rocp` | Rate of Change (Percentage) | (values, period) | DOUBLE |
| 15 | Momentum | `t_rocr` / `ta_rocr` | Rate of Change (Ratio) | (values, period) | DOUBLE |
| 16 | Momentum | `t_rocr100` / `ta_rocr100` | Rate of Change (Ratio x100) | (values, period) | DOUBLE |
| 17 | Momentum | `t_trix` / `ta_trix` | Triple Smooth EMA Rate of Change | (values, period) | DOUBLE |
| 18 | Momentum | `t_willr` / `ta_willr` | Williams' %R | (high, low, close, period) | DOUBLE |
| 19 | Momentum | `t_cci` / `ta_cci` | Commodity Channel Index | (high, low, close, period) | DOUBLE |
| 20 | Momentum | `t_adx` / `ta_adx` | Average Directional Movement Index | (high, low, close, period) | DOUBLE |
| 21 | Momentum | `t_adxr` / `ta_adxr` | Average Directional Movement Rating | (high, low, close, period) | DOUBLE |
| 22 | Momentum | `t_dx` / `ta_dx` | Directional Movement Index | (high, low, close, period) | DOUBLE |
| 23 | Momentum | `t_plus_di` / `ta_plus_di` | Plus Directional Indicator | (high, low, close, period) | DOUBLE |
| 24 | Momentum | `t_minus_di` / `ta_minus_di` | Minus Directional Indicator | (high, low, close, period) | DOUBLE |
| 25 | Momentum | `t_plus_dm` / `ta_plus_dm` | Plus Directional Movement | (high, low, period) | DOUBLE |
| 26 | Momentum | `t_minus_dm` / `ta_minus_dm` | Minus Directional Movement | (high, low, period) | DOUBLE |
| | **Volume Indicators** | | | | |
| 27 | Volume | `t_ad` / `ta_ad` | Chaikin A/D Line | (high, low, close, volume) | DOUBLE |
| | **Volatility** | | | | |
| 28 | Volatility | `t_atr` / `ta_atr` | Average True Range | (high, low, close, period) | DOUBLE |
| 29 | Volatility | `t_natr` / `ta_natr` | Normalized Average True Range | (high, low, close, period) | DOUBLE |
| 30 | Volatility | `t_trange` / `ta_trange` | True Range | (high, low, close) | DOUBLE |
| | **Pattern Recognition** | | | | |
| 31 | Pattern | `t_cdl2crows` | Two Crows | (open, high, low, close) | INT |
| 32 | Pattern | `t_cdl3blackcrows` | Three Black Crows | (open, high, low, close) | INT |
| 33 | Pattern | `t_cdl3inside` | Three Inside Up/Down | (open, high, low, close) | INT |
| 34 | Pattern | `t_cdl3linestrike` | Three-Line Strike | (open, high, low, close) | INT |
| 35 | Pattern | `t_cdl3outside` | Three Outside Up/Down | (open, high, low, close) | INT |
| 36 | Pattern | `t_cdl3starsinsouth` | Three Stars In The South | (open, high, low, close) | INT |
| 37 | Pattern | `t_cdl3whitesoldiers` | Three Advancing White Soldiers | (open, high, low, close) | INT |
| 38 | Pattern | `t_cdladvanceblock` | Advance Block | (open, high, low, close) | INT |
| 39 | Pattern | `t_cdlbelthold` | Belt-hold | (open, high, low, close) | INT |
| 40 | Pattern | `t_cdlbreakaway` | Breakaway | (open, high, low, close) | INT |
| 41 | Pattern | `t_cdlclosingmarubozu` | Closing Marubozu | (open, high, low, close) | INT |
| 42 | Pattern | `t_cdlconcealbabyswall` | Concealing Baby Swallow | (open, high, low, close) | INT |
| 43 | Pattern | `t_cdlcounterattack` | Counterattack | (open, high, low, close) | INT |
| 44 | Pattern | `t_cdldoji` | Doji | (open, high, low, close) | INT |
| 45 | Pattern | `t_cdldojistar` | Doji Star | (open, high, low, close) | INT |
| 46 | Pattern | `t_cdldragonflydoji` | Dragonfly Doji | (open, high, low, close) | INT |
| 47 | Pattern | `t_cdlengulfing` | Engulfing Pattern | (open, high, low, close) | INT |
| 48 | Pattern | `t_cdlgapsidesidewhite` | Gap Side-by-Side White Lines | (open, high, low, close) | INT |
| 49 | Pattern | `t_cdlgravestonedoji` | Gravestone Doji | (open, high, low, close) | INT |
| 50 | Pattern | `t_cdlhammer` | Hammer | (open, high, low, close) | INT |
| 51 | Pattern | `t_cdlhangingman` | Hanging Man | (open, high, low, close) | INT |
| 52 | Pattern | `t_cdlharami` | Harami Pattern | (open, high, low, close) | INT |
| 53 | Pattern | `t_cdlharamicross` | Harami Cross Pattern | (open, high, low, close) | INT |
| 54 | Pattern | `t_cdlhighwave` | High-Wave Candle | (open, high, low, close) | INT |
| 55 | Pattern | `t_cdlhikkake` | Hikkake Pattern | (open, high, low, close) | INT |
| 56 | Pattern | `t_cdlhikkakemod` | Modified Hikkake Pattern | (open, high, low, close) | INT |
| 57 | Pattern | `t_cdlhomingpigeon` | Homing Pigeon | (open, high, low, close) | INT |
| 58 | Pattern | `t_cdlidentical3crows` | Identical Three Crows | (open, high, low, close) | INT |
| 59 | Pattern | `t_cdlinneck` | In-Neck Pattern | (open, high, low, close) | INT |
| 60 | Pattern | `t_cdlinvertedhammer` | Inverted Hammer | (open, high, low, close) | INT |
| 61 | Pattern | `t_cdlkicking` | Kicking | (open, high, low, close) | INT |
| 62 | Pattern | `t_cdlkickingbylength` | Kicking (by longer marubozu) | (open, high, low, close) | INT |
| 63 | Pattern | `t_cdlladderbottom` | Ladder Bottom | (open, high, low, close) | INT |
| 64 | Pattern | `t_cdllongleggeddoji` | Long Legged Doji | (open, high, low, close) | INT |
| 65 | Pattern | `t_cdllongline` | Long Line Candle | (open, high, low, close) | INT |
| 66 | Pattern | `t_cdlmarubozu` | Marubozu | (open, high, low, close) | INT |
| 67 | Pattern | `t_cdlmatchinglow` | Matching Low | (open, high, low, close) | INT |
| 68 | Pattern | `t_cdlonneck` | On-Neck Pattern | (open, high, low, close) | INT |
| 69 | Pattern | `t_cdlpiercing` | Piercing Pattern | (open, high, low, close) | INT |
| 70 | Pattern | `t_cdlrickshawman` | Rickshaw Man | (open, high, low, close) | INT |
| 71 | Pattern | `t_cdlrisefall3methods` | Rising/Falling Three Methods | (open, high, low, close) | INT |
| 72 | Pattern | `t_cdlseparatinglines` | Separating Lines | (open, high, low, close) | INT |
| 73 | Pattern | `t_cdlshootingstar` | Shooting Star | (open, high, low, close) | INT |
| 74 | Pattern | `t_cdlshortline` | Short Line Candle | (open, high, low, close) | INT |
| 75 | Pattern | `t_cdlspinningtop` | Spinning Top | (open, high, low, close) | INT |
| 76 | Pattern | `t_cdlstalledpattern` | Stalled Pattern | (open, high, low, close) | INT |
| 77 | Pattern | `t_cdlsticksandwich` | Stick Sandwich | (open, high, low, close) | INT |
| 78 | Pattern | `t_cdltakuri` | Takuri (long lower shadow Doji) | (open, high, low, close) | INT |
| 79 | Pattern | `t_cdltasukigap` | Tasuki Gap | (open, high, low, close) | INT |
| 80 | Pattern | `t_cdlthrusting` | Thrusting Pattern | (open, high, low, close) | INT |
| 81 | Pattern | `t_cdltristar` | Tristar Pattern | (open, high, low, close) | INT |
| 82 | Pattern | `t_cdlunique3river` | Unique 3 River | (open, high, low, close) | INT |
| 83 | Pattern | `t_cdlupsidegap2crows` | Upside Gap Two Crows | (open, high, low, close) | INT |
| 84 | Pattern | `t_cdlxsidegap3methods` | Upside/Downside Gap Three Methods | (open, high, low, close) | INT |
| | **Price Transform** | | | | |
| 85 | Price | `t_avgprice` / `ta_avgprice` | Average Price (O+H+L+C)/4 | (open, high, low, close) | DOUBLE |
| 86 | Price | `t_bop` / `ta_bop` | Balance of Power | (open, high, low, close) | DOUBLE |
| 87 | Price | `t_medprice` / `ta_medprice` | Median Price (H+L)/2 | (high, low) | DOUBLE |
| 88 | Price | `t_typprice` / `ta_typprice` | Typical Price (H+L+C)/3 | (high, low, close) | DOUBLE |
| 89 | Price | `t_wclprice` / `ta_wclprice` | Weighted Close (H+L+2C)/4 | (high, low, close) | DOUBLE |
| | **Cycle Indicators** | | | | |
| 90 | Cycle | `t_ht_dcperiod` / `ta_ht_dcperiod` | Dominant Cycle Period | (values) | DOUBLE |
| 91 | Cycle | `t_ht_dcphase` / `ta_ht_dcphase` | Dominant Cycle Phase | (values) | DOUBLE |
| 92 | Cycle | `t_ht_trendline` / `ta_ht_trendline` | Instantaneous Trendline | (values) | DOUBLE |
| 93 | Cycle | `t_ht_trendmode` / `ta_ht_trendmode` | Trend vs Cycle Mode | (values) | INT |
| | **Statistics** | | | | |
| 94 | Stats | `t_linearreg` / `ta_linearreg` | Linear Regression | (values, period) | DOUBLE |
| 95 | Stats | `t_linearreg_angle` / `ta_linearreg_angle` | Linear Regression Angle | (values, period) | DOUBLE |
| 96 | Stats | `t_linearreg_intercept` / `ta_linearreg_intercept` | Linear Regression Intercept | (values, period) | DOUBLE |
| 97 | Stats | `t_linearreg_slope` / `ta_linearreg_slope` | Linear Regression Slope | (values, period) | DOUBLE |
| 98 | Stats | `t_tsf` / `ta_tsf` | Time Series Forecast | (values, period) | DOUBLE |
| 99 | Stats | `t_sum` / `ta_sum` | Summation | (values, period) | DOUBLE |
| 100 | Stats | `t_max` / `ta_max` | Highest Value over period | (values, period) | DOUBLE |
| 101 | Stats | `t_min` / `ta_min` | Lowest Value over period | (values, period) | DOUBLE |
| 102 | Stats | `t_maxindex` / `ta_maxindex` | Index of Highest Value | (values, period) | INT |
| 103 | Stats | `t_minindex` / `ta_minindex` | Index of Lowest Value | (values, period) | INT |
| | **Math Transform** | | | | |
| 104 | Math | `t_acos` | Arc Cosine | (values) | DOUBLE |
| 105 | Math | `t_asin` | Arc Sine | (values) | DOUBLE |
| 106 | Math | `t_atan` | Arc Tangent | (values) | DOUBLE |
| 107 | Math | `t_ceil` | Ceiling | (values) | DOUBLE |
| 108 | Math | `t_cos` | Cosine | (values) | DOUBLE |
| 109 | Math | `t_cosh` | Hyperbolic Cosine | (values) | DOUBLE |
| 110 | Math | `t_exp` | Exponential (e^x) | (values) | DOUBLE |
| 111 | Math | `t_floor` | Floor | (values) | DOUBLE |
| 112 | Math | `t_ln` | Natural Logarithm | (values) | DOUBLE |
| 113 | Math | `t_log10` | Base-10 Logarithm | (values) | DOUBLE |
| 114 | Math | `t_sin` | Sine | (values) | DOUBLE |
| 115 | Math | `t_sinh` | Hyperbolic Sine | (values) | DOUBLE |
| 116 | Math | `t_sqrt` | Square Root | (values) | DOUBLE |
| 117 | Math | `t_tan` | Tangent | (values) | DOUBLE |
| 118 | Math | `t_tanh` | Hyperbolic Tangent | (values) | DOUBLE |
| | **Multi-Output** (scalar only, return `LIST<STRUCT>`) | | | | |
| 119 | Multi | `t_macd` | MACD | (values, fast, slow, signal) | STRUCT(macd, signal, hist) |
| 120 | Multi | `t_bbands` | Bollinger Bands | (values, period, devup, devdn, matype) | STRUCT(upper, middle, lower) |
| 121 | Multi | `t_stoch` | Stochastic | (high, low, close, fastK, slowK, slowKMA, slowD, slowDMA) | STRUCT(slowk, slowd) |
| 122 | Multi | `t_aroon` | Aroon | (high, low, period) | STRUCT(aroon_down, aroon_up) |
| 123 | Multi | `t_minmax` | Min/Max over period | (values, period) | STRUCT(min, max) |
| 124 | Multi | `t_mama` | MESA Adaptive Moving Average | (values, fastlimit, slowlimit) | STRUCT(mama, fama) |
| 125 | Multi | `t_ht_phasor` | Hilbert Transform — Phasor | (values) | STRUCT(inphase, quadrature) |
| 126 | Multi | `t_ht_sine` | Hilbert Transform — SineWave | (values) | STRUCT(sine, leadsine) |

---

## 1. Overlap Studies

Moving averages and price-envelope indicators.

---

### t_sma / ta_sma

Simple Moving Average.

**Scalar:** `t_sma(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_sma(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_ema / ta_ema

Exponential Moving Average.

**Scalar:** `t_ema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_ema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_wma / ta_wma

Weighted Moving Average.

**Scalar:** `t_wma(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_wma(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_dema / ta_dema

Double Exponential Moving Average.

**Scalar:** `t_dema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_dema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_tema / ta_tema

Triple Exponential Moving Average.

**Scalar:** `t_tema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_tema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_trima / ta_trima

Triangular Moving Average.

**Scalar:** `t_trima(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_trima(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_kama / ta_kama

Kaufman Adaptive Moving Average.

**Scalar:** `t_kama(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_kama(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_midpoint / ta_midpoint

Midpoint over a rolling window: `(max + min) / 2` of the input series.

**Scalar:** `t_midpoint(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_midpoint(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_midprice / ta_midprice

Midpoint Price over a rolling window: `(highest_high + lowest_low) / 2`.

**Scalar:** `t_midprice(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_midprice(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

## 2. Momentum Indicators

---

### t_rsi / ta_rsi

Relative Strength Index.

**Scalar:** `t_rsi(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_rsi(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### t_cmo / ta_cmo

Chande Momentum Oscillator.

**Scalar:** `t_cmo(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_cmo(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_mom / ta_mom

Momentum: current value minus value `time_period` bars ago.

**Scalar:** `t_mom(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_mom(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### t_roc / ta_roc

Rate of Change: `((price / prev_price) - 1) * 100`.

**Scalar:** `t_roc(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_roc(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### t_rocp / ta_rocp

Rate of Change Percentage: `(price - prev_price) / prev_price`.

**Scalar:** `t_rocp(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_rocp(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### t_rocr / ta_rocr

Rate of Change Ratio: `price / prev_price`.

**Scalar:** `t_rocr(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_rocr(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### t_rocr100 / ta_rocr100

Rate of Change Ratio ×100: `(price / prev_price) * 100`.

**Scalar:** `t_rocr100(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_rocr100(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### t_trix / ta_trix

1-day Rate-Of-Change of a Triple Smooth EMA.

**Scalar:** `t_trix(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_trix(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_willr / ta_willr

Williams' %R.

**Scalar:** `t_willr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_willr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### t_cci / ta_cci

Commodity Channel Index.

**Scalar:** `t_cci(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_cci(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### t_adx / ta_adx

Average Directional Movement Index.

**Scalar:** `t_adx(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_adx(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### t_adxr / ta_adxr

Average Directional Movement Index Rating.

**Scalar:** `t_adxr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_adxr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### t_dx / ta_dx

Directional Movement Index.

**Scalar:** `t_dx(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_dx(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### t_plus_di / ta_plus_di

Plus Directional Indicator (+DI).

**Scalar:** `t_plus_di(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_plus_di(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### t_minus_di / ta_minus_di

Minus Directional Indicator (-DI).

**Scalar:** `t_minus_di(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_minus_di(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### t_plus_dm / ta_plus_dm

Plus Directional Movement (+DM).

**Scalar:** `t_plus_dm(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_plus_dm(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

### t_minus_dm / ta_minus_dm

Minus Directional Movement (-DM).

**Scalar:** `t_minus_dm(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_minus_dm(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

## 3. Volume Indicators

---

### t_ad / ta_ad

Chaikin Accumulation/Distribution Line. No time period required.

**Scalar:** `t_ad(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, volume LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_ad(high DOUBLE, low DOUBLE, close DOUBLE, volume DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| volume / volume | LIST\<DOUBLE\> / DOUBLE | Volume series |

---

## 4. Volatility

---

### t_atr / ta_atr

Average True Range.

**Scalar:** `t_atr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_atr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### t_natr / ta_natr

Normalized Average True Range (ATR as a percentage of close).

**Scalar:** `t_natr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_natr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### t_trange / ta_trange

True Range (single-period, no `time_period` parameter).

**Scalar:** `t_trange(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_trange(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

## 5. Pattern Recognition

All candlestick functions take OHLC inputs and return `LIST<INTEGER>` (scalar) or `INTEGER` (window).  
Return values: `100` = bullish signal, `-100` = bearish signal, `0` = no pattern.

**Scalar signature:** `t_cdl<name>(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<INTEGER>`  
**Window signature:** `ta_cdl<name>(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> INTEGER`

| Function | Description |
|----------|-------------|
| `t_cdl2crows` / `ta_cdl2crows` | Two Crows |
| `t_cdl3blackcrows` / `ta_cdl3blackcrows` | Three Black Crows |
| `t_cdl3inside` / `ta_cdl3inside` | Three Inside Up/Down |
| `t_cdl3linestrike` / `ta_cdl3linestrike` | Three-Line Strike |
| `t_cdl3outside` / `ta_cdl3outside` | Three Outside Up/Down |
| `t_cdl3starsinsouth` / `ta_cdl3starsinsouth` | Three Stars In The South |
| `t_cdl3whitesoldiers` / `ta_cdl3whitesoldiers` | Three Advancing White Soldiers |
| `t_cdladvanceblock` / `ta_cdladvanceblock` | Advance Block |
| `t_cdlbelthold` / `ta_cdlbelthold` | Belt-hold |
| `t_cdlbreakaway` / `ta_cdlbreakaway` | Breakaway |
| `t_cdlclosingmarubozu` / `ta_cdlclosingmarubozu` | Closing Marubozu |
| `t_cdlconcealbabyswall` / `ta_cdlconcealbabyswall` | Concealing Baby Swallow |
| `t_cdlcounterattack` / `ta_cdlcounterattack` | Counterattack |
| `t_cdldoji` / `ta_cdldoji` | Doji |
| `t_cdldojistar` / `ta_cdldojistar` | Doji Star |
| `t_cdldragonflydoji` / `ta_cdldragonflydoji` | Dragonfly Doji |
| `t_cdlengulfing` / `ta_cdlengulfing` | Engulfing Pattern |
| `t_cdlgapsidesidewhite` / `ta_cdlgapsidesidewhite` | Up/Down-gap Side-by-Side White Lines |
| `t_cdlgravestonedoji` / `ta_cdlgravestonedoji` | Gravestone Doji |
| `t_cdlhammer` / `ta_cdlhammer` | Hammer |
| `t_cdlhangingman` / `ta_cdlhangingman` | Hanging Man |
| `t_cdlharami` / `ta_cdlharami` | Harami Pattern |
| `t_cdlharamicross` / `ta_cdlharamicross` | Harami Cross Pattern |
| `t_cdlhighwave` / `ta_cdlhighwave` | High-Wave Candle |
| `t_cdlhikkake` / `ta_cdlhikkake` | Hikkake Pattern |
| `t_cdlhikkakemod` / `ta_cdlhikkakemod` | Modified Hikkake Pattern |
| `t_cdlhomingpigeon` / `ta_cdlhomingpigeon` | Homing Pigeon |
| `t_cdlidentical3crows` / `ta_cdlidentical3crows` | Identical Three Crows |
| `t_cdlinneck` / `ta_cdlinneck` | In-Neck Pattern |
| `t_cdlinvertedhammer` / `ta_cdlinvertedhammer` | Inverted Hammer |
| `t_cdlkicking` / `ta_cdlkicking` | Kicking |
| `t_cdlkickingbylength` / `ta_cdlkickingbylength` | Kicking — bull/bear determined by the longer marubozu |
| `t_cdlladderbottom` / `ta_cdlladderbottom` | Ladder Bottom |
| `t_cdllongleggeddoji` / `ta_cdllongleggeddoji` | Long Legged Doji |
| `t_cdllongline` / `ta_cdllongline` | Long Line Candle |
| `t_cdlmarubozu` / `ta_cdlmarubozu` | Marubozu |
| `t_cdlmatchinglow` / `ta_cdlmatchinglow` | Matching Low |
| `t_cdlonneck` / `ta_cdlonneck` | On-Neck Pattern |
| `t_cdlpiercing` / `ta_cdlpiercing` | Piercing Pattern |
| `t_cdlrickshawman` / `ta_cdlrickshawman` | Rickshaw Man |
| `t_cdlrisefall3methods` / `ta_cdlrisefall3methods` | Rising/Falling Three Methods |
| `t_cdlseparatinglines` / `ta_cdlseparatinglines` | Separating Lines |
| `t_cdlshootingstar` / `ta_cdlshootingstar` | Shooting Star |
| `t_cdlshortline` / `ta_cdlshortline` | Short Line Candle |
| `t_cdlspinningtop` / `ta_cdlspinningtop` | Spinning Top |
| `t_cdlstalledpattern` / `ta_cdlstalledpattern` | Stalled Pattern |
| `t_cdlsticksandwich` / `ta_cdlsticksandwich` | Stick Sandwich |
| `t_cdltakuri` / `ta_cdltakuri` | Takuri (Dragonfly Doji with very long lower shadow) |
| `t_cdltasukigap` / `ta_cdltasukigap` | Tasuki Gap |
| `t_cdlthrusting` / `ta_cdlthrusting` | Thrusting Pattern |
| `t_cdltristar` / `ta_cdltristar` | Tristar Pattern |
| `t_cdlunique3river` / `ta_cdlunique3river` | Unique 3 River |
| `t_cdlupsidegap2crows` / `ta_cdlupsidegap2crows` | Upside Gap Two Crows |
| `t_cdlxsidegap3methods` / `ta_cdlxsidegap3methods` | Upside/Downside Gap Three Methods |

---

## 6. Price Transform

Functions that transform OHLC prices into a single series. No `time_period` parameter.

---

### t_avgprice / ta_avgprice

Average Price: `(open + high + low + close) / 4`.

**Scalar:** `t_avgprice(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_avgprice(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| open | LIST\<DOUBLE\> / DOUBLE | Open price series |
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### t_bop / ta_bop

Balance of Power: `(close - open) / (high - low)`.

**Scalar:** `t_bop(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_bop(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| open | LIST\<DOUBLE\> / DOUBLE | Open price series |
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### t_medprice / ta_medprice

Median Price: `(high + low) / 2`.

**Scalar:** `t_medprice(high LIST<DOUBLE>, low LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_medprice(high DOUBLE, low DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |

---

### t_typprice / ta_typprice

Typical Price: `(high + low + close) / 3`.

**Scalar:** `t_typprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_typprice(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### t_wclprice / ta_wclprice

Weighted Close Price: `(high + low + close * 2) / 4`.

**Scalar:** `t_wclprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_wclprice(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

## 7. Cycle Indicators

Hilbert Transform-based cycle analysis functions. No `time_period` parameter.

---

### t_ht_dcperiod / ta_ht_dcperiod

Hilbert Transform — Dominant Cycle Period.

**Scalar:** `t_ht_dcperiod(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_ht_dcperiod(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### t_ht_dcphase / ta_ht_dcphase

Hilbert Transform — Dominant Cycle Phase.

**Scalar:** `t_ht_dcphase(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_ht_dcphase(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### t_ht_trendline / ta_ht_trendline

Hilbert Transform — Instantaneous Trendline.

**Scalar:** `t_ht_trendline(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `ta_ht_trendline(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### t_ht_trendmode / ta_ht_trendmode

Hilbert Transform — Trend vs Cycle Mode. Returns `1` (trending) or `0` (cycling).

**Scalar:** `t_ht_trendmode(prices LIST<DOUBLE>) -> LIST<INTEGER>`  
**Window:** `ta_ht_trendmode(price DOUBLE) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

## 8. Statistics

---

### t_linearreg / ta_linearreg

Linear Regression: endpoint value of the linear regression line.

**Scalar:** `t_linearreg(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_linearreg(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_linearreg_angle / ta_linearreg_angle

Linear Regression Angle (degrees).

**Scalar:** `t_linearreg_angle(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_linearreg_angle(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_linearreg_intercept / ta_linearreg_intercept

Linear Regression Intercept.

**Scalar:** `t_linearreg_intercept(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_linearreg_intercept(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_linearreg_slope / ta_linearreg_slope

Linear Regression Slope.

**Scalar:** `t_linearreg_slope(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_linearreg_slope(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_tsf / ta_tsf

Time Series Forecast (next value projected by linear regression).

**Scalar:** `t_tsf(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_tsf(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_sum / ta_sum

Rolling Sum.

**Scalar:** `t_sum(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_sum(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_max / ta_max

Highest value over a rolling window.

**Scalar:** `t_max(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_max(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_min / ta_min

Lowest value over a rolling window.

**Scalar:** `t_min(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `ta_min(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_maxindex / ta_maxindex

Index of highest value over a rolling window.

**Scalar:** `t_maxindex(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<INTEGER>`  
**Window:** `ta_maxindex(price DOUBLE, time_period INTEGER) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### t_minindex / ta_minindex

Index of lowest value over a rolling window.

**Scalar:** `t_minindex(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<INTEGER>`  
**Window:** `ta_minindex(price DOUBLE, time_period INTEGER) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

## 9. Math Transform

Element-wise mathematical functions. No `time_period` parameter.

| Function | Description |
|----------|-------------|
| `t_acos` / `ta_acos` | Arc Cosine |
| `t_asin` / `ta_asin` | Arc Sine |
| `t_atan` / `ta_atan` | Arc Tangent |
| `t_ceil` / `ta_ceil` | Ceiling |
| `t_cos` / `ta_cos` | Cosine |
| `t_cosh` / `ta_cosh` | Hyperbolic Cosine |
| `t_exp` / `ta_exp` | Exponential (`e^x`) |
| `t_floor` / `ta_floor` | Floor |
| `t_ln` / `ta_ln` | Natural Logarithm |
| `t_log10` / `ta_log10` | Base-10 Logarithm |
| `t_sin` / `ta_sin` | Sine |
| `t_sinh` / `ta_sinh` | Hyperbolic Sine |
| `t_sqrt` / `ta_sqrt` | Square Root |
| `t_tan` / `ta_tan` | Tangent |
| `t_tanh` / `ta_tanh` | Hyperbolic Tangent |

**Scalar signature:** `t_<fn>(values LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window signature:** `ta_<fn>(value DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| values / value | LIST\<DOUBLE\> / DOUBLE | Input series |

---

## 10. Multi-Output Functions

These functions return a `LIST<STRUCT>` where each struct contains multiple named output fields. The scalar form only — there is no `ta_` window form for multi-output functions.

---

### t_macd

Moving Average Convergence/Divergence.

**Scalar:** `t_macd(prices LIST<DOUBLE>, fast_period INTEGER, slow_period INTEGER, signal_period INTEGER) -> LIST<STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| fast_period | INTEGER | 12 | Fast EMA period |
| slow_period | INTEGER | 26 | Slow EMA period |
| signal_period | INTEGER | 9 | Signal EMA period |

**Output fields:** `macd`, `signal`, `hist`

**Example:**
```sql
SELECT unnest(t_macd(list(close ORDER BY ts), 12, 26, 9))
FROM prices
GROUP BY symbol;
```

---

### t_bbands

Bollinger Bands.

**Scalar:** `t_bbands(prices LIST<DOUBLE>, time_period INTEGER, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INTEGER) -> LIST<STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| time_period | INTEGER | 5 | Number of periods |
| nb_dev_up | DOUBLE | 2.0 | Number of standard deviations for upper band |
| nb_dev_dn | DOUBLE | 2.0 | Number of standard deviations for lower band |
| ma_type | INTEGER | 0 | MA type (0=SMA, 1=EMA, 2=WMA, …) |

**Output fields:** `upper`, `middle`, `lower`

**Example:**
```sql
SELECT unnest(t_bbands(list(close ORDER BY ts), 20, 2.0, 2.0, 0))
FROM prices
GROUP BY symbol;
```

---

### t_stoch

Stochastic Oscillator.

**Scalar:** `t_stoch(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, fastk_period INTEGER, slowk_period INTEGER, slowk_matype INTEGER, slowd_period INTEGER, slowd_matype INTEGER) -> LIST<STRUCT(slowk DOUBLE, slowd DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| high | LIST\<DOUBLE\> | — | High price series |
| low | LIST\<DOUBLE\> | — | Low price series |
| close | LIST\<DOUBLE\> | — | Close price series |
| fastk_period | INTEGER | 5 | Fast %K period |
| slowk_period | INTEGER | 3 | Slow %K smoothing period |
| slowk_matype | INTEGER | 0 | Slow %K MA type |
| slowd_period | INTEGER | 3 | Slow %D period |
| slowd_matype | INTEGER | 0 | Slow %D MA type |

**Output fields:** `slowk`, `slowd`

---

### t_aroon

Aroon Indicator.

**Scalar:** `t_aroon(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| high | LIST\<DOUBLE\> | — | High price series |
| low | LIST\<DOUBLE\> | — | Low price series |
| time_period | INTEGER | 14 | Number of periods |

**Output fields:** `aroon_down`, `aroon_up`

---

### t_minmax

Lowest and highest values over a rolling window in a single pass.

**Scalar:** `t_minmax(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(min DOUBLE, max DOUBLE)>`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series |
| time_period | INTEGER | Number of periods |

**Output fields:** `min`, `max`

---

### t_mama

MESA Adaptive Moving Average.

**Scalar:** `t_mama(prices LIST<DOUBLE>, fast_limit DOUBLE, slow_limit DOUBLE) -> LIST<STRUCT(mama DOUBLE, fama DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| fast_limit | DOUBLE | 0.5 | Upper limit for the adaptive factor |
| slow_limit | DOUBLE | 0.05 | Lower limit for the adaptive factor |

**Output fields:** `mama` (MESA Adaptive MA), `fama` (Following Adaptive MA)

---

### t_ht_phasor

Hilbert Transform — Phasor Components.

**Scalar:** `t_ht_phasor(prices LIST<DOUBLE>) -> LIST<STRUCT(inphase DOUBLE, quadrature DOUBLE)>`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series (typically close) |

**Output fields:** `inphase`, `quadrature`

---

### t_ht_sine

Hilbert Transform — SineWave Indicator.

**Scalar:** `t_ht_sine(prices LIST<DOUBLE>) -> LIST<STRUCT(sine DOUBLE, leadsine DOUBLE)>`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series (typically close) |

**Output fields:** `sine`, `leadsine`

---

## Usage Patterns

### Scalar form — aggregate over a group

```sql
-- Compute 14-period RSI for each symbol
SELECT
    symbol,
    t_rsi(list(close ORDER BY ts), 14) AS rsi_series
FROM prices
GROUP BY symbol;
```

### Window form — row-by-row with OVER()

```sql
-- Attach current RSI value to every row
SELECT
    symbol,
    ts,
    close,
    ta_rsi(close, 14) OVER (PARTITION BY symbol ORDER BY ts ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS rsi
FROM prices;
```

### Unpacking multi-output structs

```sql
-- Unpack MACD struct fields
SELECT
    symbol,
    ts,
    r.macd,
    r.signal,
    r.hist
FROM (
    SELECT symbol, unnest(t_macd(list(close ORDER BY ts), 12, 26, 9)) AS r
    FROM prices
    GROUP BY symbol
);
```

### MA type reference

Several functions accept an integer `ma_type` parameter:

| Value | Type |
|-------|------|
| 0 | SMA |
| 1 | EMA |
| 2 | WMA |
| 3 | DEMA |
| 4 | TEMA |
| 5 | TRIMA |
| 6 | KAMA |
| 7 | MAMA |
| 8 | T3 |
