---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System V Application Binary Interface - DRAFT - 24 April 2001, The Santa Cruz Operation, 2001, https://refspecs.linuxbase.org/elf/gabi4+/contents.html"
  - "System V Application Binary Interface - AMD64 Architecture Processor Supplement, Draft Version 1.0, GitLab, 2026, https://gitlab.com/x86-psABIs/x86-64-ABI"
  - "Procedure Call Standard for the Arm 64-bit Architecture (AAPCS64), Arm Limited, 2025, https://github.com/ARM-software/abi-aa/blob/main/aapcs64/aapcs64.rst"
  - "ELF for the Arm 64-bit Architecture (AAELF64) and SYSVABI64, Arm Limited, 2025, https://github.com/ARM-software/abi-aa/blob/main/aaelf64/aaelf64.rst"
  - "DWARF Debugging Information Format Specification Version 5.0, DWARF Debugging Information Format Committee, 2017, https://dwarfstd.org/doc/DWARF5.pdf"
  - "Linux man-pages 6.19 as served by man7.org (6.20 released 2026-10-21): ld.so(8), fork(2), execve(2), posix_spawn(3), signal(7), credentials(7), pty(7), termios(3), proc(5), Michael Kerrisk et al., 2026, https://man7.org/linux/man-pages/"
  - "Apple Inc., dyld and Mach-O Runtime, Apple Open Source, 2026, https://github.com/apple-oss-distributions/dyld"
  - "John R. Levine, Linkers and Loaders, Morgan Kaufmann, 1999, https://www.iecc.com/linker/"
  - "Itanium C++ ABI Specification and Exception Handling ABI, Codesourcery et al., 2026, https://itanium-cxx-abi.github.io/cxx-abi/abi.html"
  - "Randal E. Bryant and David R. O'Hallaron, Computer Systems: A Programmer's Perspective, 3rd Edition, Pearson, 2015, https://csapp.cs.cmu.edu/3e/home.html"
  - "LLVM Project, Link Time Optimization and Users Manual, LLVM Project, 2026, https://llvm.org/docs/LinkTimeOptimization.html"
  - "GCC Development Team, GCC Online Documentation: Optimize Options and LTO Internals, Free Software Foundation, 2026, https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html"
  - "LLVM Project, BOLT (Binary Optimization and Layout Tool), GitHub, 2026, https://github.com/llvm/llvm-project/blob/main/bolt/README.md"
  - "Rui Ueyama, mold: A Modern Linker, GitHub, 2026, https://github.com/rui314/mold/blob/main/docs/mold.md"
  - "LLVM Project, lld: The LLVM Linker, LLVM Project, 2026, https://lld.llvm.org/"
  - "Compiler Explorer Documentation, Matt Godbolt et al., 2026, https://github.com/compiler-explorer/compiler-explorer/tree/main/docs"
---

# Program Execution Topic Inventory

Curriculum coverage matrix mapping authoritative execution specifications to owning notes, depth-ladder targets, and existing vault foundations.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| System V gABI: Ch 4 Object Files (ELF Identification, Data Representation, Machine Information) | [[ELF-Mach-O-and-PE-Object-Formats]] | Explain | [[15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng.md\|Binary Exploitation]] |
| System V gABI: Ch 4 Object Files (Sections, Section Headers, Special Sections, String Table) | [[ELF-Mach-O-and-PE-Object-Formats]] | Explain | none |
| System V gABI: Ch 4 Object Files (Symbol Table, Symbol Values, Relocation Entries) | [[Symbols-Relocations-and-Static-Linking]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| System V gABI: Ch 5 Program Loading and Dynamic Linking (Program Headers, Segment Flags, Segments) | [[ELF-Mach-O-and-PE-Object-Formats]] | Explain | none |
| System V gABI: Ch 5 Program Loading and Dynamic Linking (Dynamic Section, Shared Object Dependencies) | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| System V gABI: Ch 5 Program Loading and Dynamic Linking (Global Offset Table, Procedure Linkage Table) | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| System V gABI: Ch 5 Program Loading (Initialization and Termination Arrays: .init_array, .fini_array) | [[Program-Startup-C-Runtime-Init-and-Auxiliary-Vectors]] | Explain | none |
| AMD64 psABI: Ch 3 Low-Level System Information (3.1 Machine Interface, Micro-Architecture Levels) | [[Calling-Conventions-System-V-AMD64-and-AAPCS64]] | Explain | none |
| AMD64 psABI: Ch 3 Low-Level System Information (3.2 Function Calling Sequence, Registers, Red Zone) | [[Calling-Conventions-System-V-AMD64-and-AAPCS64]] | Use | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI.md\|C++ Ch 111 Stable C ABI]] |
| AMD64 psABI: Ch 3 Low-Level System Information (3.3-3.4 Virtual Memory, Auxiliary Vector, Initial Stack) | [[Program-Startup-C-Runtime-Init-and-Auxiliary-Vectors]] | Explain | none |
| AMD64 psABI: Ch 3 Low-Level System Information (3.5 Code Models: Small, Medium, Large, Kernel, PIC) | [[Calling-Conventions-System-V-AMD64-and-AAPCS64]] | Explain | none |
| AMD64 psABI: Ch 4 Object Files (4.1 ELF Header, 4.3 Symbol Table, 4.4 Relocations R_X86_64_*) | [[Symbols-Relocations-and-Static-Linking]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| AMD64 psABI: Ch 3.6, 4 & 6 (DWARF Register Number Mapping, .eh_frame Section, Unwind Library Interface) | [[Stack-Unwinding-CFI-and-Exception-ABI]] | Explain | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation.md\|Observability Without Perturbation]] |
| AMD64 psABI: Ch 5 Dynamic Linking (GOT, PLT, STT_GNU_IFUNC Indirect Functions) | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| AAPCS64: Sections 1-5 Data Types and Alignment (Fundamental Types, SVE Vectors, Composite Aggregates) | [[Calling-Conventions-System-V-AMD64-and-AAPCS64]] | Explain | none |
| AAPCS64: Section 6 Base Procedure Call Standard (Registers X0-X30, SP, FP X29, LR X30, Stack Rules) | [[Calling-Conventions-System-V-AMD64-and-AAPCS64]] | Use | none |
| AAPCS64 Section 7 Standard Variants and SYSVABI64 GNU Property Notes (Pointer Authentication PAC, Branch Target Identification BTI) | [[Hardware-Security-ABIs-PAC-BTI-CET-and-RELRO]] | Explain | none |
| AAELF64 & SYSVABI64: AAELF64 Section 5 Object Files (ELF Header, Sections, .note.gnu.property, Mapping Symbols $x, $d) | [[ELF-Mach-O-and-PE-Object-Formats]] | Explain | none |
| AAELF64 & SYSVABI64: AAELF64 Section 5 Instruction Relocations (ADRP, ADD, LDR, Jump Veneers) | [[Symbols-Relocations-and-Static-Linking]] | Explain | none |
| AAELF64 & SYSVABI64: AAELF64 Section 5 TLS Relocations and SYSVABI64 Thread Local Storage (General Dynamic, Local Dynamic, Initial Exec, TLSDESC) | [[Thread-Local-Storage-Models-and-ELF-Relocations]] | Explain | none |
| AAELF64 & SYSVABI64: SYSVABI64 Program Loading (Dynamic Section DT_AARCH64_BTI_PLT, PAC PLT, GOT) | [[Hardware-Security-ABIs-PAC-BTI-CET-and-RELRO]] | Explain | none |
| DWARF 5 Spec & DWARF 6 Draft: Ch 1-2 General Description (DIEs, Attribute Forms, Location Expressions) | [[DWARF-Debug-Information-and-Symbol-Bundles]] | Explain | none |
| DWARF 5 Spec & DWARF 6 Draft: Ch 3-5 Program Scope (Compilation Unit, Subprogram, Inlined Subroutines) | [[DWARF-Debug-Information-and-Symbol-Bundles]] | Explain | none |
| DWARF 5 Spec & DWARF 6 Draft: Ch 6 Debugging Info (.debug_names Index, .debug_line State Machine) | [[DWARF-Debug-Information-and-Symbol-Bundles]] | Explain | none |
| DWARF 5 Spec & DWARF 6 Draft: Ch 6.4 Call Frame Information (.debug_frame, CIE, FDE, CFA Rules) | [[Stack-Unwinding-CFI-and-Exception-ABI]] | Explain | none |
| DWARF 5 Spec & DWARF 6 Draft: Ch 7 Data Representation (LEB128, Split DWARF .dwo/.dwp, Compression) | [[DWARF-Debug-Information-and-Symbol-Bundles]] | Rebuild | none |
| Linux ld.so(8): Search Paths (Rpath, Runpath, Tokens $ORIGIN, $LIB), Options, Environment Variables | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Use | none |
| Linux ld.so(8): Dynamic Linker Failure Diagnosis (LD_DEBUG, LD_AUDIT, --list-tunables, AT_SECURE) | [[Diagnose-Dynamic-Linker-and-ABI-Failures]] | Debug | none |
| Apple dyld & Mach-O: dyld(1) Environment Variables (DYLD_LIBRARY_PATH, @rpath, dyld shared cache) | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Use | none |
| Apple dyld & Mach-O: Mach-O Headers, Load Commands (LC_SEGMENT_64, LC_DYLD_CHAINED_FIXUPS), Segments | [[ELF-Mach-O-and-PE-Object-Formats]] | Explain | none |
| Apple dyld & Mach-O: Compact Unwinding (__compact_unwind Section Format and Encodings) | [[Stack-Unwinding-CFI-and-Exception-ABI]] | Explain | none |
| Levine, Linkers and Loaders: Ch 1-4 Linking Overview, Architecture, Object Files, Storage Allocation | [[Symbols-Relocations-and-Static-Linking]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| Levine, Linkers and Loaders: Ch 5-7 Symbol Management (Binding, Mangling, Weak Symbols), Relocation | [[Symbols-Relocations-and-Static-Linking]] | Explain | none |
| Levine, Linkers and Loaders: Ch 8-10 Shared Libraries, PIC, Dynamic Linking (PLT/GOT, DLL Imports) | [[Dynamic-Linking-PLT-GOT-and-Loaders]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| Levine, Linkers and Loaders: Ch 11 Advanced Techniques (C++ COMDATs, Static Constructors, GC Sections) | [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]] | Explain | none |
| Itanium C++ ABI: Ch 1-2 Data Layout (POD, Virtual Tables, VTT, Construction Vtables, RTTI) | [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| Itanium C++ ABI: Ch 3 & 5 Functional Interface (Adjustor Thunks, Guard Variables, Mangling Grammar) | [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]] | Explain | none |
| Itanium C++ ABI (abi-eh.html): Ch 4 Exception Handling (_Unwind_RaiseException, Personality, LSDA) | [[Stack-Unwinding-CFI-and-Exception-ABI]] | Explain | none |
| Linux Man Pages: fork(2), execve(2), posix_spawn(3) (Copy-on-Write, Multithreaded Hazards, Cloexec) | [[Processes-Fork-Exec-and-File-Descriptors]] | Use | [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md\|GIOS P2L1 Processes]] |
| Linux Man Pages: signal(7) (Dispositions, Masks, Real-Time Signals, Async-Signal Safety, Sigaltstack) | [[Signals-Process-Groups-Sessions-and-Job-Control]] | Use | [[01-CS-Foundations/Operating-Systems/IPC-Complete-Guide.md\|IPC Complete Guide]] |
| Linux Man Pages: credentials(7) (UID/GID Transitions, Saved IDs, Capabilities, Supplementary Groups) | [[Proc-Filesystem-and-Process-Credentials]] | Use | none |
| Linux Man Pages: pty(7), termios(3) (UNIX 98 PTYs, Line Discipline, Raw vs Canonical, Window Size) | [[TTYs-PTYs-and-Line-Discipline]] | Explain | none |
| Linux Man Pages: proc(5) (/proc/[pid]/maps, smaps_rollup, pagemap, fd, auxv, oom_score_adj) | [[Proc-Filesystem-and-Process-Credentials]] | Use | none |
| CS:APP 3e: Ch 3 & 5 Machine-Level Programs, Function Execution, Assembly Optimization Remarks | [[Reading-Assembly-and-Optimization-Remarks]] | Use | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_89_Reading_the_Compilers_Output/Chapter_89_Reading_the_Compilers_Output.md\|C++ Ch 89 Reading Assembly]] |
| CS:APP 3e: Ch 7 Linking (Static Linking, Relocatable Objects, Symbols, Relocation, Dynamic Linking) | [[Symbols-Relocations-and-Static-Linking]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| CS:APP 3e: Ch 8 Exceptional Control Flow (Exceptions, Process Control, Signals, Nonlocal Jumps) | [[Processes-Fork-Exec-and-File-Descriptors]] | Use | [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md\|GIOS P2L1 Processes]] |
| CS:APP 3e: Ch 9 & 10 Virtual Memory (mmap, Dynamic Allocation, Page Faults), System-Level I/O | [[Virtual-Memory-Mmap-and-Process-Address-Space]] | Use | [[01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L2-Memory-Management.md\|GIOS P3L2 Memory]] |
| LLVM & GCC Optimization: LTO, ThinLTO (Thin Link, Parallel Backend), PGO, AutoFDO, SamplePGO | [[Linkers-LTO-PGO-AutoFDO-and-BOLT]] | Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|C++ Ch 102 Linking Loading ABI]] |
| BOLT README: Post-Link Binary Optimization (LBR Profiling, perf2bolt, Basic Block Reordering) | [[Linkers-LTO-PGO-AutoFDO-and-BOLT]] | Use | none |
| mold & lld Documentation: Modern High-Speed Linker Architectures, Concurrency, Memory Mapping | [[Linkers-LTO-PGO-AutoFDO-and-BOLT]] | Explain | none |
| Compiler Explorer Documentation: Interactive Assembly Inspection, IR/AST Views, Optimization Remarks | [[Reading-Assembly-and-Optimization-Remarks]] | Use | none |

## Out of scope

- System V AMD64 psABI Ch 1-2: Introduction and Software Installation, omitted because Ch 2 only states that the psABI does not specify how software is installed.
- System V AMD64 psABI Ch 6.1, 7, 8, 9: C Library notes, predefined preprocessor symbols, the execution environment stub, and the C++ and Fortran conventions, omitted because glibc and the Itanium C++ ABI are the live references.
- System V AMD64 psABI Ch 10: ILP32 programming model (x32 ABI), omitted due to lack of adoption in modern production environments.
- CS:APP 3e Ch 1, 2, 4, 6: Hardware logic, binary integer representation, and CPU cache hierarchies, preserved in [[01-CS-Foundations/Operating-Systems/README.md\|01-CS-Foundations]] and [[12-Performance-Engineering/README.md\|12-Performance-Engineering]].
- CS:APP 3e Ch 11, 12: Network socket programming and thread synchronization, owned by [[01-CS-Foundations/Computer-Networks/README.md\|Computer Networks]] and [[04-System-Design/00-Concepts/README.md\|System Design]].
- CrowdStrike Falcon incident case study: Reassigned exclusively to 07-Packaging-and-Release to anchor kernel deployment and release gating mechanisms.
