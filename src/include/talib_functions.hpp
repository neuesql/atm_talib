// X-macro table for TA-Lib single-output functions
// This file has NO include guards — it is included multiple times with
// different macro definitions.
//
// Format: TALIB_FUNC(sql_name, ta_func, ta_lookback, pattern, return_type,
// description, category)
//   sql_name    — lowercase name used in SQL as t_<sql_name> (scalar form)
//   ta_func     — TA-Lib C function name (e.g. TA_SMA)
//   ta_lookback — TA-Lib lookback function name (e.g. TA_SMA_Lookback)
//   pattern     — P1..P8 (input signature pattern)
//   return_type — DOUBLE or INT
//   description — one sentence describing the indicator, shared between the
//                 scalar (t_*) and window/aggregate (ta_*) forms
//   category    — one of: Trend, Momentum, Volume, Volatility, Cycle,
//                 Price Transform, Statistics, Math Transform,
//                 Pattern Recognition (per Wikipedia: Technical indicator)

// ============================================================
// P1: (inReal[], timePeriod) -> outReal[]
// ============================================================

// Overlap Studies
TALIB_FUNC(sma, TA_SMA, TA_SMA_Lookback, P1, DOUBLE,
           "Simple Moving Average: the unweighted mean of the previous N "
           "values.",
           "Trend")
TALIB_FUNC(ema, TA_EMA, TA_EMA_Lookback, P1, DOUBLE,
           "Exponential Moving Average: a weighted average that discounts "
           "older values exponentially, giving more weight to recent "
           "prices.",
           "Trend")
TALIB_FUNC(wma, TA_WMA, TA_WMA_Lookback, P1, DOUBLE,
           "Weighted Moving Average: a moving average where each value is "
           "weighted linearly by its recency, with the most recent value "
           "weighted highest.",
           "Trend")
TALIB_FUNC(dema, TA_DEMA, TA_DEMA_Lookback, P1, DOUBLE,
           "Double Exponential Moving Average: reduces the lag of a plain "
           "EMA by combining it with an EMA of itself.",
           "Trend")
TALIB_FUNC(tema, TA_TEMA, TA_TEMA_Lookback, P1, DOUBLE,
           "Triple Exponential Moving Average: further reduces lag versus "
           "DEMA by combining three nested EMAs.",
           "Trend")
TALIB_FUNC(trima, TA_TRIMA, TA_TRIMA_Lookback, P1, DOUBLE,
           "Triangular Moving Average: a doubly-smoothed moving average (an "
           "SMA of an SMA) that weights the middle of the window most "
           "heavily.",
           "Trend")
TALIB_FUNC(kama, TA_KAMA, TA_KAMA_Lookback, P1, DOUBLE,
           "Kaufman Adaptive Moving Average: a moving average whose "
           "smoothing adapts to market volatility and trend efficiency.",
           "Trend")
TALIB_FUNC(midpoint, TA_MIDPOINT, TA_MIDPOINT_Lookback, P1, DOUBLE,
           "MidPoint over period: the average of the highest and lowest "
           "value of the input within the period.",
           "Trend")

// Momentum (single array + period)
TALIB_FUNC(rsi, TA_RSI, TA_RSI_Lookback, P1, DOUBLE,
           "Relative Strength Index: a 0-100 momentum oscillator derived "
           "from the ratio of average gains to average losses over the "
           "period.",
           "Momentum")
TALIB_FUNC(cmo, TA_CMO, TA_CMO_Lookback, P1, DOUBLE,
           "Chande Momentum Oscillator: a -100 to 100 momentum oscillator "
           "based on the difference between the sum of recent gains and "
           "losses.",
           "Momentum")
TALIB_FUNC(mom, TA_MOM, TA_MOM_Lookback, P1, DOUBLE,
           "Momentum: the difference between the current value and the "
           "value N periods ago.",
           "Momentum")
TALIB_FUNC(roc, TA_ROC, TA_ROC_Lookback, P1, DOUBLE,
           "Rate of Change: the percentage change between the current "
           "value and the value N periods ago.",
           "Momentum")
TALIB_FUNC(rocp, TA_ROCP, TA_ROCP_Lookback, P1, DOUBLE,
           "Rate of Change (Percentage): the fractional change between the "
           "current value and the value N periods ago.",
           "Momentum")
TALIB_FUNC(rocr, TA_ROCR, TA_ROCR_Lookback, P1, DOUBLE,
           "Rate of Change (Ratio): the ratio of the current value to the "
           "value N periods ago.",
           "Momentum")
TALIB_FUNC(rocr100, TA_ROCR100, TA_ROCR100_Lookback, P1, DOUBLE,
           "Rate of Change (Ratio x100): the ratio of the current value to "
           "the value N periods ago, scaled by 100.",
           "Momentum")
TALIB_FUNC(trix, TA_TRIX, TA_TRIX_Lookback, P1, DOUBLE,
           "Triple Smooth EMA Rate of Change: the rate of change of a "
           "triple-smoothed exponential moving average, used to filter out "
           "short-term noise.",
           "Momentum")

// Statistics
TALIB_FUNC(linearreg, TA_LINEARREG, TA_LINEARREG_Lookback, P1, DOUBLE,
           "Linear Regression: the value of the best-fit line through the "
           "last N points, evaluated at the current point.",
           "Statistics")
TALIB_FUNC(linearreg_angle, TA_LINEARREG_ANGLE, TA_LINEARREG_ANGLE_Lookback, P1,
           DOUBLE,
           "Linear Regression Angle: the angle, in degrees, of the slope of "
           "the best-fit line through the last N points.",
           "Statistics")
TALIB_FUNC(linearreg_intercept, TA_LINEARREG_INTERCEPT,
           TA_LINEARREG_INTERCEPT_Lookback, P1, DOUBLE,
           "Linear Regression Intercept: the y-intercept of the best-fit "
           "line through the last N points.",
           "Statistics")
TALIB_FUNC(linearreg_slope, TA_LINEARREG_SLOPE, TA_LINEARREG_SLOPE_Lookback, P1,
           DOUBLE,
           "Linear Regression Slope: the slope of the best-fit line "
           "through the last N points.",
           "Statistics")
TALIB_FUNC(tsf, TA_TSF, TA_TSF_Lookback, P1, DOUBLE,
           "Time Series Forecast: the best-fit line through the last N "
           "points, projected one period forward.",
           "Statistics")
TALIB_FUNC(sum, TA_SUM, TA_SUM_Lookback, P1, DOUBLE,
           "Rolling Summation: the sum of the last N values.", "Statistics")
TALIB_FUNC(max, TA_MAX, TA_MAX_Lookback, P1, DOUBLE,
           "Highest Value over period: the maximum of the last N values.",
           "Statistics")
TALIB_FUNC(min, TA_MIN, TA_MIN_Lookback, P1, DOUBLE,
           "Lowest Value over period: the minimum of the last N values.",
           "Statistics")

// P1 returning INT
TALIB_FUNC(maxindex, TA_MAXINDEX, TA_MAXINDEX_Lookback, P1, INT,
           "Index of Highest Value: the position within the last N values "
           "where the maximum occurs.",
           "Statistics")
TALIB_FUNC(minindex, TA_MININDEX, TA_MININDEX_Lookback, P1, INT,
           "Index of Lowest Value: the position within the last N values "
           "where the minimum occurs.",
           "Statistics")

// ============================================================
// P2: (inReal[]) -> outReal[] or outInt[]
// ============================================================

// Hilbert Transform
TALIB_FUNC(ht_dcperiod, TA_HT_DCPERIOD, TA_HT_DCPERIOD_Lookback, P2, DOUBLE,
           "Hilbert Transform Dominant Cycle Period: the estimated length, "
           "in bars, of the dominant price cycle.",
           "Cycle")
TALIB_FUNC(ht_dcphase, TA_HT_DCPHASE, TA_HT_DCPHASE_Lookback, P2, DOUBLE,
           "Hilbert Transform Dominant Cycle Phase: the phase angle, in "
           "degrees, of the dominant price cycle.",
           "Cycle")
TALIB_FUNC(ht_trendline, TA_HT_TRENDLINE, TA_HT_TRENDLINE_Lookback, P2, DOUBLE,
           "Hilbert Transform Instantaneous Trendline: a Hilbert-smoothed "
           "trend component of the price series.",
           "Cycle")
TALIB_FUNC(ht_trendmode, TA_HT_TRENDMODE, TA_HT_TRENDMODE_Lookback, P2, INT,
           "Hilbert Transform Trend vs Cycle Mode: returns 1 when the "
           "market is trending and 0 when it is in a cycling mode.",
           "Cycle")

// Math Transform
TALIB_FUNC(acos, TA_ACOS, TA_ACOS_Lookback, P2, DOUBLE,
           "Arc Cosine: the inverse cosine of each input value, in "
           "radians.",
           "Math Transform")
TALIB_FUNC(asin, TA_ASIN, TA_ASIN_Lookback, P2, DOUBLE,
           "Arc Sine: the inverse sine of each input value, in radians.",
           "Math Transform")
TALIB_FUNC(atan, TA_ATAN, TA_ATAN_Lookback, P2, DOUBLE,
           "Arc Tangent: the inverse tangent of each input value, in "
           "radians.",
           "Math Transform")
TALIB_FUNC(ceil, TA_CEIL, TA_CEIL_Lookback, P2, DOUBLE,
           "Ceiling: the smallest integer greater than or equal to each "
           "input value.",
           "Math Transform")
TALIB_FUNC(cos, TA_COS, TA_COS_Lookback, P2, DOUBLE,
           "Cosine of each input value, in radians.", "Math Transform")
TALIB_FUNC(cosh, TA_COSH, TA_COSH_Lookback, P2, DOUBLE,
           "Hyperbolic Cosine of each input value.", "Math Transform")
TALIB_FUNC(exp, TA_EXP, TA_EXP_Lookback, P2, DOUBLE,
           "Exponential: e raised to the power of each input value.",
           "Math Transform")
TALIB_FUNC(floor, TA_FLOOR, TA_FLOOR_Lookback, P2, DOUBLE,
           "Floor: the largest integer less than or equal to each input "
           "value.",
           "Math Transform")
TALIB_FUNC(ln, TA_LN, TA_LN_Lookback, P2, DOUBLE,
           "Natural Logarithm of each input value.", "Math Transform")
TALIB_FUNC(log10, TA_LOG10, TA_LOG10_Lookback, P2, DOUBLE,
           "Base-10 Logarithm of each input value.", "Math Transform")
TALIB_FUNC(sin, TA_SIN, TA_SIN_Lookback, P2, DOUBLE,
           "Sine of each input value, in radians.", "Math Transform")
TALIB_FUNC(sinh, TA_SINH, TA_SINH_Lookback, P2, DOUBLE,
           "Hyperbolic Sine of each input value.", "Math Transform")
TALIB_FUNC(sqrt, TA_SQRT, TA_SQRT_Lookback, P2, DOUBLE,
           "Square Root of each input value.", "Math Transform")
TALIB_FUNC(tan, TA_TAN, TA_TAN_Lookback, P2, DOUBLE,
           "Tangent of each input value, in radians.", "Math Transform")
TALIB_FUNC(tanh, TA_TANH, TA_TANH_Lookback, P2, DOUBLE,
           "Hyperbolic Tangent of each input value.", "Math Transform")

// ============================================================
// P3: (inHigh[], inLow[], inClose[], timePeriod) -> outReal[]
// ============================================================

// Volatility
TALIB_FUNC(atr, TA_ATR, TA_ATR_Lookback, P3, DOUBLE,
           "Average True Range: a Wilder-smoothed moving average of the "
           "true range, measuring volatility.",
           "Volatility")
TALIB_FUNC(natr, TA_NATR, TA_NATR_Lookback, P3, DOUBLE,
           "Normalized Average True Range: the ATR expressed as a "
           "percentage of the closing price, for cross-instrument "
           "comparison.",
           "Volatility")

// Momentum (HLC + period)
TALIB_FUNC(willr, TA_WILLR, TA_WILLR_Lookback, P3, DOUBLE,
           "Williams %R: a momentum oscillator measuring the close "
           "relative to the high-low range over the period, scaled -100 "
           "to 0.",
           "Momentum")
TALIB_FUNC(cci, TA_CCI, TA_CCI_Lookback, P3, DOUBLE,
           "Commodity Channel Index: measures the deviation of the typical "
           "price from its moving average, relative to mean absolute "
           "deviation.",
           "Momentum")
TALIB_FUNC(adx, TA_ADX, TA_ADX_Lookback, P3, DOUBLE,
           "Average Directional Movement Index: measures trend strength "
           "(not direction) by smoothing the directional movement index.",
           "Trend")
TALIB_FUNC(adxr, TA_ADXR, TA_ADXR_Lookback, P3, DOUBLE,
           "Average Directional Movement Rating: the average of the "
           "current and a prior ADX value, smoothing ADX further.",
           "Trend")
TALIB_FUNC(dx, TA_DX, TA_DX_Lookback, P3, DOUBLE,
           "Directional Movement Index: measures the difference between "
           "the plus and minus directional indicators relative to their "
           "sum.",
           "Trend")
TALIB_FUNC(plus_di, TA_PLUS_DI, TA_PLUS_DI_Lookback, P3, DOUBLE,
           "Plus Directional Indicator: the smoothed ratio of upward "
           "directional movement to the average true range.",
           "Trend")
TALIB_FUNC(minus_di, TA_MINUS_DI, TA_MINUS_DI_Lookback, P3, DOUBLE,
           "Minus Directional Indicator: the smoothed ratio of downward "
           "directional movement to the average true range.",
           "Trend")

// ============================================================
// P4: (inHigh[], inLow[], inClose[], inVolume[]) -> outReal[]
// ============================================================
TALIB_FUNC(ad, TA_AD, TA_AD_Lookback, P4, DOUBLE,
           "Chaikin Accumulation/Distribution Line: a volume-weighted "
           "running total of each period's position within its high-low "
           "range.",
           "Volume")

// ============================================================
// P5: (inOpen[], inHigh[], inLow[], inClose[]) -> outReal[] or outInt[]
// ============================================================

// Price Transform (OHLC -> DOUBLE)
TALIB_FUNC(avgprice, TA_AVGPRICE, TA_AVGPRICE_Lookback, P5, DOUBLE,
           "Average Price: the mean of the open, high, low, and close for "
           "the period.",
           "Price Transform")
TALIB_FUNC(bop, TA_BOP, TA_BOP_Lookback, P5, DOUBLE,
           "Balance of Power: measures buying versus selling pressure as "
           "the ratio of the close-open difference to the high-low range.",
           "Momentum")

// Pattern Recognition (OHLC -> INT) — standard P5 (no extra params)
TALIB_FUNC(cdl2crows, TA_CDL2CROWS, TA_CDL2CROWS_Lookback, P5, INT,
           "Two Crows candlestick pattern: a bearish reversal signal of "
           "two black candles gapping above an uptrend.",
           "Pattern Recognition")
TALIB_FUNC(cdl3blackcrows, TA_CDL3BLACKCROWS, TA_CDL3BLACKCROWS_Lookback, P5,
           INT,
           "Three Black Crows candlestick pattern: three consecutive long "
           "bearish candles signaling a downtrend.",
           "Pattern Recognition")
TALIB_FUNC(cdl3inside, TA_CDL3INSIDE, TA_CDL3INSIDE_Lookback, P5, INT,
           "Three Inside Up/Down candlestick pattern: a reversal pattern "
           "confirmed by a third candle.",
           "Pattern Recognition")
TALIB_FUNC(cdl3linestrike, TA_CDL3LINESTRIKE, TA_CDL3LINESTRIKE_Lookback, P5,
           INT,
           "Three-Line Strike candlestick pattern: three same-direction "
           "candles followed by a reversing strike candle.",
           "Pattern Recognition")
TALIB_FUNC(cdl3outside, TA_CDL3OUTSIDE, TA_CDL3OUTSIDE_Lookback, P5, INT,
           "Three Outside Up/Down candlestick pattern: an engulfing "
           "pattern confirmed by a third candle.",
           "Pattern Recognition")
TALIB_FUNC(cdl3starsinsouth, TA_CDL3STARSINSOUTH, TA_CDL3STARSINSOUTH_Lookback,
           P5, INT,
           "Three Stars In The South candlestick pattern: a bullish "
           "reversal of three declining bearish candles with shrinking "
           "bodies.",
           "Pattern Recognition")
TALIB_FUNC(cdl3whitesoldiers, TA_CDL3WHITESOLDIERS,
           TA_CDL3WHITESOLDIERS_Lookback, P5, INT,
           "Three Advancing White Soldiers candlestick pattern: three "
           "consecutive long bullish candles signaling an uptrend.",
           "Pattern Recognition")
TALIB_FUNC(cdladvanceblock, TA_CDLADVANCEBLOCK, TA_CDLADVANCEBLOCK_Lookback, P5,
           INT,
           "Advance Block candlestick pattern: three bullish candles with "
           "progressively shrinking bodies, signaling weakening momentum.",
           "Pattern Recognition")
TALIB_FUNC(cdlbelthold, TA_CDLBELTHOLD, TA_CDLBELTHOLD_Lookback, P5, INT,
           "Belt-hold candlestick pattern: a long candle that opens at its "
           "extreme for the session.",
           "Pattern Recognition")
TALIB_FUNC(cdlbreakaway, TA_CDLBREAKAWAY, TA_CDLBREAKAWAY_Lookback, P5, INT,
           "Breakaway candlestick pattern: a gap followed by a reversal "
           "that closes back into the gap.",
           "Pattern Recognition")
TALIB_FUNC(cdlclosingmarubozu, TA_CDLCLOSINGMARUBOZU,
           TA_CDLCLOSINGMARUBOZU_Lookback, P5, INT,
           "Closing Marubozu candlestick pattern: a candle with no shadow "
           "on its closing side.",
           "Pattern Recognition")
TALIB_FUNC(cdlconcealbabyswall, TA_CDLCONCEALBABYSWALL,
           TA_CDLCONCEALBABYSWALL_Lookback, P5, INT,
           "Concealing Baby Swallow candlestick pattern: a four-candle "
           "bearish continuation pattern.",
           "Pattern Recognition")
TALIB_FUNC(cdlcounterattack, TA_CDLCOUNTERATTACK, TA_CDLCOUNTERATTACK_Lookback,
           P5, INT,
           "Counterattack candlestick pattern: two opposite-colored "
           "candles that close at the same level.",
           "Pattern Recognition")
TALIB_FUNC(cdldoji, TA_CDLDOJI, TA_CDLDOJI_Lookback, P5, INT,
           "Doji candlestick pattern: a candle whose open and close are "
           "virtually equal, signaling indecision.",
           "Pattern Recognition")
TALIB_FUNC(cdldojistar, TA_CDLDOJISTAR, TA_CDLDOJISTAR_Lookback, P5, INT,
           "Doji Star candlestick pattern: a doji that gaps away from the "
           "previous candle.",
           "Pattern Recognition")
TALIB_FUNC(cdldragonflydoji, TA_CDLDRAGONFLYDOJI, TA_CDLDRAGONFLYDOJI_Lookback,
           P5, INT,
           "Dragonfly Doji candlestick pattern: a doji with a long lower "
           "shadow and no upper shadow.",
           "Pattern Recognition")
TALIB_FUNC(cdlengulfing, TA_CDLENGULFING, TA_CDLENGULFING_Lookback, P5, INT,
           "Engulfing candlestick pattern: a candle whose body fully "
           "contains the prior candle's body.",
           "Pattern Recognition")
TALIB_FUNC(cdlgapsidesidewhite, TA_CDLGAPSIDESIDEWHITE,
           TA_CDLGAPSIDESIDEWHITE_Lookback, P5, INT,
           "Gap Side-by-Side White Lines candlestick pattern: two "
           "similarly-sized candles following a gap.",
           "Pattern Recognition")
TALIB_FUNC(cdlgravestonedoji, TA_CDLGRAVESTONEDOJI,
           TA_CDLGRAVESTONEDOJI_Lookback, P5, INT,
           "Gravestone Doji candlestick pattern: a doji with a long upper "
           "shadow and no lower shadow.",
           "Pattern Recognition")
TALIB_FUNC(cdlhammer, TA_CDLHAMMER, TA_CDLHAMMER_Lookback, P5, INT,
           "Hammer candlestick pattern: a small body with a lower shadow "
           "at least twice the body length, in a downtrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlhangingman, TA_CDLHANGINGMAN, TA_CDLHANGINGMAN_Lookback, P5, INT,
           "Hanging Man candlestick pattern: a hammer-shaped candle "
           "occurring in an uptrend, signaling a bearish reversal.",
           "Pattern Recognition")
TALIB_FUNC(cdlharami, TA_CDLHARAMI, TA_CDLHARAMI_Lookback, P5, INT,
           "Harami candlestick pattern: a small candle whose body is "
           "contained within the prior candle's larger body.",
           "Pattern Recognition")
TALIB_FUNC(cdlharamicross, TA_CDLHARAMICROSS, TA_CDLHARAMICROSS_Lookback, P5,
           INT,
           "Harami Cross candlestick pattern: a Harami pattern where the "
           "second candle is a doji.",
           "Pattern Recognition")
TALIB_FUNC(cdlhighwave, TA_CDLHIGHWAVE, TA_CDLHIGHWAVE_Lookback, P5, INT,
           "High-Wave Candle pattern: a small body with very long upper "
           "and lower shadows.",
           "Pattern Recognition")
TALIB_FUNC(cdlhikkake, TA_CDLHIKKAKE, TA_CDLHIKKAKE_Lookback, P5, INT,
           "Hikkake candlestick pattern: an inside-bar setup followed by a "
           "failed breakout.",
           "Pattern Recognition")
TALIB_FUNC(cdlhikkakemod, TA_CDLHIKKAKEMOD, TA_CDLHIKKAKEMOD_Lookback, P5, INT,
           "Modified Hikkake candlestick pattern: a Hikkake pattern "
           "confirmed by subsequent price action.",
           "Pattern Recognition")
TALIB_FUNC(cdlhomingpigeon, TA_CDLHOMINGPIGEON, TA_CDLHOMINGPIGEON_Lookback, P5,
           INT,
           "Homing Pigeon candlestick pattern: two bearish candles where "
           "the second body is contained within the first.",
           "Pattern Recognition")
TALIB_FUNC(cdlidentical3crows, TA_CDLIDENTICAL3CROWS,
           TA_CDLIDENTICAL3CROWS_Lookback, P5, INT,
           "Identical Three Crows candlestick pattern: three bearish "
           "candles that each open at the prior candle's close.",
           "Pattern Recognition")
TALIB_FUNC(cdlinneck, TA_CDLINNECK, TA_CDLINNECK_Lookback, P5, INT,
           "In-Neck candlestick pattern: a small bullish candle closing "
           "near the prior candle's low, in a downtrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlinvertedhammer, TA_CDLINVERTEDHAMMER,
           TA_CDLINVERTEDHAMMER_Lookback, P5, INT,
           "Inverted Hammer candlestick pattern: a small body with a long "
           "upper shadow, in a downtrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlkicking, TA_CDLKICKING, TA_CDLKICKING_Lookback, P5, INT,
           "Kicking candlestick pattern: two opposing marubozu candles "
           "separated by a gap.",
           "Pattern Recognition")
TALIB_FUNC(cdlkickingbylength, TA_CDLKICKINGBYLENGTH,
           TA_CDLKICKINGBYLENGTH_Lookback, P5, INT,
           "Kicking by Length candlestick pattern: a Kicking pattern whose "
           "direction is determined by the longer marubozu.",
           "Pattern Recognition")
TALIB_FUNC(cdlladderbottom, TA_CDLLADDERBOTTOM, TA_CDLLADDERBOTTOM_Lookback, P5,
           INT,
           "Ladder Bottom candlestick pattern: a four-candle bearish-to-"
           "bullish reversal pattern.",
           "Pattern Recognition")
TALIB_FUNC(cdllongleggeddoji, TA_CDLLONGLEGGEDDOJI,
           TA_CDLLONGLEGGEDDOJI_Lookback, P5, INT,
           "Long Legged Doji candlestick pattern: a doji with very long "
           "upper and lower shadows.",
           "Pattern Recognition")
TALIB_FUNC(cdllongline, TA_CDLLONGLINE, TA_CDLLONGLINE_Lookback, P5, INT,
           "Long Line Candle pattern: a candle with an unusually long "
           "body.",
           "Pattern Recognition")
TALIB_FUNC(cdlmarubozu, TA_CDLMARUBOZU, TA_CDLMARUBOZU_Lookback, P5, INT,
           "Marubozu candlestick pattern: a candle with no shadows, "
           "consisting purely of its body.",
           "Pattern Recognition")
TALIB_FUNC(cdlmatchinglow, TA_CDLMATCHINGLOW, TA_CDLMATCHINGLOW_Lookback, P5,
           INT,
           "Matching Low candlestick pattern: two bearish candles that "
           "close at the same level.",
           "Pattern Recognition")
TALIB_FUNC(cdlonneck, TA_CDLONNECK, TA_CDLONNECK_Lookback, P5, INT,
           "On-Neck candlestick pattern: a bullish candle closing at the "
           "prior candle's low, in a downtrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlpiercing, TA_CDLPIERCING, TA_CDLPIERCING_Lookback, P5, INT,
           "Piercing candlestick pattern: a bullish candle closing above "
           "the midpoint of the prior bearish candle's body.",
           "Pattern Recognition")
TALIB_FUNC(cdlrickshawman, TA_CDLRICKSHAWMAN, TA_CDLRICKSHAWMAN_Lookback, P5,
           INT,
           "Rickshaw Man candlestick pattern: a long-legged doji whose "
           "body sits near the center of its range.",
           "Pattern Recognition")
TALIB_FUNC(cdlrisefall3methods, TA_CDLRISEFALL3METHODS,
           TA_CDLRISEFALL3METHODS_Lookback, P5, INT,
           "Rising/Falling Three Methods candlestick pattern: a "
           "continuation pattern of a strong candle, three small "
           "counter-trend candles, then a resuming strong candle.",
           "Pattern Recognition")
TALIB_FUNC(cdlseparatinglines, TA_CDLSEPARATINGLINES,
           TA_CDLSEPARATINGLINES_Lookback, P5, INT,
           "Separating Lines candlestick pattern: two opposite-colored "
           "candles that share the same opening price.",
           "Pattern Recognition")
TALIB_FUNC(cdlshootingstar, TA_CDLSHOOTINGSTAR, TA_CDLSHOOTINGSTAR_Lookback, P5,
           INT,
           "Shooting Star candlestick pattern: an inverted-hammer-shaped "
           "candle occurring in an uptrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlshortline, TA_CDLSHORTLINE, TA_CDLSHORTLINE_Lookback, P5, INT,
           "Short Line Candle pattern: a candle with an unusually short "
           "body.",
           "Pattern Recognition")
TALIB_FUNC(cdlspinningtop, TA_CDLSPINNINGTOP, TA_CDLSPINNINGTOP_Lookback, P5,
           INT,
           "Spinning Top candlestick pattern: a small body with upper and "
           "lower shadows of similar length.",
           "Pattern Recognition")
TALIB_FUNC(cdlstalledpattern, TA_CDLSTALLEDPATTERN,
           TA_CDLSTALLEDPATTERN_Lookback, P5, INT,
           "Stalled Pattern: three bullish candles where the third has a "
           "notably small body, signaling fading momentum.",
           "Pattern Recognition")
TALIB_FUNC(cdlsticksandwich, TA_CDLSTICKSANDWICH, TA_CDLSTICKSANDWICH_Lookback,
           P5, INT,
           "Stick Sandwich candlestick pattern: two same-close bearish "
           "candles sandwiching an opposite candle.",
           "Pattern Recognition")
TALIB_FUNC(cdltakuri, TA_CDLTAKURI, TA_CDLTAKURI_Lookback, P5, INT,
           "Takuri candlestick pattern: a doji with an unusually long "
           "lower shadow.",
           "Pattern Recognition")
TALIB_FUNC(cdltasukigap, TA_CDLTASUKIGAP, TA_CDLTASUKIGAP_Lookback, P5, INT,
           "Tasuki Gap candlestick pattern: a gap followed by a candle "
           "that partially fills it.",
           "Pattern Recognition")
TALIB_FUNC(cdlthrusting, TA_CDLTHRUSTING, TA_CDLTHRUSTING_Lookback, P5, INT,
           "Thrusting candlestick pattern: a bullish candle closing below "
           "the midpoint of the prior bearish candle's body.",
           "Pattern Recognition")
TALIB_FUNC(cdltristar, TA_CDLTRISTAR, TA_CDLTRISTAR_Lookback, P5, INT,
           "Tristar candlestick pattern: three consecutive dojis with the "
           "middle one gapping from the others.",
           "Pattern Recognition")
TALIB_FUNC(cdlunique3river, TA_CDLUNIQUE3RIVER, TA_CDLUNIQUE3RIVER_Lookback, P5,
           INT,
           "Unique 3 River candlestick pattern: a three-candle bullish "
           "reversal pattern.",
           "Pattern Recognition")
TALIB_FUNC(cdlupsidegap2crows, TA_CDLUPSIDEGAP2CROWS,
           TA_CDLUPSIDEGAP2CROWS_Lookback, P5, INT,
           "Upside Gap Two Crows candlestick pattern: two bearish candles "
           "gapping above an uptrend.",
           "Pattern Recognition")
TALIB_FUNC(cdlxsidegap3methods, TA_CDLXSIDEGAP3METHODS,
           TA_CDLXSIDEGAP3METHODS_Lookback, P5, INT,
           "Upside/Downside Gap Three Methods candlestick pattern: a gap "
           "followed by three candles that close the gap.",
           "Pattern Recognition")

// ============================================================
// P6: (inHigh[], inLow[]) -> outReal[]  (HL, no period)
// ============================================================
TALIB_FUNC(medprice, TA_MEDPRICE, TA_MEDPRICE_Lookback, P6, DOUBLE,
           "Median Price: the midpoint between the high and low for the "
           "period.",
           "Price Transform")

// ============================================================
// P7: (inHigh[], inLow[], inClose[]) -> outReal[]  (HLC, no period)
// ============================================================
TALIB_FUNC(trange, TA_TRANGE, TA_TRANGE_Lookback, P7, DOUBLE,
           "True Range: the greatest of the current high-low range, the "
           "gap up from the prior close, and the gap down from the prior "
           "close.",
           "Volatility")
TALIB_FUNC(typprice, TA_TYPPRICE, TA_TYPPRICE_Lookback, P7, DOUBLE,
           "Typical Price: the average of the high, low, and close for the "
           "period.",
           "Price Transform")
TALIB_FUNC(wclprice, TA_WCLPRICE, TA_WCLPRICE_Lookback, P7, DOUBLE,
           "Weighted Close Price: the close weighted twice as heavily as "
           "the high and low.",
           "Price Transform")

// ============================================================
// P8: (inHigh[], inLow[], timePeriod) -> outReal[]  (HL + period)
// ============================================================
TALIB_FUNC(midprice, TA_MIDPRICE, TA_MIDPRICE_Lookback, P8, DOUBLE,
           "Midpoint Price over period: the average of the highest high "
           "and lowest low over the period.",
           "Trend")
TALIB_FUNC(plus_dm, TA_PLUS_DM, TA_PLUS_DM_Lookback, P8, DOUBLE,
           "Plus Directional Movement: the raw upward price movement used "
           "to compute the plus directional indicator.",
           "Trend")
TALIB_FUNC(minus_dm, TA_MINUS_DM, TA_MINUS_DM_Lookback, P8, DOUBLE,
           "Minus Directional Movement: the raw downward price movement "
           "used to compute the minus directional indicator.",
           "Trend")

// ============================================================
// NOTE: The following functions have non-standard signatures and are
// excluded from the X-macro table. They can be added as special cases:
//   - TA_MA       (inReal[], period, maType)
//   - TA_T3       (inReal[], period, vFactor)
//   - TA_PPO      (inReal[], fastPeriod, slowPeriod, maType)
//   - TA_SAR      (inHigh[], inLow[], accel, max)
//   - TA_ULTOSC   (HLC, period1, period2, period3)
//   - TA_OBV      (inReal[], inVolume[])
//   - TA_ADOSC    (HLCV, fastPeriod, slowPeriod)
//   - TA_MFI      (HLCV, timePeriod)
//   - TA_SAREXT   (HL, many params)
//   - TA_ADD/SUB/MULT/DIV (inReal0[], inReal1[])
//   - CDL functions with penetration param:
//     TA_CDLABANDONEDBABY, TA_CDLDARKCLOUDCOVER, TA_CDLEVENINGDOJISTAR,
//     TA_CDLEVENINGSTAR, TA_CDLMATHOLD, TA_CDLMORNINGDOJISTAR,
//     TA_CDLMORNINGSTAR
//   - TA_STDDEV  (inReal[], period, nbDev)
//   - TA_VAR     (inReal[], period, nbDev)
// ============================================================
