# DuckDB TA-Lib Extension — Function Reference

This extension wraps [TA-Lib](https://ta-lib.org/) technical analysis functions for use inside DuckDB SQL queries.

## Function Forms

Every indicator is available in two forms:

- **Scalar** (`ta_` prefix): accepts `LIST<DOUBLE>` inputs, returns a `LIST` of the same length (leading lookback positions are `NULL`).
- **Window / Aggregate** (`taw_` prefix): accepts plain scalar columns and is used with a window `OVER (...)` clause, returning one `DOUBLE` per row.

### Return-type note for pattern recognition

Candlestick pattern functions return `LIST<INTEGER>` (scalar) or `INTEGER` (window). Values are `100` (bullish), `-100` (bearish), or `0` (no pattern).

---

## 1. Overlap Studies

Moving averages and price-envelope indicators.

---

### ta_sma / taw_sma

Simple Moving Average.

**Scalar:** `ta_sma(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_sma(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_ema / taw_ema

Exponential Moving Average.

**Scalar:** `ta_ema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_ema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_wma / taw_wma

Weighted Moving Average.

**Scalar:** `ta_wma(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_wma(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_dema / taw_dema

Double Exponential Moving Average.

**Scalar:** `ta_dema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_dema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_tema / taw_tema

Triple Exponential Moving Average.

**Scalar:** `ta_tema(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_tema(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_trima / taw_trima

Triangular Moving Average.

**Scalar:** `ta_trima(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_trima(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_kama / taw_kama

Kaufman Adaptive Moving Average.

**Scalar:** `ta_kama(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_kama(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_midpoint / taw_midpoint

Midpoint over a rolling window: `(max + min) / 2` of the input series.

**Scalar:** `ta_midpoint(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_midpoint(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_midprice / taw_midprice

Midpoint Price over a rolling window: `(highest_high + lowest_low) / 2`.

**Scalar:** `ta_midprice(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_midprice(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

## 2. Momentum Indicators

---

### ta_rsi / taw_rsi

Relative Strength Index.

**Scalar:** `ta_rsi(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_rsi(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### ta_cmo / taw_cmo

Chande Momentum Oscillator.

**Scalar:** `ta_cmo(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_cmo(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_mom / taw_mom

Momentum: current value minus value `time_period` bars ago.

**Scalar:** `ta_mom(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_mom(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### ta_roc / taw_roc

Rate of Change: `((price / prev_price) - 1) * 100`.

**Scalar:** `ta_roc(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_roc(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### ta_rocp / taw_rocp

Rate of Change Percentage: `(price - prev_price) / prev_price`.

**Scalar:** `ta_rocp(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_rocp(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### ta_rocr / taw_rocr

Rate of Change Ratio: `price / prev_price`.

**Scalar:** `ta_rocr(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_rocr(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### ta_rocr100 / taw_rocr100

Rate of Change Ratio ×100: `(price / prev_price) * 100`.

**Scalar:** `ta_rocr100(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_rocr100(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Lookback period |

---

### ta_trix / taw_trix

1-day Rate-Of-Change of a Triple Smooth EMA.

**Scalar:** `ta_trix(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_trix(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_willr / taw_willr

Williams' %R.

**Scalar:** `ta_willr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_willr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### ta_cci / taw_cci

Commodity Channel Index.

**Scalar:** `ta_cci(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_cci(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### ta_adx / taw_adx

Average Directional Movement Index.

**Scalar:** `ta_adx(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_adx(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### ta_adxr / taw_adxr

Average Directional Movement Index Rating.

**Scalar:** `ta_adxr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_adxr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### ta_dx / taw_dx

Directional Movement Index.

**Scalar:** `ta_dx(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_dx(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### ta_plus_di / taw_plus_di

Plus Directional Indicator (+DI).

**Scalar:** `ta_plus_di(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_plus_di(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### ta_minus_di / taw_minus_di

Minus Directional Indicator (-DI).

**Scalar:** `ta_minus_di(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_minus_di(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### ta_plus_dm / taw_plus_dm

Plus Directional Movement (+DM).

**Scalar:** `ta_plus_dm(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_plus_dm(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

### ta_minus_dm / taw_minus_dm

Minus Directional Movement (-DM).

**Scalar:** `ta_minus_dm(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_minus_dm(high DOUBLE, low DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| time_period | INTEGER | Number of periods |

---

## 3. Volume Indicators

---

### ta_ad / taw_ad

Chaikin Accumulation/Distribution Line. No time period required.

**Scalar:** `ta_ad(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, volume LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_ad(high DOUBLE, low DOUBLE, close DOUBLE, volume DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| volume / volume | LIST\<DOUBLE\> / DOUBLE | Volume series |

---

## 4. Volatility

---

### ta_atr / taw_atr

Average True Range.

**Scalar:** `ta_atr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_atr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods (typically 14) |

---

### ta_natr / taw_natr

Normalized Average True Range (ATR as a percentage of close).

**Scalar:** `ta_natr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_natr(high DOUBLE, low DOUBLE, close DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |
| time_period | INTEGER | Number of periods |

---

### ta_trange / taw_trange

True Range (single-period, no `time_period` parameter).

**Scalar:** `ta_trange(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_trange(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high / high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low / low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close / close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

## 5. Pattern Recognition

All candlestick functions take OHLC inputs and return `LIST<INTEGER>` (scalar) or `INTEGER` (window).  
Return values: `100` = bullish signal, `-100` = bearish signal, `0` = no pattern.

**Scalar signature:** `ta_cdl<name>(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<INTEGER>`  
**Window signature:** `taw_cdl<name>(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> INTEGER`

| Function | Description |
|----------|-------------|
| `ta_cdl2crows` / `taw_cdl2crows` | Two Crows |
| `ta_cdl3blackcrows` / `taw_cdl3blackcrows` | Three Black Crows |
| `ta_cdl3inside` / `taw_cdl3inside` | Three Inside Up/Down |
| `ta_cdl3linestrike` / `taw_cdl3linestrike` | Three-Line Strike |
| `ta_cdl3outside` / `taw_cdl3outside` | Three Outside Up/Down |
| `ta_cdl3starsinsouth` / `taw_cdl3starsinsouth` | Three Stars In The South |
| `ta_cdl3whitesoldiers` / `taw_cdl3whitesoldiers` | Three Advancing White Soldiers |
| `ta_cdladvanceblock` / `taw_cdladvanceblock` | Advance Block |
| `ta_cdlbelthold` / `taw_cdlbelthold` | Belt-hold |
| `ta_cdlbreakaway` / `taw_cdlbreakaway` | Breakaway |
| `ta_cdlclosingmarubozu` / `taw_cdlclosingmarubozu` | Closing Marubozu |
| `ta_cdlconcealbabyswall` / `taw_cdlconcealbabyswall` | Concealing Baby Swallow |
| `ta_cdlcounterattack` / `taw_cdlcounterattack` | Counterattack |
| `ta_cdldoji` / `taw_cdldoji` | Doji |
| `ta_cdldojistar` / `taw_cdldojistar` | Doji Star |
| `ta_cdldragonflydoji` / `taw_cdldragonflydoji` | Dragonfly Doji |
| `ta_cdlengulfing` / `taw_cdlengulfing` | Engulfing Pattern |
| `ta_cdlgapsidesidewhite` / `taw_cdlgapsidesidewhite` | Up/Down-gap Side-by-Side White Lines |
| `ta_cdlgravestonedoji` / `taw_cdlgravestonedoji` | Gravestone Doji |
| `ta_cdlhammer` / `taw_cdlhammer` | Hammer |
| `ta_cdlhangingman` / `taw_cdlhangingman` | Hanging Man |
| `ta_cdlharami` / `taw_cdlharami` | Harami Pattern |
| `ta_cdlharamicross` / `taw_cdlharamicross` | Harami Cross Pattern |
| `ta_cdlhighwave` / `taw_cdlhighwave` | High-Wave Candle |
| `ta_cdlhikkake` / `taw_cdlhikkake` | Hikkake Pattern |
| `ta_cdlhikkakemod` / `taw_cdlhikkakemod` | Modified Hikkake Pattern |
| `ta_cdlhomingpigeon` / `taw_cdlhomingpigeon` | Homing Pigeon |
| `ta_cdlidentical3crows` / `taw_cdlidentical3crows` | Identical Three Crows |
| `ta_cdlinneck` / `taw_cdlinneck` | In-Neck Pattern |
| `ta_cdlinvertedhammer` / `taw_cdlinvertedhammer` | Inverted Hammer |
| `ta_cdlkicking` / `taw_cdlkicking` | Kicking |
| `ta_cdlkickingbylength` / `taw_cdlkickingbylength` | Kicking — bull/bear determined by the longer marubozu |
| `ta_cdlladderbottom` / `taw_cdlladderbottom` | Ladder Bottom |
| `ta_cdllongleggeddoji` / `taw_cdllongleggeddoji` | Long Legged Doji |
| `ta_cdllongline` / `taw_cdllongline` | Long Line Candle |
| `ta_cdlmarubozu` / `taw_cdlmarubozu` | Marubozu |
| `ta_cdlmatchinglow` / `taw_cdlmatchinglow` | Matching Low |
| `ta_cdlonneck` / `taw_cdlonneck` | On-Neck Pattern |
| `ta_cdlpiercing` / `taw_cdlpiercing` | Piercing Pattern |
| `ta_cdlrickshawman` / `taw_cdlrickshawman` | Rickshaw Man |
| `ta_cdlrisefall3methods` / `taw_cdlrisefall3methods` | Rising/Falling Three Methods |
| `ta_cdlseparatinglines` / `taw_cdlseparatinglines` | Separating Lines |
| `ta_cdlshootingstar` / `taw_cdlshootingstar` | Shooting Star |
| `ta_cdlshortline` / `taw_cdlshortline` | Short Line Candle |
| `ta_cdlspinningtop` / `taw_cdlspinningtop` | Spinning Top |
| `ta_cdlstalledpattern` / `taw_cdlstalledpattern` | Stalled Pattern |
| `ta_cdlsticksandwich` / `taw_cdlsticksandwich` | Stick Sandwich |
| `ta_cdltakuri` / `taw_cdltakuri` | Takuri (Dragonfly Doji with very long lower shadow) |
| `ta_cdltasukigap` / `taw_cdltasukigap` | Tasuki Gap |
| `ta_cdlthrusting` / `taw_cdlthrusting` | Thrusting Pattern |
| `ta_cdltristar` / `taw_cdltristar` | Tristar Pattern |
| `ta_cdlunique3river` / `taw_cdlunique3river` | Unique 3 River |
| `ta_cdlupsidegap2crows` / `taw_cdlupsidegap2crows` | Upside Gap Two Crows |
| `ta_cdlxsidegap3methods` / `taw_cdlxsidegap3methods` | Upside/Downside Gap Three Methods |

---

## 6. Price Transform

Functions that transform OHLC prices into a single series. No `time_period` parameter.

---

### ta_avgprice / taw_avgprice

Average Price: `(open + high + low + close) / 4`.

**Scalar:** `ta_avgprice(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_avgprice(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| open | LIST\<DOUBLE\> / DOUBLE | Open price series |
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### ta_bop / taw_bop

Balance of Power: `(close - open) / (high - low)`.

**Scalar:** `ta_bop(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_bop(open DOUBLE, high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| open | LIST\<DOUBLE\> / DOUBLE | Open price series |
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### ta_medprice / taw_medprice

Median Price: `(high + low) / 2`.

**Scalar:** `ta_medprice(high LIST<DOUBLE>, low LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_medprice(high DOUBLE, low DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |

---

### ta_typprice / taw_typprice

Typical Price: `(high + low + close) / 3`.

**Scalar:** `ta_typprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_typprice(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

### ta_wclprice / taw_wclprice

Weighted Close Price: `(high + low + close * 2) / 4`.

**Scalar:** `ta_wclprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_wclprice(high DOUBLE, low DOUBLE, close DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| high | LIST\<DOUBLE\> / DOUBLE | High price series |
| low | LIST\<DOUBLE\> / DOUBLE | Low price series |
| close | LIST\<DOUBLE\> / DOUBLE | Close price series |

---

## 7. Cycle Indicators

Hilbert Transform-based cycle analysis functions. No `time_period` parameter.

---

### ta_ht_dcperiod / taw_ht_dcperiod

Hilbert Transform — Dominant Cycle Period.

**Scalar:** `ta_ht_dcperiod(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_ht_dcperiod(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### ta_ht_dcphase / taw_ht_dcphase

Hilbert Transform — Dominant Cycle Phase.

**Scalar:** `ta_ht_dcphase(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_ht_dcphase(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### ta_ht_trendline / taw_ht_trendline

Hilbert Transform — Instantaneous Trendline.

**Scalar:** `ta_ht_trendline(prices LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window:** `taw_ht_trendline(price DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

### ta_ht_trendmode / taw_ht_trendmode

Hilbert Transform — Trend vs Cycle Mode. Returns `1` (trending) or `0` (cycling).

**Scalar:** `ta_ht_trendmode(prices LIST<DOUBLE>) -> LIST<INTEGER>`  
**Window:** `taw_ht_trendmode(price DOUBLE) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series (typically close) |

---

## 8. Statistics

---

### ta_linearreg / taw_linearreg

Linear Regression: endpoint value of the linear regression line.

**Scalar:** `ta_linearreg(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_linearreg(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_linearreg_angle / taw_linearreg_angle

Linear Regression Angle (degrees).

**Scalar:** `ta_linearreg_angle(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_linearreg_angle(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_linearreg_intercept / taw_linearreg_intercept

Linear Regression Intercept.

**Scalar:** `ta_linearreg_intercept(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_linearreg_intercept(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_linearreg_slope / taw_linearreg_slope

Linear Regression Slope.

**Scalar:** `ta_linearreg_slope(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_linearreg_slope(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_tsf / taw_tsf

Time Series Forecast (next value projected by linear regression).

**Scalar:** `ta_tsf(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_tsf(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_sum / taw_sum

Rolling Sum.

**Scalar:** `ta_sum(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_sum(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_max / taw_max

Highest value over a rolling window.

**Scalar:** `ta_max(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_max(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_min / taw_min

Lowest value over a rolling window.

**Scalar:** `ta_min(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<DOUBLE>`  
**Window:** `taw_min(price DOUBLE, time_period INTEGER) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_maxindex / taw_maxindex

Index of highest value over a rolling window.

**Scalar:** `ta_maxindex(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<INTEGER>`  
**Window:** `taw_maxindex(price DOUBLE, time_period INTEGER) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

### ta_minindex / taw_minindex

Index of lowest value over a rolling window.

**Scalar:** `ta_minindex(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<INTEGER>`  
**Window:** `taw_minindex(price DOUBLE, time_period INTEGER) OVER (...) -> INTEGER`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices / price | LIST\<DOUBLE\> / DOUBLE | Input price series |
| time_period | INTEGER | Number of periods |

---

## 9. Math Transform

Element-wise mathematical functions. No `time_period` parameter.

| Function | Description |
|----------|-------------|
| `ta_acos` / `taw_acos` | Arc Cosine |
| `ta_asin` / `taw_asin` | Arc Sine |
| `ta_atan` / `taw_atan` | Arc Tangent |
| `ta_ceil` / `taw_ceil` | Ceiling |
| `ta_cos` / `taw_cos` | Cosine |
| `ta_cosh` / `taw_cosh` | Hyperbolic Cosine |
| `ta_exp` / `taw_exp` | Exponential (`e^x`) |
| `ta_floor` / `taw_floor` | Floor |
| `ta_ln` / `taw_ln` | Natural Logarithm |
| `ta_log10` / `taw_log10` | Base-10 Logarithm |
| `ta_sin` / `taw_sin` | Sine |
| `ta_sinh` / `taw_sinh` | Hyperbolic Sine |
| `ta_sqrt` / `taw_sqrt` | Square Root |
| `ta_tan` / `taw_tan` | Tangent |
| `ta_tanh` / `taw_tanh` | Hyperbolic Tangent |

**Scalar signature:** `ta_<fn>(values LIST<DOUBLE>) -> LIST<DOUBLE>`  
**Window signature:** `taw_<fn>(value DOUBLE) OVER (...) -> DOUBLE`

| Parameter | Type | Description |
|-----------|------|-------------|
| values / value | LIST\<DOUBLE\> / DOUBLE | Input series |

---

## 10. Multi-Output Functions

These functions return a `LIST<STRUCT>` where each struct contains multiple named output fields. The scalar form only — there is no `taw_` window form for multi-output functions.

---

### ta_macd

Moving Average Convergence/Divergence.

**Scalar:** `ta_macd(prices LIST<DOUBLE>, fast_period INTEGER, slow_period INTEGER, signal_period INTEGER) -> LIST<STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| fast_period | INTEGER | 12 | Fast EMA period |
| slow_period | INTEGER | 26 | Slow EMA period |
| signal_period | INTEGER | 9 | Signal EMA period |

**Output fields:** `macd`, `signal`, `hist`

**Example:**
```sql
SELECT unnest(ta_macd(list(close ORDER BY ts), 12, 26, 9))
FROM prices
GROUP BY symbol;
```

---

### ta_bbands

Bollinger Bands.

**Scalar:** `ta_bbands(prices LIST<DOUBLE>, time_period INTEGER, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INTEGER) -> LIST<STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)>`

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
SELECT unnest(ta_bbands(list(close ORDER BY ts), 20, 2.0, 2.0, 0))
FROM prices
GROUP BY symbol;
```

---

### ta_stoch

Stochastic Oscillator.

**Scalar:** `ta_stoch(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, fastk_period INTEGER, slowk_period INTEGER, slowk_matype INTEGER, slowd_period INTEGER, slowd_matype INTEGER) -> LIST<STRUCT(slowk DOUBLE, slowd DOUBLE)>`

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

### ta_aroon

Aroon Indicator.

**Scalar:** `ta_aroon(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| high | LIST\<DOUBLE\> | — | High price series |
| low | LIST\<DOUBLE\> | — | Low price series |
| time_period | INTEGER | 14 | Number of periods |

**Output fields:** `aroon_down`, `aroon_up`

---

### ta_minmax

Lowest and highest values over a rolling window in a single pass.

**Scalar:** `ta_minmax(prices LIST<DOUBLE>, time_period INTEGER) -> LIST<STRUCT(min DOUBLE, max DOUBLE)>`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series |
| time_period | INTEGER | Number of periods |

**Output fields:** `min`, `max`

---

### ta_mama

MESA Adaptive Moving Average.

**Scalar:** `ta_mama(prices LIST<DOUBLE>, fast_limit DOUBLE, slow_limit DOUBLE) -> LIST<STRUCT(mama DOUBLE, fama DOUBLE)>`

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| prices | LIST\<DOUBLE\> | — | Input price series |
| fast_limit | DOUBLE | 0.5 | Upper limit for the adaptive factor |
| slow_limit | DOUBLE | 0.05 | Lower limit for the adaptive factor |

**Output fields:** `mama` (MESA Adaptive MA), `fama` (Following Adaptive MA)

---

### ta_ht_phasor

Hilbert Transform — Phasor Components.

**Scalar:** `ta_ht_phasor(prices LIST<DOUBLE>) -> LIST<STRUCT(inphase DOUBLE, quadrature DOUBLE)>`

| Parameter | Type | Description |
|-----------|------|-------------|
| prices | LIST\<DOUBLE\> | Input price series (typically close) |

**Output fields:** `inphase`, `quadrature`

---

### ta_ht_sine

Hilbert Transform — SineWave Indicator.

**Scalar:** `ta_ht_sine(prices LIST<DOUBLE>) -> LIST<STRUCT(sine DOUBLE, leadsine DOUBLE)>`

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
    ta_rsi(list(close ORDER BY ts), 14) AS rsi_series
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
    taw_rsi(close, 14) OVER (PARTITION BY symbol ORDER BY ts ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS rsi
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
    SELECT symbol, unnest(ta_macd(list(close ORDER BY ts), 12, 26, 9)) AS r
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
