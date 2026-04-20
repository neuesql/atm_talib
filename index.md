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

Categories per [Wikipedia: Technical indicator](https://en.wikipedia.org/wiki/Technical_indicator).

| Category | Scalar | Window | Description |
|----------|--------|--------|-------------|
| **[Trend](https://en.wikipedia.org/wiki/Technical_indicator#Trend)** — identify price direction | | | |
| Trend | `t_sma` | `ta_sma` | Simple Moving Average |
| Trend | `t_ema` | `ta_ema` | Exponential Moving Average |
| Trend | `t_wma` | `ta_wma` | Weighted Moving Average |
| Trend | `t_dema` | `ta_dema` | Double Exponential Moving Average |
| Trend | `t_tema` | `ta_tema` | Triple Exponential Moving Average |
| Trend | `t_trima` | `ta_trima` | Triangular Moving Average |
| Trend | `t_kama` | `ta_kama` | Kaufman Adaptive Moving Average |
| Trend | `t_mama` | `ta_mama` | MESA Adaptive Moving Average |
| Trend | `t_macd` | `ta_macd` | MACD |
| Trend | `t_adx` | `ta_adx` | Average Directional Movement Index |
| Trend | `t_adxr` | `ta_adxr` | Average Directional Movement Rating |
| Trend | `t_dx` | `ta_dx` | Directional Movement Index |
| Trend | `t_plus_di` | `ta_plus_di` | Plus Directional Indicator |
| Trend | `t_minus_di` | `ta_minus_di` | Minus Directional Indicator |
| Trend | `t_plus_dm` | `ta_plus_dm` | Plus Directional Movement |
| Trend | `t_minus_dm` | `ta_minus_dm` | Minus Directional Movement |
| Trend | `t_aroon` | `ta_aroon` | Aroon |
| Trend | `t_midpoint` | `ta_midpoint` | MidPoint over period |
| Trend | `t_midprice` | `ta_midprice` | Midpoint Price over period |
| **[Momentum](https://en.wikipedia.org/wiki/Technical_indicator#Momentum)** — measure speed of price change | | | |
| Momentum | `t_rsi` | `ta_rsi` | Relative Strength Index |
| Momentum | `t_stoch` | `ta_stoch` | Stochastic Oscillator |
| Momentum | `t_cmo` | `ta_cmo` | Chande Momentum Oscillator |
| Momentum | `t_mom` | `ta_mom` | Momentum |
| Momentum | `t_roc` | `ta_roc` | Rate of Change |
| Momentum | `t_rocp` | `ta_rocp` | Rate of Change (Percentage) |
| Momentum | `t_rocr` | `ta_rocr` | Rate of Change (Ratio) |
| Momentum | `t_rocr100` | `ta_rocr100` | Rate of Change (Ratio x100) |
| Momentum | `t_trix` | `ta_trix` | Triple Smooth EMA Rate of Change |
| Momentum | `t_willr` | `ta_willr` | Williams' %R |
| Momentum | `t_cci` | `ta_cci` | Commodity Channel Index |
| Momentum | `t_bop` | `ta_bop` | Balance of Power |
| **[Volume](https://en.wikipedia.org/wiki/Technical_indicator#Volume)** — measure buying/selling pressure | | | |
| Volume | `t_ad` | `ta_ad` | Chaikin A/D Line |
| **[Volatility](https://en.wikipedia.org/wiki/Technical_indicator#Volatility)** — measure price fluctuation | | | |
| Volatility | `t_atr` | `ta_atr` | Average True Range |
| Volatility | `t_natr` | `ta_natr` | Normalized Average True Range |
| Volatility | `t_trange` | `ta_trange` | True Range |
| Volatility | `t_bbands` | `ta_bbands` | Bollinger Bands |
| Volatility | `t_minmax` | `ta_minmax` | Min/Max over period |
| **[Cycle](https://en.wikipedia.org/wiki/Technical_indicator)** — detect periodic patterns | | | |
| Cycle | `t_ht_dcperiod` | `ta_ht_dcperiod` | Dominant Cycle Period |
| Cycle | `t_ht_dcphase` | `ta_ht_dcphase` | Dominant Cycle Phase |
| Cycle | `t_ht_trendline` | `ta_ht_trendline` | Instantaneous Trendline |
| Cycle | `t_ht_trendmode` | `ta_ht_trendmode` | Trend vs Cycle Mode |
| Cycle | `t_ht_phasor` | `ta_ht_phasor` | Hilbert Transform — Phasor |
| Cycle | `t_ht_sine` | `ta_ht_sine` | Hilbert Transform — SineWave |
| **Price Transform** — derive synthetic price series | | | |
| Price | `t_avgprice` | `ta_avgprice` | Average Price (O+H+L+C)/4 |
| Price | `t_medprice` | `ta_medprice` | Median Price (H+L)/2 |
| Price | `t_typprice` | `ta_typprice` | Typical Price (H+L+C)/3 |
| Price | `t_wclprice` | `ta_wclprice` | Weighted Close (H+L+2C)/4 |
| **Statistics** — statistical measures over rolling windows | | | |
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
| **Math Transform** (scalar only) — element-wise math | | | |
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
| **[Pattern Recognition](https://en.wikipedia.org/wiki/Candlestick_pattern)** (scalar only) — candlestick patterns | | | |
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

---

## Function Signatures

| # | Function | Signature | Return |
|---|----------|-----------|--------|
| | **Trend** | | |
| 1 | `t_sma` / `ta_sma` | (values, period) | DOUBLE |
| 2 | `t_ema` / `ta_ema` | (values, period) | DOUBLE |
| 3 | `t_wma` / `ta_wma` | (values, period) | DOUBLE |
| 4 | `t_dema` / `ta_dema` | (values, period) | DOUBLE |
| 5 | `t_tema` / `ta_tema` | (values, period) | DOUBLE |
| 6 | `t_trima` / `ta_trima` | (values, period) | DOUBLE |
| 7 | `t_kama` / `ta_kama` | (values, period) | DOUBLE |
| 8 | `t_mama` / `ta_mama` | (values, fast_limit, slow_limit) | STRUCT(mama, fama) |
| 9 | `t_macd` / `ta_macd` | (values, fast_period, slow_period, signal_period) | STRUCT(macd, signal, hist) |
| 10 | `t_adx` / `ta_adx` | (high, low, close, period) | DOUBLE |
| 11 | `t_adxr` / `ta_adxr` | (high, low, close, period) | DOUBLE |
| 12 | `t_dx` / `ta_dx` | (high, low, close, period) | DOUBLE |
| 13 | `t_plus_di` / `ta_plus_di` | (high, low, close, period) | DOUBLE |
| 14 | `t_minus_di` / `ta_minus_di` | (high, low, close, period) | DOUBLE |
| 15 | `t_plus_dm` / `ta_plus_dm` | (high, low, period) | DOUBLE |
| 16 | `t_minus_dm` / `ta_minus_dm` | (high, low, period) | DOUBLE |
| 17 | `t_aroon` / `ta_aroon` | (high, low, period) | STRUCT(aroon_down, aroon_up) |
| 18 | `t_midpoint` / `ta_midpoint` | (values, period) | DOUBLE |
| 19 | `t_midprice` / `ta_midprice` | (high, low, period) | DOUBLE |
| | **Momentum** | | |
| 20 | `t_rsi` / `ta_rsi` | (values, period) | DOUBLE |
| 21 | `t_stoch` / `ta_stoch` | (high, low, close, fastK, slowK, slowKMA, slowD, slowDMA) | STRUCT(slowk, slowd) |
| 22 | `t_cmo` / `ta_cmo` | (values, period) | DOUBLE |
| 23 | `t_mom` / `ta_mom` | (values, period) | DOUBLE |
| 24 | `t_roc` / `ta_roc` | (values, period) | DOUBLE |
| 25 | `t_rocp` / `ta_rocp` | (values, period) | DOUBLE |
| 26 | `t_rocr` / `ta_rocr` | (values, period) | DOUBLE |
| 27 | `t_rocr100` / `ta_rocr100` | (values, period) | DOUBLE |
| 28 | `t_trix` / `ta_trix` | (values, period) | DOUBLE |
| 29 | `t_willr` / `ta_willr` | (high, low, close, period) | DOUBLE |
| 30 | `t_cci` / `ta_cci` | (high, low, close, period) | DOUBLE |
| 31 | `t_bop` / `ta_bop` | (open, high, low, close) | DOUBLE |
| | **Volume** | | |
| 32 | `t_ad` / `ta_ad` | (high, low, close, volume) | DOUBLE |
| | **Volatility** | | |
| 33 | `t_atr` / `ta_atr` | (high, low, close, period) | DOUBLE |
| 34 | `t_natr` / `ta_natr` | (high, low, close, period) | DOUBLE |
| 35 | `t_trange` / `ta_trange` | (high, low, close) | DOUBLE |
| 36 | `t_bbands` / `ta_bbands` | (values, period, devup, devdn, matype) | STRUCT(upper, middle, lower) |
| 37 | `t_minmax` / `ta_minmax` | (values, period) | STRUCT(min, max) |
| | **Cycle** | | |
| 38 | `t_ht_dcperiod` / `ta_ht_dcperiod` | (values) | DOUBLE |
| 39 | `t_ht_dcphase` / `ta_ht_dcphase` | (values) | DOUBLE |
| 40 | `t_ht_trendline` / `ta_ht_trendline` | (values) | DOUBLE |
| 41 | `t_ht_trendmode` / `ta_ht_trendmode` | (values) | INTEGER |
| 42 | `t_ht_phasor` / `ta_ht_phasor` | (values) | STRUCT(inphase, quadrature) |
| 43 | `t_ht_sine` / `ta_ht_sine` | (values) | STRUCT(sine, leadsine) |
| | **Price Transform** | | |
| 44 | `t_avgprice` / `ta_avgprice` | (open, high, low, close) | DOUBLE |
| 45 | `t_medprice` / `ta_medprice` | (high, low) | DOUBLE |
| 46 | `t_typprice` / `ta_typprice` | (high, low, close) | DOUBLE |
| 47 | `t_wclprice` / `ta_wclprice` | (high, low, close) | DOUBLE |
| | **Statistics** | | |
| 48 | `t_linearreg` / `ta_linearreg` | (values, period) | DOUBLE |
| 49 | `t_linearreg_angle` / `ta_linearreg_angle` | (values, period) | DOUBLE |
| 50 | `t_linearreg_intercept` / `ta_linearreg_intercept` | (values, period) | DOUBLE |
| 51 | `t_linearreg_slope` / `ta_linearreg_slope` | (values, period) | DOUBLE |
| 52 | `t_tsf` / `ta_tsf` | (values, period) | DOUBLE |
| 53 | `t_sum` / `ta_sum` | (values, period) | DOUBLE |
| 54 | `t_max` / `ta_max` | (values, period) | DOUBLE |
| 55 | `t_min` / `ta_min` | (values, period) | DOUBLE |
| 56 | `t_maxindex` / `ta_maxindex` | (values, period) | INTEGER |
| 57 | `t_minindex` / `ta_minindex` | (values, period) | INTEGER |
| | **Math Transform** (scalar only) | | |
| 58 | `t_acos` | (values) | DOUBLE |
| 59 | `t_asin` | (values) | DOUBLE |
| 60 | `t_atan` | (values) | DOUBLE |
| 61 | `t_ceil` | (values) | DOUBLE |
| 62 | `t_cos` | (values) | DOUBLE |
| 63 | `t_cosh` | (values) | DOUBLE |
| 64 | `t_exp` | (values) | DOUBLE |
| 65 | `t_floor` | (values) | DOUBLE |
| 66 | `t_ln` | (values) | DOUBLE |
| 67 | `t_log10` | (values) | DOUBLE |
| 68 | `t_sin` | (values) | DOUBLE |
| 69 | `t_sinh` | (values) | DOUBLE |
| 70 | `t_sqrt` | (values) | DOUBLE |
| 71 | `t_tan` | (values) | DOUBLE |
| 72 | `t_tanh` | (values) | DOUBLE |
| | **Pattern Recognition** (scalar only) | | |
| 73–126 | `t_cdl*` | (open, high, low, close) | INTEGER |

---

## Function Details

### Trend

1. **SMA** [Wikipedia](https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average): Simple Moving Average
$SMA = \frac{1}{n}\sum_{i=0}^{n-1} x_{t-i}$

2. **EMA** [Wikipedia](https://en.wikipedia.org/wiki/Exponential_smoothing): Exponential Moving Average
$EMA_t = \alpha \cdot x_t + (1 - \alpha) \cdot EMA_{t-1}, \quad \alpha = \frac{2}{n+1}$

3. **WMA** [Wikipedia](https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average): Weighted Moving Average
$WMA = \frac{\sum_{i=0}^{n-1}(n-i) \cdot x_{t-i}}{\sum_{i=1}^{n} i}$

4. **DEMA** [Wikipedia](https://en.wikipedia.org/wiki/Double_exponential_moving_average): Double Exponential Moving Average
$DEMA = 2 \cdot EMA(x, n) - EMA(EMA(x, n), n)$

5. **TEMA** [Wikipedia](https://en.wikipedia.org/wiki/Triple_exponential_moving_average): Triple Exponential Moving Average
$TEMA = 3E_1 - 3E_2 + E_3, \quad E_1 = EMA,\ E_2 = EMA(E_1),\ E_3 = EMA(E_2)$

6. **TRIMA** [Investopedia](https://www.investopedia.com/terms/t/triangularmoving-average.asp): Triangular Moving Average
$TRIMA = SMA(SMA(x, m), m), \quad m = \lceil(n+1)/2\rceil$

7. **KAMA** [Tulip](https://tulipindicators.org/kama): Kaufman Adaptive Moving Average
$KAMA_t = KAMA_{t-1} + SC^2 \cdot (x_t - KAMA_{t-1}), \quad SC = ER \cdot (\alpha_f - \alpha_s) + \alpha_s$

8. **MAMA** [Investopedia](https://www.investopedia.com/terms/m/mesa-adaptive-moving-average.asp): MESA Adaptive Moving Average
$MAMA_t = MAMA_{t-1} + \alpha_t(x_t - MAMA_{t-1}); \quad FAMA_t = FAMA_{t-1} + 0.5\alpha_t(MAMA_t - FAMA_{t-1})$

9. **MACD** [Wikipedia](https://en.wikipedia.org/wiki/MACD): Moving Average Convergence/Divergence
$MACD = EMA(x, f) - EMA(x, s); \quad Signal = EMA(MACD, p); \quad Hist = MACD - Signal$

10. **ADX** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Average Directional Index
$ADX = SMA(DX, n)$

11. **ADXR** [Investopedia](https://www.investopedia.com/terms/a/adxr.asp): Average Directional Movement Rating
$ADXR = \frac{ADX_t + ADX_{t-n}}{2}$

12. **DX** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Directional Movement Index
$DX = \frac{|+DI - (-DI)|}{+DI + (-DI)} \times 100$

13. **PLUS_DI** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Plus Directional Indicator
$+DI = \frac{Smooth(+DM, n)}{ATR(n)} \times 100$

14. **MINUS_DI** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Minus Directional Indicator
$-DI = \frac{Smooth(-DM, n)}{ATR(n)} \times 100$

15. **PLUS_DM** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Plus Directional Movement
$+DM = H_t - H_{t-1} \text{ if } > 0 \text{ and } > (L_{t-1} - L_t), \text{ else } 0$

16. **MINUS_DM** [Wikipedia](https://en.wikipedia.org/wiki/Average_directional_movement_index): Minus Directional Movement
$-DM = L_{t-1} - L_t \text{ if } > 0 \text{ and } > (H_t - H_{t-1}), \text{ else } 0$

17. **AROON** [Tulip](https://tulipindicators.org/aroon): Aroon Indicator
$Up = \frac{n - \text{bars since } H_n}{n} \times 100; \quad Down = \frac{n - \text{bars since } L_n}{n} \times 100$

18. **MIDPOINT** [Investopedia](https://www.investopedia.com/terms/m/midprice.asp): Midpoint over period
$MID = \frac{\max(x, n) + \min(x, n)}{2}$

19. **MIDPRICE** [Investopedia](https://www.investopedia.com/terms/m/midprice.asp): Midpoint Price over period
$MIDPRICE = \frac{H_{highest}(n) + L_{lowest}(n)}{2}$

### Momentum

20. **RSI** [Wikipedia](https://en.wikipedia.org/wiki/Relative_strength_index): Relative Strength Index
$RSI = 100 - \frac{100}{1 + \frac{\overline{gain}}{\overline{loss}}}$

21. **STOCH** [Wikipedia](https://en.wikipedia.org/wiki/Stochastic_oscillator): Stochastic Oscillator
$\%K = \frac{C - L_n}{H_n - L_n} \times 100; \quad \%D = SMA(\%K)$

22. **CMO** [Investopedia](https://www.investopedia.com/terms/c/chandemomentumoscillator.asp): Chande Momentum Oscillator
$CMO = \frac{\sum up - \sum down}{\sum up + \sum down} \times 100$

23. **MOM** [Investopedia](https://www.investopedia.com/terms/m/momentum.asp): Momentum
$MOM = x_t - x_{t-n}$

24. **ROC** [Wikipedia](https://en.wikipedia.org/wiki/Momentum_(technical_analysis)): Rate of Change
$ROC = \frac{x_t - x_{t-n}}{x_{t-n}} \times 100$

25. **ROCP** [Tulip](https://tulipindicators.org/roc): Rate of Change (Percentage)
$ROCP = \frac{x_t - x_{t-n}}{x_{t-n}}$

26. **ROCR** [Tulip](https://tulipindicators.org/rocr): Rate of Change (Ratio)
$ROCR = \frac{x_t}{x_{t-n}}$

27. **ROCR100** [Tulip](https://tulipindicators.org/rocr): Rate of Change (Ratio x100)
$ROCR100 = \frac{x_t}{x_{t-n}} \times 100$

28. **TRIX** [Wikipedia](https://en.wikipedia.org/wiki/Trix_(technical_analysis)): Triple Smooth EMA Rate of Change
$TRIX = \frac{E_3(t) - E_3(t-1)}{E_3(t-1)} \times 100, \quad E_3 = EMA(EMA(EMA(x)))$

29. **WILLR** [Wikipedia](https://en.wikipedia.org/wiki/Williams_%25R): Williams %R
$\%R = \frac{H_n - C}{H_n - L_n} \times (-100)$

30. **CCI** [Wikipedia](https://en.wikipedia.org/wiki/Commodity_channel_index): Commodity Channel Index
$CCI = \frac{TP - SMA(TP, n)}{0.015 \cdot MD}, \quad TP = \frac{H+L+C}{3}$

31. **BOP** [Investopedia](https://www.investopedia.com/terms/b/balanceofpower.asp): Balance of Power
$BOP = \frac{C - O}{H - L}$

### Volume

32. **AD** [Wikipedia](https://en.wikipedia.org/wiki/Accumulation/distribution_index): Chaikin A/D Line
$AD = \sum \frac{(C - L) - (H - C)}{H - L} \times V$

### Volatility

33. **ATR** [Wikipedia](https://en.wikipedia.org/wiki/Average_true_range): Average True Range
$ATR = Wilder\_Smooth(TR, n)$

34. **NATR** [Investopedia](https://www.investopedia.com/terms/a/atr.asp): Normalized Average True Range
$NATR = \frac{ATR}{C} \times 100$

35. **TRANGE** [Wikipedia](https://en.wikipedia.org/wiki/Average_true_range): True Range
$TR = \max(H - L,\ |H - C_{t-1}|,\ |L - C_{t-1}|)$

36. **BBANDS** [Wikipedia](https://en.wikipedia.org/wiki/Bollinger_Bands): Bollinger Bands
$Mid = SMA(x, n); \quad Upper = Mid + k\sigma; \quad Lower = Mid - k\sigma$

37. **MINMAX** [Tulip](https://tulipindicators.org/min): Min/Max over period
$\min(x_{t-n+1}, \ldots, x_t)$ and $\max(x_{t-n+1}, \ldots, x_t)$ in single pass

### Price Transform

38. **AVGPRICE** [Tulip](https://tulipindicators.org/avgprice): Average Price
$AVGPRICE = \frac{O + H + L + C}{4}$

39. **MEDPRICE** [Tulip](https://tulipindicators.org/medprice): Median Price
$MEDPRICE = \frac{H + L}{2}$

40. **TYPPRICE** [Wikipedia](https://en.wikipedia.org/wiki/Typical_price): Typical Price
$TYPPRICE = \frac{H + L + C}{3}$

41. **WCLPRICE** [Tulip](https://tulipindicators.org/wcprice): Weighted Close Price
$WCLPRICE = \frac{H + L + 2C}{4}$

### Cycle

42. **HT_DCPERIOD** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Dominant Cycle Period
Hilbert Transform extracts dominant cycle length from price data.

43. **HT_DCPHASE** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Dominant Cycle Phase
Phase angle $\phi$ of the dominant cycle in degrees.

44. **HT_TRENDLINE** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Instantaneous Trendline
Hilbert-smoothed trend component of the price series.

45. **HT_TRENDMODE** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Trend vs Cycle Mode
Returns $1$ if trending, $0$ if in cycle mode.

46. **HT_PHASOR** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Hilbert Transform — Phasor
In-phase $I$ and quadrature $Q$ components of dominant cycle.

47. **HT_SINE** [Wikipedia](https://en.wikipedia.org/wiki/Hilbert_transform): Hilbert Transform — SineWave
$sine = \sin(\phi); \quad leadsine = \sin(\phi + \pi/4)$ where $\phi$ = dominant cycle phase

### Statistics

40. **LINEARREG** [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression): Linear Regression
$\hat{y}_t = b_0 + b_1 \cdot t$

41. **LINEARREG_ANGLE** [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression): Linear Regression Angle
$\theta = \arctan(b_1)$ in degrees

42. **LINEARREG_INTERCEPT** [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression): Linear Regression Intercept
$b_0$ of best-fit line $y = b_0 + b_1 t$

43. **LINEARREG_SLOPE** [Wikipedia](https://en.wikipedia.org/wiki/Linear_regression): Linear Regression Slope
$b_1 = \frac{n\sum ty - \sum t \sum y}{n\sum t^2 - (\sum t)^2}$

44. **TSF** [Investopedia](https://www.investopedia.com/terms/t/time-series-forecast.asp): Time Series Forecast
$TSF = b_0 + b_1 \cdot (n + 1)$

45. **SUM** [Tulip](https://tulipindicators.org/sum): Rolling Summation
$SUM = \sum_{i=0}^{n-1} x_{t-i}$

46. **MAX** [Tulip](https://tulipindicators.org/max): Highest Value over period
$MAX = \max(x_t, x_{t-1}, \ldots, x_{t-n+1})$

47. **MIN** [Tulip](https://tulipindicators.org/min): Lowest Value over period
$MIN = \min(x_t, x_{t-1}, \ldots, x_{t-n+1})$

48. **MAXINDEX** [Tulip](https://tulipindicators.org/max): Index of Highest Value
Position $i$ where $x_i = \max(x)$ within the window.

49. **MININDEX** [Tulip](https://tulipindicators.org/min): Index of Lowest Value
Position $i$ where $x_i = \min(x)$ within the window.

### Math Transform

50. **ACOS** [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions): Arc Cosine
$y = \cos^{-1}(x)$

51. **ASIN** [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions): Arc Sine
$y = \sin^{-1}(x)$

52. **ATAN** [Wikipedia](https://en.wikipedia.org/wiki/Inverse_trigonometric_functions): Arc Tangent
$y = \tan^{-1}(x)$

53. **CEIL** [Wikipedia](https://en.wikipedia.org/wiki/Floor_and_ceiling_functions): Ceiling
$y = \lceil x \rceil$

54. **COS** [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions): Cosine
$y = \cos(x)$

55. **COSH** [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions): Hyperbolic Cosine
$y = \cosh(x) = \frac{e^x + e^{-x}}{2}$

56. **EXP** [Wikipedia](https://en.wikipedia.org/wiki/Exponential_function): Exponential
$y = e^x$

57. **FLOOR** [Wikipedia](https://en.wikipedia.org/wiki/Floor_and_ceiling_functions): Floor
$y = \lfloor x \rfloor$

58. **LN** [Wikipedia](https://en.wikipedia.org/wiki/Natural_logarithm): Natural Logarithm
$y = \ln(x)$

59. **LOG10** [Wikipedia](https://en.wikipedia.org/wiki/Common_logarithm): Base-10 Logarithm
$y = \log_{10}(x)$

60. **SIN** [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions): Sine
$y = \sin(x)$

61. **SINH** [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions): Hyperbolic Sine
$y = \sinh(x) = \frac{e^x - e^{-x}}{2}$

62. **SQRT** [Wikipedia](https://en.wikipedia.org/wiki/Square_root): Square Root
$y = \sqrt{x}$

63. **TAN** [Wikipedia](https://en.wikipedia.org/wiki/Trigonometric_functions): Tangent
$y = \tan(x)$

64. **TANH** [Wikipedia](https://en.wikipedia.org/wiki/Hyperbolic_functions): Hyperbolic Tangent
$y = \tanh(x) = \frac{e^x - e^{-x}}{e^x + e^{-x}}$

### Pattern Recognition

All candlestick patterns take $(O, H, L, C)$ and return $+100$ (bullish), $-100$ (bearish), or $0$ (no pattern).
See [Candlestick pattern (Wikipedia)](https://en.wikipedia.org/wiki/Candlestick_pattern).

65. **CDL2CROWS** [Investopedia](https://www.investopedia.com/terms/t/twocrows.asp): Two Crows — bearish reversal, two black candles gapping above uptrend
66. **CDL3BLACKCROWS** [Wikipedia](https://en.wikipedia.org/wiki/Three_black_crows): Three Black Crows — three consecutive long bearish candles
67. **CDL3INSIDE** [Investopedia](https://www.investopedia.com/terms/t/three-inside-updown.asp): Three Inside Up/Down — reversal confirmed by third candle
68. **CDL3LINESTRIKE** [Investopedia](https://www.investopedia.com/terms/t/three-line-strike.asp): Three-Line Strike — three same-direction then strike candle
69. **CDL3OUTSIDE** [Investopedia](https://www.investopedia.com/terms/t/three-outside-updown.asp): Three Outside Up/Down — engulfing confirmed by third candle
70. **CDL3STARSINSOUTH** [Investopedia](https://www.investopedia.com/terms/t/three-stars-south.asp): Three Stars In The South — declining bearish, shrinking bodies
71. **CDL3WHITESOLDIERS** [Wikipedia](https://en.wikipedia.org/wiki/Three_white_soldiers): Three White Soldiers — three consecutive long bullish candles
72. **CDLADVANCEBLOCK** [Investopedia](https://www.investopedia.com/terms/a/advance-block.asp): Advance Block — three bullish with shrinking bodies
73. **CDLBELTHOLD** [Investopedia](https://www.investopedia.com/terms/b/belt-hold.asp): Belt-hold — long candle opening at its extreme
74. **CDLBREAKAWAY** [Investopedia](https://www.investopedia.com/terms/b/breakaway-gap.asp): Breakaway — gap then reversal closing the gap
75. **CDLCLOSINGMARUBOZU** [Investopedia](https://www.investopedia.com/terms/m/marubozu.asp): Closing Marubozu — no shadow on closing side
76. **CDLCONCEALBABYSWALL** [Investopedia](https://www.investopedia.com/terms/c/concealing-baby-swallow.asp): Concealing Baby Swallow — four-candle bearish pattern
77. **CDLCOUNTERATTACK** [Investopedia](https://www.investopedia.com/terms/c/counterattack.asp): Counterattack — two opposite candles, same close
78. **CDLDOJI** [Wikipedia](https://en.wikipedia.org/wiki/Doji): Doji — $|O - C| \approx 0$
79. **CDLDOJISTAR** [Investopedia](https://www.investopedia.com/terms/d/doji.asp): Doji Star — doji gapping from previous candle
80. **CDLDRAGONFLYDOJI** [Investopedia](https://www.investopedia.com/terms/d/dragonfly-doji.asp): Dragonfly Doji — long lower shadow, no upper
81. **CDLENGULFING** [Investopedia](https://www.investopedia.com/terms/b/bullishengulfingpattern.asp): Engulfing — second body fully contains first
82. **CDLGAPSIDESIDEWHITE** [Investopedia](https://www.investopedia.com/terms/u/upside-gap-two-crows.asp): Gap Side-by-Side White — two similar candles after gap
83. **CDLGRAVESTONEDOJI** [Investopedia](https://www.investopedia.com/terms/g/gravestone-doji.asp): Gravestone Doji — long upper shadow, no lower
84. **CDLHAMMER** [Wikipedia](https://en.wikipedia.org/wiki/Hammer_(candlestick_pattern)): Hammer — small body, lower shadow $\geq 2\times$ body
85. **CDLHANGINGMAN** [Wikipedia](https://en.wikipedia.org/wiki/Hanging_man_(candlestick_pattern)): Hanging Man — hammer in uptrend (bearish)
86. **CDLHARAMI** [Wikipedia](https://en.wikipedia.org/wiki/Harami_(candlestick_pattern)): Harami — second candle inside first's body
87. **CDLHARAMICROSS** [Investopedia](https://www.investopedia.com/terms/h/haramicross.asp): Harami Cross — harami where second is doji
88. **CDLHIGHWAVE** [Investopedia](https://www.investopedia.com/terms/l/long-legged-doji.asp): High-Wave Candle — very long shadows, small body
89. **CDLHIKKAKE** [Investopedia](https://www.investopedia.com/terms/h/hikkakepattern.asp): Hikkake — inside bar breakout failure
90. **CDLHIKKAKEMOD** [Investopedia](https://www.investopedia.com/terms/h/hikkakepattern.asp): Modified Hikkake — confirmed by subsequent action
91. **CDLHOMINGPIGEON** [Investopedia](https://www.investopedia.com/terms/h/homing-pigeon.asp): Homing Pigeon — two bearish, second inside first
92. **CDLIDENTICAL3CROWS** [Investopedia](https://www.investopedia.com/terms/i/identical-three-crows.asp): Identical Three Crows — each opens at prior close
93. **CDLINNECK** [Investopedia](https://www.investopedia.com/terms/i/inneck-pattern.asp): In-Neck — small bullish closing near prior low
94. **CDLINVERTEDHAMMER** [Investopedia](https://www.investopedia.com/terms/i/invertedhammer.asp): Inverted Hammer — small body, long upper shadow
95. **CDLKICKING** [Investopedia](https://www.investopedia.com/terms/k/kicking-pattern.asp): Kicking — two opposing marubozu with gap
96. **CDLKICKINGBYLENGTH** [Investopedia](https://www.investopedia.com/terms/k/kicking-pattern.asp): Kicking by Length — direction by longer marubozu
97. **CDLLADDERBOTTOM** [Investopedia](https://www.investopedia.com/terms/l/ladder-bottom.asp): Ladder Bottom — four bearish then bullish reversal
98. **CDLLONGLEGGEDDOJI** [Investopedia](https://www.investopedia.com/terms/l/long-legged-doji.asp): Long Legged Doji — very long both shadows
99. **CDLLONGLINE** [Investopedia](https://www.investopedia.com/terms/l/long-line-candle.asp): Long Line Candle — unusually long body
100. **CDLMARUBOZU** [Investopedia](https://www.investopedia.com/terms/m/marubozu.asp): Marubozu — no shadows (pure body)
101. **CDLMATCHINGLOW** [Investopedia](https://www.investopedia.com/terms/m/matching-low.asp): Matching Low — two bearish with same close
102. **CDLONNECK** [Investopedia](https://www.investopedia.com/terms/o/on-neck-pattern.asp): On-Neck — bullish closing at prior low
103. **CDLPIERCING** [Investopedia](https://www.investopedia.com/terms/p/piercing-pattern.asp): Piercing — bullish closing above midpoint of prior bearish
104. **CDLRICKSHAWMAN** [Investopedia](https://www.investopedia.com/terms/r/rickshaw-man.asp): Rickshaw Man — long-legged doji, body at center
105. **CDLRISEFALL3METHODS** [Investopedia](https://www.investopedia.com/terms/r/rising-three-methods.asp): Rising/Falling Three Methods — continuation pattern
106. **CDLSEPARATINGLINES** [Investopedia](https://www.investopedia.com/terms/s/separating-lines.asp): Separating Lines — opposite candles, same open
107. **CDLSHOOTINGSTAR** [Wikipedia](https://en.wikipedia.org/wiki/Shooting_star_(candlestick_pattern)): Shooting Star — inverted hammer in uptrend
108. **CDLSHORTLINE** [Investopedia](https://www.investopedia.com/terms/s/short-line-candle.asp): Short Line Candle — unusually short body
109. **CDLSPINNINGTOP** [Wikipedia](https://en.wikipedia.org/wiki/Spinning_top_(candlestick_pattern)): Spinning Top — small body with both shadows
110. **CDLSTALLEDPATTERN** [Investopedia](https://www.investopedia.com/terms/s/stalled-pattern.asp): Stalled Pattern — three bullish, third small
111. **CDLSTICKSANDWICH** [Investopedia](https://www.investopedia.com/terms/s/stick-sandwich.asp): Stick Sandwich — same-close sandwiching opposite
112. **CDLTAKURI** [Investopedia](https://www.investopedia.com/terms/t/takuri.asp): Takuri — doji with very long lower shadow
113. **CDLTASUKIGAP** [Investopedia](https://www.investopedia.com/terms/t/tasuki-gap.asp): Tasuki Gap — partial-fill candle after gap
114. **CDLTHRUSTING** [Investopedia](https://www.investopedia.com/terms/t/thrusting-pattern.asp): Thrusting — bullish closing below midpoint of prior
115. **CDLTRISTAR** [Investopedia](https://www.investopedia.com/terms/t/tristar.asp): Tristar — three dojis with middle gapping
116. **CDLUNIQUE3RIVER** [Investopedia](https://www.investopedia.com/terms/u/unique-three-river.asp): Unique 3 River — three-candle bullish reversal
117. **CDLUPSIDEGAP2CROWS** [Investopedia](https://www.investopedia.com/terms/u/upside-gap-two-crows.asp): Upside Gap Two Crows — two bearish gapping above
118. **CDLXSIDEGAP3METHODS** [Investopedia](https://www.investopedia.com/terms/u/updown-gap-sidebyside-white-lines.asp): Gap Three Methods — opposite candle closing gap

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
