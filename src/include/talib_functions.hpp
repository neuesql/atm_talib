// X-macro table for TA-Lib single-output functions
// This file has NO include guards — it is included multiple times with different macro definitions.
//
// Format: TALIB_FUNC(sql_name, ta_func, ta_lookback, pattern, return_type)
//   sql_name    — lowercase name used in SQL as t_<sql_name> (scalar form)
//   ta_func     — TA-Lib C function name (e.g. TA_SMA)
//   ta_lookback — TA-Lib lookback function name (e.g. TA_SMA_Lookback)
//   pattern     — P1..P8 (input signature pattern)
//   return_type — DOUBLE or INT

// ============================================================
// P1: (inReal[], timePeriod) -> outReal[]
// ============================================================

// Overlap Studies
TALIB_FUNC(sma,      TA_SMA,      TA_SMA_Lookback,      P1, DOUBLE)
TALIB_FUNC(ema,      TA_EMA,      TA_EMA_Lookback,      P1, DOUBLE)
TALIB_FUNC(wma,      TA_WMA,      TA_WMA_Lookback,      P1, DOUBLE)
TALIB_FUNC(dema,     TA_DEMA,     TA_DEMA_Lookback,     P1, DOUBLE)
TALIB_FUNC(tema,     TA_TEMA,     TA_TEMA_Lookback,     P1, DOUBLE)
TALIB_FUNC(trima,    TA_TRIMA,    TA_TRIMA_Lookback,    P1, DOUBLE)
TALIB_FUNC(kama,     TA_KAMA,     TA_KAMA_Lookback,     P1, DOUBLE)
TALIB_FUNC(midpoint, TA_MIDPOINT, TA_MIDPOINT_Lookback, P1, DOUBLE)

// Momentum (single array + period)
TALIB_FUNC(rsi,      TA_RSI,      TA_RSI_Lookback,      P1, DOUBLE)
TALIB_FUNC(cmo,      TA_CMO,      TA_CMO_Lookback,      P1, DOUBLE)
TALIB_FUNC(mom,      TA_MOM,      TA_MOM_Lookback,      P1, DOUBLE)
TALIB_FUNC(roc,      TA_ROC,      TA_ROC_Lookback,      P1, DOUBLE)
TALIB_FUNC(rocp,     TA_ROCP,     TA_ROCP_Lookback,     P1, DOUBLE)
TALIB_FUNC(rocr,     TA_ROCR,     TA_ROCR_Lookback,     P1, DOUBLE)
TALIB_FUNC(rocr100,  TA_ROCR100,  TA_ROCR100_Lookback,  P1, DOUBLE)
TALIB_FUNC(trix,     TA_TRIX,     TA_TRIX_Lookback,     P1, DOUBLE)

// Statistics
TALIB_FUNC(linearreg,           TA_LINEARREG,           TA_LINEARREG_Lookback,           P1, DOUBLE)
TALIB_FUNC(linearreg_angle,     TA_LINEARREG_ANGLE,     TA_LINEARREG_ANGLE_Lookback,     P1, DOUBLE)
TALIB_FUNC(linearreg_intercept, TA_LINEARREG_INTERCEPT, TA_LINEARREG_INTERCEPT_Lookback, P1, DOUBLE)
TALIB_FUNC(linearreg_slope,     TA_LINEARREG_SLOPE,     TA_LINEARREG_SLOPE_Lookback,     P1, DOUBLE)
TALIB_FUNC(tsf,                 TA_TSF,                 TA_TSF_Lookback,                 P1, DOUBLE)
TALIB_FUNC(sum,                 TA_SUM,                 TA_SUM_Lookback,                 P1, DOUBLE)
TALIB_FUNC(max,                 TA_MAX,                 TA_MAX_Lookback,                 P1, DOUBLE)
TALIB_FUNC(min,                 TA_MIN,                 TA_MIN_Lookback,                 P1, DOUBLE)

// P1 returning INT
TALIB_FUNC(maxindex,  TA_MAXINDEX,  TA_MAXINDEX_Lookback,  P1, INT)
TALIB_FUNC(minindex,  TA_MININDEX,  TA_MININDEX_Lookback,  P1, INT)

// ============================================================
// P2: (inReal[]) -> outReal[] or outInt[]
// ============================================================

// Hilbert Transform
TALIB_FUNC(ht_dcperiod,  TA_HT_DCPERIOD,  TA_HT_DCPERIOD_Lookback,  P2, DOUBLE)
TALIB_FUNC(ht_dcphase,   TA_HT_DCPHASE,   TA_HT_DCPHASE_Lookback,   P2, DOUBLE)
TALIB_FUNC(ht_trendline, TA_HT_TRENDLINE, TA_HT_TRENDLINE_Lookback, P2, DOUBLE)
TALIB_FUNC(ht_trendmode, TA_HT_TRENDMODE, TA_HT_TRENDMODE_Lookback, P2, INT)

// Math Transform
TALIB_FUNC(acos,  TA_ACOS,  TA_ACOS_Lookback,  P2, DOUBLE)
TALIB_FUNC(asin,  TA_ASIN,  TA_ASIN_Lookback,  P2, DOUBLE)
TALIB_FUNC(atan,  TA_ATAN,  TA_ATAN_Lookback,  P2, DOUBLE)
TALIB_FUNC(ceil,  TA_CEIL,  TA_CEIL_Lookback,  P2, DOUBLE)
TALIB_FUNC(cos,   TA_COS,   TA_COS_Lookback,   P2, DOUBLE)
TALIB_FUNC(cosh,  TA_COSH,  TA_COSH_Lookback,  P2, DOUBLE)
TALIB_FUNC(exp,   TA_EXP,   TA_EXP_Lookback,   P2, DOUBLE)
TALIB_FUNC(floor, TA_FLOOR, TA_FLOOR_Lookback, P2, DOUBLE)
TALIB_FUNC(ln,    TA_LN,    TA_LN_Lookback,    P2, DOUBLE)
TALIB_FUNC(log10, TA_LOG10, TA_LOG10_Lookback, P2, DOUBLE)
TALIB_FUNC(sin,   TA_SIN,   TA_SIN_Lookback,   P2, DOUBLE)
TALIB_FUNC(sinh,  TA_SINH,  TA_SINH_Lookback,  P2, DOUBLE)
TALIB_FUNC(sqrt,  TA_SQRT,  TA_SQRT_Lookback,  P2, DOUBLE)
TALIB_FUNC(tan,   TA_TAN,   TA_TAN_Lookback,   P2, DOUBLE)
TALIB_FUNC(tanh,  TA_TANH,  TA_TANH_Lookback,  P2, DOUBLE)

// ============================================================
// P3: (inHigh[], inLow[], inClose[], timePeriod) -> outReal[]
// ============================================================

// Volatility
TALIB_FUNC(atr,  TA_ATR,  TA_ATR_Lookback,  P3, DOUBLE)
TALIB_FUNC(natr, TA_NATR, TA_NATR_Lookback, P3, DOUBLE)

// Momentum (HLC + period)
TALIB_FUNC(willr,    TA_WILLR,    TA_WILLR_Lookback,    P3, DOUBLE)
TALIB_FUNC(cci,      TA_CCI,      TA_CCI_Lookback,      P3, DOUBLE)
TALIB_FUNC(adx,      TA_ADX,      TA_ADX_Lookback,      P3, DOUBLE)
TALIB_FUNC(adxr,     TA_ADXR,     TA_ADXR_Lookback,     P3, DOUBLE)
TALIB_FUNC(dx,       TA_DX,       TA_DX_Lookback,       P3, DOUBLE)
TALIB_FUNC(plus_di,  TA_PLUS_DI,  TA_PLUS_DI_Lookback,  P3, DOUBLE)
TALIB_FUNC(minus_di, TA_MINUS_DI, TA_MINUS_DI_Lookback, P3, DOUBLE)

// ============================================================
// P4: (inHigh[], inLow[], inClose[], inVolume[]) -> outReal[]
// ============================================================
TALIB_FUNC(ad, TA_AD, TA_AD_Lookback, P4, DOUBLE)

// ============================================================
// P5: (inOpen[], inHigh[], inLow[], inClose[]) -> outReal[] or outInt[]
// ============================================================

// Price Transform (OHLC -> DOUBLE)
TALIB_FUNC(avgprice, TA_AVGPRICE, TA_AVGPRICE_Lookback, P5, DOUBLE)
TALIB_FUNC(bop,      TA_BOP,      TA_BOP_Lookback,      P5, DOUBLE)

// Pattern Recognition (OHLC -> INT) — standard P5 (no extra params)
TALIB_FUNC(cdl2crows,          TA_CDL2CROWS,          TA_CDL2CROWS_Lookback,          P5, INT)
TALIB_FUNC(cdl3blackcrows,     TA_CDL3BLACKCROWS,     TA_CDL3BLACKCROWS_Lookback,     P5, INT)
TALIB_FUNC(cdl3inside,         TA_CDL3INSIDE,         TA_CDL3INSIDE_Lookback,         P5, INT)
TALIB_FUNC(cdl3linestrike,     TA_CDL3LINESTRIKE,     TA_CDL3LINESTRIKE_Lookback,     P5, INT)
TALIB_FUNC(cdl3outside,        TA_CDL3OUTSIDE,        TA_CDL3OUTSIDE_Lookback,        P5, INT)
TALIB_FUNC(cdl3starsinsouth,   TA_CDL3STARSINSOUTH,   TA_CDL3STARSINSOUTH_Lookback,   P5, INT)
TALIB_FUNC(cdl3whitesoldiers,  TA_CDL3WHITESOLDIERS,  TA_CDL3WHITESOLDIERS_Lookback,  P5, INT)
TALIB_FUNC(cdladvanceblock,    TA_CDLADVANCEBLOCK,    TA_CDLADVANCEBLOCK_Lookback,    P5, INT)
TALIB_FUNC(cdlbelthold,        TA_CDLBELTHOLD,        TA_CDLBELTHOLD_Lookback,        P5, INT)
TALIB_FUNC(cdlbreakaway,       TA_CDLBREAKAWAY,       TA_CDLBREAKAWAY_Lookback,       P5, INT)
TALIB_FUNC(cdlclosingmarubozu, TA_CDLCLOSINGMARUBOZU, TA_CDLCLOSINGMARUBOZU_Lookback, P5, INT)
TALIB_FUNC(cdlconcealbabyswall,TA_CDLCONCEALBABYSWALL,TA_CDLCONCEALBABYSWALL_Lookback, P5, INT)
TALIB_FUNC(cdlcounterattack,   TA_CDLCOUNTERATTACK,   TA_CDLCOUNTERATTACK_Lookback,   P5, INT)
TALIB_FUNC(cdldoji,            TA_CDLDOJI,            TA_CDLDOJI_Lookback,            P5, INT)
TALIB_FUNC(cdldojistar,        TA_CDLDOJISTAR,        TA_CDLDOJISTAR_Lookback,        P5, INT)
TALIB_FUNC(cdldragonflydoji,   TA_CDLDRAGONFLYDOJI,   TA_CDLDRAGONFLYDOJI_Lookback,   P5, INT)
TALIB_FUNC(cdlengulfing,       TA_CDLENGULFING,       TA_CDLENGULFING_Lookback,       P5, INT)
TALIB_FUNC(cdlgapsidesidewhite,TA_CDLGAPSIDESIDEWHITE,TA_CDLGAPSIDESIDEWHITE_Lookback, P5, INT)
TALIB_FUNC(cdlgravestonedoji,  TA_CDLGRAVESTONEDOJI,  TA_CDLGRAVESTONEDOJI_Lookback,  P5, INT)
TALIB_FUNC(cdlhammer,          TA_CDLHAMMER,          TA_CDLHAMMER_Lookback,          P5, INT)
TALIB_FUNC(cdlhangingman,      TA_CDLHANGINGMAN,      TA_CDLHANGINGMAN_Lookback,      P5, INT)
TALIB_FUNC(cdlharami,          TA_CDLHARAMI,          TA_CDLHARAMI_Lookback,          P5, INT)
TALIB_FUNC(cdlharamicross,     TA_CDLHARAMICROSS,     TA_CDLHARAMICROSS_Lookback,     P5, INT)
TALIB_FUNC(cdlhighwave,        TA_CDLHIGHWAVE,        TA_CDLHIGHWAVE_Lookback,        P5, INT)
TALIB_FUNC(cdlhikkake,         TA_CDLHIKKAKE,         TA_CDLHIKKAKE_Lookback,         P5, INT)
TALIB_FUNC(cdlhikkakemod,      TA_CDLHIKKAKEMOD,      TA_CDLHIKKAKEMOD_Lookback,      P5, INT)
TALIB_FUNC(cdlhomingpigeon,    TA_CDLHOMINGPIGEON,    TA_CDLHOMINGPIGEON_Lookback,    P5, INT)
TALIB_FUNC(cdlidentical3crows, TA_CDLIDENTICAL3CROWS, TA_CDLIDENTICAL3CROWS_Lookback, P5, INT)
TALIB_FUNC(cdlinneck,          TA_CDLINNECK,          TA_CDLINNECK_Lookback,          P5, INT)
TALIB_FUNC(cdlinvertedhammer,  TA_CDLINVERTEDHAMMER,  TA_CDLINVERTEDHAMMER_Lookback,  P5, INT)
TALIB_FUNC(cdlkicking,         TA_CDLKICKING,         TA_CDLKICKING_Lookback,         P5, INT)
TALIB_FUNC(cdlkickingbylength, TA_CDLKICKINGBYLENGTH, TA_CDLKICKINGBYLENGTH_Lookback, P5, INT)
TALIB_FUNC(cdlladderbottom,    TA_CDLLADDERBOTTOM,    TA_CDLLADDERBOTTOM_Lookback,    P5, INT)
TALIB_FUNC(cdllongleggeddoji,  TA_CDLLONGLEGGEDDOJI,  TA_CDLLONGLEGGEDDOJI_Lookback,  P5, INT)
TALIB_FUNC(cdllongline,        TA_CDLLONGLINE,        TA_CDLLONGLINE_Lookback,        P5, INT)
TALIB_FUNC(cdlmarubozu,        TA_CDLMARUBOZU,        TA_CDLMARUBOZU_Lookback,        P5, INT)
TALIB_FUNC(cdlmatchinglow,     TA_CDLMATCHINGLOW,     TA_CDLMATCHINGLOW_Lookback,     P5, INT)
TALIB_FUNC(cdlonneck,          TA_CDLONNECK,          TA_CDLONNECK_Lookback,          P5, INT)
TALIB_FUNC(cdlpiercing,        TA_CDLPIERCING,        TA_CDLPIERCING_Lookback,        P5, INT)
TALIB_FUNC(cdlrickshawman,     TA_CDLRICKSHAWMAN,     TA_CDLRICKSHAWMAN_Lookback,     P5, INT)
TALIB_FUNC(cdlrisefall3methods,TA_CDLRISEFALL3METHODS,TA_CDLRISEFALL3METHODS_Lookback, P5, INT)
TALIB_FUNC(cdlseparatinglines, TA_CDLSEPARATINGLINES, TA_CDLSEPARATINGLINES_Lookback, P5, INT)
TALIB_FUNC(cdlshootingstar,    TA_CDLSHOOTINGSTAR,    TA_CDLSHOOTINGSTAR_Lookback,    P5, INT)
TALIB_FUNC(cdlshortline,       TA_CDLSHORTLINE,       TA_CDLSHORTLINE_Lookback,       P5, INT)
TALIB_FUNC(cdlspinningtop,     TA_CDLSPINNINGTOP,     TA_CDLSPINNINGTOP_Lookback,     P5, INT)
TALIB_FUNC(cdlstalledpattern,  TA_CDLSTALLEDPATTERN,  TA_CDLSTALLEDPATTERN_Lookback,  P5, INT)
TALIB_FUNC(cdlsticksandwich,   TA_CDLSTICKSANDWICH,   TA_CDLSTICKSANDWICH_Lookback,   P5, INT)
TALIB_FUNC(cdltakuri,          TA_CDLTAKURI,          TA_CDLTAKURI_Lookback,          P5, INT)
TALIB_FUNC(cdltasukigap,       TA_CDLTASUKIGAP,       TA_CDLTASUKIGAP_Lookback,       P5, INT)
TALIB_FUNC(cdlthrusting,       TA_CDLTHRUSTING,       TA_CDLTHRUSTING_Lookback,       P5, INT)
TALIB_FUNC(cdltristar,         TA_CDLTRISTAR,         TA_CDLTRISTAR_Lookback,         P5, INT)
TALIB_FUNC(cdlunique3river,    TA_CDLUNIQUE3RIVER,    TA_CDLUNIQUE3RIVER_Lookback,    P5, INT)
TALIB_FUNC(cdlupsidegap2crows, TA_CDLUPSIDEGAP2CROWS, TA_CDLUPSIDEGAP2CROWS_Lookback, P5, INT)
TALIB_FUNC(cdlxsidegap3methods,TA_CDLXSIDEGAP3METHODS,TA_CDLXSIDEGAP3METHODS_Lookback,P5, INT)

// ============================================================
// P6: (inHigh[], inLow[]) -> outReal[]  (HL, no period)
// ============================================================
TALIB_FUNC(medprice, TA_MEDPRICE, TA_MEDPRICE_Lookback, P6, DOUBLE)

// ============================================================
// P7: (inHigh[], inLow[], inClose[]) -> outReal[]  (HLC, no period)
// ============================================================
TALIB_FUNC(trange,   TA_TRANGE,   TA_TRANGE_Lookback,   P7, DOUBLE)
TALIB_FUNC(typprice, TA_TYPPRICE, TA_TYPPRICE_Lookback, P7, DOUBLE)
TALIB_FUNC(wclprice, TA_WCLPRICE, TA_WCLPRICE_Lookback, P7, DOUBLE)

// ============================================================
// P8: (inHigh[], inLow[], timePeriod) -> outReal[]  (HL + period)
// ============================================================
TALIB_FUNC(midprice, TA_MIDPRICE, TA_MIDPRICE_Lookback, P8, DOUBLE)
TALIB_FUNC(plus_dm,  TA_PLUS_DM,  TA_PLUS_DM_Lookback,  P8, DOUBLE)
TALIB_FUNC(minus_dm, TA_MINUS_DM, TA_MINUS_DM_Lookback, P8, DOUBLE)

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
//     TA_CDLEVENINGSTAR, TA_CDLMATHOLD, TA_CDLMORNINGDOJISTAR, TA_CDLMORNINGSTAR
//   - TA_STDDEV  (inReal[], period, nbDev)
//   - TA_VAR     (inReal[], period, nbDev)
// ============================================================
