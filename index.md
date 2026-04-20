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

- **sma** — Simple Moving Average. $SMA = \frac{1}{n}\sum_{i=0}^{n-1} x_{t-i}$
- **ema** — Exponential Moving Average. $EMA_t = \alpha \cdot x_t + (1 - \alpha) \cdot EMA_{t-1}$, where $\alpha = \frac{2}{n+1}$
- **wma** — Weighted Moving Average. $WMA = \frac{\sum_{i=0}^{n-1}(n-i) \cdot x_{t-i}}{\sum_{i=1}^{n} i}$
- **dema** — Double EMA. $DEMA = 2 \cdot EMA(x, n) - EMA(EMA(x, n), n)$
- **tema** — Triple EMA. $TEMA = 3 \cdot E_1 - 3 \cdot E_2 + E_3$, where $E_1 = EMA$, $E_2 = EMA(E_1)$, $E_3 = EMA(E_2)$
- **trima** — Triangular Moving Average. $TRIMA = SMA(SMA(x, m), m)$, where $m = \lceil(n+1)/2\rceil$
- **kama** — Kaufman Adaptive MA. $KAMA_t = KAMA_{t-1} + SC^2 \cdot (x_t - KAMA_{t-1})$, where $SC = ER \cdot (\alpha_{fast} - \alpha_{slow}) + \alpha_{slow}$
- **midpoint** — Midpoint over period. $MID = \frac{\max(x, n) + \min(x, n)}{2}$
- **midprice** — Midpoint Price. $MIDPRICE = \frac{H_{highest}(n) + L_{lowest}(n)}{2}$

### Momentum Indicators

- **rsi** — Relative Strength Index. $RSI = 100 - \frac{100}{1 + \frac{\overline{gain}}{\overline{loss}}}$
- **cmo** — Chande Momentum Oscillator. $CMO = \frac{\sum up - \sum down}{\sum up + \sum down} \times 100$
- **mom** — Momentum. $MOM = x_t - x_{t-n}$
- **roc** — Rate of Change. $ROC = \frac{x_t - x_{t-n}}{x_{t-n}} \times 100$
- **rocp** — Rate of Change Percentage. $ROCP = \frac{x_t - x_{t-n}}{x_{t-n}}$
- **rocr** — Rate of Change Ratio. $ROCR = \frac{x_t}{x_{t-n}}$
- **rocr100** — Rate of Change Ratio x100. $ROCR100 = \frac{x_t}{x_{t-n}} \times 100$
- **trix** — Triple Smooth EMA ROC. $TRIX = \frac{E_3(t) - E_3(t-1)}{E_3(t-1)} \times 100$, where $E_3 = EMA(EMA(EMA(x)))$
- **willr** — Williams %R. $\%R = \frac{H_n - C}{H_n - L_n} \times (-100)$
- **cci** — Commodity Channel Index. $CCI = \frac{TP - SMA(TP, n)}{0.015 \cdot MD}$, where $TP = \frac{H+L+C}{3}$
- **adx** — Average Directional Index. $ADX = SMA(DX, n)$
- **adxr** — ADX Rating. $ADXR = \frac{ADX_t + ADX_{t-n}}{2}$
- **dx** — Directional Movement Index. $DX = \frac{|+DI - (-DI)|}{+DI + (-DI)} \times 100$
- **plus_di** — Plus Directional Indicator. $+DI = \frac{Smooth(+DM, n)}{ATR(n)} \times 100$
- **minus_di** — Minus Directional Indicator. $-DI = \frac{Smooth(-DM, n)}{ATR(n)} \times 100$
- **plus_dm** — Plus Directional Movement. $+DM = H_t - H_{t-1}$ if $> 0$ and $> (L_{t-1} - L_t)$, else $0$
- **minus_dm** — Minus Directional Movement. $-DM = L_{t-1} - L_t$ if $> 0$ and $> (H_t - H_{t-1})$, else $0$

### Volume Indicators

- **ad** — Chaikin A/D Line. $AD = \sum \frac{(C - L) - (H - C)}{H - L} \times V$

### Volatility

- **atr** — Average True Range. $ATR = \frac{1}{n}\sum_{i=1}^{n} TR_i$ (Wilder smoothing)
- **natr** — Normalized ATR. $NATR = \frac{ATR}{C} \times 100$
- **trange** — True Range. $TR = \max(H - L,\ |H - C_{t-1}|,\ |L - C_{t-1}|)$

### Price Transform

- **avgprice** — Average Price. $AVGPRICE = \frac{O + H + L + C}{4}$
- **bop** — Balance of Power. $BOP = \frac{C - O}{H - L}$
- **medprice** — Median Price. $MEDPRICE = \frac{H + L}{2}$
- **typprice** — Typical Price. $TYPPRICE = \frac{H + L + C}{3}$
- **wclprice** — Weighted Close. $WCLPRICE = \frac{H + L + 2C}{4}$

### Cycle Indicators

- **ht_dcperiod** — Dominant Cycle Period. Hilbert Transform extracts the dominant cycle length from price data.
- **ht_dcphase** — Dominant Cycle Phase. Phase angle $\phi$ of the dominant cycle in degrees.
- **ht_trendline** — Instantaneous Trendline. Hilbert-smoothed trend component of the price series.
- **ht_trendmode** — Trend vs Cycle Mode. Returns $1$ if trending, $0$ if in cycle mode.

### Statistics

- **linearreg** — Linear Regression. $y = b_0 + b_1 \cdot t$, returns endpoint $\hat{y}_t$
- **linearreg_angle** — Linear Regression Angle. $\theta = \arctan(b_1)$ in degrees
- **linearreg_intercept** — Linear Regression Intercept. $b_0$ of best-fit line $y = b_0 + b_1 t$
- **linearreg_slope** — Linear Regression Slope. $b_1 = \frac{n\sum ty - \sum t \sum y}{n\sum t^2 - (\sum t)^2}$
- **tsf** — Time Series Forecast. $TSF = b_0 + b_1 \cdot (n + 1)$ (projects one period ahead)
- **sum** — Rolling Sum. $SUM = \sum_{i=0}^{n-1} x_{t-i}$
- **max** — Highest Value. $MAX = \max(x_{t}, x_{t-1}, \ldots, x_{t-n+1})$
- **min** — Lowest Value. $MIN = \min(x_{t}, x_{t-1}, \ldots, x_{t-n+1})$
- **maxindex** — Index of Highest. Position $i$ where $x_i = \max(x)$ within the window.
- **minindex** — Index of Lowest. Position $i$ where $x_i = \min(x)$ within the window.

### Math Transform

- **acos** — Arc Cosine. $y = \cos^{-1}(x)$
- **asin** — Arc Sine. $y = \sin^{-1}(x)$
- **atan** — Arc Tangent. $y = \tan^{-1}(x)$
- **ceil** — Ceiling. $y = \lceil x \rceil$
- **cos** — Cosine. $y = \cos(x)$
- **cosh** — Hyperbolic Cosine. $y = \cosh(x) = \frac{e^x + e^{-x}}{2}$
- **exp** — Exponential. $y = e^x$
- **floor** — Floor. $y = \lfloor x \rfloor$
- **ln** — Natural Logarithm. $y = \ln(x)$
- **log10** — Base-10 Logarithm. $y = \log_{10}(x)$
- **sin** — Sine. $y = \sin(x)$
- **sinh** — Hyperbolic Sine. $y = \sinh(x) = \frac{e^x - e^{-x}}{2}$
- **sqrt** — Square Root. $y = \sqrt{x}$
- **tan** — Tangent. $y = \tan(x)$
- **tanh** — Hyperbolic Tangent. $y = \tanh(x) = \frac{e^x - e^{-x}}{e^x + e^{-x}}$

### Pattern Recognition

All candlestick patterns take $(O, H, L, C)$ and return $+100$ (bullish), $-100$ (bearish), or $0$ (no pattern).

- **cdl2crows** — Two Crows: bearish reversal, two black candles gapping above uptrend
- **cdl3blackcrows** — Three Black Crows: three consecutive long bearish candles
- **cdl3inside** — Three Inside Up/Down: reversal confirmed by third candle breaking first's range
- **cdl3linestrike** — Three-Line Strike: three same-direction candles then a strike candle
- **cdl3outside** — Three Outside Up/Down: engulfing pattern confirmed by third candle
- **cdl3starsinsouth** — Three Stars In The South: three declining bearish candles, shrinking bodies
- **cdl3whitesoldiers** — Three White Soldiers: three consecutive long bullish candles
- **cdladvanceblock** — Advance Block: three bullish candles with shrinking bodies
- **cdlbelthold** — Belt-hold: long candle opening at its extreme
- **cdlbreakaway** — Breakaway: gap then three candles, reversal closing the gap
- **cdlclosingmarubozu** — Closing Marubozu: no shadow on the closing side
- **cdlconcealbabyswall** — Concealing Baby Swallow: four-candle bearish pattern
- **cdlcounterattack** — Counterattack: two opposite candles with same close
- **cdldoji** — Doji: $|O - C| \approx 0$
- **cdldojistar** — Doji Star: doji gapping from previous candle
- **cdldragonflydoji** — Dragonfly Doji: doji with long lower shadow, no upper
- **cdlengulfing** — Engulfing: second body fully contains the first
- **cdlgapsidesidewhite** — Gap Side-by-Side White: two similar white candles after gap
- **cdlgravestonedoji** — Gravestone Doji: doji with long upper shadow, no lower
- **cdlhammer** — Hammer: small body at top, lower shadow $\geq 2 \times$ body
- **cdlhangingman** — Hanging Man: hammer in uptrend (bearish)
- **cdlharami** — Harami: second candle contained within first's body
- **cdlharamicross** — Harami Cross: harami where second candle is a doji
- **cdlhighwave** — High-Wave Candle: very long shadows, small body
- **cdlhikkake** — Hikkake: inside bar breakout failure
- **cdlhikkakemod** — Modified Hikkake: confirmed by subsequent price action
- **cdlhomingpigeon** — Homing Pigeon: two bearish, second inside first (bullish)
- **cdlidentical3crows** — Identical Three Crows: each opens at prior close
- **cdlinneck** — In-Neck: small bullish closing near prior low
- **cdlinvertedhammer** — Inverted Hammer: small body at bottom, long upper shadow
- **cdlkicking** — Kicking: two opposing marubozu with gap
- **cdlkickingbylength** — Kicking by Length: direction by longer marubozu
- **cdlladderbottom** — Ladder Bottom: four bearish then bullish reversal
- **cdllongleggeddoji** — Long Legged Doji: very long both shadows
- **cdllongline** — Long Line Candle: unusually long body
- **cdlmarubozu** — Marubozu: no shadows (pure body)
- **cdlmatchinglow** — Matching Low: two bearish with same close
- **cdlonneck** — On-Neck: bullish closing exactly at prior low
- **cdlpiercing** — Piercing: bullish closing above midpoint of prior bearish
- **cdlrickshawman** — Rickshaw Man: long-legged doji, body at center
- **cdlrisefall3methods** — Rising/Falling Three Methods: continuation pattern
- **cdlseparatinglines** — Separating Lines: opposite candles, same open
- **cdlshootingstar** — Shooting Star: inverted hammer in uptrend
- **cdlshortline** — Short Line Candle: unusually short body
- **cdlspinningtop** — Spinning Top: small body with both shadows
- **cdlstalledpattern** — Stalled Pattern: three bullish, third small (slowing)
- **cdlsticksandwich** — Stick Sandwich: same-close candles sandwiching opposite
- **cdltakuri** — Takuri: doji with very long lower shadow
- **cdltasukigap** — Tasuki Gap: gap then partial-fill candle (continuation)
- **cdlthrusting** — Thrusting: bullish closing below midpoint of prior bearish
- **cdltristar** — Tristar: three dojis with middle gapping
- **cdlunique3river** — Unique 3 River: three-candle bullish reversal
- **cdlupsidegap2crows** — Upside Gap Two Crows: two bearish gapping above uptrend
- **cdlxsidegap3methods** — Gap Three Methods: opposite candle closing the gap

### Multi-Output

- **macd** — MACD. $MACD = EMA(x, f) - EMA(x, s)$; $Signal = EMA(MACD, p)$; $Hist = MACD - Signal$
- **bbands** — Bollinger Bands. $Mid = SMA(x, n)$; $Upper = Mid + k\sigma$; $Lower = Mid - k\sigma$
- **stoch** — Stochastic. $\%K = \frac{C - L_n}{H_n - L_n} \times 100$; $\%D = SMA(\%K)$
- **aroon** — Aroon. $Up = \frac{n - \text{bars since } H_n}{n} \times 100$; $Down = \frac{n - \text{bars since } L_n}{n} \times 100$
- **minmax** — Min/Max. $\min(x_{t-n+1}, \ldots, x_t)$ and $\max(x_{t-n+1}, \ldots, x_t)$ in single pass
- **mama** — MESA Adaptive MA. Phase-adaptive MA: $MAMA_t = MAMA_{t-1} + \alpha_t(x_t - MAMA_{t-1})$
- **ht_phasor** — Hilbert Phasor. In-phase $I$ and quadrature $Q$ components of dominant cycle.
- **ht_sine** — Hilbert SineWave. $\sin(\phi)$ and $\sin(\phi + \pi/4)$ of dominant cycle phase $\phi$.

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
