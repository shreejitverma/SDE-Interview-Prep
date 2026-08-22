---
tags: [trading/microstructure, type/moc]
aliases: [Microstructure MOC, Market Fundamentals MOC]
status: seed
module: 01
created: 2026-08-22
---

# MOC — 01 Market & Microstructure Fundamentals

Execution mechanics, microstructure models, structural market fragmentation, and the regulatory fabric governing trade execution.

---

## Core Concepts
- [[Notes/Order Types and State Transitions]] — Limit, Market, Stop, Pegged, IOC, FOK, Post-Only, and native exchange states.
- [[Notes/Continuous Trading vs Discrete Auctions]] — Uncrossing algorithms, continuous double auctions, opening/closing crosses, volatility halts.
- [[Notes/Maker-Taker vs Inverted Fee Models]] — Economic incentives, adverse selection, queue priority dynamics, fee-adjusted routing.
- [[Notes/Price Discovery and Microstructure Noise]] — Information arrivals, bid-ask spread decomposition (order handling, inventory risk, adverse selection).
- [[Notes/Market Fragmentation and Reg NMS]] — Rule 611 (Order Protection), Rule 610 (Access Fees), SIP vs. Direct Market Data Feeds, synthetic NBBO.
- [[Notes/European Market Structure and MiFID II]] — RTS 25 clock synchronization, dark pool caps, Systematic Internalisers, tick size regimes.
- [[Notes/Order Book Dynamics and Queue Position]] — Queue depletion models, cancel ratios, top-of-book replenishment probability.

## Labs & Implementations
- [[Labs/Lab - 01 Continuous Double Auction Simulator]] — Build an allocation-free discrete event order book uncrossing engine.

## Drills & War Stories
- [[Drills/Drill - 01 Microstructure and Order Matching Mechanics]] — Rapid-fire calibration on order lifecycles and regulatory routing constraints.
- [[Notes/War Story - The 2010 Flash Crash and Stub Quotes]] — Breakdown of broken queue dynamics, stub quotes, and liquidity vacuums.

## Canonical Sources
- [[Sources/Trading and Exchanges by Larry Harris]] — Canonical text on market structure and practitioner dynamics.
- [[Sources/Empirical Market Microstructure by Joel Hasbrouck]] — Statistical foundations of microstructure noise and price discovery.
