# First Principles Engineering: Index

A phased table of contents with direct links.
Each phase builds on the ones before it; the [Roadmap](./00-Start-Here/Roadmap.md) explains why the order is what it is.

**Author:** Shreejit Verma

## Phase 0: Orientation
- [README](./README.md): what is here and how the tracks fit together.
- [Roadmap](./00-Start-Here/Roadmap.md): the dependency order between tracks.
- [Checklist](./00-Start-Here/Checklist.md): a phased progress tracker.

## Phase 1: Foundations
- **CS basics**
  - [Concurrency in C++](./01-CS-Foundations/Operating-Systems/Concurrency-Cpp): producer-consumer.
  - [Socket programming in C++](./01-CS-Foundations/Computer-Networks/Socket-Programming-Cpp): a TCP server.
  - [DBMS](./01-CS-Foundations/DBMS) and [object-oriented programming](./01-CS-Foundations/Object-Oriented-Programming).
- **Languages**
  - [C++](./02-Programming-Languages/C++): [C++ Zero to Godhood](./02-Programming-Languages/C++/CPP_Zero_to_Godhood), the [STL reference](./02-Programming-Languages/C++/stl_complete_reference.md), the [advanced guide](./02-Programming-Languages/C++/Ultimate-CPP-Advanced-Guide.md).
  - [Python](./02-Programming-Languages/Python): [Python Zero to Godhood](./02-Programming-Languages/Python/Complete-Python-Zero-to-Godhood.md).
  - [Java](./02-Programming-Languages/Java): collections and advanced concurrency.
  - [JavaScript](./02-Programming-Languages/JavaScript).

## Phase 2: Core Engineering
- **Data structures and algorithms**
  - [Gold-standard C++ patterns](./03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns): Dijkstra, union-find, knapsack.
  - [Topics by pattern](./03-Data-Structures-Algorithms/01-Topics) and [practice platforms](./03-Data-Structures-Algorithms/02-Practice-Platforms).
- **System design**
  - [Concepts](./04-System-Design/00-Concepts): CAP, sharding, caching.
  - [LLD](./04-System-Design/01-LLD): SOLID and common problems.
  - [Case studies](./04-System-Design/02-Case-Studies): URL shortener, rate limiter, chat.
  - [Design patterns](./04-System-Design/03-Design-Patterns): creational and behavioral.
  - [Most asked design questions](./04-System-Design/Most%20Asked%20Design%20Questions): 80+ implementations in C++ and Python.

## Phase 3: Quantitative Finance
- [Mathematics](./05-Quantitative-Finance/01-Mathematics): Black-Scholes, Monte Carlo, Greeks.
- [Quant dev](./05-Quantitative-Finance/02-Quant-Dev): order book (C++), arena memory pool.
- [Algorithmic trading](./05-Quantitative-Finance/03-Algo-Trading): event-driven backtester, mean reversion, Bollinger bands.

## Phase 4: Production Engineering
- [Development practices](./10-Development-Practices): unit testing, CI/CD, Docker.
- [Security](./11-Security-And-Cryptography): secure C++, OWASP Top 10.
- [Performance](./12-Performance-Engineering): false sharing, profiling.
- [Technical whitepapers](./15-Technical-Whitepapers): performance (Gregg), kernels, memory (Drepper), exploitation and hardening, and the seminal CS and low-latency canon (about 159 papers).

## Phase 5: Distinguished Engineering
- [Advanced concurrency](./08-Distinguished-Engineering/01-Advanced-Concurrency): lock-free stack.
- [Distributed systems internals](./08-Distinguished-Engineering/02-Distributed-Systems-Internals): Raft, consistent hashing.
- [Database internals](./08-Distinguished-Engineering/03-Database-Internals): LSM tree, write-ahead log.
- [Architecture patterns](./08-Distinguished-Engineering/04-Architecture-Patterns): circuit breaker.
- [Engineering leadership](./09-Engineering-Leadership): RFC template, code review checklist.

## Phase 6: Low-Latency Systems
- [Low-latency systems vault](./14-Low-Latency-Systems/00%20Home.md): microstructure, exchanges, matching engines, mechanical sympathy, kernel bypass, measurement, FPGAs.
- [12-week production calibration roadmap](./14-Low-Latency-Systems/Roadmap%20-%2012-Week%20Production%20Calibration.md).
- [Interview question bank](./14-Low-Latency-Systems/Interview).

## Phase 7: Agentic AI
- [Agentic AI: Zero to Godhood](./13-Agentic-AI/Agentic_AI_Zero_to_Godhood): fourteen volumes from transformer internals to production agent systems.
  - Foundations: LLM internals, inference mechanics, working with model APIs.
  - Core craft: the agent loop from scratch, architectures, RAG, context engineering.
  - Systems: multi-agent orchestration, frameworks and SDKs, Model Context Protocol.
  - Rigor: evaluation and observability, safety and security, production engineering.
  - Frontier: coding agents, computer use, RL for agents, capstone projects.

## Phase 8: Career
- [Interview prep](./06-Interview-Prep): [STAR method](./06-Interview-Prep/01-Behavioral/star_method.md), [resume guide](./06-Interview-Prep/02-Resume/resume_guide.md), [mock interview transcript](./06-Interview-Prep/03-Mock-Interviews/transcript.md).
- [Project portfolio](./07-Project-Portfolio/README.md): projects that demonstrate depth.
- [Interview Command Center](./16-Interview-Command-Center/00-Dashboard.md): mission control for an active search.
  - [Coaching prompt](./16-Interview-Command-Center/Coaching-Prompt.md): AI coaching system prompt for all five roles.
  - Role hubs: [SDE](./16-Interview-Command-Center/01-Roles/SDE/_Hub.md), [Quant Dev](./16-Interview-Command-Center/01-Roles/Quant-Dev/_Hub.md), [Quant Research](./16-Interview-Command-Center/01-Roles/Quant-Research/_Hub.md), [AI Engineer](./16-Interview-Command-Center/01-Roles/AI-Engineer/_Hub.md), [Low Latency](./16-Interview-Command-Center/01-Roles/Low-Latency/_Hub.md).
  - [Company index](./16-Interview-Command-Center/02-Companies/_Company-Index.md): 25+ company profiles.
  - [Pipeline](./16-Interview-Command-Center/03-Pipeline/_Pipeline-Dashboard.md): tracking by stage, from applied to offer.
  - [Retrospectives](./16-Interview-Command-Center/04-Retrospectives/_Retro-Dashboard.md): post-interview analysis.
  - [Behavioral](./16-Interview-Command-Center/05-Behavioral/_Story-Index.md): STAR story bank.
  - [Daily log](./16-Interview-Command-Center/06-Daily-Log/_Daily-Log-Hub.md): practice journal and weekly reviews.
