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
| Languages | Deep "Zero to Godhood" guides for C++ and Python, plus Java and JavaScript |
| DSA | ~6,800 solved problems in C++ and Python, organized by pattern, plus Blind 75 and NeetCode 150 |
| System design | HLD case studies, LLD problems, 80+ "design a ..." implementations, design pattern catalogs in C++, Python, Java |
| Quant finance | Black-Scholes, Greeks, Monte Carlo, order book in C++, memory pools, event-driven backtester |
| Low-latency systems | 129-note Obsidian vault on exchange architecture, matching engines, kernel bypass, FPGAs, lock-free C++ |
| Agentic AI | 14-volume curriculum from LLM foundations to multi-agent systems, MCP, evals, and coding agents |
| Distinguished engineering | Lock-free stack, Raft, consistent hashing, LSM tree, WAL, circuit breaker |
| Technical whitepapers | Reading notes on ~159 papers (Linux eBPF, memory hierarchies, Windows NT, exploitation, hardening, seminal CS and low-latency/HFT canon), linked to public copies |
| Interview prep | Behavioral, resume, mock interview checklists, and a phased roadmap |
| Interview Command Center | Obsidian mission control: 5 role hubs with skill matrices and question banks, templates, QuickAdd macros, and dashboards; the private application pipeline is kept outside this repo |


Roughly 15,000 tracked files. Most of the value is in the code and the long-form notes, not in this README.
Every knowledge note carries typed frontmatter (type, track, level, status), and every folder has an entry note, so the vault can be browsed from any folder or queried with Dataview.

---

## Start here

1. Read [00-Start-Here/Roadmap.md](./00-Start-Here/Roadmap.md) for the overall path.
2. Copy [00-Start-Here/Checklist.md](./00-Start-Here/Checklist.md) and track your progress against it.
3. Use [INDEX.md](./INDEX.md) as the phased table of contents.
4. Pick a track below based on your target role.
5. Work the [Review queue](./00-Start-Here/Review-Queue.md): notes come due by status and last review date.

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
02-Programming-Languages/      C++, Python, Java, JavaScript
03-Data-Structures-Algorithms/ Topic-wise solutions, LeetCode, gold-standard C++ patterns
04-System-Design/              Concepts, LLD, HLD case studies, design patterns
05-Quantitative-Finance/       Mathematics, quant dev (C++), algo trading (Python)
06-Interview-Prep/             Behavioral, resume, mock interviews
07-Project-Portfolio/          Portfolio project ideas (README.md)
08-Distinguished-Engineering/  Concurrency, distributed systems, DB internals, architecture
09-Engineering-Leadership/     Technical writing, mentorship, code review
10-Development-Practices/      Testing, CI/CD, cloud-native
11-Security-And-Cryptography/  Secure coding, common vulnerabilities
12-Performance-Engineering/    CPU architecture, profiling
13-Agentic-AI/                 Agentic AI: Zero to Godhood (14 volumes)
14-Low-Latency-Systems/        Low-latency trading systems vault (Obsidian)
15-Technical-Whitepapers/      Technical whitepapers archive (eBPF, Memory, Kernels, Exploitation, Hardening)
16-Interview-Command-Center/   Obsidian mission control: role hubs, templates, macros, dashboards (private data symlinked in)
INDEX.md                       Phased table of contents with direct links
CS-Subjects/                   Legacy index that now points into 01-CS-Foundations
tools/                         Vault audit, link repair, and guard scripts (see tools/README.md)

```

### 01 - CS Foundations

Operating systems, computer networks, DBMS, and object-oriented programming.
Not just notes: there is working code for the concepts that interviewers actually probe.

- [Concurrency in C++](./01-CS-Foundations/Operating-Systems/Concurrency-Cpp): producer-consumer and synchronization primitives.
- [Socket programming in C++](./01-CS-Foundations/Computer-Networks/Socket-Programming-Cpp): a TCP server from `socket()` up.

### 02 - Programming Languages

The two flagship guides are written as full books, with LaTeX and PDF builds checked in.

- [C++ Zero to Godhood](./02-Programming-Languages/C++/CPP_Zero_to_Godhood): the complete evolution of the language, C++98 through C++26, with a mindmap and a compiled PDF.
- [Python Zero to Godhood](./02-Programming-Languages/Python/Complete-Python-Zero-to-Godhood.md): generators, decorators, async, quant libraries.
- [C++ STL complete reference](./02-Programming-Languages/C++/stl_complete_reference.md): every container, algorithm, and complexity in one table.
- [Ultimate C++ Advanced Guide](./02-Programming-Languages/C++/Ultimate-CPP-Advanced-Guide.md) and [Ultimate C++ Design Patterns](./02-Programming-Languages/C++/Ultimate-CPP-Design-Patterns.md).
- Blind 75 and NeetCode 150 solved in [C++](./02-Programming-Languages/C++/Blind-75-LeetCode-CPP.md) and [Python](./02-Programming-Languages/Python/Blind-75-LeetCode-Python.md).
- [Java](./02-Programming-Languages/Java), [JavaScript](./02-Programming-Languages/JavaScript).

### 03 - Data Structures and Algorithms

About 3,500 C++ and 3,300 Python files, organized by pattern under [01-Topics](./03-Data-Structures-Algorithms/01-Topics): arrays, strings, linked lists, stacks and queues, trees, tries, graphs, heaps, hashing, DP, backtracking, greedy, divide and conquer, branch and bound, bit manipulation, geometry, mathematics.

- [Gold-standard C++ patterns](./03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns): the reference implementations to memorize (Dijkstra, union-find, 0/1 knapsack).
- [Blind 75 must-do LeetCode](./03-Data-Structures-Algorithms/01-Topics/Blind%2075%20Must%20Do%20Leetcode).
- [LeetCode](./03-Data-Structures-Algorithms/02-Practice-Platforms/LeetCode) solutions and guides.

### 04 - System Design

- [Concepts](./04-System-Design/00-Concepts): CAP, sharding, caching, load balancing.
- [LLD](./04-System-Design/01-LLD): SOLID principles and common problems.
- [HLD case studies](./04-System-Design/02-Case-Studies): URL shortener, rate limiter, real-time chat, distributed ID generator.
- [Most Asked Design Questions](./04-System-Design/Most%20Asked%20Design%20Questions): 80+ "Design X" problems (LRU cache, Twitter, skiplist, file system, underground system, web crawler) in both C++ and Python.
- [Low Level Design](./04-System-Design/Low%20Level%20Design): distributed cache, distributed event bus, rate limiter, service orchestrator.
- Design pattern catalogs: [C++](./04-System-Design/Design%20Patterns), [Python](./04-System-Design/Design%20Patterns/python-patterns), [Java](./04-System-Design/design-patterns-java).
- [InterviewReady](./04-System-Design/InterviewReady) reference material and the [top-20 questions list](./04-System-Design/top-20-questions.md).

### 05 - Quantitative Finance

- [Option pricing](./05-Quantitative-Finance/01-Mathematics/Option-Pricing): Black-Scholes and the Greeks.
- [Monte Carlo](./05-Quantitative-Finance/01-Mathematics/Monte-Carlo): option pricing by simulation.
- [Order book in C++](./05-Quantitative-Finance/02-Quant-Dev/Order-Book-Cpp) and a [memory pool](./05-Quantitative-Finance/02-Quant-Dev/Memory-Management).
- [Event-driven backtester](./05-Quantitative-Finance/03-Algo-Trading/Backtesting) and [strategies](./05-Quantitative-Finance/03-Algo-Trading/Strategies) (mean reversion, Bollinger bands).

### 08 - Distinguished Engineering

Small, complete implementations of the things senior interviews go deep on ([section index](./08-Distinguished-Engineering/README.md)).

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

### 15 - Technical Whitepapers

Reading notes and an index for [~159 technical papers](./15-Technical-Whitepapers) for systems programmers, performance engineers, and security researchers.
The papers themselves are not stored here; 82 of them link to a legitimate public copy under their heading.

- [Systems Performance & eBPF](./15-Technical-Whitepapers/01-Systems-Performance-and-Tracing): the 14-paper Brendan Gregg canon (BPF superpowers, the USE method, flamegraphs, off-CPU analysis), plus syscall tracing overhead benchmarks.
- [Operating Systems & Kernels](./15-Technical-Whitepapers/02-Operating-Systems-and-Kernels): Dennis Ritchie's 1974 UNIX treatise, MIT's xv6 teaching OS, Nick Blundell's OS from scratch, the Linux scheduler wasted cores analysis, and the Windows NT Executive architecture.
- [Memory Architecture & Concurrency](./15-Technical-Whitepapers/03-Memory-Architecture-and-Concurrency): Ulrich Drepper's 114-page masterwork on caches, TLBs, and NUMA; the Ousterhout threading debate; and database join memory effects.
- [Networking & Diagnostics](./15-Technical-Whitepapers/04-Networking-and-Protocols): TCP Fast Open (0-RTT), Van Jacobson's netchannels, tcpdump/tcptrace analysis, and DDoS mitigation.
- [Offensive Security & Exploitation](./15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation): reverse engineering, stack buffer overflows, PE binary infection, advanced SQL injection (Chris Anley), and Same-Origin Policy bypasses.
- [Defensive Security & Hardening](./15-Technical-Whitepapers/06-Defensive-Security-and-Hardening): Michael Boelen's Linux hardening trilogy, container security (Docker, LXC, Chromium sandbox), and the OWASP ASVS standard.
- [Polish Technical Papers (pl)](./15-Technical-Whitepapers/07-Polish-Technical-Papers-pl): 31 Polish research papers on ELF analysis, kernel rootkits, web security (Michał Sajdak), and SELinux.
- [Developer Tooling](./15-Technical-Whitepapers/08-Developer-Tooling-and-Foundations): The AWK Programming Language (Aho, Kernighan, Weinberg) and Vim for humans.
- [Seminal Computer Science Papers](./15-Technical-Whitepapers/09-Seminal-Computer-Science-Papers): 20 foundational papers spanning computability (Turing), information theory (Shannon), distributed consensus (Lamport, Paxos, Raft, FLP), cloud & big data (GFS, MapReduce, Bigtable, Dynamo, Spark), relational databases (Codd, Gray, Mohan ARIES), and Internet architecture (Cerf-Kahn, Saltzer, Van Jacobson).
- [Seminal Low-Latency Systems Papers](./15-Technical-Whitepapers/10-Seminal-Low-Latency-Systems-Papers): 18 seminal papers for low-latency & HFT engineers covering C++ memory models (Boehm-Adve, McKenney), lock-free/wait-free algorithms (Herlihy, LMAX Disruptor, RCU), kernel bypass (Netmap, Stanford IX, RAMCloud), and market microstructure dynamics (Kyle, Glosten-Milgrom, Cont OFI, Stoikov Micro-Price, Budish, Avellaneda-Stoikov).

### Other sections

- [06 - Interview Prep](./06-Interview-Prep/README.md): behavioral answers, resume guidance, mock interview checklists.
- [07 - Project Portfolio](./07-Project-Portfolio/README.md): portfolio project ideas by track.
- [09 - Engineering Leadership](./09-Engineering-Leadership/README.md), [10 - Development Practices](./10-Development-Practices/README.md), [11 - Security and Cryptography](./11-Security-And-Cryptography/README.md), [12 - Performance Engineering](./12-Performance-Engineering/README.md).
- [16 - Interview Command Center](./16-Interview-Command-Center/00-Dashboard.md): the Obsidian homepage; role hubs, macros, dashboards, and the review loop.
- [CS-Subjects](./CS-Subjects/README.md): a legacy index kept for old links; everything it lists lives in `01-CS-Foundations`.

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
cd "04-System-Design/Design Patterns/python-patterns" && make
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

## Maintaining the vault

- [tools/README.md](./tools/README.md) lists the audit, link-repair, index, frontmatter, and style scripts; every editing script is a dry run unless given `--apply`.
- [AUDIT.md](./AUDIT.md) is the generated health report (links, orphans, frontmatter, style, large files); regenerate it with `python3 tools/audit_vault.py`.
- CI runs the same checks on every pull request (`.no-mistakes.yaml`, `.github/workflows/ci.yml`): no broken links, no emojis or em dashes, no private data, and the tool tests.
- [CLAUDE.md](./CLAUDE.md) records the conventions: frontmatter schema, note standard, entry notes, archive folders, and the private-data rules.

## Contributing

Issues and pull requests are welcome, especially corrections to solutions, additional language ports, and new case studies.
Keep the existing numbered directory layout, keep code self-contained and compilable, and see [CODE_OF_CONDUCT.md](./CODE_OF_CONDUCT.md).

Third-party books, course notes, and paid course material are never committed; link to the publisher or author instead.
`.gitignore` blocks `*.pdf` except the vault's own Zero to Godhood books and openly licensed files.

## License

[MIT](./LICENSE). Copyright (c) 2021 Shreejit Verma.
