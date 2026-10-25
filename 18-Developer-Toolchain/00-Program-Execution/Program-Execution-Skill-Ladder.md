---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System V Application Binary Interface - AMD64 Architecture Processor Supplement, Draft Version 1.0, GitLab, 2026, https://gitlab.com/x86-psABIs/x86-64-ABI"
  - "Procedure Call Standard for the Arm 64-bit Architecture (AAPCS64), Arm Limited, 2025, https://github.com/ARM-software/abi-aa/blob/main/aapcs64/aapcs64.rst"
  - "DWARF Debugging Information Format Specification Version 5.0, DWARF Debugging Information Format Committee, 2017, https://dwarfstd.org/doc/DWARF5.pdf"
  - "Linux man-pages 6.19 as served by man7.org (6.20 released 2026-10-21), Michael Kerrisk et al., 2026, https://man7.org/linux/man-pages/"
  - "John R. Levine, Linkers and Loaders, Morgan Kaufmann, 1999, https://www.iecc.com/linker/"
  - "Itanium C++ ABI Specification, Codesourcery et al., 2026, https://itanium-cxx-abi.github.io/cxx-abi/abi.html"
---

# Program Execution Skill Ladder

Defines observable, testable engineering capabilities across program execution mechanics from junior engineer (L4) to distinguished engineer (L8+).

## Level 4 (L4)

- Spawns child processes safely and manages standard file descriptors without descriptor leaks using `posix_spawn` or `fork`/`exec` with `FD_CLOEXEC` as detailed in [[Processes-Fork-Exec-and-File-Descriptors]].
- Inspects basic process runtime state, memory maps, and file descriptor tables using the `/proc` filesystem as covered in [[Proc-Filesystem-and-Process-Credentials]].
- Inspects binary object headers, symbol tables, and shared library dependencies using `readelf`, `objdump`, and `ldd` as guided in [[ELF-Mach-O-and-PE-Object-Formats]].
- Configures dynamic library search paths using `RPATH` and `RUNPATH` flags without breaking deployment isolation as outlined in [[Dynamic-Linking-PLT-GOT-and-Loaders]].
- Configures terminal line settings between canonical and raw mode using `termios` flags as described in [[TTYs-PTYs-and-Line-Discipline]].

## Level 5 (L5)

- Handles asynchronous signals safely in multithreaded programs using `sigaction`, dedicated signal-masking threads, and async-signal-safe routines as established in [[Signals-Process-Groups-Sessions-and-Job-Control]].
- Diagnoses dynamic linker startup failures, symbol binding mismatches, and `GLIBC` version incompatibility using `LD_DEBUG` as practiced in [[Diagnose-Dynamic-Linker-and-ABI-Failures]].
- Resolves broken transitive dynamic library lookups and unintended symbol preemption in plugin architectures as demonstrated in [[Drill-Program-Execution-Broken-Rpath-and-Symbol-Interposition]].
- Reads disassembly systematically and correlates compiler vectorization and inlining decisions with optimization remarks as explained in [[Reading-Assembly-and-Optimization-Remarks]].
- Distinguishes virtual memory sizing metrics including RSS, PSS, USS, and VSZ to identify memory bloat and copy-on-write page sharing as modeled in [[Virtual-Memory-Mmap-and-Process-Address-Space]].
- Leverages Split DWARF (`.dwo`/`.dwp`) and external debug repositories to inspect stripped production crash dumps as executed in [[Drill-Program-Execution-Stripped-Binary-Crash-and-Split-DWARF]].

## Level 6 (L6)

- Traces executable program startup from kernel `execve` through the auxiliary vector, vDSO linkage, `_start`, and `.init_array` execution order as detailed in [[Program-Startup-C-Runtime-Init-and-Auxiliary-Vectors]].
- Implements custom static archive extraction and ELF relocation resolution for x86-64 and AArch64 instruction targets as mastered in [[Symbols-Relocations-and-Static-Linking]].
- Debugs cross-DSO C++ exception unwinding and runtime type identification failures across shared library boundaries as solved in [[Drill-Program-Execution-Cross-DSO-Exception-and-RTTI-Failure]].
- Resolves thread-local storage exhaustion and `dlopen` failures caused by Initial Exec TLS surplus depletion as mastered in [[Thread-Local-Storage-Models-and-ELF-Relocations]].
- Configures GNU symbol versioning scripts and `IFUNC` dynamic resolvers to dispatch optimized vector routines without early GOT relocation hazards as shown in [[Symbol-Versioning-IFUNC-and-Linker-Scripts]].
- Audits and configures hardware security mitigations including full `RELRO`, stack canaries, ARM PAC/BTI, and Intel CET shadow stacks as documented in [[Hardware-Security-ABIs-PAC-BTI-CET-and-RELRO]].

## Level 7 (L7)

- Evaluates trade-offs between frame-pointer unwinding and DWARF Call Frame Information tables to optimize continuous profiling overhead as articulated in [[Stack-Unwinding-CFI-and-Exception-ABI]].
- Designs distributed ThinLTO build and caching pipelines across large monorepos to balance link latency against runtime throughput as analyzed in [[Linkers-LTO-PGO-AutoFDO-and-BOLT]].
- Implements post-link binary optimization pipelines using BOLT and hardware branch recording to maximize CPU instruction cache utilization as detailed in [[Linkers-LTO-PGO-AutoFDO-and-BOLT]].
- Authors comprehensive Architectural Decision Records balancing static linking against dynamic loading across multi-tenant fleet architectures as codified in [[Program-Execution-Decision-Records]].
- Bridges toolchain discrepancies and security boundary constraints between macOS Darwin Mach ports and Linux ELF namespaces as analyzed in [[macOS-vs-Linux-Toolchain-Mechanics]].

## Level 8+ (L8+)

- Architects fleet-wide compiler optimization and binary layout strategies combining AutoFDO, distributed ThinLTO, and BOLT to deliver measurable multi-megawatt datacenter power reductions as formulated in [[Program-Execution-Decision-Records]].
- Authors organizational ABI stability policies and symbol visibility standards that eliminate ODR hazards and diamond dependency conflicts across tens of thousands of internal libraries as guided in [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]].
- Designs kernel and runtime unwinding infrastructure capable of non-blocking execution inside non-maskable interrupt (NMI) and eBPF contexts as proven in [[Stack-Unwinding-CFI-and-Exception-ABI]].
- Redesigns core process lifecycle and execution abstractions to eliminate memory locking contention and multi-millisecond page table duplication pauses during large-scale process fork operations as addressed in [[Processes-Fork-Exec-and-File-Descriptors]].
- Anticipates and directs toolchain evolution across next-generation binary specifications, DWARF 6 working standards, and hardware capability security architectures as unified across [[DWARF-Debug-Information-and-Symbol-Bundles]].
