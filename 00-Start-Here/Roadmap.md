# Roadmap

Author: Shreejit Verma
GitHub: https://github.com/shreejitverma

## How to use this repository

First Principles Engineering is organized as a dependency graph, not a reading list.
Each track assumes the ones beneath it, so gaps show up as confusion higher up.
When something higher up stops making sense, drop one layer and rebuild it there.

```
Foundations (01, 02)
  -> Core engineering (03, 04)
       -> Production engineering (10, 11, 12, 15)
            -> Distinguished engineering (08, 09)
       -> Quantitative finance (05) -> Low-latency systems (14)
       -> Agentic AI (13)
Career (06, 07, 16) draws on all of the above.
```

## Tracks

### 01 - CS Foundations
The bedrock that everything else reduces to.
- **Operating systems:** processes, threads, synchronization, memory.
- **Networks:** TCP/IP, HTTP, sockets.
- **DBMS:** SQL, indexing, transactions.
- **OOP:** design principles and SOLID.

### 02 - Programming Languages
Language internals, not just syntax.
- **C++:** the language of HFT and low-latency systems; start with C++ Zero to Godhood.
- **Python:** the language of data science and quant research.
- **Java, Go, Rust, JavaScript:** for general systems and application engineering.

### 03 - Data Structures and Algorithms
Problem solving by pattern.
- **01-Topics:** solutions organized by pattern (arrays, graphs, DP, and more).
- **02-Practice-Platforms:** LeetCode solutions and guides.
- **03-Resources:** e-books and cheat sheets.
- **04-Gold-Standard-Cpp-Patterns:** the implementations to know cold.

### 04 - System Design
Architecture at scale.
- **LLD:** class design, schema design, design patterns.
- **HLD:** distributed systems, scalability, case studies.

### 05 - Quantitative Finance
The specialized pathway for quant roles.
- **Mathematics:** probability, stochastic calculus, option pricing.
- **Quant dev:** order books and allocation-free C++.
- **Algorithmic trading:** strategies and backtesting.

### 08 and 09 - Distinguished Engineering and Leadership
Build the systems senior engineers are expected to reason about: lock-free structures, consensus, storage engines, resilience patterns.
Then learn to communicate designs through RFCs and code review.

### 10, 11, 12, 15 - Production Engineering
Testing, CI/CD, secure coding, and performance measurement, backed by the primary literature in the whitepaper archive.

### 13 - Agentic AI
Fourteen volumes from transformer internals to production agent systems.

### 14 - Low-Latency Systems
The full stack of electronic trading, from market microstructure to kernel bypass and FPGAs.

### 06, 07, 16 - Career
Behavioral and resume guides, portfolio projects, and the Interview Command Center for running an active search.
