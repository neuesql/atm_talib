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

| **SMA** | Simple Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average) |
|---|---|---|
| $SMA = \frac{1}{n}\sum_{i=0}^{n-1} x_{t-i}$ |||

| **EMA** | Exponential Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Exponential_smoothing) |
|---|---|---|
| $EMA_t = \alpha \cdot x_t + (1 - \alpha) \cdot EMA_{t-1}, \quad \alpha = \frac{2}{n+1}$ |||

| **WMA** | Weighted Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average) |
|---|---|---|
| $WMA = \frac{\sum_{i=0}^{n-1}(n-i) \cdot x_{t-i}}{\sum_{i=1}^{n} i}$ |||

| **DEMA** | Double Exponential Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Double_exponential_moving_average) |
|---|---|---|
| $DEMA = 2 \cdot EMA(x, n) - EMA(EMA(x, n), n)$ |||

| **TEMA** | Triple Exponential Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Triple_exponential_moving_average) |
|---|---|---|
| $TEMA = 3E_1 - 3E_2 + E_3, \quad E_1 = EMA,\ E_2 = EMA(E_1),\ E_3 = EMA(E_2)$ |||

| **TRIMA** | Triangular Moving Average | [Investopedia](https://www.investopedia.com/terms/t/triangularmoving-average.asp) |
|---|---|---|
| $TRIMA = SMA(SMA(x, m), m), \quad m = \lceil(n+1)/2\rceil$ |||

| **KAMA** | Kaufman Adaptive Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/Kaufman%27s_Adaptive_Moving_Average) |
|---|---|---|
| $KAMA_t = KAMA_{t-1} + SC^2 \cdot (x_t - KAMA_{t-1}), \quad SC = ER \cdot (\alpha_f - \alpha_s) + \alpha_s$ |||

| **MIDPOINT** | Midpoint over period | [TA-Lib docs](https://ta-lib.org/functions/midpoint/) |
|---|---|---|
| $MID = \frac{\max(x, n) + \min(x, n)}{2}$ |||

| **MIDPRICE** | Midpoint Price over period | [TA-Lib docs](https://ta-lib.org/functions/midprice/) |
|---|---|---|
| $MIDPRICE = \frac{H_{highest}(n) + L_{lowest}(n)}{2}$ |||

### Momentum Indicators

| **RSI** | Relative Strength Index | [Wikipedia](https://en.wikipedia.org/wiki/Relative_strength_index) |
|---|---|---|
| $RSI = 100 - \frac{100}{1 + \frac{\overline{gain}}{\overline{loss}}}$ |||

| **CMO** | Chande Momentum Oscillator | [Investopedia](https://www.investopedia.com/terms/c/chandemomentumoscillator.asp) |
|---|---|---|
| $CMO = \frac{\sum up - \sum down}{\sum up + \sum down} \times 100$ |||

| **MOM** | Momentum | [Investopedia](https://www.investopedia.com/terms/m/momentum.asp) |
|---|---|---|
| $MOM = x_t - x_{t-n}$ |||

| **ROC** | Rate of Change | [Wikipedia](https://en.wikipedia.org/wiki/Momentum_(technical_analysis)) |
|---|---|---|
| $ROC = \frac{x_t - x_{t-n}}{x_{t-n}} \times 100$ |||

| **ROCP** | Rate of Change (Percentage) | [TA-Lib docs](https://ta-lib.org/functions/rocp/) |
|---|---|---|
| $ROCP = \frac{x_t - x_{t-n}}{x_{t-n}}$ |||

| **ROCR** | Rate of Change (Ratio) | [TA-Lib docs](https://ta-lib.org/functions/rocr/) |
|---|---|---|
| $ROCR = \frac{x_t}{x_{t-n}}$ |||

| **ROCR100** | Rate of Change (Ratio x100) | [TA-Lib docs](https://ta-lib.org/functions/rocr100/) |
|---|---|---|
| $ROCR100 = \frac{x_t}{x_{t-n}} \times 100$ |||

| **TRIX** | Triple Smooth EMA Rate of Change | [Wikipedia](https://en.wikipedia.org/wiki/Trix_(technical_analysis)) |
|---|---|---|
| $TRIX = \frac{E_3(t) - E_3(t-1)}{E_3(t-1)} \times 100, \quad E_3 = EMA(EMA(EMA(x)))$ |||

| **WILLR** | Williams %R | [Wikipedia](https://en.wikipedia.org/wiki/Williams_%25R) |
|---|---|---|
| $\%R = \frac{H_n - C}{H_n - L_n} \times (-100)$ |||

| **CCI** | Commodity Channel Index | [Wikipedia](https://en.wikipedia.org/wiki/Commodity_channel_index) |
|---|---|---|
| $CCI = \frac{TP - SMA(TP, n)}{0.015 \cdot MD}, \quad TP = \frac{H+L+C}{3}$ |||

| **ADX** | Average Directional Index | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $ADX = SMA(DX, n)$ |||

| **ADXR** | Average Directional Movement Rating | [Investopedia](https://www.investopedia.com/terms/a/adxr.asp) |
|---|---|---|
| $ADXR = \frac{ADX_t + ADX_{t-n}}{2}$ |||

| **DX** | Directional Movement Index | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $DX = \frac{|+DI - (-DI)|}{+DI + (-DI)} \times 100$ |||

| **PLUS_DI** | Plus Directional Indicator | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $+DI = \frac{Smooth(+DM, n)}{ATR(n)} \times 100$ |||

| **MINUS_DI** | Minus Directional Indicator | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $-DI = \frac{Smooth(-DM, n)}{ATR(n)} \times 100$ |||

| **PLUS_DM** | Plus Directional Movement | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $+DM = H_t - H_{t-1} \text{ if } > 0 \text{ and } > (L_{t-1} - L_t), \text{ else } 0$ |||

| **MINUS_DM** | Minus Directional Movement | [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index) |
|---|---|---|
| $-DM = L_{t-1} - L_t \text{ if } > 0 \text{ and } > (H_t - H_{t-1}), \text{ else } 0$ |||

### Volume Indicators

| **AD** | Chaikin A/D Line | [Wikipedia](https://en.wikipedia.org/wiki/Accumulation/distribution_index) |
|---|---|---|
| $AD = \sum \frac{(C - L) - (H - C)}{H - L} \times V$ |||

### Volatility

| **ATR** | Average True Range | [Wikipedia](https://en.wikipedia.org/wiki/Average_true_range) |
|---|---|---|
| $ATR = Wilder\_Smooth(TR, n)$ |||

| **NATR** | Normalized Average True Range | [Investopedia](https://www.investopedia.com/terms/a/atr.asp) |
|---|---|---|
| $NATR = \frac{ATR}{C} \times 100$ |||

| **TRANGE** | True Range | [Wikipedia](https://en.wikipedia.org/wiki/Average_true_range) |
|---|---|---|
| $TR = \max(H - L,\ \|H - C_{t-1}\|,\ \|L - C_{t-1}\|)$ |||

### Price Transform

| **AVGPRICE** | Average Price | [TA-Lib docs](https://ta-lib.org/functions/avgprice/) |
|---|---|---|
| $AVGPRICE = \frac{O + H + L + C}{4}$ |||

| **BOP** | Balance of Power | [Investopedia](https://www.investopedia.com/terms/b/balanceofpower.asp) |
|---|---|---|
| $BOP = \frac{C - O}{H - L}$ |||

| **MEDPRICE** | Median Price | [TA-Lib docs](https://ta-lib.org/functions/medprice/) |
|---|---|---|
| $MEDPRICE = \frac{H + L}{2}$ |||

| **TYPPRICE** | Typical Price | [Wikipedia](https://en.wikipedia.org/wiki/Typical_price) |
|---|---|---|
| $TYPPRICE = \frac{H + L + C}{3}$ |||

| **WCLPRICE** | Weighted Close Price | [TA-Lib docs](https://ta-lib.org/functions/wclprice/) |
|---|---|---|
| $WCLPRICE = \frac{H + L + 2C}{4}$ |||

### Cycle Indicators

| **HT_DCPERIOD** | Dominant Cycle Period | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| Hilbert Transform extracts dominant cycle length from price data |||

| **HT_DCPHASE** | Dominant Cycle Phase | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| Phase angle $\phi$ of the dominant cycle in degrees |||

| **HT_TRENDLINE** | Instantaneous Trendline | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| Hilbert-smoothed trend component of the price series |||

| **HT_TRENDMODE** | Trend vs Cycle Mode | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| Returns $1$ if trending, $0$ if in cycle mode |||

### Statistics

| **LINEARREG** | Linear Regression | [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression) |
|---|---|---|
| $\hat{y}_t = b_0 + b_1 \cdot t$ (endpoint of best-fit line) |||

| **LINEARREG_ANGLE** | Linear Regression Angle | [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression) |
|---|---|---|
| $\theta = \arctan(b_1) \text{ in degrees}$ |||

| **LINEARREG_INTERCEPT** | Linear Regression Intercept | [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression) |
|---|---|---|
| $b_0$ of best-fit line $y = b_0 + b_1 t$ |||

| **LINEARREG_SLOPE** | Linear Regression Slope | [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression) |
|---|---|---|
| $b_1 = \frac{n\sum ty - \sum t \sum y}{n\sum t^2 - (\sum t)^2}$ |||

| **TSF** | Time Series Forecast | [Investopedia](https://www.investopedia.com/terms/t/time-series-forecast.asp) |
|---|---|---|
| $TSF = b_0 + b_1 \cdot (n + 1)$ (one period ahead projection) |||

| **SUM** | Rolling Summation | [TA-Lib docs](https://ta-lib.org/functions/sum/) |
|---|---|---|
| $SUM = \sum_{i=0}^{n-1} x_{t-i}$ |||

| **MAX** | Highest Value over period | [TA-Lib docs](https://ta-lib.org/functions/max/) |
|---|---|---|
| $MAX = \max(x_t, x_{t-1}, \ldots, x_{t-n+1})$ |||

| **MIN** | Lowest Value over period | [TA-Lib docs](https://ta-lib.org/functions/min/) |
|---|---|---|
| $MIN = \min(x_t, x_{t-1}, \ldots, x_{t-n+1})$ |||

| **MAXINDEX** | Index of Highest Value | [TA-Lib docs](https://ta-lib.org/functions/maxindex/) |
|---|---|---|
| Position $i$ where $x_i = \max(x)$ within the window |||

| **MININDEX** | Index of Lowest Value | [TA-Lib docs](https://ta-lib.org/functions/minindex/) |
|---|---|---|
| Position $i$ where $x_i = \min(x)$ within the window |||

### Math Transform

| **ACOS** | Arc Cosine | [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions) |
|---|---|---|
| $y = \cos^{-1}(x)$ |||

| **ASIN** | Arc Sine | [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions) |
|---|---|---|
| $y = \sin^{-1}(x)$ |||

| **ATAN** | Arc Tangent | [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions) |
|---|---|---|
| $y = \tan^{-1}(x)$ |||

| **CEIL** | Ceiling | [Wikipedia](https://en.wikipedia.org/wiki/Floor_and_ceiling_functions) |
|---|---|---|
| $y = \lceil x \rceil$ |||

| **COS** | Cosine | [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions) |
|---|---|---|
| $y = \cos(x)$ |||

| **COSH** | Hyperbolic Cosine | [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions) |
|---|---|---|
| $y = \cosh(x) = \frac{e^x + e^{-x}}{2}$ |||

| **EXP** | Exponential | [Wikipedia](https://en.wikipedia.org/wiki/Exponential_function) |
|---|---|---|
| $y = e^x$ |||

| **FLOOR** | Floor | [Wikipedia](https://en.wikipedia.org/wiki/Floor_and_ceiling_functions) |
|---|---|---|
| $y = \lfloor x \rfloor$ |||

| **LN** | Natural Logarithm | [Wikipedia](https://en.wikipedia.org/wiki/Natural_logarithm) |
|---|---|---|
| $y = \ln(x)$ |||

| **LOG10** | Base-10 Logarithm | [Wikipedia](https://en.wikipedia.org/wiki/Common_logarithm) |
|---|---|---|
| $y = \log_{10}(x)$ |||

| **SIN** | Sine | [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions) |
|---|---|---|
| $y = \sin(x)$ |||

| **SINH** | Hyperbolic Sine | [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions) |
|---|---|---|
| $y = \sinh(x) = \frac{e^x - e^{-x}}{2}$ |||

| **SQRT** | Square Root | [Wikipedia](https://en.wikipedia.org/wiki/Square_root) |
|---|---|---|
| $y = \sqrt{x}$ |||

| **TAN** | Tangent | [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions) |
|---|---|---|
| $y = \tan(x)$ |||

| **TANH** | Hyperbolic Tangent | [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions) |
|---|---|---|
| $y = \tanh(x) = \frac{e^x - e^{-x}}{e^x + e^{-x}}$ |||

### Pattern Recognition

All candlestick patterns take $(O, H, L, C)$ and return $+100$ (bullish), $-100$ (bearish), or $0$ (no pattern). See [Candlestick pattern (Wikipedia)](https://en.wikipedia.org/wiki/Candlestick_pattern).

| Function | Description | Reference |
|----------|-------------|-----------|
| **CDL2CROWS** | Two Crows: bearish reversal, two black candles gapping above uptrend | [Investopedia](https://www.investopedia.com/terms/t/twocrows.asp) |
| **CDL3BLACKCROWS** | Three Black Crows: three consecutive long bearish candles | [Wikipedia](https://en.wikipedia.org/wiki/Three_black_crows) |
| **CDL3INSIDE** | Three Inside Up/Down: reversal confirmed by third candle | [Investopedia](https://www.investopedia.com/terms/t/three-inside-updown.asp) |
| **CDL3LINESTRIKE** | Three-Line Strike: three same-direction then strike candle | [Investopedia](https://www.investopedia.com/terms/t/three-line-strike.asp) |
| **CDL3OUTSIDE** | Three Outside Up/Down: engulfing confirmed by third candle | [Investopedia](https://www.investopedia.com/terms/t/three-outside-updown.asp) |
| **CDL3STARSINSOUTH** | Three Stars In The South: declining bearish, shrinking bodies | [Investopedia](https://www.investopedia.com/terms/t/three-stars-south.asp) |
| **CDL3WHITESOLDIERS** | Three White Soldiers: three consecutive long bullish candles | [Wikipedia](https://en.wikipedia.org/wiki/Three_white_soldiers) |
| **CDLADVANCEBLOCK** | Advance Block: three bullish with shrinking bodies | [Investopedia](https://www.investopedia.com/terms/a/advance-block.asp) |
| **CDLBELTHOLD** | Belt-hold: long candle opening at its extreme | [Investopedia](https://www.investopedia.com/terms/b/belt-hold.asp) |
| **CDLBREAKAWAY** | Breakaway: gap then reversal closing the gap | [Investopedia](https://www.investopedia.com/terms/b/breakaway-gap.asp) |
| **CDLCLOSINGMARUBOZU** | Closing Marubozu: no shadow on closing side | [Investopedia](https://www.investopedia.com/terms/m/marubozu.asp) |
| **CDLCONCEALBABYSWALL** | Concealing Baby Swallow: four-candle bearish pattern | [Investopedia](https://www.investopedia.com/terms/c/concealing-baby-swallow.asp) |
| **CDLCOUNTERATTACK** | Counterattack: two opposite candles, same close | [Investopedia](https://www.investopedia.com/terms/c/counterattack.asp) |
| **CDLDOJI** | Doji: $\|O - C\| \approx 0$ | [Wikipedia](https://en.wikipedia.org/wiki/Doji) |
| **CDLDOJISTAR** | Doji Star: doji gapping from previous candle | [Investopedia](https://www.investopedia.com/terms/d/doji.asp) |
| **CDLDRAGONFLYDOJI** | Dragonfly Doji: long lower shadow, no upper | [Investopedia](https://www.investopedia.com/terms/d/dragonfly-doji.asp) |
| **CDLENGULFING** | Engulfing: second body fully contains first | [Wikipedia](https://en.wikipedia.org/wiki/Engulfing_pattern) |
| **CDLGAPSIDESIDEWHITE** | Gap Side-by-Side White: two similar candles after gap | [Investopedia](https://www.investopedia.com/terms/u/upside-gap-two-crows.asp) |
| **CDLGRAVESTONEDOJI** | Gravestone Doji: long upper shadow, no lower | [Investopedia](https://www.investopedia.com/terms/g/gravestone-doji.asp) |
| **CDLHAMMER** | Hammer: small body, lower shadow $\geq 2\times$ body | [Wikipedia](https://en.wikipedia.org/wiki/Hammer_(candlestick_pattern)) |
| **CDLHANGINGMAN** | Hanging Man: hammer in uptrend (bearish) | [Wikipedia](https://en.wikipedia.org/wiki/Hanging_man_(candlestick_pattern)) |
| **CDLHARAMI** | Harami: second candle inside first's body | [Wikipedia](https://en.wikipedia.org/wiki/Harami_(candlestick_pattern)) |
| **CDLHARAMICROSS** | Harami Cross: harami where second is doji | [Investopedia](https://www.investopedia.com/terms/h/haramicross.asp) |
| **CDLHIGHWAVE** | High-Wave Candle: very long shadows, small body | [Investopedia](https://www.investopedia.com/terms/l/long-legged-doji.asp) |
| **CDLHIKKAKE** | Hikkake: inside bar breakout failure | [Investopedia](https://www.investopedia.com/terms/h/hikkakepattern.asp) |
| **CDLHIKKAKEMOD** | Modified Hikkake: confirmed by subsequent action | [Investopedia](https://www.investopedia.com/terms/h/hikkakepattern.asp) |
| **CDLHOMINGPIGEON** | Homing Pigeon: two bearish, second inside first | [Investopedia](https://www.investopedia.com/terms/h/homing-pigeon.asp) |
| **CDLIDENTICAL3CROWS** | Identical Three Crows: each opens at prior close | [Investopedia](https://www.investopedia.com/terms/i/identical-three-crows.asp) |
| **CDLINNECK** | In-Neck: small bullish closing near prior low | [Investopedia](https://www.investopedia.com/terms/i/inneck-pattern.asp) |
| **CDLINVERTEDHAMMER** | Inverted Hammer: small body, long upper shadow | [Investopedia](https://www.investopedia.com/terms/i/invertedhammer.asp) |
| **CDLKICKING** | Kicking: two opposing marubozu with gap | [Investopedia](https://www.investopedia.com/terms/k/kicking-pattern.asp) |
| **CDLKICKINGBYLENGTH** | Kicking by Length: direction by longer marubozu | [Investopedia](https://www.investopedia.com/terms/k/kicking-pattern.asp) |
| **CDLLADDERBOTTOM** | Ladder Bottom: four bearish then bullish reversal | [Investopedia](https://www.investopedia.com/terms/l/ladder-bottom.asp) |
| **CDLLONGLEGGEDDOJI** | Long Legged Doji: very long both shadows | [Investopedia](https://www.investopedia.com/terms/l/long-legged-doji.asp) |
| **CDLLONGLINE** | Long Line Candle: unusually long body | [Investopedia](https://www.investopedia.com/terms/l/long-line-candle.asp) |
| **CDLMARUBOZU** | Marubozu: no shadows (pure body) | [Investopedia](https://www.investopedia.com/terms/m/marubozu.asp) |
| **CDLMATCHINGLOW** | Matching Low: two bearish with same close | [Investopedia](https://www.investopedia.com/terms/m/matching-low.asp) |
| **CDLONNECK** | On-Neck: bullish closing at prior low | [Investopedia](https://www.investopedia.com/terms/o/on-neck-pattern.asp) |
| **CDLPIERCING** | Piercing: bullish closing above midpoint of prior bearish | [Wikipedia](https://en.wikipedia.org/wiki/Piercing_pattern) |
| **CDLRICKSHAWMAN** | Rickshaw Man: long-legged doji, body at center | [Investopedia](https://www.investopedia.com/terms/r/rickshaw-man.asp) |
| **CDLRISEFALL3METHODS** | Rising/Falling Three Methods: continuation pattern | [Wikipedia](https://en.wikipedia.org/wiki/Rising_three_methods) |
| **CDLSEPARATINGLINES** | Separating Lines: opposite candles, same open | [Investopedia](https://www.investopedia.com/terms/s/separating-lines.asp) |
| **CDLSHOOTINGSTAR** | Shooting Star: inverted hammer in uptrend | [Wikipedia](https://en.wikipedia.org/wiki/Shooting_star_(candlestick_pattern)) |
| **CDLSHORTLINE** | Short Line Candle: unusually short body | [Investopedia](https://www.investopedia.com/terms/s/short-line-candle.asp) |
| **CDLSPINNINGTOP** | Spinning Top: small body with both shadows | [Wikipedia](https://en.wikipedia.org/wiki/Spinning_top_(candlestick_pattern)) |
| **CDLSTALLEDPATTERN** | Stalled Pattern: three bullish, third small | [Investopedia](https://www.investopedia.com/terms/s/stalled-pattern.asp) |
| **CDLSTICKSANDWICH** | Stick Sandwich: same-close sandwiching opposite | [Investopedia](https://www.investopedia.com/terms/s/stick-sandwich.asp) |
| **CDLTAKURI** | Takuri: doji with very long lower shadow | [Investopedia](https://www.investopedia.com/terms/t/takuri.asp) |
| **CDLTASUKIGAP** | Tasuki Gap: partial-fill candle after gap | [Investopedia](https://www.investopedia.com/terms/t/tasuki-gap.asp) |
| **CDLTHRUSTING** | Thrusting: bullish closing below midpoint of prior | [Investopedia](https://www.investopedia.com/terms/t/thrusting-pattern.asp) |
| **CDLTRISTAR** | Tristar: three dojis with middle gapping | [Investopedia](https://www.investopedia.com/terms/t/tristar.asp) |
| **CDLUNIQUE3RIVER** | Unique 3 River: three-candle bullish reversal | [Investopedia](https://www.investopedia.com/terms/u/unique-three-river.asp) |
| **CDLUPSIDEGAP2CROWS** | Upside Gap Two Crows: two bearish gapping above | [Investopedia](https://www.investopedia.com/terms/u/upside-gap-two-crows.asp) |
| **CDLXSIDEGAP3METHODS** | Gap Three Methods: opposite candle closing gap | [Investopedia](https://www.investopedia.com/terms/u/updown-gap-sidebyside-white-lines.asp) |

### Multi-Output

| **MACD** | Moving Average Convergence/Divergence | [Wikipedia](https://en.wikipedia.org/wiki/MACD) |
|---|---|---|
| $MACD = EMA(x, f) - EMA(x, s); \quad Signal = EMA(MACD, p); \quad Hist = MACD - Signal$ |||

| **BBANDS** | Bollinger Bands | [Wikipedia](https://en.wikipedia.org/wiki/Bollinger_Bands) |
|---|---|---|
| $Mid = SMA(x, n); \quad Upper = Mid + k\sigma; \quad Lower = Mid - k\sigma$ |||

| **STOCH** | Stochastic Oscillator | [Wikipedia](https://en.wikipedia.org/wiki/Stochastic_oscillator) |
|---|---|---|
| $\%K = \frac{C - L_n}{H_n - L_n} \times 100; \quad \%D = SMA(\%K)$ |||

| **AROON** | Aroon Indicator | [Wikipedia](https://en.wikipedia.org/wiki/Aroon_indicator) |
|---|---|---|
| $Up = \frac{n - \text{bars since } H_n}{n} \times 100; \quad Down = \frac{n - \text{bars since } L_n}{n} \times 100$ |||

| **MINMAX** | Min/Max over period | [TA-Lib docs](https://ta-lib.org/functions/minmax/) |
|---|---|---|
| $\min(x_{t-n+1}, \ldots, x_t)$ and $\max(x_{t-n+1}, \ldots, x_t)$ in single pass |||

| **MAMA** | MESA Adaptive Moving Average | [Wikipedia](https://en.wikipedia.org/wiki/MESA_Adaptive_Moving_Average) |
|---|---|---|
| $MAMA_t = MAMA_{t-1} + \alpha_t(x_t - MAMA_{t-1}); \quad FAMA_t = FAMA_{t-1} + 0.5\alpha_t(MAMA_t - FAMA_{t-1})$ |||

| **HT_PHASOR** | Hilbert Transform — Phasor | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| In-phase $I$ and quadrature $Q$ components of dominant cycle |||

| **HT_SINE** | Hilbert Transform — SineWave | [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform) |
|---|---|---|
| $sine = \sin(\phi); \quad leadsine = \sin(\phi + \pi/4)$ where $\phi$ = dominant cycle phase |||

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
