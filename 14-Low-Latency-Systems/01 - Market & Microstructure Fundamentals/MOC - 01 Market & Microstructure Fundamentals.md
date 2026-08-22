---
tags: [trading/microstructure, type/moc]
aliases: [Microstructure MOC, Market Fundamentals MOC]
status: evergreen
module: 01
created: 2026-08-22
---

# MOC — 01 Market & Microstructure Fundamentals

Execution mechanics, microstructure models, structural market fragmentation, and the regulatory fabric governing trade execution.

```mermaid
flowchart TD
    subgraph MicrostructureCore ["Market Microstructure Core"]
        OT["Order Types & States"] --> CDA["Continuous vs Discrete Crosses"]
        CDA --> FEES["Maker-Taker vs Inverted Fees"]
        FEES --> NOISE["Price Discovery & Microstructure Noise"]
        NOISE --> NMS["Market Fragmentation & Reg NMS"]
        NMS --> MIFID["European Structure & MiFID II"]
        MIFID --> QUEUE["Order Book Dynamics & Queue Position"]
    end

    subgraph AppliedLabs ["Practicum & Drills"]
        QUEUE --> LAB1["Lab 01: Double Auction & Cross Simulator"]
        QUEUE --> DRILL1["Drill 01: Microstructure Mechanics Drill"]
        QUEUE --> WS1["War Story: 2010 Flash Crash"]
    end
```

---

## Core Concepts
- [[01 - Market & Microstructure Fundamentals/Order Types and State Transitions]] — Limit, Market, Stop, Pegged, IOC, FOK, Post-Only, and native exchange state machines.
- [[01 - Market & Microstructure Fundamentals/Continuous Trading vs Discrete Auctions]] — Uncrossing algorithms, continuous double auctions, opening/closing crosses, NOII feeds, volatility halts.
- [[01 - Market & Microstructure Fundamentals/Maker-Taker vs Inverted Fee Models]] — Economic incentives, adverse selection, queue priority dynamics, fee-adjusted routing.
- [[01 - Market & Microstructure Fundamentals/Price Discovery and Microstructure Noise]] — Information arrivals, Roll's effective spread model, Kyle's lambda price impact, bid-ask bounce.
- [[01 - Market & Microstructure Fundamentals/Market Fragmentation and Reg NMS]] — Rule 611 (Order Protection), Rule 610 (Access Fees), ISOs, SIP vs Direct Feeds, Synthetic NBBO.
- [[01 - Market & Microstructure Fundamentals/European Market Structure and MiFID II]] — RTS 25 clock synchronization, Double Volume Caps, Systematic Internalisers, tick size regimes (RTS 11).
- [[01 - Market & Microstructure Fundamentals/Order Book Dynamics and Queue Position]] — Queue depletion models, 98:1 cancel-to-trade ratios, Level-2 proportional cancellation estimation.

## Labs & Implementations
- [[01 - Market & Microstructure Fundamentals/Lab - 01 Continuous Double Auction Simulator]] — Build an allocation-free discrete call cross uncrossing engine and continuous matching simulator in C++20.

## Drills & War Stories
- [[01 - Market & Microstructure Fundamentals/Drill - 01 Microstructure and Order Matching Mechanics]] — Rapid-fire calibration on order lifecycles, race conditions, fee-adjusted routing, and queue depletion.
- [[01 - Market & Microstructure Fundamentals/War Story - The 2010 Flash Crash]] — Deep-dive forensic breakdown of the May 6, 2010 Flash Crash: Waddell & Reed algorithm cascades, cross-market E-mini to equity contagion, and \$0.01 stub quotes.

## Canonical Sources
- [[Sources/Trading and Exchanges by Larry Harris]] — Canonical text on market structure and practitioner dynamics.
- [[Sources/The Microstructure of Financial Markets by Rama Cont and Sasha Stoikov]] — Foundational quantitative microstructure models.
- [[Sources/Flash Boys by Michael Lewis]] — Physical telecommunications race and market fragmentation.
