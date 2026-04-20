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

## Multi-Output Function Signatures

| # | Function | Signature | Return |
|---|----------|-----------|--------|
| 1 | `ta_macd` | (close DOUBLE, fast INT, slow INT, signal INT) | STRUCT(macd, signal, hist) |
| 2 | `ta_bbands` | (close DOUBLE, period INT, devup DOUBLE, devdn DOUBLE, matype INT) | STRUCT(upper, middle, lower) |
| 3 | `ta_stoch` | (high DOUBLE, low DOUBLE, close DOUBLE, fastK INT, slowK INT, slowKMA INT, slowD INT, slowDMA INT) | STRUCT(slowk, slowd) |
| 4 | `ta_aroon` | (high DOUBLE, low DOUBLE, period INT) | STRUCT(aroon_down, aroon_up) |
| 5 | `ta_minmax` | (close DOUBLE, period INT) | STRUCT(min, max) |
| 6 | `ta_mama` | (close DOUBLE, fast_limit DOUBLE, slow_limit DOUBLE) | STRUCT(mama, fama) |
| 7 | `ta_ht_phasor` | (close DOUBLE) | STRUCT(inphase, quadrature) |
| 8 | `ta_ht_sine` | (close DOUBLE) | STRUCT(sine, leadsine) |

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

These functions return multiple named output fields per computation.

- **Scalar** (`t_` prefix): returns `LIST<STRUCT>` — one struct per input element.
- **Window** (`ta_` prefix): returns `STRUCT` — one struct per row, use with `OVER (...)`. Access fields with dot notation: `ta_macd(...).macd`.

---

### t_macd / ta_macd

Moving Average Convergence/Divergence.

**Scalar:** `t_macd(prices LIST<DOUBLE>, fast_period INTEGER, slow_period INTEGER, signal_period INTEGER) -> LIST<STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)>`  
**Window:** `ta_macd(price DOUBLE, fast_period INTEGER, slow_period INTEGER, signal_period INTEGER) OVER (...) -> STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)`

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

### t_bbands / ta_bbands

Bollinger Bands.

**Scalar:** `t_bbands(prices LIST<DOUBLE>, time_period INTEGER, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INTEGER) -> LIST<STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)>`  
**Window:** `ta_bbands(price DOUBLE, time_period INTEGER, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INTEGER) OVER (...) -> STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)`

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

### t_stoch / ta_stoch

Stochastic Oscillator.

**Scalar:** `t_stoch(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, fastk_period INTEGER, slowk_period INTEGER, slowk_matype INTEGER, slowd_period INTEGER, slowd_matype INTEGER) -> LIST<STRUCT(slowk DOUBLE, slowd DOUBLE)>`  
**Window:** `ta_stoch(high DOUBLE, low DOUBLE, close DOUBLE, fastk_period INTEGER, slowk_period INTEGER, slowk_matype INTEGER, slowd_period INTEGER, slowd_matype INTEGER) OVER (...) -> STRUCT(slowk DOUBLE, slowd DOUBLE)`

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

### t_aroon / ta_aroon

Aroon Indicator.

**Scalar:** `t_aroon(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)>`  
**Window:** `ta_aroon(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| high | LIST\<DOUBLE\> | — | High price series |
| low | LIST\<DOUBLE\> | — | Low price series |
| time_period | INTEGER | 14 | Number of periods |

**Output fields:** `aroon_down`, `aroon_up`

---

### t_minmax / ta_minmax

Lowest and highest values over a rolling window in a single pass.

**Scalar:** `t_minmax(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(min DOUBLE, max DOUBLE)>`  
**Window:** `ta_minmax(price DOUBLE, time_period INTEGER) OVER (...) -> STRUCT(min DOUBLE, max DOUBLE)`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series |
| time_period | INTEGER | Number of periods |

**Output fields:** `min`, `max`

---

### t_mama / ta_mama

MESA Adaptive Moving Average.

**Scalar:** `t_mama(prices LIST<DOUBLE>, fast_limit DOUBLE, slow_limit DOUBLE) -> LIST<STRUCT(mama DOUBLE, fama DOUBLE)>`  
**Window:** `ta_mama(price DOUBLE, fast_limit DOUBLE, slow_limit DOUBLE) OVER (...) -> STRUCT(mama DOUBLE, fama DOUBLE)`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| fast_limit | DOUBLE | 0.5 | Upper limit for the adaptive factor |
| slow_limit | DOUBLE | 0.05 | Lower limit for the adaptive factor |

**Output fields:** `mama` (MESA Adaptive MA), `fama` (Following Adaptive MA)

---

### t_ht_phasor / ta_ht_phasor

Hilbert Transform — Phasor Components.

**Scalar:** `t_ht_phasor(prices LIST<DOUBLE>) -> LIST<STRUCT(inphase DOUBLE, quadrature DOUBLE)>`  
**Window:** `ta_ht_phasor(price DOUBLE) OVER (...) -> STRUCT(inphase DOUBLE, quadrature DOUBLE)`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series (typically close) |

**Output fields:** `inphase`, `quadrature`

---

### t_ht_sine / ta_ht_sine

Hilbert Transform — SineWave Indicator.

**Scalar:** `t_ht_sine(prices LIST<DOUBLE>) -> LIST<STRUCT(sine DOUBLE, leadsine DOUBLE)>`  
**Window:** `ta_ht_sine(price DOUBLE) OVER (...) -> STRUCT(sine DOUBLE, leadsine DOUBLE)`

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
