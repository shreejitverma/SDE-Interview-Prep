---
type: moc
track: [sde, distinguished, low-latency, quant-dev, ai-eng]
level:
status: draft
last_reviewed:
sources:
  - "Scott Chacon and Ben Straub, Pro Git, 2nd Edition, Apress, 2014, https://git-scm.com/book/en/v2"
  - "Michael Kerrisk, The Linux Programming Interface, No Starch Press, 2010, https://man7.org/tlpi/"
  - "Brendan Gregg, Systems Performance: Enterprise and the Cloud, 2nd Edition, Addison-Wesley, 2020, https://www.brendangregg.com/systems-performance-2nd-edition-book.html"
  - "Titus Winters, Tom Manshreck, and Hyrum Wright, Software Engineering at Google, O'Reilly Media, 2020, https://abseil.io/resources/swe-book"
  - "Natalie Weizenbaum, PubGrub: Next-Generation Version Solving, Medium, 2018, https://nex3.medium.com/pubgrub-2fb6470504f"
  - "Andrey Mokhov, Neil Mitchell, and Simon Peyton Jones, Build Systems a la Carte, Journal of Functional Programming, Cambridge University Press, 2020, https://doi.org/10.1017/S0956796820000088"
  - "Robert O'Callahan, Chris Jones, Nathan Froyd, Kyle Huey, Albert Noll, and Nimrod Partush, Engineering Record and Replay for Deployability, USENIX ATC, 2017, https://www.usenix.org/conference/atc17/technical-sessions/presentation/ocallahan"
---

# Arena and Capstones Topic Inventory

This inventory maps all cross-pillar capstones, multi-system scenario banks, mastery schedules, and staff-plus evaluation rubrics in pillar 11 to their owning notes, target depth levels, and vault-wide coverage.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Curriculum Roadmap and Reading Order | [[README\|README.md]] | Use, Explain (L4-L8+) | [[00-Start-Here/Roadmap.md\|Roadmap]] |
| Competency Framework and Observables | [[Arena-Skill-Ladder\|Arena-Skill-Ladder.md]] | Use, Explain, Decide at scale (L4-L8+) | [[08-Distinguished-Engineering/README.md\|Distinguished Engineering]] |
| 24-Week Systems Mastery Schedule | [[Toolchain-24-Week-Mastery-Plan\|Toolchain-24-Week-Mastery-Plan.md]] | Use, Explain (L4-L8+) | [[00-Start-Here/Review-Queue.md\|Review Queue]] |
| Staff and Principal Interview Standards | [[Staff-and-Principal-Toolchain-Interview-Bar\|Staff-and-Principal-Toolchain-Interview-Bar.md]] | Explain, Decide at scale (L6-L8+) | [[16-Interview-Command-Center/01-Roles/SDE/Question-Bank.md\|SDE Question Bank]] |
| Cross-Pillar Diagnostic Scenarios | [[Cross-Pillar-Debugging-Scenario-Bank\|Cross-Pillar-Debugging-Scenario-Bank.md]] | Debug, Explain (L5-L8+) | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|Python Backend Debugging]] |
| Enterprise Workspace Infrastructure Design | [[Cross-Pillar-Design-Scenario-Bank\|Cross-Pillar-Design-Scenario-Bank.md]] | Decide at scale (L6-L8+) | [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_06_Internal_Developer_Platforms_and_Backstage.md\|Internal Developer Platforms]] |
| Timed Production Outage Simulations | [[Production-Incident-Simulation-Drills\|Production-Incident-Simulation-Drills.md]] | Debug (L5-L8+) | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing.md\|Deterministic Replay]] |
| Git Plumbing and Object Store in Rust | [[Capstone-Mini-Git-in-Rust\|Capstone-Mini-Git-in-Rust.md]] | Rebuild, Explain (L5-L7) | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/02-Python-Core.md\|Python Core]] |
| Native Debugger with DWARF Line Lookup | [[Capstone-Ptrace-Debugger-with-DWARF\|Capstone-Ptrace-Debugger-with-DWARF.md]] | Rebuild, Debug (L6-L8+) | [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md\|Processes and Process Management]] |
| Sampling Profiler and Folded Stacks | [[Capstone-Sampling-Profiler-Folded-Stacks\|Capstone-Sampling-Profiler-Folded-Stacks.md]] | Rebuild, Explain (L6-L8+) | [[12-Performance-Engineering/02-Profiling/profiling_guide.md\|Profiling Guide]] |
| Polyglot Release Pipeline with SLSA | [[Capstone-Polyglot-Packaging-and-Release-Pipeline\|Capstone-Polyglot-Packaging-and-Release-Pipeline.md]] | Rebuild, Decide at scale (L5-L7) | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore.md\|Supply Chain Security]] |
| Hermetic Build with Remote Cache | [[Capstone-Hermetic-Cached-Monorepo-Build\|Capstone-Hermetic-Cached-Monorepo-Build.md]] | Rebuild, Decide at scale (L6-L8+) | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/10-part-compilation/39_build_systems_and_modules.md\|Build Systems and Modules]] |
| PubGrub CDCL Version Constraint Solver | [[Capstone-PubGrub-Dependency-Resolver\|Capstone-PubGrub-Dependency-Resolver.md]] | Rebuild, Explain (L5-L7) | none |
| POSIX Unix Shell with Job Control | [[Capstone-Unix-Shell-with-Job-Control\|Capstone-Unix-Shell-with-Job-Control.md]] | Rebuild, Explain (L5-L7) | [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Sessions-Process-Groups-and-Controlling-Terminals.md\|Sessions and Process Groups]] |

## Out of scope

- Single-language basic syntax and introductory programming tutorials: covered comprehensively in `02-Programming-Languages/`.
- Commercial cloud provider GUI administration and proprietary console workflows: covered in `17-Cloud-AI-Engineering/`.
- Pure abstract algorithmic puzzles lacking systems runtime interaction: covered in `03-Data-Structures-Algorithms/`.
- Silicon circuit design, FPGA programming, and hardware description languages: out of scope for software developer toolchain engineering.
- Proprietary closed-source enterprise SaaS vendor configurations that lack open standards: only open specifications and reproducible tooling are covered.
