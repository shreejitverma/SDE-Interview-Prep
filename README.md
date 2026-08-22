# SDE Interview Prep

A single, opinionated knowledge base for going from junior engineer to distinguished engineer, with dedicated tracks for quantitative development, ultra-low-latency trading systems, and agentic AI.
It is part curriculum, part reference vault, part runnable code.
It has been built and maintained continuously since 2021.

**Author:** Shreejit Verma ([@shreejitverma](https://github.com/shreejitverma))
**License:** MIT

---

## What is in here

| Layer | Content |
| :--- | :--- |
| Foundations | OS, networks, DBMS, OOP, with runnable C++ and Python |
| Languages | Deep "Zero to Godhood" guides for C++ and Python, plus Java, Go, Rust, JavaScript |
| DSA | ~6,800 solved problems in C++ and Python, organized by pattern, plus Blind 75 and NeetCode 150 |
| System design | HLD case studies, LLD problems, 80+ "design a ..." implementations, design pattern catalogs in C++, Python, Java |
| Quant finance | Black-Scholes, Greeks, Monte Carlo, order book in C++, memory pools, event-driven backtester |
| Low-latency systems | 129-note Obsidian vault on exchange architecture, matching engines, kernel bypass, FPGAs, lock-free C++ |
| Agentic AI | 14-volume curriculum from LLM foundations to multi-agent systems, MCP, evals, and coding agents |
| Distinguished engineering | Lock-free stack, Raft, consistent hashing, LSM tree, WAL, circuit breaker |
| Interview prep | Behavioral, resume, mock interview checklists, and a phased roadmap |

Roughly 15,000 tracked files. Most of the value is in the code and the long-form notes, not in this README.

---

## Start here

1. Read [00-Start-Here/Roadmap.md](./00-Start-Here/Roadmap.md) for the overall path.
2. Copy [00-Start-Here/Checklist.md](./00-Start-Here/Checklist.md) and track your progress against it.
3. Use [INDEX.md](./INDEX.md) as the phased table of contents.
4. Pick a track below based on your target role.

### If you are targeting...

- **General SDE (FAANG-style):** `01` -> `03` -> `04` -> `06`.
- **Quant developer / HFT C++:** `02/C++` -> `03/04-Gold-Standard-Cpp-Patterns` -> `05` -> `14` -> `12`.
- **Senior / staff / distinguished:** `04` -> `08` -> `09` -> `10` -> `11`.
- **AI / agent engineering:** `13`, then `04` for the production side.

---

## Repository map

```
00-Start-Here/                 Roadmap and progress checklist
01-CS-Foundations/             OS, Networks, DBMS, OOP (with C++ concurrency and socket code)
02-Programming-Languages/      C++, Python, Java, Go, Rust, JavaScript
03-Data-Structures-Algorithms/ Topic-wise solutions, LeetCode, gold-standard C++ patterns
04-System-Design/              Concepts, LLD, HLD case studies, design patterns, reference PDFs
05-Quantitative-Finance/       Mathematics, quant dev (C++), algo trading (Python)
06-Interview-Prep/             Behavioral, resume, mock interviews
07-Project-Portfolio/          Portfolio guidance
08-Distinguished-Engineering/  Concurrency, distributed systems, DB internals, architecture
09-Engineering-Leadership/     Technical writing, mentorship, code review
10-Development-Practices/      Testing, CI/CD, cloud-native
11-Security-And-Cryptography/  Secure coding, common vulnerabilities
12-Performance-Engineering/    CPU architecture, profiling
13-Agentic-AI/                 Agentic AI: Zero to Godhood (14 volumes)
14-Low-Latency-Systems/        Low-latency trading systems vault (Obsidian)
INDEX.md                       Phased table of contents with direct links
```

### 01 - CS Foundations

Operating systems, computer networks, DBMS, and object-oriented programming.
Not just notes: there is working code for the concepts that interviewers actually probe.

- [Concurrency in C++](./01-CS-Foundations/Operating-Systems/Concurrency-Cpp): producer-consumer and synchronization primitives.
- [Socket programming in C++](./01-CS-Foundations/Computer-Networks/Socket-Programming-Cpp): a TCP server from `socket()` up.

### 02 - Programming Languages

The two flagship guides are written as full books, with LaTeX and PDF builds checked in.

- [C++ Zero to Godhood](./02-Programming-Languages/C++/CPP_Zero_to_Godhood): the complete evolution of the language, C++98 through C++23, with a mindmap and a compiled PDF.
- [Python Zero to Godhood](./02-Programming-Languages/Python/Complete-Python-Zero-to-Godhood.md): generators, decorators, async, quant libraries.
- [C++ STL complete reference](./02-Programming-Languages/C++/stl_complete_reference.md): every container, algorithm, and complexity in one table.
- [Ultimate C++ Advanced Guide](./02-Programming-Languages/C++/Ultimate-CPP-Advanced-Guide.md) and [Ultimate C++ Design Patterns](./02-Programming-Languages/C++/Ultimate-CPP-Design-Patterns.md).
- Blind 75 and NeetCode 150 solved in [C++](./02-Programming-Languages/C++/Blind-75-LeetCode-CPP.md) and [Python](./02-Programming-Languages/Python/Blind-75-LeetCode-Python.md).
- [Java](./02-Programming-Languages/Java), [Go](./02-Programming-Languages/Go), [Rust](./02-Programming-Languages/Rust), [JavaScript](./02-Programming-Languages/JavaScript).

### 03 - Data Structures and Algorithms

About 3,500 C++ and 3,300 Python files, organized by pattern under [01-Topics](./03-Data-Structures-Algorithms/01-Topics): arrays, strings, linked lists, stacks and queues, trees, tries, graphs, heaps, hashing, DP, backtracking, greedy, divide and conquer, branch and bound, bit manipulation, geometry, mathematics.

- [Gold-standard C++ patterns](./03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns): the reference implementations to memorize (Dijkstra, union-find, 0/1 knapsack).
- [Blind 75 must-do LeetCode](./03-Data-Structures-Algorithms/01-Topics/Blind%2075%20Must%20Do%20Leetcode).
- [LeetCode](./03-Data-Structures-Algorithms/02-Practice-Platforms/LeetCode) solutions and guides.
- [Resources](./03-Data-Structures-Algorithms/03-Resources): e-books and cheat sheets.

### 04 - System Design

- [Concepts](./04-System-Design/00-Concepts): CAP, sharding, caching, load balancing.
- [LLD](./04-System-Design/01-LLD): SOLID principles and common problems.
- [HLD case studies](./04-System-Design/02-Case-Studies): URL shortener, rate limiter, real-time chat, distributed ID generator.
- [Most Asked Design Questions](./04-System-Design/Most%20Asked%20Design%20Questions): 80+ "Design X" problems (LRU cache, Twitter, skiplist, file system, underground system, web crawler) in both C++ and Python.
- [Low Level Design](./04-System-Design/Low%20Level%20Design): distributed cache, distributed event bus, rate limiter, service orchestrator.
- Design pattern catalogs: [C++](./04-System-Design/Design%20Patterns), [Python](./04-System-Design/python-design-patterns), [Java](./04-System-Design/design-patterns-java).
- Reference material from ByteByteGo, InterviewReady, and Arpit Bhayani, plus the top-20 questions list.

### 05 - Quantitative Finance

- [Option pricing](./05-Quantitative-Finance/01-Mathematics/Option-Pricing): Black-Scholes and the Greeks.
- [Monte Carlo](./05-Quantitative-Finance/01-Mathematics/Monte-Carlo): option pricing by simulation.
- [Order book in C++](./05-Quantitative-Finance/02-Quant-Dev/Order-Book-Cpp) and a [memory pool](./05-Quantitative-Finance/02-Quant-Dev/Memory-Management).
- [Event-driven backtester](./05-Quantitative-Finance/03-Algo-Trading/Backtesting) and [strategies](./05-Quantitative-Finance/03-Algo-Trading/Strategies) (mean reversion, Bollinger bands).

### 08 - Distinguished Engineering

Small, complete implementations of the things senior interviews go deep on.

| Topic | File |
| :--- | :--- |
| Lock-free stack | [lock_free_stack.cpp](./08-Distinguished-Engineering/01-Advanced-Concurrency/lock_free_stack.cpp) |
| Raft consensus | [raft_consensus.py](./08-Distinguished-Engineering/02-Distributed-Systems-Internals/raft_consensus.py) |
| Consistent hashing | [consistent_hashing.py](./08-Distinguished-Engineering/02-Distributed-Systems-Internals/consistent_hashing.py) |
| LSM tree | [lsm_tree.cpp](./08-Distinguished-Engineering/03-Database-Internals/lsm_tree.cpp) |
| Write-ahead log | [wal.cpp](./08-Distinguished-Engineering/03-Database-Internals/wal.cpp) |
| Circuit breaker | [circuit_breaker.py](./08-Distinguished-Engineering/04-Architecture-Patterns/circuit_breaker.py) |

Related: [false sharing](./12-Performance-Engineering/01-Cpu-Architecture/false_sharing.cpp) and a [memory leak demo](./12-Performance-Engineering/02-Profiling/memory_leak_demo.cpp) for profiling practice in `12`.

### 13 - Agentic AI: Zero to Godhood

A [14-volume curriculum](./13-Agentic-AI/Agentic_AI_Zero_to_Godhood) ordered as a dependency graph: LLM foundations, working with LLMs, tool use and the agent loop, agent architectures, RAG, memory and context engineering, multi-agent systems, frameworks and SDKs, Model Context Protocol, evaluation and observability, safety and security, production engineering, coding agents and computer use, and frontier capstones.
Appendices include a glossary, paper list, benchmark index, interview drills, and a pattern library.

### 14 - Low-Latency Systems

An Obsidian vault (open the folder as a vault; start at [00 Home.md](./14-Low-Latency-Systems/00%20Home.md)) covering the full stack of ultra-low-latency electronic trading:

market microstructure, exchange architecture, matching engine internals, hardware mechanical sympathy (caches, MESI, NUMA, TLB), OS and kernel tuning (`isolcpus`, `nohz_full`, IRQ affinity), kernel-bypass networking (Onload, DPDK), time and measurement (PTP, `rdtsc`, HDR histograms, coordinated omission), low-latency C++ (memory model, SPSC/MPMC rings, allocation-free loops), messaging and IPC (Disruptor, Aeron), protocols and codecs (ITCH, OUCH, MDP3/SBE, FIX), participant-side tick-to-trade pipelines, FPGAs, reliability and testing, and an industry map with canonical papers and talks.

Includes a [12-week production calibration roadmap](./14-Low-Latency-Systems/Roadmap%20-%2012-Week%20Production%20Calibration.md) and an [interview question bank with answers](./14-Low-Latency-Systems/Interview).

---

## Running the code

There is no single build system; each piece is self-contained.

```bash
# Any single-file C++ example
g++ -std=c++20 -O2 -Wall -Wextra -pthread \
  08-Distinguished-Engineering/01-Advanced-Concurrency/lock_free_stack.cpp -o lock_free_stack && ./lock_free_stack

# Any single-file Python example
python3 05-Quantitative-Finance/01-Mathematics/Option-Pricing/black_scholes.py

# C++ design patterns (CMake project)
cmake -S "04-System-Design/Design Patterns" -B build && cmake --build build

# Python design patterns (has its own Makefile and tests)
cd 04-System-Design/python-design-patterns && make
```

The Java projects under `04-System-Design` use Maven (`mvn test`).
The `14-Low-Latency-Systems` and `13-Agentic-AI` tracks are notes, not code, and read best in Obsidian.

---

## How to use this effectively

- Do not read linearly. Pick a target role, follow the track above, and treat everything else as reference.
- For DSA, implement the gold-standard patterns from memory before opening the topic folders.
- For system design, write your own solution to a case study before reading the one here.
- For quant and low-latency, build the order book and memory pool yourself, then diff against the repo.
- The [Checklist](./00-Start-Here/Checklist.md) is a five-phase, ten-week plan. It works if you actually tick the boxes.

---

## Contributing

Issues and pull requests are welcome, especially corrections to solutions, additional language ports, and new case studies.
Keep the existing numbered directory layout, keep code self-contained and compilable, and see [CODE_OF_CONDUCT.md](./CODE_OF_CONDUCT.md).

Large binary references (PDFs) are kept for convenience; please do not add more without a strong reason.

## License

[MIT](./LICENSE). Copyright (c) 2021 Shreejit Verma.
