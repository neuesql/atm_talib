# DuckDB TA-Lib Extension — SQL Cookbook

This cookbook provides working SQL examples for the DuckDB TA-Lib extension. Each recipe shows both the **scalar form** (`ta_` prefix, takes LIST inputs) and the **window form** (`taw_` prefix, uses `OVER()`) where applicable.

## Function Forms at a Glance

| Form | Prefix | Input | Usage |
|------|--------|-------|-------|
| Scalar | `ta_` | `LIST<DOUBLE>` | Batch compute over an array literal or subquery |
| Window | `taw_` | Scalar column | Per-row streaming with `OVER (ORDER BY ... ROWS BETWEEN ...)` |

---

## 1. Table Setup

```sql
CREATE TABLE ohlc (
    ts      TIMESTAMP,
    open    DOUBLE,
    high    DOUBLE,
    low     DOUBLE,
    close   DOUBLE,
    volume  DOUBLE
);

INSERT INTO ohlc VALUES
    ('2024-01-01', 10.0, 11.2, 9.8,  10.5, 120000),
    ('2024-01-02', 10.5, 11.8, 10.2, 11.3, 135000),
    ('2024-01-03', 11.3, 12.1, 11.0, 11.8, 98000),
    ('2024-01-04', 11.8, 12.5, 11.5, 12.2, 145000),
    ('2024-01-05', 12.2, 13.0, 11.9, 12.7, 167000),
    ('2024-01-06', 12.7, 13.4, 12.3, 13.1, 112000),
    ('2024-01-07', 13.1, 13.8, 12.8, 13.5, 130000),
    ('2024-01-08', 13.5, 14.2, 13.1, 13.9, 118000),
    ('2024-01-09', 13.9, 14.6, 13.5, 14.3, 155000),
    ('2024-01-10', 14.3, 15.0, 13.9, 14.8, 142000),
    ('2024-01-11', 14.8, 15.3, 14.2, 15.0, 138000),
    ('2024-01-12', 15.0, 15.8, 14.7, 15.4, 160000),
    ('2024-01-13', 15.4, 16.1, 15.0, 15.8, 175000),
    ('2024-01-14', 15.8, 16.5, 15.3, 16.2, 190000),
    ('2024-01-15', 16.2, 16.9, 15.8, 16.6, 165000),
    ('2024-01-16', 16.6, 17.2, 16.1, 16.9, 148000),
    ('2024-01-17', 16.9, 17.6, 16.5, 17.3, 155000),
    ('2024-01-18', 17.3, 18.0, 16.8, 17.7, 172000),
    ('2024-01-19', 17.7, 18.4, 17.2, 18.1, 168000),
    ('2024-01-20', 18.1, 18.8, 17.6, 18.5, 182000);
```

---

## 2. Trend Following

### SMA — Simple Moving Average

Signature: `ta_sma(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar: compute 5-period SMA over the full close series
SELECT unnest(
    ta_sma(
        (SELECT list(close ORDER BY ts) FROM ohlc),
        5
    )
) AS sma_5;

-- Window: rolling 5-period SMA per row
SELECT
    ts,
    close,
    taw_sma(close, 5) OVER (ORDER BY ts ROWS BETWEEN 4 PRECEDING AND CURRENT ROW) AS sma_5
FROM ohlc;
```

### EMA — Exponential Moving Average

Signature: `ta_ema(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_ema((SELECT list(close ORDER BY ts) FROM ohlc), 10)
) AS ema_10;

-- Window
SELECT
    ts,
    close,
    taw_ema(close, 10) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) AS ema_10
FROM ohlc;
```

### DEMA — Double Exponential Moving Average

Signature: `ta_dema(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_dema((SELECT list(close ORDER BY ts) FROM ohlc), 5)
) AS dema_5;

-- Window
SELECT
    ts,
    close,
    taw_dema(close, 5) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) AS dema_5
FROM ohlc;
```

### TEMA — Triple Exponential Moving Average

Signature: `ta_tema(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_tema((SELECT list(close ORDER BY ts) FROM ohlc), 5)
) AS tema_5;

-- Window
SELECT
    ts,
    close,
    taw_tema(close, 5) OVER (ORDER BY ts ROWS BETWEEN 14 PRECEDING AND CURRENT ROW) AS tema_5
FROM ohlc;
```

### KAMA — Kaufman Adaptive Moving Average

Signature: `ta_kama(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_kama((SELECT list(close ORDER BY ts) FROM ohlc), 10)
) AS kama_10;

-- Window
SELECT
    ts,
    close,
    taw_kama(close, 10) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) AS kama_10
FROM ohlc;
```

### Crossover Signal (SMA 5 vs SMA 10)

```sql
-- Identify golden cross / death cross events
WITH signals AS (
    SELECT
        ts,
        close,
        taw_sma(close, 5)  OVER (ORDER BY ts ROWS BETWEEN  4 PRECEDING AND CURRENT ROW) AS sma_5,
        taw_sma(close, 10) OVER (ORDER BY ts ROWS BETWEEN  9 PRECEDING AND CURRENT ROW) AS sma_10
    FROM ohlc
)
SELECT
    ts,
    close,
    sma_5,
    sma_10,
    CASE
        WHEN sma_5 > sma_10
         AND lag(sma_5) OVER (ORDER BY ts) <= lag(sma_10) OVER (ORDER BY ts) THEN 'GOLDEN CROSS'
        WHEN sma_5 < sma_10
         AND lag(sma_5) OVER (ORDER BY ts) >= lag(sma_10) OVER (ORDER BY ts) THEN 'DEATH CROSS'
        ELSE NULL
    END AS crossover
FROM signals;
```

---

## 3. Mean Reversion

### RSI — Relative Strength Index

Signature: `ta_rsi(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_rsi((SELECT list(close ORDER BY ts) FROM ohlc), 14)
) AS rsi_14;

-- Window
SELECT
    ts,
    close,
    taw_rsi(close, 14) OVER (ORDER BY ts ROWS BETWEEN 14 PRECEDING AND CURRENT ROW) AS rsi_14
FROM ohlc;
```

### RSI-based overbought/oversold signal

```sql
WITH rsi_vals AS (
    SELECT
        ts,
        close,
        taw_rsi(close, 14) OVER (ORDER BY ts ROWS BETWEEN 14 PRECEDING AND CURRENT ROW) AS rsi_14
    FROM ohlc
)
SELECT
    ts,
    close,
    round(rsi_14, 2) AS rsi_14,
    CASE
        WHEN rsi_14 >= 70 THEN 'OVERBOUGHT'
        WHEN rsi_14 <= 30 THEN 'OVERSOLD'
        ELSE 'NEUTRAL'
    END AS signal
FROM rsi_vals
WHERE rsi_14 IS NOT NULL;
```

### Bollinger Bands

Signature: `ta_bbands(prices LIST<DOUBLE>, time_period INT, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INT) -> LIST<STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)>`

The `ma_type` values follow the TA-Lib `TA_MAType` enum: `0`=SMA, `1`=EMA, `2`=WMA, etc.

```sql
-- Scalar: compute Bollinger Bands (20-period, 2 std dev, SMA)
WITH bb_raw AS (
    SELECT unnest(
        ta_bbands(
            (SELECT list(close ORDER BY ts) FROM ohlc),
            20,    -- time_period
            2.0,   -- nb_dev_up
            2.0,   -- nb_dev_dn
            0      -- ma_type = SMA
        )
    ) AS bb
)
SELECT
    bb.upper  AS upper_band,
    bb.middle AS middle_band,
    bb.lower  AS lower_band
FROM bb_raw;

-- Combine with price data using row_number for alignment
WITH prices AS (
    SELECT row_number() OVER (ORDER BY ts) AS rn, ts, close FROM ohlc
),
bb_raw AS (
    SELECT
        generate_subscripts(arr, 1) AS rn,
        arr[generate_subscripts(arr, 1)].upper  AS upper,
        arr[generate_subscripts(arr, 1)].middle AS middle,
        arr[generate_subscripts(arr, 1)].lower  AS lower
    FROM (
        SELECT ta_bbands(
            (SELECT list(close ORDER BY ts) FROM ohlc),
            20, 2.0, 2.0, 0
        ) AS arr
    )
)
SELECT p.ts, p.close, b.upper, b.middle, b.lower
FROM prices p
JOIN bb_raw b USING (rn);
```

---

## 4. Momentum

### MACD — Moving Average Convergence/Divergence

Signature: `ta_macd(prices LIST<DOUBLE>, fast_period INT, slow_period INT, signal_period INT) -> LIST<STRUCT(macd DOUBLE, signal DOUBLE, hist DOUBLE)>`

```sql
-- Scalar: standard MACD (12, 26, 9)
WITH macd_raw AS (
    SELECT unnest(
        ta_macd(
            (SELECT list(close ORDER BY ts) FROM ohlc),
            12,  -- fast_period
            26,  -- slow_period
            9    -- signal_period
        )
    ) AS m
)
SELECT
    m.macd   AS macd_line,
    m.signal AS signal_line,
    m.hist   AS histogram
FROM macd_raw
WHERE m.macd IS NOT NULL;
```

### ROC — Rate of Change

Signature: `ta_roc(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_roc((SELECT list(close ORDER BY ts) FROM ohlc), 10)
) AS roc_10;

-- Window
SELECT
    ts,
    close,
    taw_roc(close, 10) OVER (ORDER BY ts ROWS BETWEEN 10 PRECEDING AND CURRENT ROW) AS roc_10
FROM ohlc;
```

### Williams %R

Signature: `ta_willr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_willr(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc),
        14
    )
) AS willr_14;

-- Window
SELECT
    ts,
    close,
    taw_willr(high, low, close, 14) OVER (ORDER BY ts ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS willr_14
FROM ohlc;
```

### CCI — Commodity Channel Index

Signature: `ta_cci(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_cci(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc),
        20
    )
) AS cci_20;

-- Window
SELECT
    ts,
    close,
    taw_cci(high, low, close, 20) OVER (ORDER BY ts ROWS BETWEEN 19 PRECEDING AND CURRENT ROW) AS cci_20
FROM ohlc;
```

### ADX — Average Directional Index

Signature: `ta_adx(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_adx(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc),
        14
    )
) AS adx_14;

-- Window
SELECT
    ts,
    taw_adx(high, low, close, 14) OVER (ORDER BY ts ROWS BETWEEN 27 PRECEDING AND CURRENT ROW) AS adx_14
FROM ohlc;
```

---

## 5. Volume Analysis

### AD — Chaikin A/D Line

Signature: `ta_ad(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, volume LIST<DOUBLE>) -> LIST<DOUBLE>`

Note: AD has no lookback period — it outputs a value for every input bar.

```sql
-- Scalar
SELECT unnest(
    ta_ad(
        (SELECT list(high   ORDER BY ts) FROM ohlc),
        (SELECT list(low    ORDER BY ts) FROM ohlc),
        (SELECT list(close  ORDER BY ts) FROM ohlc),
        (SELECT list(volume ORDER BY ts) FROM ohlc)
    )
) AS ad_line;

-- Window
SELECT
    ts,
    close,
    volume,
    taw_ad(high, low, close, volume) OVER (ORDER BY ts) AS ad_line
FROM ohlc;
```

---

## 6. Volatility

### ATR — Average True Range

Signature: `ta_atr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_atr(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc),
        14
    )
) AS atr_14;

-- Window
SELECT
    ts,
    taw_atr(high, low, close, 14) OVER (ORDER BY ts ROWS BETWEEN 14 PRECEDING AND CURRENT ROW) AS atr_14
FROM ohlc;
```

### NATR — Normalized Average True Range

Signature: `ta_natr(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_natr(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc),
        14
    )
) AS natr_14;

-- Window
SELECT
    ts,
    close,
    taw_natr(high, low, close, 14) OVER (ORDER BY ts ROWS BETWEEN 14 PRECEDING AND CURRENT ROW) AS natr_14
FROM ohlc;
```

### TRANGE — True Range

Signature: `ta_trange(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`

```sql
-- Scalar (no period required)
SELECT unnest(
    ta_trange(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS true_range;

-- Window
SELECT
    ts,
    high, low, close,
    taw_trange(high, low, close) OVER (ORDER BY ts) AS true_range
FROM ohlc;
```

---

## 7. Pattern Detection

Pattern recognition functions take OHLC inputs and return an integer: `100` = bullish, `-100` = bearish, `0` = no pattern.

### CDLDOJI

Signature: `ta_cdldoji(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<INT>`

```sql
-- Scalar
SELECT unnest(
    ta_cdldoji(
        (SELECT list(open  ORDER BY ts) FROM ohlc),
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS doji_signal;

-- Window: flag doji candles
SELECT
    ts, open, high, low, close,
    taw_cdldoji(open, high, low, close) OVER (ORDER BY ts) AS doji
FROM ohlc
WHERE taw_cdldoji(open, high, low, close) OVER (ORDER BY ts) != 0;
```

### CDLHAMMER

Signature: `ta_cdlhammer(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<INT>`

```sql
-- Scalar
SELECT unnest(
    ta_cdlhammer(
        (SELECT list(open  ORDER BY ts) FROM ohlc),
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS hammer_signal;

-- Window
SELECT
    ts, open, high, low, close,
    taw_cdlhammer(open, high, low, close) OVER (ORDER BY ts) AS hammer
FROM ohlc;
```

### CDLENGULFING

Signature: `ta_cdlengulfing(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<INT>`

```sql
-- Scalar
SELECT unnest(
    ta_cdlengulfing(
        (SELECT list(open  ORDER BY ts) FROM ohlc),
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS engulfing_signal;

-- Window: label direction
SELECT
    ts, open, high, low, close,
    CASE taw_cdlengulfing(open, high, low, close) OVER (ORDER BY ts)
        WHEN  100 THEN 'BULLISH ENGULFING'
        WHEN -100 THEN 'BEARISH ENGULFING'
        ELSE NULL
    END AS pattern
FROM ohlc;
```

### Scanning multiple patterns at once

```sql
-- Find any bullish candle pattern on each bar
SELECT
    ts, open, high, low, close,
    taw_cdlhammer    (open, high, low, close) OVER (ORDER BY ts) AS hammer,
    taw_cdlengulfing (open, high, low, close) OVER (ORDER BY ts) AS engulfing,
    taw_cdldoji      (open, high, low, close) OVER (ORDER BY ts) AS doji,
    taw_cdlharami    (open, high, low, close) OVER (ORDER BY ts) AS harami
FROM ohlc;
```

---

## 8. Price Analysis

### AVGPRICE — Average of OHLC

Signature: `ta_avgprice(open LIST<DOUBLE>, high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_avgprice(
        (SELECT list(open  ORDER BY ts) FROM ohlc),
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS avg_price;

-- Window
SELECT ts, taw_avgprice(open, high, low, close) OVER (ORDER BY ts) AS avg_price
FROM ohlc;
```

### TYPPRICE — Typical Price (H+L+C)/3

Signature: `ta_typprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_typprice(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS typical_price;

-- Window
SELECT ts, taw_typprice(high, low, close) OVER (ORDER BY ts) AS typical_price
FROM ohlc;
```

### MEDPRICE — Median Price (H+L)/2

Signature: `ta_medprice(high LIST<DOUBLE>, low LIST<DOUBLE>) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_medprice(
        (SELECT list(high ORDER BY ts) FROM ohlc),
        (SELECT list(low  ORDER BY ts) FROM ohlc)
    )
) AS median_price;

-- Window
SELECT ts, taw_medprice(high, low) OVER (ORDER BY ts) AS median_price
FROM ohlc;
```

### WCLPRICE — Weighted Close Price (H+L+2*C)/4

Signature: `ta_wclprice(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_wclprice(
        (SELECT list(high  ORDER BY ts) FROM ohlc),
        (SELECT list(low   ORDER BY ts) FROM ohlc),
        (SELECT list(close ORDER BY ts) FROM ohlc)
    )
) AS wcl_price;

-- Window
SELECT ts, taw_wclprice(high, low, close) OVER (ORDER BY ts) AS wcl_price
FROM ohlc;
```

---

## 9. Statistical

### LINEARREG — Linear Regression Value

Signature: `ta_linearreg(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT unnest(
    ta_linearreg((SELECT list(close ORDER BY ts) FROM ohlc), 14)
) AS linreg_14;

-- Window
SELECT
    ts,
    close,
    taw_linearreg(close, 14) OVER (ORDER BY ts ROWS BETWEEN 13 PRECEDING AND CURRENT ROW) AS linreg_14
FROM ohlc;
```

### MAX / MIN — Rolling Maximum / Minimum

Signatures:
- `ta_max(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`
- `ta_min(prices LIST<DOUBLE>, time_period INT) -> LIST<DOUBLE>`

```sql
-- Scalar
SELECT
    unnest(ta_max((SELECT list(close ORDER BY ts) FROM ohlc), 5)) AS rolling_max_5,
    unnest(ta_min((SELECT list(close ORDER BY ts) FROM ohlc), 5)) AS rolling_min_5;

-- Window: rolling 5-day high/low channel
SELECT
    ts,
    close,
    taw_max(high, 5) OVER (ORDER BY ts ROWS BETWEEN 4 PRECEDING AND CURRENT ROW) AS channel_high,
    taw_min(low,  5) OVER (ORDER BY ts ROWS BETWEEN 4 PRECEDING AND CURRENT ROW) AS channel_low
FROM ohlc;
```

### Linear Regression Slope and Angle

```sql
-- Detect trend direction using slope sign
SELECT
    ts,
    close,
    taw_linearreg_slope(close, 10) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) AS slope,
    taw_linearreg_angle(close, 10) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) AS angle_deg,
    CASE
        WHEN taw_linearreg_slope(close, 10) OVER (ORDER BY ts ROWS BETWEEN 9 PRECEDING AND CURRENT ROW) > 0
        THEN 'UPTREND'
        ELSE 'DOWNTREND'
    END AS trend
FROM ohlc;
```

---

## 10. Multi-Output Functions

Multi-output functions return a `LIST<STRUCT(...)>`. Use `unnest()` plus struct field access to unpack the results.

### MACD (revisited with full row alignment)

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(close ORDER BY ts) AS price_arr
    FROM ohlc
),
macd_result AS (
    SELECT
        unnest(ts_arr)                AS ts,
        unnest(ta_macd(price_arr, 12, 26, 9)) AS m
    FROM base
)
SELECT
    ts,
    m.macd   AS macd_line,
    m.signal AS signal_line,
    m.hist   AS histogram
FROM macd_result
WHERE m.macd IS NOT NULL
ORDER BY ts;
```

### BBANDS — Bollinger Bands

Signature: `ta_bbands(prices LIST<DOUBLE>, time_period INT, nb_dev_up DOUBLE, nb_dev_dn DOUBLE, ma_type INT) -> LIST<STRUCT(upper DOUBLE, middle DOUBLE, lower DOUBLE)>`

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(close ORDER BY ts) AS price_arr
    FROM ohlc
),
bb_result AS (
    SELECT
        unnest(ts_arr)                              AS ts,
        unnest(ta_bbands(price_arr, 20, 2.0, 2.0, 0)) AS bb
    FROM base
)
SELECT
    ts,
    bb.upper  AS upper_band,
    bb.middle AS middle_band,
    bb.lower  AS lower_band,
    -- Percent B: where price sits within the bands
    -- (requires joining back to close prices)
FROM bb_result
WHERE bb.upper IS NOT NULL
ORDER BY ts;
```

### STOCH — Stochastic Oscillator

Signature: `ta_stoch(high LIST<DOUBLE>, low LIST<DOUBLE>, close LIST<DOUBLE>, fastk_period INT, slowk_period INT, slowk_matype INT, slowd_period INT, slowd_matype INT) -> LIST<STRUCT(slowk DOUBLE, slowd DOUBLE)>`

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(high  ORDER BY ts) AS high_arr,
        list(low   ORDER BY ts) AS low_arr,
        list(close ORDER BY ts) AS close_arr
    FROM ohlc
),
stoch_result AS (
    SELECT
        unnest(ts_arr)                                              AS ts,
        unnest(ta_stoch(high_arr, low_arr, close_arr, 5, 3, 0, 3, 0)) AS s
    FROM base
)
SELECT
    ts,
    s.slowk AS stoch_k,
    s.slowd AS stoch_d,
    CASE
        WHEN s.slowk > 80 THEN 'OVERBOUGHT'
        WHEN s.slowk < 20 THEN 'OVERSOLD'
        ELSE 'NEUTRAL'
    END AS signal
FROM stoch_result
WHERE s.slowk IS NOT NULL
ORDER BY ts;
```

### AROON — Aroon Oscillator

Signature: `ta_aroon(high LIST<DOUBLE>, low LIST<DOUBLE>, time_period INT) -> LIST<STRUCT(aroon_down DOUBLE, aroon_up DOUBLE)>`

```sql
WITH base AS (
    SELECT
        list(ts   ORDER BY ts) AS ts_arr,
        list(high ORDER BY ts) AS high_arr,
        list(low  ORDER BY ts) AS low_arr
    FROM ohlc
),
aroon_result AS (
    SELECT
        unnest(ts_arr)                          AS ts,
        unnest(ta_aroon(high_arr, low_arr, 14)) AS ar
    FROM base
)
SELECT
    ts,
    ar.aroon_up   AS aroon_up,
    ar.aroon_down AS aroon_down,
    ar.aroon_up - ar.aroon_down AS aroon_oscillator
FROM aroon_result
WHERE ar.aroon_up IS NOT NULL
ORDER BY ts;
```

### MINMAX — Rolling Min and Max in one pass

Signature: `ta_minmax(prices LIST<DOUBLE>, time_period INT) -> LIST<STRUCT(min DOUBLE, max DOUBLE)>`

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(close ORDER BY ts) AS price_arr
    FROM ohlc
),
mm_result AS (
    SELECT
        unnest(ts_arr)                       AS ts,
        unnest(ta_minmax(price_arr, 10))     AS mm
    FROM base
)
SELECT
    ts,
    mm.min AS rolling_min,
    mm.max AS rolling_max,
    mm.max - mm.min AS range_width
FROM mm_result
WHERE mm.min IS NOT NULL
ORDER BY ts;
```

### MAMA — MESA Adaptive Moving Average

Signature: `ta_mama(prices LIST<DOUBLE>, fast_limit DOUBLE, slow_limit DOUBLE) -> LIST<STRUCT(mama DOUBLE, fama DOUBLE)>`

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(close ORDER BY ts) AS price_arr
    FROM ohlc
),
mama_result AS (
    SELECT
        unnest(ts_arr)                           AS ts,
        unnest(ta_mama(price_arr, 0.5, 0.05))   AS m
    FROM base
)
SELECT
    ts,
    m.mama AS mama,
    m.fama AS fama
FROM mama_result
WHERE m.mama IS NOT NULL
ORDER BY ts;
```

---

## Tips

### Aligning scalar results with timestamps

The scalar (`ta_`) functions return arrays that are aligned with the input in index order. Use parallel `unnest` or a CTE with `list(ts ORDER BY ts)` alongside the prices to keep timestamps attached:

```sql
WITH base AS (
    SELECT
        list(ts    ORDER BY ts) AS ts_arr,
        list(close ORDER BY ts) AS price_arr
    FROM ohlc
)
SELECT
    unnest(ts_arr)                     AS ts,
    unnest(ta_sma(price_arr, 5))       AS sma_5
FROM base;
```

### NULL handling

Lookback positions (the first `time_period - 1` rows where the indicator cannot be computed) are returned as `NULL`. Filter them out with `WHERE sma_5 IS NOT NULL` or use `COALESCE` as needed.

### MA type constants

Several functions accept a `ma_type` integer parameter mapping to `TA_MAType`:

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
