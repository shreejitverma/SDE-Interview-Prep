---
type: moc
track: [sde, distinguished, low-latency, quant-dev, ai-eng]
level:
status: draft
last_reviewed:
sources:
  - "Titus Winters, Tom Manshreck, and Hyrum Wright, Software Engineering at Google, O'Reilly Media, 2020, https://abseil.io/resources/swe-book"
  - "Michael Kerrisk, The Linux Programming Interface, No Starch Press, 2010, https://man7.org/tlpi/"
  - "Brendan Gregg, Systems Performance: Enterprise and the Cloud, 2nd Edition, Addison-Wesley, 2020, https://www.brendangregg.com/systems-performance-2nd-edition-book.html"
---

# Arena and Capstones Skill Ladder

This skill ladder defines observable, testable competencies across engineering levels L4 through L8+ for cross-cutting developer toolchain mastery, multi-system incident triage, and organizational developer infrastructure.
Every competency is formulated as a verifiable operational behavior linked to the corresponding capstone projects, scenario banks, and drills.

## Level 4: Foundational Systems Execution

- Can isolate process exit statuses, standard stream redirection bugs, and signal propagation failures in local terminal environments within fifteen minutes using [[Capstone-Unix-Shell-with-Job-Control|Capstone-Unix-Shell-with-Job-Control.md]].
- Can parse loose Git commit, tree, and blob objects from disk by computing SHA-1 and SHA-256 hashes and decompressing zlib payloads using [[Capstone-Mini-Git-in-Rust|Capstone-Mini-Git-in-Rust.md]].
- Can inspect register states and stack frames from an interactive debugger on unstripped binaries using [[Capstone-Ptrace-Debugger-with-DWARF|Capstone-Ptrace-Debugger-with-DWARF.md]].
- Can run local wheel packaging and cross-platform binary builds under containerized environments using [[Capstone-Polyglot-Packaging-and-Release-Pipeline|Capstone-Polyglot-Packaging-and-Release-Pipeline.md]].
- Can execute a guided 24-week curriculum milestone, passing all automated unit checks in the verification harness using [[Toolchain-24-Week-Mastery-Plan|Toolchain-24-Week-Mastery-Plan.md]].

## Level 5: Independent Diagnostic and Tooling Delivery

- Can diagnose and remediate a production segmentation fault from a core dump in a stripped binary by manually locating and loading detached DWARF debug symbols using [[Cross-Pillar-Debugging-Scenario-Bank|Cross-Pillar-Debugging-Scenario-Bank.md]].
- Can implement a recursive-descent command parser and manage pipeline file descriptor plumbing for multi-stage Unix pipelines using [[Capstone-Unix-Shell-with-Job-Control|Capstone-Unix-Shell-with-Job-Control.md]].
- Can decompress Git packfile delta chains across OFS_DELTA and REF_DELTA objects to extract repository revisions using [[Capstone-Mini-Git-in-Rust|Capstone-Mini-Git-in-Rust.md]].
- Can implement a software breakpoint engine using ptrace and int3 opcode patching on Linux x86-64 and AArch64 using [[Capstone-Ptrace-Debugger-with-DWARF|Capstone-Ptrace-Debugger-with-DWARF.md]].
- Can resolve a multi-package dependency conflict graph and emit deterministic lockfiles using a custom PubGrub constraint solver using [[Capstone-PubGrub-Dependency-Resolver|Capstone-PubGrub-Dependency-Resolver.md]].
- Can configure and execute multi-platform wheel generation with cibuildwheel and dist (formerly cargo-dist, renamed in v0.24.0) `dist plan` dry-run validation using [[Capstone-Polyglot-Packaging-and-Release-Pipeline|Capstone-Polyglot-Packaging-and-Release-Pipeline.md]].

## Level 6: Senior Cross-System Incident Triage and Infrastructure Design

- Can lead triage of an ambiguous multi-system production outage spanning corrupted build caches, dynamic linker symbol collisions, and memory leaks in under thirty minutes using [[Production-Incident-Simulation-Drills|Production-Incident-Simulation-Drills.md]].
- Can construct a statistical sampling profiler engine that collects thread stack traces via timer signals and formats output into folded stack frames for flame graph rendering using [[Capstone-Sampling-Profiler-Folded-Stacks|Capstone-Sampling-Profiler-Folded-Stacks.md]].
- Can evaluate DWARF .debug_line state machine bytecode and navigate .debug_info DIE trees to map arbitrary memory addresses to source lines using [[Capstone-Ptrace-Debugger-with-DWARF|Capstone-Ptrace-Debugger-with-DWARF.md]].
- Can design and build a hermetically isolated polyglot build workspace using content-addressable action graphs and remote execution cache layers using [[Capstone-Hermetic-Cached-Monorepo-Build|Capstone-Hermetic-Cached-Monorepo-Build.md]].
- Can implement terminal process group synchronization and job control state machines handling SIGINT, SIGTSTP, and SIGCHLD across background jobs using [[Capstone-Unix-Shell-with-Job-Control|Capstone-Unix-Shell-with-Job-Control.md]].
- Can deliver a complete multi-language release pipeline integrating SLSA provenance attestations, CycloneDX SBOM generation, and Cosign keyless signatures using [[Capstone-Polyglot-Packaging-and-Release-Pipeline|Capstone-Polyglot-Packaging-and-Release-Pipeline.md]].

## Level 7: Staff Organizational Architecture and Paved Roads

- Can author an enterprise architectural decision record justifying a monorepo or polyrepo repository strategy for 5,000 engineers, complete with merge queue topology and performance modeling, using [[Cross-Pillar-Design-Scenario-Bank|Cross-Pillar-Design-Scenario-Bank.md]].
- Can pass staff-level systems design interviews by defending toolchain trade-offs, ROI metrics, build performance economics, and blast-radius mitigations using [[Staff-and-Principal-Toolchain-Interview-Bar|Staff-and-Principal-Toolchain-Interview-Bar.md]].
- Can establish organizational quarantine and automated validation architectures to defend against malicious transitive dependency substitutions and repository takeovers using [[Production-Incident-Simulation-Drills|Production-Incident-Simulation-Drills.md]].
- Can architect an organization-wide hermetic build migration spanning legacy CMake, setuptools, and Cargo projects toward unified Bazel or Buck2 build graphs using [[Capstone-Hermetic-Cached-Monorepo-Build|Capstone-Hermetic-Cached-Monorepo-Build.md]].
- Can formulate and enforce company-wide dependency resolution invariants and ABI boundary versioning rules across C++, Python, and Rust services using [[Capstone-PubGrub-Dependency-Resolver|Capstone-PubGrub-Dependency-Resolver.md]].
- Can mentor senior engineers through complex cross-pillar failure postmortems and establish repeatable debugging runbooks across business units using [[Cross-Pillar-Debugging-Scenario-Bank|Cross-Pillar-Debugging-Scenario-Bank.md]].

## Level 8+: Principal and Distinguished Toolchain Strategy

- Can define the multi-year developer infrastructure and tooling strategy for an enterprise of 50,000 engineers, optimizing compute budgets, developer feedback loops, and security postures using [[Staff-and-Principal-Toolchain-Interview-Bar|Staff-and-Principal-Toolchain-Interview-Bar.md]].
- Can architect novel binary verification and supply-chain transparency systems that guarantee reproducible builds across distributed global infrastructure using [[Capstone-Hermetic-Cached-Monorepo-Build|Capstone-Hermetic-Cached-Monorepo-Build.md]].
- Can advise executive engineering leadership on the developer productivity economics of large-scale language migrations, compiler toolchain upgrades, and AI agent integration using [[Cross-Pillar-Design-Scenario-Bank|Cross-Pillar-Design-Scenario-Bank.md]].
- Can diagnose previously unknown compiler optimization bugs, dynamic linker edge cases, or kernel tracing regressions that block enterprise fleet deployment using [[Cross-Pillar-Debugging-Scenario-Bank|Cross-Pillar-Debugging-Scenario-Bank.md]].
- Can guide industry standards bodies and open-source steering committees on package distribution formats, debugger protocols, and supply-chain attestation specifications using [[Capstone-Polyglot-Packaging-and-Release-Pipeline|Capstone-Polyglot-Packaging-and-Release-Pipeline.md]].
