---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Andrey Mokhov, Neil Mitchell, Simon Peyton Jones, Build Systems a la Carte: Theory and Practice, Journal of Functional Programming, Vol. 30, e11, 2020, https://doi.org/10.1017/S0956796820000088"
  - "Rachel Potvin, Josh Levenberg, Why Google Stores Billions of Lines of Code in a Single Repository, Communications of the ACM, Vol. 59, No. 7, pp. 78-87, 2016, https://doi.org/10.1145/2854146"
  - "Gang Ren, Eric Tune, Tipp Moseley, Yixin Shi, Silvius Rus, Robert Hundt, Google-Wide Profiling: A Continuous Profiling Infrastructure for Data Centers, IEEE Micro, Vol. 30, No. 4, pp. 65-79, 2010, https://research.google/pubs/google-wide-profiling-a-continuous-profiling-infrastructure-for-data-centers/"
  - "Charlie Curtsinger, Emery D. Berger, Coz: Finding Code that Counts with Causal Profiling, SOSP 2015, pp. 184-197, 2015, https://arxiv.org/abs/1608.03676"
  - "Todd Mytkowicz, Amer Diwan, Matthias Hauswirth, Peter F. Sweeney, Producing Wrong Data Without Doing Anything Obviously Wrong!, ASPLOS XIV, pp. 265-276, 2009, https://doi.org/10.1145/1508244.1508275"
  - "Andreas Zeller, Ralf Hildebrandt, Simplifying and Isolating Failure-Inducing Input, IEEE Transactions on Software Engineering, Vol. 28, No. 2, pp. 183-200, 2002, https://doi.org/10.1109/32.988498"
  - "Konstantin Serebryany, Derek Bruening, Alexander Potapenko, Dmitry Vyukov, AddressSanitizer: A Fast Address Sanity Checker, USENIX ATC 12, pp. 309-318, 2012, https://www.usenix.org/conference/atc12/technical-sessions/presentation/serebryany"
  - "Konstantin Serebryany, Timur Iskhodzhanov, ThreadSanitizer: data race detection in practice, WBIA '09, pp. 62-71, 2009, https://doi.org/10.1145/1791194.1791203"
  - "Robert O'Callahan et al., Engineering Record and Replay for Deployability, USENIX ATC 17, pp. 377-389, 2017, https://www.usenix.org/system/files/conference/atc17/atc17-o_callahan.pdf"
  - "Maksim Panchenko, Rafael Auler, Bill Nell, Guilherme Ottoni, BOLT: A Practical Binary Optimizer for Data Centers and Beyond, CGO 2019, pp. 2-14, 2019, https://arxiv.org/abs/1807.06735"
  - "Caitlin Sadowski et al., Tricorder: Building a Program Analysis Ecosystem, ICSE 2015, pp. 598-608, 2015, https://research.google/pubs/tricorder-building-a-program-analysis-ecosystem/"
  - "Caitlin Sadowski et al., Lessons from Building Static Analysis Tools at Google, Communications of the ACM, Vol. 61, No. 4, pp. 58-66, 2018, https://doi.org/10.1145/3188720"
  - "Santiago Torres-Arias et al., in-toto: Providing farm-to-table guarantees for bits and bytes, USENIX Security 19, pp. 1393-1410, 2019, https://www.usenix.org/conference/usenixsecurity19/presentation/torres-arias"
  - "Chris Lamb, Stefano Zacchiroli, Reproducible Builds: Increasing the Integrity of Software Supply Chains, IEEE Software, Vol. 39, No. 2, pp. 62-70, 2022, https://arxiv.org/abs/2104.06020"
  - "Ken Thompson, Reflections on Trusting Trust, Communications of the ACM, Vol. 27, No. 8, pp. 761-763, 1984, https://doi.org/10.1145/358198.358210"
  - "Nicholas Nethercote, Julian Seward, Valgrind: A Framework for Heavyweight Dynamic Binary Instrumentation, PLDI '07, pp. 89-100, 2007, https://doi.org/10.1145/1250734.1250746"
  - "Evgeniy Stepanov, Konstantin Serebryany, MemorySanitizer: fast detector of uninitialized memory use in C++, CGO 2015, pp. 46-55, 2015, https://doi.org/10.1109/CGO.2015.7054186"
  - "Dehao Chen, David Xinliang Li, Tipp Moseley, AutoFDO: Automatic Feedback-Directed Optimization for Warehouse-Scale Applications, CGO '16, pp. 12-23, 2016, https://doi.org/10.1145/2854038.2854044"
  - "Teresa Johnson, Mehdi Amini, David Xinliang Li, ThinLTO: Scalable and Incremental LTO, CGO 2017, pp. 111-121, 2017, https://doi.org/10.1109/CGO.2017.7863733"
  - "Brendan Gregg, The Flame Graph, Communications of the ACM, Vol. 59, No. 6, pp. 48-57, 2016, https://doi.org/10.1145/2909476"
  - "Benjamin H. Sigelman et al., Dapper, a Large-Scale Distributed Systems Tracing Infrastructure, Google Technical Report dapper-2010-1, 2010, https://research.google/pubs/dapper-a-large-scale-distributed-systems-tracing-infrastructure/"
  - "Caitlin Sadowski et al., Modern Code Review: A Case Study at Google, ICSE-SEIP '18, pp. 181-190, 2018, https://sback.it/publications/icse2018seip.pdf"
  - "Natalie Weizenbaum, PubGrub: Next-Generation Version Solving, Medium Engineering Essay, 2018, https://nex3.medium.com/pubgrub-2fb6470504f"
  - "Russ Cox, Minimal Version Selection, research.swtch.com, 2018, https://research.swtch.com/vgo-mvs"
---

# Developer Toolchain Papers Canon Topic Inventory

This inventory maps the seminal computer science literature and canonical technical specifications of developer toolchains to notes in 18-Developer-Toolchain.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Thompson 1984: Quines, Self-Reproducing Compilers, Compiler Backdoors (Diverse Double-Compiling is the later Wheeler 2005 countermeasure) | [[Paper-Reflections-on-Trusting-Trust]] | Explain | none |
| Zeller & Hildebrandt 2002: Test Case Reduction, Minimizing Delta Debugging (`ddmin`) | [[Paper-Zeller-Delta-Debugging]] | Explain, Rebuild | none |
| Nethercote & Seward 2007: VEX Intermediate Representation, Dynamic Binary Instrumentation | [[Paper-Valgrind-Dynamic-Binary-Instrumentation]] | Explain | none |
| Mytkowicz et al. 2009: Measurement Bias, Hardware Layout Variance, Setup Randomization | [[Paper-Producing-Wrong-Data-Measurement-Bias]] | Explain | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon.md\|Brendan-Gregg-Performance-Canon]] |
| Serebryany & Iskhodzhanov 2009: Lockset & Happens-Before Hybrid Race Detection | [[Paper-ThreadSanitizer]] | Explain | none |
| Ren et al. 2010: Continuous Datacenter Profiling, PMU Sampling, AutoFDO Pipeline | [[Paper-Google-Wide-Profiling]] | Explain | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation.md\|Linux-Tracing-and-Instrumentation]] |
| Sigelman et al. 2010: Span Trees, RPC Context Propagation, Out-of-Band Tracing | [[Paper-Dapper-Distributed-Systems-Tracing]] | Explain | [[04-System-Design/00-Concepts/SLOs-and-Observability.md\|SLOs-and-Observability]] |
| Serebryany et al. 2012: Direct Shadow Memory, Redzones, Quarantine Allocator | [[Paper-AddressSanitizer]] | Explain | none |
| Stepanov & Serebryany 2015: Bit-Exact Shadow Memory, Uninitialized Reads, Origin Tracking | [[Paper-MemorySanitizer]] | Explain | none |
| Sadowski et al. 2015: Program Analysis Ecosystem, Review Integration, Autofixes | [[Paper-Tricorder-Program-Analysis-Ecosystem]] | Explain | none |
| Curtsinger & Berger 2015: Causal Profiling, Virtual Speedups via Pauses, Progress Points | [[Paper-Coz-Causal-Profiling]] | Explain | none |
| Chen et al. 2016: Hardware Sampling, LBR Branch Profiles, Warehouse-Scale Inlining | [[Paper-AutoFDO-Warehouse-Scale-Optimization]] | Explain | none |
| Gregg 2016: Hierarchical Stack Trace Aggregation, Differential Flame Graphs | [[Paper-The-Flame-Graph-Gregg]] | Explain | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon.md\|Brendan-Gregg-Performance-Canon]] |
| Potvin & Levenberg 2016: Monorepo Architecture, Spanner-Backed Piper, CitC FUSE | [[Paper-Google-Monorepo-Piper-and-CitC]] | Explain, Decide at Scale | none |
| Johnson et al. 2017: Module Summaries, Distributed Thin Links, Incremental LTO | [[Paper-ThinLTO-Scalable-Incremental-LTO]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|Chapter_102_Linking_Loading_ABI_and_WPO]] |
| O'Callahan et al. 2017: Deterministic Record and Replay, PMU Retired Branch Counts | [[Paper-rr-Record-and-Replay]] | Explain | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing.md\|Deterministic Replay and Packet Injection Testing]] |
| Sadowski et al. 2018: Static Analysis Retrospective, The <10% False Positive Bar | [[Paper-Lessons-From-Static-Analysis-at-Google]] | Explain, Decide at Scale | none |
| Sadowski et al. 2018: Empirical Study of 9M Code Reviews, Readability, Defect Rates | [[Paper-Modern-Code-Review-at-Google]] | Explain, Decide at Scale | [[09-Engineering-Leadership/03-Code-Review/checklist.md\|checklist]] |
| Mokhov et al. 2018/2020: Build Systems a la Carte, Task Schedulers, Trace Rebuilders | [[Paper-Build-Systems-a-la-Carte]] | Explain, Rebuild | none |
| Weizenbaum 2018 & Cox 2018: CDCL Version Solving (PubGrub) vs Polynomial-Time MVS | [[Paper-PubGrub-and-Minimal-Version-Selection]] | Explain, Rebuild | none |
| Panchenko et al. 2019: Post-Link Binary Optimization, CFG Reconstruction, HFSort | [[Paper-BOLT-Binary-Optimizer]] | Explain | none |
| Torres-Arias et al. 2019: Supply Chain Layouts, Link Attestations, Material Tracking | [[Paper-in-toto-Supply-Chain-Guarantees]] | Explain, Decide at Scale | none |
| Lamb & Zacchiroli 2022: Reproducible Builds, Non-Determinism Mitigation, Attestation | [[Paper-Reproducible-Builds-Lamb-Zacchiroli]] | Explain, Decide at Scale | none |

## Out of scope

- General distributed systems consensus papers (Paxos, Raft), database transaction papers (Spanner, Bigtable, Dynamo), and core networking protocols are out of scope because they belong to 15-Technical-Whitepapers and system design canon; 10-Papers strictly owns the developer toolchain canon spanning compilers, linkers, build systems, debuggers, profilers, repository scaling, code review, static analysis, and software supply chain integrity.
