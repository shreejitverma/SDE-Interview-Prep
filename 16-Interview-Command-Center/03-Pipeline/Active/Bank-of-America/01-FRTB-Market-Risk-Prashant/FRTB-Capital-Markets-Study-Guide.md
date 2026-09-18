---
tags: [prep-guide, frtb, market-risk, python, bank-of-america, capital-markets]
---

# 📖 BofA Quant Python Developer — FRTB & Capital Markets Study Guide

> **Target Role:** Quant / Python Developer — FRTB & Market Risk (Prashant Raghuvanshi)
> **Domain Focus:** Basel III.1 / FRTB (SA & IMA), Risk Sensitivities, Capital Calculations, P&L Reconciliation

---

## 🏛️ Part 1: FRTB Regulatory Foundations

### 1.1 Why FRTB Was Created
The **Fundamental Review of the Trading Book (FRTB)** represents the most comprehensive reform of global market risk capital standards since the 1996 Market Risk Amendment. Issued by the **Basel Committee on Banking Supervision (BCBS)** as part of Basel III.1 (often called Basel IV), it replaces the patchwork framework of Basel 2.5.

#### Critical Flaws in Basel 2.5 Exposed by the 2008 Crisis:
1. **VaR Underestimation of Tail Risk:** Value at Risk (VaR) only measured the cutoff loss at a percentile (e.g., 99%), ignoring the severity of losses beyond the cutoff ("fat tails").
2. **Arbitrage Between Trading Book & Banking Book:** Banks could move illiquid assets into the trading book to benefit from lower capital requirements, even if they had no intention or ability to actively trade them.
3. **Internal Model Variability:** Different tier-1 banks reported drastically different capital requirements for identical trading portfolios due to subjective modeling choices.
4. **Failure to Capture Market Illiquidity:** Basel 2.5 assumed all positions could be liquidated or hedged in 10 days, catastrophic during the credit freeze of 2008.
5. **Inadequate Credit Default & Migration Risk:** Jump-to-default and credit rating migration risks were insufficiently capitalized.

### 1.2 The Strict Trading Book / Banking Book Boundary
FRTB introduces strict boundaries:
- **Trading Book:** Financial instruments held with trading intent or to hedge other trading book positions. Presumptive list of instruments.
- **Banking Book:** Instruments intended to be held to maturity or for liquidity management.
- **Switching Penalties:** Moving an asset between books requires explicit regulatory approval. Any capital benefit resulting from a switch is strictly disallowed.

---

## 📐 Part 2: FRTB Standardized Approach (SA) — Deep Mathematical Architecture

Every bank subject to FRTB must implement the Standardized Approach. Even if a bank is approved for IMA (Internal Models Approach), the SA serves as an **unconditional capital floor** and regulatory benchmark.

```
┌─────────────────────────────────────────────────────────────────┐
│               FRTB Standardized Approach (SA) Capital           │
│                                                                 │
│   Total Capital = SBM Capital + Default Risk (DRC) + RRAO       │
└───────────────┬─────────────────────────┬───────────────────────┘
                │                         │
      ┌─────────▼────────┐      ┌─────────▼────────┐
      │   SBM Capital    │      │ Default Risk DRC │
      │  (Sensitivities) │      │ (Jump-to-Default)│
      └─────────┬────────┘      └──────────────────┘
                │
   ┌────────────┴────────────┬────────────────────────┐
   │                         │                        │
┌──▼──────────┐       ┌──────▼──────┐          ┌──────▼──────┐
│    Delta    │       │    Vega     │          │  Curvature  │
│ Sensitivity │       │ Sensitivity │          │ Sensitivity │
└─────────────┘       └─────────────┘          └─────────────┘
```

The Standardized Approach capital charge consists of three pillars:
$$\text{Total SA Capital} = K_{\text{SBM}} + K_{\text{DRC}} + K_{\text{RRAO}}$$

---

### 2.1 Sensitivities-Based Method (SBM)

SBM determines capital based on standardized regulatory shocks applied to front-office risk factor sensitivities.

#### The 7 SBM Risk Classes:
1. **General Interest Rate Risk (GIRR):** Yield curves across currencies (specified tenors: 0.25y, 0.5y, 1y, 2y, 3y, 5y, 10y, 15y, 20y, 30y), inflation rates, basis spreads.
2. **Credit Spread Risk (CSR) — Non-Securitized:** Sovereign, municipal, corporate bond credit curves.
3. **Credit Spread Risk (CSR) — Securitized (Non-CTP):** Asset-backed securities, RMBS, CMBS.
4. **Credit Spread Risk (CSR) — Securitized (CTP):** Correlation Trading Portfolio.
5. **Equity Risk:** Spot equity prices, repo rates, equity dividends across market cap & sector buckets.
6. **Commodity Risk:** Energy, metals, agriculture, freight.
7. **Foreign Exchange (FX) Risk:** Spot exchange rates against the bank's reporting currency.

#### The 3 Sensitivity Types:
- **Delta Sensitivity ($s_k$):** First-order linear sensitivity to underlying price/rate shift:
  $$s_k = \frac{\partial V}{\partial r_k} \approx \frac{V(r_k + \Delta r_k) - V(r_k)}{\Delta r_k}$$
- **Vega Sensitivity ($v_k$):** Sensitivity of option prices to implied volatility:
  $$v_k = \sigma_k \frac{\partial V}{\partial \sigma_k}$$
- **Curvature Sensitivity ($CV_k$):** Non-linear second-order risk not captured by Delta:
  $$CV_k = -\min\left( V(r_k + RW_k) - V(r_k) - RW_k \cdot s_k, \; V(r_k - RW_k) - V(r_k) + RW_k \cdot s_k, \; 0 \right)$$

---

### 2.2 SBM Step-by-Step Mathematical Calculation Flow

#### Step 1: Compute Net Sensitivity & Weighted Sensitivity
For each risk factor $k$ in bucket $b$:
$$WS_k = RW_k \cdot s_k$$
where $RW_k$ is the supervisory risk weight specified by BCBS (e.g., higher for emerging market currencies or high-yield credit).

#### Step 2: Intra-Bucket Aggregation (Within Bucket $b$)
Aggregate all weighted sensitivities in bucket $b$ using the supervisory correlation matrix $\rho_{kl}$:
$$K_b = \sqrt{\sum_k WS_k^2 + \sum_k \sum_{l \neq k} \rho_{kl} WS_k WS_l}$$
*Note: If the expression inside the square root is negative (can happen in extreme curvature hedging), $K_b$ is floored.*

#### Step 3: Cross-Bucket Aggregation (Across Buckets in Risk Class)
Aggregate bucket capital charges $K_b$ across all buckets within a risk class using cross-bucket correlation $\gamma_{bc}$:
$$K_{\text{RiskClass}} = \sqrt{\sum_b K_b^2 + \sum_b \sum_{c \neq b} \gamma_{bc} S_b S_c}$$
where $S_b = \sum_{k \in b} WS_k$ (bounded: $\max(\min(S_b, K_b), -K_b)$).

#### Step 4: The 3 Correlation Scenarios
To guard against correlation breakdown during market crises, banks must calculate $K_{\text{RiskClass}}$ under **three distinct correlation regimes**:
1. **Medium Correlations:** Prescribed standard correlations ($\rho_{kl}, \gamma_{bc}$).
2. **High Correlations:** $\rho_{kl}^{\text{high}} = \min(1.25 \cdot \rho_{kl}, 1.0)$
3. **Low Correlations:** $\rho_{kl}^{\text{low}} = \max(2 \cdot \rho_{kl} - 1.0, 0.75 \cdot \rho_{kl})$

The final SBM Capital for the risk class is the **maximum** across all 3 scenarios:
$$K_{\text{SBM, RiskClass}} = \max\left( K_{\text{Medium}}, K_{\text{High}}, K_{\text{Low}} \right)$$

Total firm-wide SBM is simply the direct sum across all 7 risk classes (no diversification benefits between risk classes):
$$K_{\text{SBM}} = \sum_{j=1}^{7} K_{\text{SBM, RiskClass}_j}$$

---

### 2.3 Default Risk Charge (DRC)
Captures jump-to-default risk for debt and equity instruments:
1. Calculate **Gross Jump-to-Default (JTD)** for each position:
   $$\text{JTD}_{\text{long}} = \max\left(0, \text{Notional} \cdot (1 - \text{LGD}) - \text{Market Value}\right)$$
   $$\text{JTD}_{\text{short}} = \min\left(0, \text{Notional} \cdot (1 - \text{LGD}) - \text{Market Value}\right)$$
2. Offset long and short JTDs on the same obligor.
3. Apply supervisory default risk weights based on credit rating (AAA $\to$ CCC).

### 2.4 Residual Risk Add-On (RRAO)
To prevent complex exotic options from bypassing the framework:
- Instruments with **exotic underlying** (e.g., weather, crypto, longevity) $\to$ **1.0% gross notional surcharge**.
- Instruments with **other residual risks** (e.g., Bermudan swaptions, correlation options) $\to$ **0.1% gross notional surcharge**.
- RRAO is strictly additive with zero netting across positions.

---

## 🔬 Part 3: FRTB Internal Models Approach (IMA)

Banks with sophisticated quant infrastructure apply for IMA approval on a **desk-by-desk basis**.

```
┌─────────────────────────────────────────────────────────────┐
│                    FRTB IMA Architecture                     │
├──────────────────────────────┬──────────────────────────────┤
│ Expected Shortfall (ES)      │ 97.5% confidence             │
│ Replaces 99% VaR             │ Integrates tail expectations │
├──────────────────────────────┼──────────────────────────────┤
│ Liquidity Horizons (LH)      │ 10, 20, 40, 60, 120 days     │
│ Scaled by risk illiquidity   │ Sub-portfolio rolling shocks │
├──────────────────────────────┼──────────────────────────────┤
│ P&L Attribution Test (PLAT)  │ Desk-level validation:       │
│ HPL vs RTPL                  │ Spearman Correlation ≥ 0.80  │
│ (Failing desks revert to SA) │ Kolmogorov-Smirnov p ≥ 0.05  │
├──────────────────────────────┼──────────────────────────────┤
│ Non-Modellable Risk (NMRF)   │ Stress Capital Add-on (SES)  │
│ Fails RFET data count tests  │ Extreme stress scenario      │
└──────────────────────────────┴──────────────────────────────┘
```

---

### 3.1 Expected Shortfall (ES) vs VaR

$$\text{VaR}_\alpha(X) = \inf \{ x \in \mathbb{R} : P(X > x) \le 1 - \alpha \}$$

$$\text{ES}_\alpha(X) = \mathbb{E}[ X \mid X > \text{VaR}_\alpha(X) ]$$

Why ES at 97.5% equals or exceeds VaR at 99%:
- Under a standard Gaussian distribution, $\text{ES}_{97.5\%} \approx \text{VaR}_{99\%}$.
- However, financial returns exhibit **leptokurtosis (fat tails)**. In real-world crises (e.g., March 2020 COVID shock), losses past VaR are extreme. ES captures the conditional expectation of disaster, penalizing high-kurtosis trading strategies (like naked put writing).

---

### 3.2 The Desk-Level Tests: PLAT and Backtesting

To maintain IMA status, each trading desk must pass two continuous quantitative tests:

#### 1. P&L Attribution Test (PLAT)
Compares two daily P&L measures:
- **Hypothetical P&L (HPL):** Daily P&L computed by the front-office pricing engine using static positions from $T-1$ re-evaluated with market prices at $T$ (excludes fees, intraday trading).
- **Risk-Theoretic P&L (RTPL):** Daily P&L generated by the market risk system's pricing model using the risk factors and sensitivities used in the ES engine.

**Metrics:**
- **Spearman Rank Correlation:** Measures directional agreement ($r_s \ge 0.80$ to stay in Green zone).
- **Kolmogorov-Smirnov (KS) Test:** Measures difference in cumulative distribution functions ($p \ge 0.05$ to stay in Green zone).

*If a desk enters the **Red Zone** (correlation $< 0.70$ or KS $p < 0.01$), it immediately loses IMA approval and must compute capital under the Standardized Approach (often a 2x-4x capital penalty).*

#### 2. Desk-Level & Firm-Level Backtesting
- Compares VaR / ES against daily actual P&L and hypothetical P&L over a 250-business-day trailing window.
- Exceptions (losses exceeding 99% 1-day VaR):
  - **Green Zone:** 0 to 4 exceptions (Normal)
  - **Amber Zone:** 5 to 9 exceptions (Supervisory capital multiplier increased)
  - **Red Zone:** 10+ exceptions (Automatic revocation of model approval)

---

### 3.3 Risk Factor Eligibility Test (RFET) & Non-Modellable Risk Factors (NMRF)
Every risk factor fed into the IMA model must prove it has sufficient market liquidity:
- **RFET Criteria:** Must have at least **24 real price observations** over the past 12 months with no 90-day gap, OR at least **100 observations** over 12 months.
- If a risk factor fails RFET, it is designated **Non-Modellable (NMRF)**.
- Capital for NMRF cannot be diversified; each NMRF must be capitalized using an independent extreme stress scenario (SES - Stressed Capital Add-on).

---

## 🧮 Part 4: Financial Instruments & Greeks in Capital Markets

### 4.1 Fixed Income & Interest Rate Derivatives
- **Bonds:** Price $P = \sum_{t=1}^n \frac{C}{(1+y)^t} + \frac{M}{(1+y)^n}$.
  - **DV01 / PV01:** Dollar value of 1 basis point (0.01%) shift in yield curve: $\text{DV01} = -\frac{\partial P}{\partial y} \cdot 0.0001$.
  - **Modified Duration:** Percentage price change per unit yield shift: $D_{\text{mod}} = \frac{1}{P} \frac{\partial P}{\partial y}$.
  - **Key Rate Duration (KRD):** Sensitivity to shifts in specific tenor vertices (e.g., 2Y, 5Y, 10Y, 30Y) — essential for FRTB GIRR bucket assignment!
- **Interest Rate Swaps (IRS):**
  - Receive Fixed vs Pay Floating (SOFR / EURIBOR).
  - Net present value $V = V_{\text{fixed}} - V_{\text{float}}$.
  - Sensitivities: Curve delta across tenor buckets.

### 4.2 Equity & FX Derivatives
- **Black-Scholes Options Greeks:**
  - $\Delta = \frac{\partial V}{\partial S}$ (feeds into Delta SBM)
  - $\Gamma = \frac{\partial^2 V}{\partial S^2}$ (feeds into Curvature SBM)
  - $\mathcal{V} = \frac{\partial V}{\partial \sigma}$ (feeds into Vega SBM)
  - $\rho = \frac{\partial V}{\partial r}$ (feeds into GIRR Delta)
  - $\Theta = \frac{\partial V}{\partial t}$ (P&L time decay / carry)

### 4.3 Credit Derivatives (CDS)
- **Credit Default Swap (CDS):** Protection buyer pays quarterly spread $s$ in exchange for compensation upon credit event (bankruptcy, failure to pay).
- **CS01 / Spread DV01:** P&L change per 1 basis point widening in credit spread.
- **Hazard Rate $\lambda(t)$:** Conditional default intensity.

---

## 🔍 Part 5: Practical Reconciliation & Discrepancy Investigation

In the BofA team, a primary responsibility is **investigating discrepancies between risk calculations, trading positions, sensitivities, P&L, and regulatory capital results**.

### Common Production Breaks & Root Causes:

| Discrepancy Type | Typical Manifestation | Root Cause to Investigate | Remediation Procedure |
|:-----------------|:----------------------|:--------------------------|:----------------------|
| **Sensitivity Break** | Risk engine delta $\neq$ Front office pricing delta | 1. Different curve construction (e.g., dual-curve SOFR discount vs LIBOR legacy)<br>2. Finite difference bump size mismatch ($\Delta S = 1\%$ vs $0.01\%$) | Align bump convention; verify curve calibration inputs between FO and Risk models |
| **P&L Attribution Break** | HPL vs RTPL Spearman correlation drops $< 0.80$ | 1. RTPL model omitting high-order cross-gamma or smile effects<br>2. Market data timing differences (EOD snapshot 4:00 PM vs 4:15 PM) | Audit risk factor mapping; ensure synchronized market data snapshot timestamp |
| **Capital Spike in SBM** | SBM Capital doubles unexpectedly overnight | 1. Portfolio crossed boundary into Low/High correlation worst-case scenario<br>2. Mis-mapped risk class bucket (e.g., high-yield corporate treated as sovereign) | Run correlation scenario decomposition; check static data / rating feed for position |
| **Position Recon Break** | Capital numbers missing major trades | 1. Late-booked trades after risk batch cutoff<br>2. Trade lifecycle events (exercise, expiry, corporate actions) processed out of order | Inspect trade ingest timestamp vs batch snapshot timestamp; rerun delta batch with late trades |
| **NMRF Identification Break**| Sudden surge in SES capital add-on | 1. Stale trade ticker in market data vendor feed<br>2. RFET observation count failed due to bank holiday missing feed | Verify vendor feed pipeline; backfill verified trade records to restore modellability status |

---

## 💡 Part 6: How to Ace Prashant Raghuvanshi's Interview

When speaking to Prashant:
1. **Speak with regulatory rigor:** Use terms like *SBM aggregation*, *3 correlation regimes*, *PLAT Spearman/KS thresholds*, *RFET real price criteria*, and *audit lineage*.
2. **Bridge the gap between Quant Math & Python Engineering:** Emphasize that you don't just calculate formulas on a toy Jupyter notebook; you design **production-grade, low-latency, memory-efficient Python microservices and batch pipelines** that execute these calculations for thousands of trades every day.
3. **Emphasize data reconciliation:** Demonstrate that you understand how subtle data mismatches (e.g., market data timing, static data mapping) can cause regulatory audit failures.
