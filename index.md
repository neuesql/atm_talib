# DuckDB TA-Lib Extension — Function Reference

This extension wraps [TA-Lib](https://ta-lib.org/) technical analysis functions for use inside DuckDB SQL queries.

## Function Forms

Every indicator is available in two forms:

- **Scalar** (`t_` prefix): accepts `LIST<DOUBLE>` inputs, returns a `LIST` of the same length (leading lookback positions are `NULL`).
- **Window / Aggregate** (`ta_` prefix): accepts plain scalar columns and is used with a window `OVER (...)` clause, returning one value per row.

### Return-type notes

- Most functions return `DOUBLE`. Pattern recognition functions return `INTEGER` (`100` = bullish, `-100` = bearish, `0` = no pattern).
- Multi-output window functions return `STRUCT` — access fields with dot notation: `ta_macd(...).macd`.
- Multi-output scalar functions return `LIST<STRUCT>` — unpack with `unnest()`.

---

## Function Index

| Category | Scalar | Window | Description |
|----------|--------|--------|-------------|
| **Overlap Studies** | | | |
| Overlap | `t_sma` | `ta_sma` | Simple Moving Average |
| Overlap | `t_ema` | `ta_ema` | Exponential Moving Average |
| Overlap | `t_wma` | `ta_wma` | Weighted Moving Average |
| Overlap | `t_dema` | `ta_dema` | Double Exponential Moving Average |
| Overlap | `t_tema` | `ta_tema` | Triple Exponential Moving Average |
| Overlap | `t_trima` | `ta_trima` | Triangular Moving Average |
| Overlap | `t_kama` | `ta_kama` | Kaufman Adaptive Moving Average |
| Overlap | `t_midpoint` | `ta_midpoint` | MidPoint over period |
| Overlap | `t_midprice` | `ta_midprice` | Midpoint Price over period |
| **Momentum Indicators** | | | |
| Momentum | `t_rsi` | `ta_rsi` | Relative Strength Index |
| Momentum | `t_cmo` | `ta_cmo` | Chande Momentum Oscillator |
| Momentum | `t_mom` | `ta_mom` | Momentum |
| Momentum | `t_roc` | `ta_roc` | Rate of Change |
| Momentum | `t_rocp` | `ta_rocp` | Rate of Change (Percentage) |
| Momentum | `t_rocr` | `ta_rocr` | Rate of Change (Ratio) |
| Momentum | `t_rocr100` | `ta_rocr100` | Rate of Change (Ratio x100) |
| Momentum | `t_trix` | `ta_trix` | Triple Smooth EMA Rate of Change |
| Momentum | `t_willr` | `ta_willr` | Williams' %R |
| Momentum | `t_cci` | `ta_cci` | Commodity Channel Index |
| Momentum | `t_adx` | `ta_adx` | Average Directional Movement Index |
| Momentum | `t_adxr` | `ta_adxr` | Average Directional Movement Rating |
| Momentum | `t_dx` | `ta_dx` | Directional Movement Index |
| Momentum | `t_plus_di` | `ta_plus_di` | Plus Directional Indicator |
| Momentum | `t_minus_di` | `ta_minus_di` | Minus Directional Indicator |
| Momentum | `t_plus_dm` | `ta_plus_dm` | Plus Directional Movement |
| Momentum | `t_minus_dm` | `ta_minus_dm` | Minus Directional Movement |
| **Volume Indicators** | | | |
| Volume | `t_ad` | `ta_ad` | Chaikin A/D Line |
| **Volatility** | | | |
| Volatility | `t_atr` | `ta_atr` | Average True Range |
| Volatility | `t_natr` | `ta_natr` | Normalized Average True Range |
| Volatility | `t_trange` | `ta_trange` | True Range |
| **Price Transform** | | | |
| Price | `t_avgprice` | `ta_avgprice` | Average Price (O+H+L+C)/4 |
| Price | `t_bop` | `ta_bop` | Balance of Power |
| Price | `t_medprice` | `ta_medprice` | Median Price (H+L)/2 |
| Price | `t_typprice` | `ta_typprice` | Typical Price (H+L+C)/3 |
| Price | `t_wclprice` | `ta_wclprice` | Weighted Close (H+L+2C)/4 |
| **Cycle Indicators** | | | |
| Cycle | `t_ht_dcperiod` | `ta_ht_dcperiod` | Dominant Cycle Period |
| Cycle | `t_ht_dcphase` | `ta_ht_dcphase` | Dominant Cycle Phase |
| Cycle | `t_ht_trendline` | `ta_ht_trendline` | Instantaneous Trendline |
| Cycle | `t_ht_trendmode` | `ta_ht_trendmode` | Trend vs Cycle Mode |
| **Statistics** | | | |
| Stats | `t_linearreg` | `ta_linearreg` | Linear Regression |
| Stats | `t_linearreg_angle` | `ta_linearreg_angle` | Linear Regression Angle |
| Stats | `t_linearreg_intercept` | `ta_linearreg_intercept` | Linear Regression Intercept |
| Stats | `t_linearreg_slope` | `ta_linearreg_slope` | Linear Regression Slope |
| Stats | `t_tsf` | `ta_tsf` | Time Series Forecast |
| Stats | `t_sum` | `ta_sum` | Summation |
| Stats | `t_max` | `ta_max` | Highest Value over period |
| Stats | `t_min` | `ta_min` | Lowest Value over period |
| Stats | `t_maxindex` | `ta_maxindex` | Index of Highest Value |
| Stats | `t_minindex` | `ta_minindex` | Index of Lowest Value |
| **Math Transform** (scalar only) | | | |
| Math | `t_acos` | — | Arc Cosine |
| Math | `t_asin` | — | Arc Sine |
| Math | `t_atan` | — | Arc Tangent |
| Math | `t_ceil` | — | Ceiling |
| Math | `t_cos` | — | Cosine |
| Math | `t_cosh` | — | Hyperbolic Cosine |
| Math | `t_exp` | — | Exponential (e^x) |
| Math | `t_floor` | — | Floor |
| Math | `t_ln` | — | Natural Logarithm |
| Math | `t_log10` | — | Base-10 Logarithm |
| Math | `t_sin` | — | Sine |
| Math | `t_sinh` | — | Hyperbolic Sine |
| Math | `t_sqrt` | — | Square Root |
| Math | `t_tan` | — | Tangent |
| Math | `t_tanh` | — | Hyperbolic Tangent |
| **Pattern Recognition** (scalar only) | | | |
| Pattern | `t_cdl2crows` | — | Two Crows |
| Pattern | `t_cdl3blackcrows` | — | Three Black Crows |
| Pattern | `t_cdl3inside` | — | Three Inside Up/Down |
| Pattern | `t_cdl3linestrike` | — | Three-Line Strike |
| Pattern | `t_cdl3outside` | — | Three Outside Up/Down |
| Pattern | `t_cdl3starsinsouth` | — | Three Stars In The South |
| Pattern | `t_cdl3whitesoldiers` | — | Three Advancing White Soldiers |
| Pattern | `t_cdladvanceblock` | — | Advance Block |
| Pattern | `t_cdlbelthold` | — | Belt-hold |
| Pattern | `t_cdlbreakaway` | — | Breakaway |
| Pattern | `t_cdlclosingmarubozu` | — | Closing Marubozu |
| Pattern | `t_cdlconcealbabyswall` | — | Concealing Baby Swallow |
| Pattern | `t_cdlcounterattack` | — | Counterattack |
| Pattern | `t_cdldoji` | — | Doji |
| Pattern | `t_cdldojistar` | — | Doji Star |
| Pattern | `t_cdldragonflydoji` | — | Dragonfly Doji |
| Pattern | `t_cdlengulfing` | — | Engulfing Pattern |
| Pattern | `t_cdlgapsidesidewhite` | — | Gap Side-by-Side White Lines |
| Pattern | `t_cdlgravestonedoji` | — | Gravestone Doji |
| Pattern | `t_cdlhammer` | — | Hammer |
| Pattern | `t_cdlhangingman` | — | Hanging Man |
| Pattern | `t_cdlharami` | — | Harami Pattern |
| Pattern | `t_cdlharamicross` | — | Harami Cross Pattern |
| Pattern | `t_cdlhighwave` | — | High-Wave Candle |
| Pattern | `t_cdlhikkake` | — | Hikkake Pattern |
| Pattern | `t_cdlhikkakemod` | — | Modified Hikkake Pattern |
| Pattern | `t_cdlhomingpigeon` | — | Homing Pigeon |
| Pattern | `t_cdlidentical3crows` | — | Identical Three Crows |
| Pattern | `t_cdlinneck` | — | In-Neck Pattern |
| Pattern | `t_cdlinvertedhammer` | — | Inverted Hammer |
| Pattern | `t_cdlkicking` | — | Kicking |
| Pattern | `t_cdlkickingbylength` | — | Kicking (by longer marubozu) |
| Pattern | `t_cdlladderbottom` | — | Ladder Bottom |
| Pattern | `t_cdllongleggeddoji` | — | Long Legged Doji |
| Pattern | `t_cdllongline` | — | Long Line Candle |
| Pattern | `t_cdlmarubozu` | — | Marubozu |
| Pattern | `t_cdlmatchinglow` | — | Matching Low |
| Pattern | `t_cdlonneck` | — | On-Neck Pattern |
| Pattern | `t_cdlpiercing` | — | Piercing Pattern |
| Pattern | `t_cdlrickshawman` | — | Rickshaw Man |
| Pattern | `t_cdlrisefall3methods` | — | Rising/Falling Three Methods |
| Pattern | `t_cdlseparatinglines` | — | Separating Lines |
| Pattern | `t_cdlshootingstar` | — | Shooting Star |
| Pattern | `t_cdlshortline` | — | Short Line Candle |
| Pattern | `t_cdlspinningtop` | — | Spinning Top |
| Pattern | `t_cdlstalledpattern` | — | Stalled Pattern |
| Pattern | `t_cdlsticksandwich` | — | Stick Sandwich |
| Pattern | `t_cdltakuri` | — | Takuri (long lower shadow Doji) |
| Pattern | `t_cdltasukigap` | — | Tasuki Gap |
| Pattern | `t_cdlthrusting` | — | Thrusting Pattern |
| Pattern | `t_cdltristar` | — | Tristar Pattern |
| Pattern | `t_cdlunique3river` | — | Unique 3 River |
| Pattern | `t_cdlupsidegap2crows` | — | Upside Gap Two Crows |
| Pattern | `t_cdlxsidegap3methods` | — | Upside/Downside Gap Three Methods |
| **Multi-Output** | | | |
| Multi | `t_macd` | `ta_macd` | MACD |
| Multi | `t_bbands` | `ta_bbands` | Bollinger Bands |
| Multi | `t_stoch` | `ta_stoch` | Stochastic |
| Multi | `t_aroon` | `ta_aroon` | Aroon |
| Multi | `t_minmax` | `ta_minmax` | Min/Max over period |
| Multi | `t_mama` | `ta_mama` | MESA Adaptive Moving Average |
| Multi | `t_ht_phasor` | `ta_ht_phasor` | Hilbert Transform — Phasor |
| Multi | `t_ht_sine` | `ta_ht_sine` | Hilbert Transform — SineWave |

---

## Function Signatures

| # | Function | Signature | Return |
|---|----------|-----------|--------|
| | **Overlap Studies** | | |
| 1 | `t_sma` / `ta_sma` | (values, period) | DOUBLE |
| 2 | `t_ema` / `ta_ema` | (values, period) | DOUBLE |
| 3 | `t_wma` / `ta_wma` | (values, period) | DOUBLE |
| 4 | `t_dema` / `ta_dema` | (values, period) | DOUBLE |
| 5 | `t_tema` / `ta_tema` | (values, period) | DOUBLE |
| 6 | `t_trima` / `ta_trima` | (values, period) | DOUBLE |
| 7 | `t_kama` / `ta_kama` | (values, period) | DOUBLE |
| 8 | `t_midpoint` / `ta_midpoint` | (values, period) | DOUBLE |
| 9 | `t_midprice` / `ta_midprice` | (high, low, period) | DOUBLE |
| | **Momentum Indicators** | | |
| 10 | `t_rsi` / `ta_rsi` | (values, period) | DOUBLE |
| 11 | `t_cmo` / `ta_cmo` | (values, period) | DOUBLE |
| 12 | `t_mom` / `ta_mom` | (values, period) | DOUBLE |
| 13 | `t_roc` / `ta_roc` | (values, period) | DOUBLE |
| 14 | `t_rocp` / `ta_rocp` | (values, period) | DOUBLE |
| 15 | `t_rocr` / `ta_rocr` | (values, period) | DOUBLE |
| 16 | `t_rocr100` / `ta_rocr100` | (values, period) | DOUBLE |
| 17 | `t_trix` / `ta_trix` | (values, period) | DOUBLE |
| 18 | `t_willr` / `ta_willr` | (high, low, close, period) | DOUBLE |
| 19 | `t_cci` / `ta_cci` | (high, low, close, period) | DOUBLE |
| 20 | `t_adx` / `ta_adx` | (high, low, close, period) | DOUBLE |
| 21 | `t_adxr` / `ta_adxr` | (high, low, close, period) | DOUBLE |
| 22 | `t_dx` / `ta_dx` | (high, low, close, period) | DOUBLE |
| 23 | `t_plus_di` / `ta_plus_di` | (high, low, close, period) | DOUBLE |
| 24 | `t_minus_di` / `ta_minus_di` | (high, low, close, period) | DOUBLE |
| 25 | `t_plus_dm` / `ta_plus_dm` | (high, low, period) | DOUBLE |
| 26 | `t_minus_dm` / `ta_minus_dm` | (high, low, period) | DOUBLE |
| | **Volume Indicators** | | |
| 27 | `t_ad` / `ta_ad` | (high, low, close, volume) | DOUBLE |
| | **Volatility** | | |
| 28 | `t_atr` / `ta_atr` | (high, low, close, period) | DOUBLE |
| 29 | `t_natr` / `ta_natr` | (high, low, close, period) | DOUBLE |
| 30 | `t_trange` / `ta_trange` | (high, low, close) | DOUBLE |
| | **Price Transform** | | |
| 31 | `t_avgprice` / `ta_avgprice` | (open, high, low, close) | DOUBLE |
| 32 | `t_bop` / `ta_bop` | (open, high, low, close) | DOUBLE |
| 33 | `t_medprice` / `ta_medprice` | (high, low) | DOUBLE |
| 34 | `t_typprice` / `ta_typprice` | (high, low, close) | DOUBLE |
| 35 | `t_wclprice` / `ta_wclprice` | (high, low, close) | DOUBLE |
| | **Cycle Indicators** | | |
| 36 | `t_ht_dcperiod` / `ta_ht_dcperiod` | (values) | DOUBLE |
| 37 | `t_ht_dcphase` / `ta_ht_dcphase` | (values) | DOUBLE |
| 38 | `t_ht_trendline` / `ta_ht_trendline` | (values) | DOUBLE |
| 39 | `t_ht_trendmode` / `ta_ht_trendmode` | (values) | INTEGER |
| | **Statistics** | | |
| 40 | `t_linearreg` / `ta_linearreg` | (values, period) | DOUBLE |
| 41 | `t_linearreg_angle` / `ta_linearreg_angle` | (values, period) | DOUBLE |
| 42 | `t_linearreg_intercept` / `ta_linearreg_intercept` | (values, period) | DOUBLE |
| 43 | `t_linearreg_slope` / `ta_linearreg_slope` | (values, period) | DOUBLE |
| 44 | `t_tsf` / `ta_tsf` | (values, period) | DOUBLE |
| 45 | `t_sum` / `ta_sum` | (values, period) | DOUBLE |
| 46 | `t_max` / `ta_max` | (values, period) | DOUBLE |
| 47 | `t_min` / `ta_min` | (values, period) | DOUBLE |
| 48 | `t_maxindex` / `ta_maxindex` | (values, period) | INTEGER |
| 49 | `t_minindex` / `ta_minindex` | (values, period) | INTEGER |
| | **Math Transform** (scalar only) | | |
| 50 | `t_acos` | (values) | DOUBLE |
| 51 | `t_asin` | (values) | DOUBLE |
| 52 | `t_atan` | (values) | DOUBLE |
| 53 | `t_ceil` | (values) | DOUBLE |
| 54 | `t_cos` | (values) | DOUBLE |
| 55 | `t_cosh` | (values) | DOUBLE |
| 56 | `t_exp` | (values) | DOUBLE |
| 57 | `t_floor` | (values) | DOUBLE |
| 58 | `t_ln` | (values) | DOUBLE |
| 59 | `t_log10` | (values) | DOUBLE |
| 60 | `t_sin` | (values) | DOUBLE |
| 61 | `t_sinh` | (values) | DOUBLE |
| 62 | `t_sqrt` | (values) | DOUBLE |
| 63 | `t_tan` | (values) | DOUBLE |
| 64 | `t_tanh` | (values) | DOUBLE |
| | **Pattern Recognition** (scalar only) | | |
| 65 | `t_cdl2crows` | (open, high, low, close) | INTEGER |
| 66 | `t_cdl3blackcrows` | (open, high, low, close) | INTEGER |
| 67 | `t_cdl3inside` | (open, high, low, close) | INTEGER |
| 68 | `t_cdl3linestrike` | (open, high, low, close) | INTEGER |
| 69 | `t_cdl3outside` | (open, high, low, close) | INTEGER |
| 70 | `t_cdl3starsinsouth` | (open, high, low, close) | INTEGER |
| 71 | `t_cdl3whitesoldiers` | (open, high, low, close) | INTEGER |
| 72 | `t_cdladvanceblock` | (open, high, low, close) | INTEGER |
| 73 | `t_cdlbelthold` | (open, high, low, close) | INTEGER |
| 74 | `t_cdlbreakaway` | (open, high, low, close) | INTEGER |
| 75 | `t_cdlclosingmarubozu` | (open, high, low, close) | INTEGER |
| 76 | `t_cdlconcealbabyswall` | (open, high, low, close) | INTEGER |
| 77 | `t_cdlcounterattack` | (open, high, low, close) | INTEGER |
| 78 | `t_cdldoji` | (open, high, low, close) | INTEGER |
| 79 | `t_cdldojistar` | (open, high, low, close) | INTEGER |
| 80 | `t_cdldragonflydoji` | (open, high, low, close) | INTEGER |
| 81 | `t_cdlengulfing` | (open, high, low, close) | INTEGER |
| 82 | `t_cdlgapsidesidewhite` | (open, high, low, close) | INTEGER |
| 83 | `t_cdlgravestonedoji` | (open, high, low, close) | INTEGER |
| 84 | `t_cdlhammer` | (open, high, low, close) | INTEGER |
| 85 | `t_cdlhangingman` | (open, high, low, close) | INTEGER |
| 86 | `t_cdlharami` | (open, high, low, close) | INTEGER |
| 87 | `t_cdlharamicross` | (open, high, low, close) | INTEGER |
| 88 | `t_cdlhighwave` | (open, high, low, close) | INTEGER |
| 89 | `t_cdlhikkake` | (open, high, low, close) | INTEGER |
| 90 | `t_cdlhikkakemod` | (open, high, low, close) | INTEGER |
| 91 | `t_cdlhomingpigeon` | (open, high, low, close) | INTEGER |
| 92 | `t_cdlidentical3crows` | (open, high, low, close) | INTEGER |
| 93 | `t_cdlinneck` | (open, high, low, close) | INTEGER |
| 94 | `t_cdlinvertedhammer` | (open, high, low, close) | INTEGER |
| 95 | `t_cdlkicking` | (open, high, low, close) | INTEGER |
| 96 | `t_cdlkickingbylength` | (open, high, low, close) | INTEGER |
| 97 | `t_cdlladderbottom` | (open, high, low, close) | INTEGER |
| 98 | `t_cdllongleggeddoji` | (open, high, low, close) | INTEGER |
| 99 | `t_cdllongline` | (open, high, low, close) | INTEGER |
| 100 | `t_cdlmarubozu` | (open, high, low, close) | INTEGER |
| 101 | `t_cdlmatchinglow` | (open, high, low, close) | INTEGER |
| 102 | `t_cdlonneck` | (open, high, low, close) | INTEGER |
| 103 | `t_cdlpiercing` | (open, high, low, close) | INTEGER |
| 104 | `t_cdlrickshawman` | (open, high, low, close) | INTEGER |
| 105 | `t_cdlrisefall3methods` | (open, high, low, close) | INTEGER |
| 106 | `t_cdlseparatinglines` | (open, high, low, close) | INTEGER |
| 107 | `t_cdlshootingstar` | (open, high, low, close) | INTEGER |
| 108 | `t_cdlshortline` | (open, high, low, close) | INTEGER |
| 109 | `t_cdlspinningtop` | (open, high, low, close) | INTEGER |
| 110 | `t_cdlstalledpattern` | (open, high, low, close) | INTEGER |
| 111 | `t_cdlsticksandwich` | (open, high, low, close) | INTEGER |
| 112 | `t_cdltakuri` | (open, high, low, close) | INTEGER |
| 113 | `t_cdltasukigap` | (open, high, low, close) | INTEGER |
| 114 | `t_cdlthrusting` | (open, high, low, close) | INTEGER |
| 115 | `t_cdltristar` | (open, high, low, close) | INTEGER |
| 116 | `t_cdlunique3river` | (open, high, low, close) | INTEGER |
| 117 | `t_cdlupsidegap2crows` | (open, high, low, close) | INTEGER |
| 118 | `t_cdlxsidegap3methods` | (open, high, low, close) | INTEGER |
| | **Multi-Output** | | |
| 119 | `t_macd` / `ta_macd` | (values, fast_period, slow_period, signal_period) | STRUCT(macd, signal, hist) |
| 120 | `t_bbands` / `ta_bbands` | (values, period, devup, devdn, matype) | STRUCT(upper, middle, lower) |
| 121 | `t_stoch` / `ta_stoch` | (high, low, close, fastK, slowK, slowKMA, slowD, slowDMA) | STRUCT(slowk, slowd) |
| 122 | `t_aroon` / `ta_aroon` | (high, low, period) | STRUCT(aroon_down, aroon_up) |
| 123 | `t_minmax` / `ta_minmax` | (values, period) | STRUCT(min, max) |
| 124 | `t_mama` / `ta_mama` | (values, fast_limit, slow_limit) | STRUCT(mama, fama) |
| 125 | `t_ht_phasor` / `ta_ht_phasor` | (values) | STRUCT(inphase, quadrature) |
| 126 | `t_ht_sine` / `ta_ht_sine` | (values) | STRUCT(sine, leadsine) |

---

## Function Details

### Overlap Studies

- **sma** — Simple Moving Average: `sum(close, n) / n`
- **ema** — Exponential Moving Average: `close * k + ema_prev * (1 - k)`, where `k = 2 / (n + 1)`
- **wma** — Weighted Moving Average: `sum(close_i * weight_i) / sum(weights)`, weights linearly increase
- **dema** — Double EMA: `2 * EMA(n) - EMA(EMA(n))`
- **tema** — Triple EMA: `3 * EMA - 3 * EMA(EMA) + EMA(EMA(EMA))`
- **trima** — Triangular Moving Average: `SMA(SMA(n))`
- **kama** — Kaufman Adaptive MA: adapts smoothing constant based on price efficiency ratio
- **midpoint** — Midpoint over period: `(max(close, n) + min(close, n)) / 2`
- **midprice** — Midpoint Price: `(highest_high(n) + lowest_low(n)) / 2`

### Momentum Indicators

- **rsi** — Relative Strength Index: `100 - 100 / (1 + avg_gain / avg_loss)`
- **cmo** — Chande Momentum Oscillator: `(sum_up - sum_down) / (sum_up + sum_down) * 100`
- **mom** — Momentum: `close - close[n]`
- **roc** — Rate of Change: `(close - close[n]) / close[n] * 100`
- **rocp** — Rate of Change Percentage: `(close - close[n]) / close[n]`
- **rocr** — Rate of Change Ratio: `close / close[n]`
- **rocr100** — Rate of Change Ratio x100: `close / close[n] * 100`
- **trix** — Triple Smooth EMA ROC: 1-day percent change of triple-smoothed EMA
- **willr** — Williams %R: `(highest_high - close) / (highest_high - lowest_low) * -100`
- **cci** — Commodity Channel Index: `(tp - sma(tp)) / (0.015 * mean_deviation)`
- **adx** — Average Directional Index: smoothed average of DX over n periods
- **adxr** — ADX Rating: `(adx + adx[n]) / 2`
- **dx** — Directional Movement Index: `|+DI - -DI| / (+DI + -DI) * 100`
- **plus_di** — Plus Directional Indicator: `smoothed(+DM) / ATR * 100`
- **minus_di** — Minus Directional Indicator: `smoothed(-DM) / ATR * 100`
- **plus_dm** — Plus Directional Movement: `high - prev_high` if positive and > `prev_low - low`
- **minus_dm** — Minus Directional Movement: `prev_low - low` if positive and > `high - prev_high`

### Volume Indicators

- **ad** — Chaikin A/D Line: `cumsum(((close - low) - (high - close)) / (high - low) * volume)`

### Volatility

- **atr** — Average True Range: smoothed average of `true_range` over n periods
- **natr** — Normalized ATR: `ATR / close * 100`
- **trange** — True Range: `max(high - low, |high - prev_close|, |low - prev_close|)`

### Price Transform

- **avgprice** — Average Price: `(open + high + low + close) / 4`
- **bop** — Balance of Power: `(close - open) / (high - low)`
- **medprice** — Median Price: `(high + low) / 2`
- **typprice** — Typical Price: `(high + low + close) / 3`
- **wclprice** — Weighted Close: `(high + low + 2 * close) / 4`

### Cycle Indicators

- **ht_dcperiod** — Dominant Cycle Period: Hilbert Transform-derived dominant cycle length
- **ht_dcphase** — Dominant Cycle Phase: phase angle of the dominant cycle (degrees)
- **ht_trendline** — Instantaneous Trendline: Hilbert Transform smoothed trendline
- **ht_trendmode** — Trend vs Cycle Mode: returns 1 (trending) or 0 (cycling)

### Statistics

- **linearreg** — Linear Regression: endpoint value of best-fit line over n periods
- **linearreg_angle** — Linear Reg Angle: `atan(slope)` in degrees
- **linearreg_intercept** — Linear Reg Intercept: y-intercept of best-fit line
- **linearreg_slope** — Linear Reg Slope: slope of best-fit line over n periods
- **tsf** — Time Series Forecast: projected next value from linear regression
- **sum** — Rolling Sum: `sum(values, n)`
- **max** — Highest Value: `max(values, n)`
- **min** — Lowest Value: `min(values, n)`
- **maxindex** — Index of Highest: position of max value within the window
- **minindex** — Index of Lowest: position of min value within the window

### Math Transform

- **acos** — Arc Cosine: `acos(x)`
- **asin** — Arc Sine: `asin(x)`
- **atan** — Arc Tangent: `atan(x)`
- **ceil** — Ceiling: smallest integer >= x
- **cos** — Cosine: `cos(x)`
- **cosh** — Hyperbolic Cosine: `cosh(x)`
- **exp** — Exponential: `e^x`
- **floor** — Floor: largest integer <= x
- **ln** — Natural Logarithm: `ln(x)`
- **log10** — Base-10 Logarithm: `log10(x)`
- **sin** — Sine: `sin(x)`
- **sinh** — Hyperbolic Sine: `sinh(x)`
- **sqrt** — Square Root: `sqrt(x)`
- **tan** — Tangent: `tan(x)`
- **tanh** — Hyperbolic Tangent: `tanh(x)`

### Pattern Recognition

All candlestick patterns take (open, high, low, close) and return `100` (bullish), `-100` (bearish), or `0` (no pattern).

- **cdl2crows** — Two Crows: bearish reversal, two black candles gapping above uptrend
- **cdl3blackcrows** — Three Black Crows: three consecutive long bearish candles
- **cdl3inside** — Three Inside Up/Down: reversal confirmed by third candle breaking first candle's range
- **cdl3linestrike** — Three-Line Strike: three same-direction candles followed by a strike candle
- **cdl3outside** — Three Outside Up/Down: engulfing pattern confirmed by third candle
- **cdl3starsinsouth** — Three Stars In The South: three declining bearish candles with shrinking bodies
- **cdl3whitesoldiers** — Three Advancing White Soldiers: three consecutive long bullish candles
- **cdladvanceblock** — Advance Block: three bullish candles with shrinking bodies (weakening)
- **cdlbelthold** — Belt-hold: long candle opening at its extreme (marubozu-like)
- **cdlbreakaway** — Breakaway: gap followed by three candles, then reversal closing the gap
- **cdlclosingmarubozu** — Closing Marubozu: candle with no shadow on the closing side
- **cdlconcealbabyswall** — Concealing Baby Swallow: four-candle bearish pattern with harami
- **cdlcounterattack** — Counterattack: two opposite candles with same close
- **cdldoji** — Doji: open and close are virtually the same
- **cdldojistar** — Doji Star: doji gapping away from previous candle
- **cdldragonflydoji** — Dragonfly Doji: doji with long lower shadow, no upper shadow
- **cdlengulfing** — Engulfing: second candle completely engulfs the first
- **cdlgapsidesidewhite** — Gap Side-by-Side White: two white candles of similar size after a gap
- **cdlgravestonedoji** — Gravestone Doji: doji with long upper shadow, no lower shadow
- **cdlhammer** — Hammer: small body at top, long lower shadow (2x body)
- **cdlhangingman** — Hanging Man: hammer shape appearing in uptrend (bearish)
- **cdlharami** — Harami: small candle contained within previous candle's body
- **cdlharamicross** — Harami Cross: harami where second candle is a doji
- **cdlhighwave** — High-Wave Candle: very long upper and lower shadows, small body
- **cdlhikkake** — Hikkake: inside bar breakout failure pattern
- **cdlhikkakemod** — Modified Hikkake: hikkake confirmed by subsequent price action
- **cdlhomingpigeon** — Homing Pigeon: two bearish candles, second inside first (bullish)
- **cdlidentical3crows** — Identical Three Crows: three black crows each opening at prior close
- **cdlinneck** — In-Neck: bearish candle followed by small bullish closing near prior low
- **cdlinvertedhammer** — Inverted Hammer: small body at bottom, long upper shadow
- **cdlkicking** — Kicking: two opposing marubozu with a gap between
- **cdlkickingbylength** — Kicking by Length: direction determined by longer marubozu
- **cdlladderbottom** — Ladder Bottom: four bearish candles then bullish reversal
- **cdllongleggeddoji** — Long Legged Doji: doji with very long upper and lower shadows
- **cdllongline** — Long Line Candle: candle with unusually long body
- **cdlmarubozu** — Marubozu: candle with no shadows (pure body)
- **cdlmatchinglow** — Matching Low: two bearish candles with same close (support)
- **cdlonneck** — On-Neck: bearish candle followed by small bullish closing at prior low
- **cdlpiercing** — Piercing: bullish candle closing above midpoint of prior bearish candle
- **cdlrickshawman** — Rickshaw Man: long-legged doji with body at center
- **cdlrisefall3methods** — Rising/Falling Three Methods: trend continuation with small counter-trend candles
- **cdlseparatinglines** — Separating Lines: two opposite candles opening at same price
- **cdlshootingstar** — Shooting Star: inverted hammer in uptrend (bearish reversal)
- **cdlshortline** — Short Line Candle: candle with unusually short body
- **cdlspinningtop** — Spinning Top: small body with upper and lower shadows
- **cdlstalledpattern** — Stalled Pattern: three bullish candles, third with small body (slowing)
- **cdlsticksandwich** — Stick Sandwich: two same-close candles sandwiching an opposite candle
- **cdltakuri** — Takuri: dragonfly doji with very long lower shadow
- **cdltasukigap** — Tasuki Gap: gap followed by a partial-fill candle (continuation)
- **cdlthrusting** — Thrusting: bearish candle then bullish closing below midpoint of first
- **cdltristar** — Tristar: three consecutive dojis with middle gapping (reversal)
- **cdlunique3river** — Unique 3 River: three-candle bullish reversal at bottom
- **cdlupsidegap2crows** — Upside Gap Two Crows: two bearish candles gapping above uptrend
- **cdlxsidegap3methods** — Gap Three Methods: gap followed by opposite candle closing the gap

### Multi-Output

- **macd** — MACD: `macd = EMA(fast) - EMA(slow)`, `signal = EMA(macd, signal_period)`, `hist = macd - signal`
- **bbands** — Bollinger Bands: `middle = SMA(n)`, `upper = middle + k * stddev`, `lower = middle - k * stddev`
- **stoch** — Stochastic: `%K = (close - lowest_low) / (highest_high - lowest_low) * 100`, `%D = SMA(%K)`
- **aroon** — Aroon: `up = (n - bars_since_high) / n * 100`, `down = (n - bars_since_low) / n * 100`
- **minmax** — Min/Max: lowest and highest values over rolling window in single pass
- **mama** — MESA Adaptive MA: phase-adaptive moving average with fast/slow limits
- **ht_phasor** — Hilbert Phasor: in-phase and quadrature components of the dominant cycle
- **ht_sine** — Hilbert SineWave: sine and lead-sine of the dominant cycle phase

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
    ta_rsi(close, 14) OVER (PARTITION BY symbol ORDER BY ts) AS rsi
FROM prices;
```

### Unpacking multi-output structs (scalar)

```sql
-- Unpack MACD struct fields from scalar form
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

### Multi-output window form — STRUCT per row

```sql
-- MACD with dot-notation field access
SELECT symbol, ts, close, m.macd, m.signal, m.hist
FROM (
    SELECT *, ta_macd(close, 12, 26, 9) OVER (PARTITION BY symbol ORDER BY ts) AS m
    FROM prices
);

-- Bollinger Bands
SELECT ts, b.upper, b.middle, b.lower
FROM (
    SELECT *, ta_bbands(close, 20, 2.0, 2.0, 0) OVER (ORDER BY ts) AS b
    FROM prices
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
