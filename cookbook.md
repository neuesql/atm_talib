# DuckDB TA-Lib Extension — SQL Cookbook

One example per category. Window functions (`ta_`) are the default; scalar (`t_`) shown as alternative.

---

## Sample Data

```sql
CREATE TABLE ohlc AS
SELECT * FROM (VALUES
    ('2024-01-01'::DATE, 10.0, 11.2,  9.8, 10.5, 120000),
    ('2024-01-02'::DATE, 10.5, 11.8, 10.2, 11.3, 135000),
    ('2024-01-03'::DATE, 11.3, 12.1, 11.0, 11.8,  98000),
    ('2024-01-04'::DATE, 11.8, 12.5, 11.5, 12.2, 145000),
    ('2024-01-05'::DATE, 12.2, 13.0, 11.9, 12.7, 167000),
    ('2024-01-06'::DATE, 12.7, 13.4, 12.3, 13.1, 112000),
    ('2024-01-07'::DATE, 13.1, 13.8, 12.8, 13.5, 130000),
    ('2024-01-08'::DATE, 13.5, 14.2, 13.1, 13.9, 118000),
    ('2024-01-09'::DATE, 13.9, 14.6, 13.5, 14.3, 155000),
    ('2024-01-10'::DATE, 14.3, 15.0, 13.9, 14.8, 142000),
    ('2024-01-11'::DATE, 14.8, 15.3, 14.2, 15.0, 138000),
    ('2024-01-12'::DATE, 15.0, 15.8, 14.7, 15.4, 160000),
    ('2024-01-13'::DATE, 15.4, 16.1, 15.0, 15.8, 175000),
    ('2024-01-14'::DATE, 15.8, 16.5, 15.3, 16.2, 190000),
    ('2024-01-15'::DATE, 16.2, 16.9, 15.8, 16.6, 165000),
    ('2024-01-16'::DATE, 16.6, 17.2, 16.1, 16.9, 148000),
    ('2024-01-17'::DATE, 16.9, 17.6, 16.5, 17.3, 155000),
    ('2024-01-18'::DATE, 17.3, 18.0, 16.8, 17.7, 172000),
    ('2024-01-19'::DATE, 17.7, 18.4, 17.2, 18.1, 168000),
    ('2024-01-20'::DATE, 18.1, 18.8, 17.6, 18.5, 182000)
) AS t(ts, open, high, low, close, volume);
```

---

## Trend — SMA crossover signal

```sql
-- Window: 5-period and 10-period SMA, detect golden/death cross
WITH signals AS (
    SELECT ts, close,
           ta_sma(close, 5)  OVER w AS sma_5,
           ta_sma(close, 10) OVER w AS sma_10
    FROM ohlc
    WINDOW w AS (ORDER BY ts)
)
SELECT ts, close, round(sma_5, 2) AS sma_5, round(sma_10, 2) AS sma_10,
       CASE
           WHEN sma_5 > sma_10 AND lag(sma_5) OVER (ORDER BY ts) <= lag(sma_10) OVER (ORDER BY ts)
               THEN 'GOLDEN CROSS'
           WHEN sma_5 < sma_10 AND lag(sma_5) OVER (ORDER BY ts) >= lag(sma_10) OVER (ORDER BY ts)
               THEN 'DEATH CROSS'
       END AS signal
FROM signals
WHERE sma_10 IS NOT NULL;
```

Scalar alternative:

```sql
SELECT unnest(t_sma((SELECT list(close ORDER BY ts) FROM ohlc), 5)) AS sma_5;
```

---

## Momentum — RSI overbought/oversold

```sql
-- Window: 14-period RSI with signal zones
SELECT ts, close,
       round(ta_rsi(close, 14) OVER (ORDER BY ts), 2) AS rsi,
       CASE
           WHEN ta_rsi(close, 14) OVER (ORDER BY ts) >= 70 THEN 'OVERBOUGHT'
           WHEN ta_rsi(close, 14) OVER (ORDER BY ts) <= 30 THEN 'OVERSOLD'
           ELSE 'NEUTRAL'
       END AS signal
FROM ohlc;
```

Scalar alternative:

```sql
SELECT unnest(t_rsi((SELECT list(close ORDER BY ts) FROM ohlc), 14)) AS rsi;
```

---

## Momentum (multi-output) — Stochastic Oscillator

```sql
-- Window: Stochastic %K/%D with overbought/oversold zones
SELECT ts, close, s.slowk, s.slowd,
       CASE
           WHEN s.slowk > 80 THEN 'OVERBOUGHT'
           WHEN s.slowk < 20 THEN 'OVERSOLD'
           ELSE 'NEUTRAL'
       END AS signal
FROM (
    SELECT ts, close,
           ta_stoch(high, low, close, 5, 3, 0, 3, 0) OVER (ORDER BY ts) AS s
    FROM ohlc
)
WHERE s.slowk IS NOT NULL;
```

---

## Trend (multi-output) — MACD signal line crossover

```sql
-- Window: MACD with histogram direction
SELECT ts, close, round(m.macd, 4) AS macd, round(m.signal, 4) AS signal, round(m.hist, 4) AS hist,
       CASE WHEN m.hist > 0 THEN 'BULLISH' ELSE 'BEARISH' END AS direction
FROM (
    SELECT ts, close,
           ta_macd(close, 12, 26, 9) OVER (ORDER BY ts) AS m
    FROM ohlc
)
WHERE m.macd IS NOT NULL;
```

---

## Volume — Chaikin A/D Line

```sql
-- Window: cumulative A/D line (no period parameter)
SELECT ts, close, volume,
       round(ta_ad(high, low, close, volume) OVER (ORDER BY ts), 0) AS ad_line
FROM ohlc;
```

---

## Volatility — Bollinger Bands squeeze detection

```sql
-- Window: Bollinger Bands with bandwidth for squeeze detection
SELECT ts, close,
       round(b.upper, 2) AS upper, round(b.middle, 2) AS middle, round(b.lower, 2) AS lower,
       round((b.upper - b.lower) / b.middle * 100, 2) AS bandwidth_pct
FROM (
    SELECT ts, close,
           ta_bbands(close, 10, 2.0, 2.0, 0) OVER (ORDER BY ts) AS b
    FROM ohlc
)
WHERE b.upper IS NOT NULL;
```

---

## Price Transform — Typical Price

```sql
-- Window: typical price as a single synthetic series
SELECT ts, round(ta_typprice(high, low, close) OVER (ORDER BY ts), 2) AS typical_price
FROM ohlc;
```

---

## Cycle — Hilbert Transform trend mode

```sql
-- Window: detect whether market is trending or cycling
SELECT ts, close,
       ta_ht_trendmode(close) OVER (ORDER BY ts) AS mode,
       CASE ta_ht_trendmode(close) OVER (ORDER BY ts)
           WHEN 1 THEN 'TRENDING'
           WHEN 0 THEN 'CYCLING'
       END AS regime
FROM ohlc;
```

---

## Statistics — Linear Regression trend direction

```sql
-- Window: slope sign determines trend direction
SELECT ts, close,
       round(ta_linearreg_slope(close, 10) OVER (ORDER BY ts), 4) AS slope,
       CASE WHEN ta_linearreg_slope(close, 10) OVER (ORDER BY ts) > 0
            THEN 'UP' ELSE 'DOWN'
       END AS trend
FROM ohlc;
```

---

## Pattern Recognition — Scan multiple candlestick patterns

Pattern functions return `100` (bullish), `-100` (bearish), or `0` (no pattern).

```sql
-- Window: scan for common reversal patterns
SELECT ts, open, high, low, close,
       ta_cdlhammer(open, high, low, close)    OVER (ORDER BY ts) AS hammer,
       ta_cdlengulfing(open, high, low, close) OVER (ORDER BY ts) AS engulfing,
       ta_cdldoji(open, high, low, close)      OVER (ORDER BY ts) AS doji
FROM ohlc;
```

---

## Math Transform — Log returns

```sql
-- Scalar: compute log of close prices
SELECT unnest(t_ln((SELECT list(close ORDER BY ts) FROM ohlc))) AS ln_close;
```

---

## Tips

**NULL handling** — The first `period - 1` rows return NULL (lookback). Filter with `WHERE col IS NOT NULL`.

**Multi-output field access** — Use a subquery to access struct fields:

```sql
SELECT m.macd, m.signal FROM (
    SELECT ta_macd(close, 12, 26, 9) OVER (ORDER BY ts) AS m FROM ohlc
);
```

**MA type constants** — Functions like `ta_bbands` accept `ma_type`:

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
