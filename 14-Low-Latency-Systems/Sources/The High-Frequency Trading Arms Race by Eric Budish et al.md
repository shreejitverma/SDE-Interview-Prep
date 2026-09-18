---
tags: [trading/canon, trading/sources, type/source-summary]
aliases: [The High-Frequency Trading Arms Race, Eric Budish, Frequent Batch Auctions, Continuous Limit Order Books Flaw, HFT Arms Race]
status: evergreen
module: 14
created: 2026-09-17
---

# Source Summary — The High-Frequency Trading Arms Race: Frequent Batch Auctions as a Market Design Response
**Authors**: Eric Budish (University of Chicago Booth), Peter Cramton (University of Maryland), and John Shim (University of Chicago)  
**Publication**: The Quarterly Journal of Economics (QJE, 2015)  
**Category**: Market Microstructure, Exchange Architecture, High-Frequency Trading Economics

---

## Executive Summary & Core Thesis
Budish, Cramton, and Shim's 2015 paper is the definitive critique of the **Continuous Limit Order Book (CLOB)** market design. They demonstrate that the conventional continuous-time matching engine creates an inherent market failure: **Latency Arbitrage**.

In continuous time, two economically identical assets (such as the SPY ETF and E-mini S&P 500 futures) have correlated equilibrium values. Whenever a correlated asset moves, there is a race between market makers attempting to cancel stale quotes and predatory high-frequency traders attempting to pick off those stale quotes. Because continuous time rewards speed down to single picoseconds, market participants engage in a socially wasteful, multi-billion-dollar latency arms race (microwave towers, custom silicon, FPGA matching) that widens bid-ask spreads and harms organic liquidity. 

As a structural market design solution, the authors propose **Frequent Batch Auctions (FBA)**—discretizing time into uniform intervals (e.g., 100 milliseconds) and matching orders at a single uniform clearing price.

```mermaid
flowchart TD
    subgraph ContinuousMarket ["Continuous Limit Order Book (CLOB) Flaw"]
        direction TB
        E["Public Signal Event (e.g. S&P Futures jumps at t=0)"]
        RACE{"The Latency Race (Microseconds)"}
        MM["Market Maker Order Cancel (Arrives at t = 1.002 µs)"]
        HFT["Latency Arbitrageur Sniper (Arrives at t = 1.001 µs)"]
        
        E --> RACE
        RACE --> HFT
        RACE --> MM
        HFT -->|Snipe Winner (Fills Quote)| STALE["Stale Liquidity Sniped"]
        MM -->|Lost Race by 1 ns| REJECT["Cancel Rejected (Already Traded)"]
        STALE --> COST["Market Maker widens spread to cover Adverse Selection Tax"]
    end

    subgraph BatchAuction ["Frequent Batch Auction (FBA) Fix"]
        direction TB
        T["Batch Interval: [t, t + 100ms]"]
        ORDERS["All orders & cancels received within interval are batched"]
        UNIFORM["Orders processed simultaneously via Uniform-Price Auction"]
        T --> ORDERS --> UNIFORM
        UNIFORM --> ZERO["Latency advantage under 100ms has ZERO economic payoff"]
    end
```

---

## Key Economic Models & Findings

### 1. The Mechanics of Latency Arbitrage
Let asset $x$ (ES futures) and asset $y$ (SPY ETF) be perfectly correlated.
At time $t$, new information arrives indicating asset value increases by $\Delta v > 0$.
- In a continuous limit order book, whoever reaches the exchange matching engine first wins:
  - If the market maker cancels first: no trade occurs; spread remains tight.
  - If the latency arbitrageur arrives first: the arbitrageur buys at the stale price $p$, making an instantaneous riskless profit $\pi = \Delta v$, and the market maker suffers an adverse selection loss $-\pi$.
- Because both participants have access to identical public information, this is not fundamental price discovery—it is a pure transfer rent driven by nanosecond speed advantages.

### 2. The Inefficiency of Continuous Time
- Market makers anticipate getting sniped on every correlated price jump. To break even, they must widen their bid-ask spreads:
$$\text{Spread} = \text{Order Processing Cost} + \text{Inventory Risk} + \mathbf{\text{Adverse Selection (Latency Arbitrage Tax)}}$$
- As a consequence, continuous markets force ordinary retail and institutional investors to pay wider spreads to subsidize the latency arms race.

### 3. Frequent Batch Auctions (FBA) Architecture
- Divide the trading day into discrete, uniform time steps:
$$t_0, t_0 + \tau, t_0 + 2\tau, \dots$$
- During interval $[t_k, t_k + \tau)$, orders and cancellations are collected without continuous matching.
- At the end of each interval $\tau$ (e.g., $\tau = 100\text{ ms}$ or $10\text{ ms}$):
  1. All bids and asks are aggregated into discrete supply and demand curves.
  2. A single **uniform clearing price** $P^*$ is calculated at the intersection.
  3. If supply exceeds demand at $P^*$, allocation is executed pro-rata.
- **Economic Consequence**: Latency differences smaller than $\tau$ provide zero economic advantage. Speed competition is replaced by price competition, narrowing bid-ask spreads and deepening market liquidity.

---

## Engineering Implications for Low-Latency Systems

1. **Why Nanoseconds Matter in Current Markets**: The Budish paper provides the mathematical proof of why proprietary trading firms invest heavily in FPGA NICs, kernel bypass, and sub-10-nanosecond hardware pipelines: in continuous markets, being 5 nanoseconds slower than a competitor drops your execution fill rate from 100% to 0% on correlated liquidity sweeps.
2. **Speed Bumps and Asymmetric Delays**: Modern exchanges have implemented market design responses based on this paper:
   - **IEX (Investors Exchange)**: Implements a 350-microsecond coiled-fiber "speed bump" allowing market makers' pegged quote updates to outpace stale order snipe attempts.
   - **Eurex & Aquis**: Implemented asymmetric delays and batch auction formats for equity and derivative products.
3. **Queue Position Dynamics**: When tick size is large and spread is fixed at 1 tick, execution priority defaults to FIFO queue position. Low latency is required not just for sniping, but to establish queue priority at newly formed price levels.

---

## Related Notes
- [[15-Technical-Whitepapers/10-Seminal-Low-Latency-Systems-Papers/04-Market-Microstructure-and-Order-Dynamics]]
- [[08 - Order Book & Matching Engine/Order Book Data Structures and Algorithms]]
- [[08 - Order Book & Matching Engine/Matching Engine Core Logic and Execution]]
- [[10 - Quantitative Strategies/Statistical Arbitrage and Pairs Trading]]
- [[14 - Industry Map & Canon/MOC - 14 Industry Map & Canon]]
