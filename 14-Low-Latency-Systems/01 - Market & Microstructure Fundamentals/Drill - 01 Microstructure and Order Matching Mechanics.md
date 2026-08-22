---
tags: [trading/microstructure, type/drill]
aliases: [Drill 01, Microstructure Drill, Order Mechanics Drill]
status: evergreen
module: 01
created: 2026-08-22
---

# Drill 01 — Microstructure & Order Matching Mechanics

> [!summary]
> Rapid-fire technical drill calibrating your operational intuition across order state machine races, fee-adjusted routing economics, auction uncrossing rules, and Level-2 queue depletion mechanics. Attempt each problem before unfolding the solution.

---

### Problem 1: In-Flight Cancel vs Execution Race Condition
**Scenario**:
Your automated market making engine quotes Bid $\$100.00$ (Size: 100). At timestamp $t=0.000\text{ ms}$, your alpha model detects an impending price crash and transmits an immediate `CancelOrder` request over OUCH/TCP. At $t=0.150\text{ ms}$, your engine receives an `ExecutionReport: Filled (Qty: 100 @ $100.00)`. At $t=0.180\text{ ms}$, your engine receives a `CancelReject: Order Already Filled`.

**Questions**:
1. What was the exact sequence of events at the exchange matching engine?
2. How should your local order state machine transition upon receiving the `ExecutionReport` while in state `PENDING_CANCEL`?
3. What immediate risk action must your strategy take regarding the opposite-side Ask quote?

> [!question]- Unfold Solution
> 1. **Exchange Sequence**: The exchange Sequencer received and matched an incoming aggressive SELL order against your resting quote at $t \approx 0.100\text{ ms}$ (emitting the `Fill`), and subsequently processed your `Cancel` request at $t \approx 0.150\text{ ms}$ when the order had already been unlinked and filled (emitting the `CancelReject`).
> 2. **State Machine Transition**: The state machine must transition from `PENDING_CANCEL` $\to$ `FILLED`, setting `leaves_qty = 0` and updating inventory position by $+100$ shares. It must ignore the subsequent `CancelReject` or treat it as a terminal acknowledgment of the canceled state.
> 3. **Strategy Risk Action**: Because you just experienced an adverse fill on your Bid immediately before a market crash, you now hold 100 shares of toxic long inventory. You must immediately **cancel your resting Ask quote** (to prevent it from getting stuck or filled on stale prices) and dynamically re-hedge the $+100$ long delta on a fast venue.

---

### Problem 2: Fee-Adjusted Routing on Inverted Exchanges
**Scenario**:
You are designing an aggressive execution router for a stock with an NBBO of **$\$50.00 \times \$50.01$**.
Two venues display liquidity at the National Best Offer ($\$50.01$):
- **Venue A (Standard Maker-Taker)**: Displays 5,000 shares @ $\$50.01$. Taker Access Fee: **$+30\text{ mils}$ (\$0.0030/share)**.
- **Venue B (Inverted / Taker-Maker)**: Displays 2,000 shares @ $\$50.01$. Taker Rebate: **$-15\text{ mils}$ (-\$0.0015/share)**.

You need to buy 1,000 shares aggressively via an Immediate-Or-Cancel (IOC) order.

**Questions**:
1. What is the net-effective execution price on Venue A vs Venue B?
2. Which venue should the router target first?
3. What is the exact dollar PnL savings on this 1,000-share execution?

> [!question]- Unfold Solution
> 1. **Net-Effective Prices**:
>    - **Venue A**: $\$50.01 + \$0.0030 = \mathbf{\$50.0130}$ per share.
>    - **Venue B**: $\$50.01 - \$0.0015 = \mathbf{\$50.0085}$ per share.
> 2. **Routing Choice**: The router must target **Venue B first**.
> 3. **Dollar Savings**:
>    $$\text{Savings per share} = \$50.0130 - \$50.0085 = \$0.0045 \text{ (45 mils)}$$
>    $$\text{Total Savings on 1,000 shares} = 1,000 \times \$0.0045 = \mathbf{\$4.50}$$
>    *(Over 10,000,000 shares/day, this fee optimization generates \$45,000/day in pure alpha).*

---

### Problem 3: Discrete Cross Imbalance Tie-Breaking
**Scenario**:
An Opening Cross has accumulated the following demand and supply schedules across three price tiers:

| Price Tier | Cumulative Demand (Buy Qty) | Cumulative Supply (Sell Qty) |
| :--- | :--- | :--- |
| **$\$100.10$** | 10,000 shares | 60,000 shares |
| **$\$100.00$** | 50,000 shares | 50,000 shares |
| **$\$99.90$** | 70,000 shares | 20,000 shares |

Reference price (previous close) is **$\$100.05$**.

**Questions**:
1. What is the matched volume at each price tier?
2. What is the official clearing price $P^*$?
3. What is the remaining imbalance quantity and side?

> [!question]- Unfold Solution
> 1. **Matched Volume per Tier**:
>    - At $\$100.10$: $\min(10000, 60000) = \mathbf{10,000\text{ shares}}$.
>    - At $\$100.00$: $\min(50000, 50000) = \mathbf{50,000\text{ shares}}$.
>    - At $\$99.90$: $\min(70000, 20000) = \mathbf{20,000\text{ shares}}$.
> 2. **Official Clearing Price**: **$\$100.00$** (Tier 1 objective maximizes matched volume at 50,000 shares).
> 3. **Imbalance**: $\text{Demand} - \text{Supply} = 50,000 - 50,000 = \mathbf{0\text{ shares}}$ (Zero imbalance / perfect match).

---

### Problem 4: Level-2 Queue Estimation Under Heavy Cancellations
**Scenario**:
You submit a passive BUY limit order of 100 contracts in CME E-mini S&P futures at $\$5,000.00$. When your order is acknowledged, total displayed depth at $\$5,000.00$ is **2,000 contracts** (so 1,900 contracts are ahead of you in FIFO priority).
Over the next 500 milliseconds:
- Total depth at $\$5,000.00$ drops to **800 contracts**.
- The market data feed broadcasts that **200 contracts executed as trades** at $\$5,000.00$.

**Questions**:
1. What was the total volume of order cancellations at this price level?
2. Using the Proportional Cancellation Model, how many contracts are estimated to remain ahead of your order?

> [!question]- Unfold Solution
> 1. **Total Canceled Volume**:
>    $$V_{\text{cancel}} = \text{Old Depth} - \text{New Depth} - V_{\text{trade}} = 2000 - 800 - 200 = \mathbf{1,000\text{ contracts canceled}}.$$
> 2. **Proportional Queue Position Estimation**:
>    - Step 1 (Trade deduction): $\text{Ahead}_1 = 1,900 - 200 = 1,700$.
>    - Step 2 (Proportional cancel deduction):
>      $$\text{Cancel Deduction} = 1000 \times \frac{1900}{2000} = 950 \text{ contracts}.$$
>    - Estimated Contracts Ahead:
>      $$Q(t) = 1,700 - 950 = \mathbf{750\text{ contracts remaining ahead}}.$$
>    *(Your position advanced from #1,900 to #750 despite only 200 contracts actually executing as trades).*

---

## Related
- [[01 - Market & Microstructure Fundamentals/Order Types and State Transitions]]
- [[01 - Market & Microstructure Fundamentals/Maker-Taker vs Inverted Fee Models]]
- [[01 - Market & Microstructure Fundamentals/Continuous Trading vs Discrete Auctions]]
- [[01 - Market & Microstructure Fundamentals/Order Book Dynamics and Queue Position]]
- [[01 - Market & Microstructure Fundamentals/MOC - 01 Market & Microstructure Fundamentals]]
