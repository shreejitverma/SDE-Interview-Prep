# First Principles Engineering

Software engineering, rebuilt from the ground up: from CPU caches and syscalls to consensus protocols, matching engines, and agent systems.
Every topic is learned by deriving it, implementing it, and measuring it, not by memorizing answers.

This repository is part curriculum, part reference vault, and part runnable code.
It covers the path from junior engineer to distinguished engineer, with deep tracks for quantitative development, ultra-low-latency trading systems, and agentic AI.
It has been built and maintained continuously since 2021.

**Author:** Shreejit Verma ([@shreejitverma](https://github.com/shreejitverma))
**License:** [MIT](./LICENSE)

> Formerly `SDE-Interview-Prep`.
> Interview preparation is still here, but it is now one application of the material rather than its purpose.
> Old GitHub links redirect automatically.

---

## Why "first principles"

Most engineering study material teaches answers.
This repository tries to teach the reasons the answers are true, so they can be rebuilt under new constraints.

- **Derive before you use.** A lock-free stack, a Raft log, and an LSM tree are small enough to write yourself; the implementations here are deliberately compact so the mechanism stays visible.
- **Go one layer down.** Latency questions end at the cache line, the TLB, and the NIC; distributed-systems questions end at the failure model; agent questions end at the transformer and the context window.
- **Measure, do not guess.** The performance and low-latency tracks lean on profilers, histograms, and coordinated-omission-aware measurement instead of folklore.
- **Read the canon.** The whitepaper archive points at the original papers (Lamport, Codd, Drepper, Gregg, Kyle, Avellaneda-Stoikov) rather than summaries of summaries.

---

## At a glance

| Layer | What is in it |
| :--- | :--- |
| Foundations | Operating systems, networks, DBMS, OOP, with runnable C++ for concurrency and sockets |
| Languages | Book-length "Zero to Godhood" guides for C++ (C++98 to C++23, with LaTeX and PDF builds) and Python, plus Java and JavaScript |
| Algorithms | About 6,800 solution files (3,500 C++, 3,300 Python) organized by pattern, plus Blind 75 and NeetCode 150 |
| System design | Concepts, LLD, HLD case studies, 80+ "Design X" implementations, pattern catalogs in C++, Python, and Java |
| Quant finance | Black-Scholes, Greeks, Monte Carlo, a C++ order book, an arena allocator, an event-driven backtester |
| Low-latency systems | A 148-note Obsidian vault on exchanges, matching engines, kernel bypass, FPGAs, and lock-free C++ |
| Agentic AI | A 14-volume curriculum from LLM internals to multi-agent systems, MCP, evals, and coding agents |
| Distinguished engineering | Lock-free stack, Raft, consistent hashing, LSM tree, write-ahead log, circuit breaker |
| Whitepapers | An annotated guide to about 159 papers: eBPF and performance, kernels, memory, networking, security, and the CS and HFT canon |
| Career | Behavioral and resume guides, mock interviews, and an Obsidian interview command center |

Roughly 15,600 tracked files.
Most of the value is in the code and the long-form notes, not in this README.

---

## Start here

1. Read the [Roadmap](./00-Start-Here/Roadmap.md) for how the tracks fit together.
2. Copy the [Checklist](./00-Start-Here/Checklist.md) and track progress against it.
3. Use [INDEX.md](./INDEX.md) as the phased table of contents.
4. Pick a track below and treat everything else as reference.

### Tracks

| Goal | Path |
| :--- | :--- |
| Strong general software engineer | `01` -> `03` -> `04` -> `06` |
| Quant developer or HFT C++ engineer | `02/C++` -> `03/04-Gold-Standard-Cpp-Patterns` -> `05` -> `14` -> `12` |
| Senior, staff, or distinguished engineer | `04` -> `08` -> `09` -> `10` -> `11` |
| AI and agent engineer | `13`, then `04` and `10` for the production side |
| Systems and performance engineer | `01` -> `12` -> `15` -> `14/04` to `14/08` |

---

## Repository map

```
00-Start-Here/                 Roadmap and progress checklist
01-CS-Foundations/             OS, networks, DBMS, OOP (with C++ concurrency and socket code)
02-Programming-Languages/      C++, Python, Java, JavaScript
03-Data-Structures-Algorithms/ Pattern-wise solutions, practice platforms, gold-standard C++ patterns
04-System-Design/              Concepts, LLD, HLD case studies, design patterns, reference PDFs
05-Quantitative-Finance/       Mathematics, quant dev (C++), algorithmic trading (Python)
06-Interview-Prep/             Behavioral, resume, mock interviews
07-Project-Portfolio/          Project ideas that demonstrate depth
08-Distinguished-Engineering/  Concurrency, distributed systems, database internals, architecture patterns
09-Engineering-Leadership/     Technical writing (RFC template), code review
10-Development-Practices/      Testing, CI/CD, cloud-native
11-Security-And-Cryptography/  Secure coding, OWASP Top 10
12-Performance-Engineering/    CPU architecture, profiling
13-Agentic-AI/                 Agentic AI: Zero to Godhood (14 volumes)
14-Low-Latency-Systems/        Low-latency trading systems vault (Obsidian)
15-Technical-Whitepapers/      Annotated whitepaper archive (performance, kernels, memory, security, canon)
16-Interview-Command-Center/   Obsidian mission control for an active job search
INDEX.md                       Phased table of contents with direct links
```

### 01 - CS Foundations

Operating systems, computer networks, DBMS, and object-oriented programming.
The concepts that get probed hardest come with working code.

- [Concurrency in C++](./01-CS-Foundations/Operating-Systems/Concurrency-Cpp): producer-consumer and synchronization primitives.
- [Socket programming in C++](./01-CS-Foundations/Computer-Networks/Socket-Programming-Cpp): a TCP server from `socket()` up.

### 02 - Programming Languages

The two flagship guides are written as full books.

- [C++ Zero to Godhood](./02-Programming-Languages/C++/CPP_Zero_to_Godhood): the complete evolution of the language, C++98 through C++23, with a mindmap, LaTeX source, and a compiled PDF.
- [Python Zero to Godhood](./02-Programming-Languages/Python/Complete-Python-Zero-to-Godhood.md): generators, decorators, async, and the quant stack.
- [C++ STL complete reference](./02-Programming-Languages/C++/stl_complete_reference.md): every container and algorithm with its complexity.
- [Ultimate C++ Advanced Guide](./02-Programming-Languages/C++/Ultimate-CPP-Advanced-Guide.md) and [Ultimate C++ Design Patterns](./02-Programming-Languages/C++/Ultimate-CPP-Design-Patterns.md).
- Blind 75 and NeetCode 150 solved in [C++](./02-Programming-Languages/C++/Blind-75-LeetCode-CPP.md) and [Python](./02-Programming-Languages/Python/Blind-75-LeetCode-Python.md).
- [Java](./02-Programming-Languages/Java), [JavaScript](./02-Programming-Languages/JavaScript).

### 03 - Data Structures and Algorithms

Solutions organized by pattern under [01-Topics](./03-Data-Structures-Algorithms/01-Topics): arrays, strings, linked lists, stacks and queues, trees, tries, graphs, heaps, hashing, DP, backtracking, greedy, divide and conquer, branch and bound, bit manipulation, geometry, and mathematics.

- [Gold-standard C++ patterns](./03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns): the reference implementations worth knowing cold (Dijkstra, union-find, 0/1 knapsack).
- [Blind 75 must-do LeetCode](./03-Data-Structures-Algorithms/01-Topics/Blind%2075%20Must%20Do%20Leetcode).
- [LeetCode](./03-Data-Structures-Algorithms/02-Practice-Platforms/LeetCode) solutions and guides.
- [Resources](./03-Data-Structures-Algorithms/03-Resources): e-books and cheat sheets.

### 04 - System Design

- [Concepts](./04-System-Design/00-Concepts): CAP, sharding, caching, load balancing.
- [LLD](./04-System-Design/01-LLD): SOLID principles and common problems.
- [HLD case studies](./04-System-Design/02-Case-Studies): URL shortener, rate limiter, real-time chat, distributed ID generator.
- [Most Asked Design Questions](./04-System-Design/Most%20Asked%20Design%20Questions): 80+ "Design X" problems (LRU cache, Twitter, skiplist, file system, web crawler) in C++ and Python.
- [Low Level Design](./04-System-Design/Low%20Level%20Design): distributed cache, distributed event bus, rate limiter, service orchestrator.
- Design pattern catalogs: [C++](./04-System-Design/Design%20Patterns), [Python](./04-System-Design/python-design-patterns), [Java](./04-System-Design/design-patterns-java).
- Reference material from ByteByteGo, InterviewReady, and Arpit Bhayani, plus a [top-20 questions list](./04-System-Design/top-20-questions.md).

### 05 - Quantitative Finance

- [Option pricing](./05-Quantitative-Finance/01-Mathematics/Option-Pricing): Black-Scholes and the Greeks.
- [Monte Carlo](./05-Quantitative-Finance/01-Mathematics/Monte-Carlo): option pricing by simulation.
- [Order book in C++](./05-Quantitative-Finance/02-Quant-Dev/Order-Book-Cpp) and an [arena memory pool](./05-Quantitative-Finance/02-Quant-Dev/Memory-Management).
- [Event-driven backtester](./05-Quantitative-Finance/03-Algo-Trading/Backtesting) and [strategies](./05-Quantitative-Finance/03-Algo-Trading/Strategies) (mean reversion, Bollinger bands).

### 08 - Distinguished Engineering

Small, complete implementations of the systems that senior design discussions go deep on.

| Topic | File |
| :--- | :--- |
| Lock-free stack | [lock_free_stack.cpp](./08-Distinguished-Engineering/01-Advanced-Concurrency/lock_free_stack.cpp) |
| Raft consensus | [raft_consensus.py](./08-Distinguished-Engineering/02-Distributed-Systems-Internals/raft_consensus.py) |
| Consistent hashing | [consistent_hashing.py](./08-Distinguished-Engineering/02-Distributed-Systems-Internals/consistent_hashing.py) |
| LSM tree | [lsm_tree.cpp](./08-Distinguished-Engineering/03-Database-Internals/lsm_tree.cpp) |
| Write-ahead log | [wal.cpp](./08-Distinguished-Engineering/03-Database-Internals/wal.cpp) |
| Circuit breaker | [circuit_breaker.py](./08-Distinguished-Engineering/04-Architecture-Patterns/circuit_breaker.py) |

Related, in `12`: [false sharing](./12-Performance-Engineering/01-Cpu-Architecture/false_sharing.cpp) and a [memory leak demo](./12-Performance-Engineering/02-Profiling/memory_leak_demo.cpp) for profiler practice.

### 13 - Agentic AI: Zero to Godhood

A [14-volume curriculum](./13-Agentic-AI/Agentic_AI_Zero_to_Godhood) ordered as a dependency graph: LLM foundations, working with LLMs, tool use and the agent loop, agent architectures, RAG, memory and context engineering, multi-agent systems, frameworks and SDKs, Model Context Protocol, evaluation and observability, safety and security, production engineering, coding agents and computer use, and frontier capstones.
Appendices include a glossary, the paper canon, a benchmark index, interview drills, and a pattern library.

### 14 - Low-Latency Systems

An Obsidian vault covering the full stack of ultra-low-latency electronic trading.
Open the folder as a vault and start at [00 Home.md](./14-Low-Latency-Systems/00%20Home.md).

It covers market microstructure, exchange architecture, matching engine internals, mechanical sympathy (caches, MESI, NUMA, TLB), OS and kernel tuning (`isolcpus`, `nohz_full`, IRQ affinity), kernel-bypass networking (Onload, DPDK), time and measurement (PTP, `rdtsc`, HDR histograms, coordinated omission), low-latency C++ (memory model, SPSC and MPMC rings, allocation-free hot paths), messaging and IPC (Disruptor, Aeron), protocols and codecs (ITCH, OUCH, MDP3/SBE, FIX), tick-to-trade pipelines, FPGAs, reliability and testing, and an industry map with the canonical papers and talks.

It includes a [12-week production calibration roadmap](./14-Low-Latency-Systems/Roadmap%20-%2012-Week%20Production%20Calibration.md) and an [interview question bank with answers](./14-Low-Latency-Systems/Interview).

### 15 - Technical Whitepapers

An [annotated guide to about 159 technical whitepapers](./15-Technical-Whitepapers) for systems programmers, performance engineers, and security researchers.
The source archive was scanned with ClamAV and came back clean; see the [audit report](./15-Technical-Whitepapers/ClamAV-Audit-Report.md).

- [Systems performance and eBPF](./15-Technical-Whitepapers/01-Systems-Performance-and-Tracing): the Brendan Gregg canon (BPF, the USE method, flame graphs, off-CPU analysis) and syscall tracing overhead.
- [Operating systems and kernels](./15-Technical-Whitepapers/02-Operating-Systems-and-Kernels): Ritchie and Thompson's UNIX paper, xv6, OS from scratch, the Linux scheduler "wasted cores" study, and Windows NT internals.
- [Memory architecture and concurrency](./15-Technical-Whitepapers/03-Memory-Architecture-and-Concurrency): Drepper's "What Every Programmer Should Know About Memory", the threads-versus-events debate, and memory effects in database joins.
- [Networking and protocols](./15-Technical-Whitepapers/04-Networking-and-Protocols): TCP Fast Open, Van Jacobson's netchannels, tcpdump and tcptrace analysis, DDoS mitigation.
- [Offensive security and exploitation](./15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation): reverse engineering, stack overflows, PE infection, advanced SQL injection, Same-Origin Policy bypasses.
- [Defensive security and hardening](./15-Technical-Whitepapers/06-Defensive-Security-and-Hardening): Linux hardening, container and sandbox security, the OWASP ASVS.
- [Polish technical papers](./15-Technical-Whitepapers/07-Polish-Technical-Papers-pl): 31 papers on ELF analysis, kernel rootkits, web security, and SELinux.
- [Developer tooling](./15-Technical-Whitepapers/08-Developer-Tooling-and-Foundations): The AWK Programming Language and Vim.
- [Seminal computer science papers](./15-Technical-Whitepapers/09-Seminal-Computer-Science-Papers): Turing, Shannon, Lamport, Paxos, Raft, FLP, GFS, MapReduce, Bigtable, Dynamo, Spark, Codd, ARIES, Cerf-Kahn, end-to-end arguments.
- [Seminal low-latency systems papers](./15-Technical-Whitepapers/10-Seminal-Low-Latency-Systems-Papers): C++ memory models, lock-free and wait-free algorithms, the LMAX Disruptor, RCU, kernel bypass (Netmap, IX, RAMCloud), and market microstructure (Kyle, Glosten-Milgrom, order flow imbalance, micro-price, Avellaneda-Stoikov).

### 16 - Interview Command Center

An Obsidian vault for running a job search like an engineering project.
Start at the [dashboard](./16-Interview-Command-Center/00-Dashboard.md).
It holds role hubs (SDE, quant dev, quant research, AI engineer, low latency), 25+ company profiles, a pipeline tracker by stage, a STAR story bank, retrospectives, and a daily practice log.
The [Gmail sync guide](./16-Interview-Command-Center/03-Pipeline/Gmail-Sync-Guide.md) documents the scripts that keep the pipeline current.

---

## Running the code

There is no single build system; each example is self-contained.

```bash
# Any single-file C++ example
g++ -std=c++20 -O2 -Wall -Wextra -pthread \
  08-Distinguished-Engineering/01-Advanced-Concurrency/lock_free_stack.cpp -o lock_free_stack && ./lock_free_stack

# Any single-file Python example
python3 05-Quantitative-Finance/01-Mathematics/Option-Pricing/black_scholes.py

# C++ design patterns (CMake project)
cmake -S "04-System-Design/Design Patterns" -B build && cmake --build build

# Python design patterns (own Makefile and tests)
cd 04-System-Design/python-design-patterns && make
```

The Java projects under `04-System-Design` use Maven (`mvn test`).
The Bollinger band strategy needs `matplotlib`.
The `13-Agentic-AI`, `14-Low-Latency-Systems`, and `16-Interview-Command-Center` tracks are notes, not code, and read best in Obsidian.

---

## How to use this well

- Do not read linearly. Pick a track, follow it, and treat the rest as reference.
- Implement before you read. Write the gold-standard patterns, the order book, and the memory pool from memory, then diff against the repository.
- For system design, write your own solution to a case study before reading the one here.
- For performance claims, build with `-O2`, measure, and only then explain.
- The [Checklist](./00-Start-Here/Checklist.md) is a phased plan. It works if you actually tick the boxes.

---

## Contributing

Issues and pull requests are welcome, especially corrections to solutions, additional language ports, and new case studies.
Keep the numbered directory layout, keep code self-contained and compiling cleanly with `-Wall -Wextra`, and follow the [Code of Conduct](./CODE_OF_CONDUCT.md).

Large binary references (PDFs) are kept for convenience; please do not add more without a strong reason.

## License

[MIT](./LICENSE). Copyright (c) 2021 Shreejit Verma.
