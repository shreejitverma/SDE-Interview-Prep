---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Virtual Memory, Mmap, and Process Address Space

> [!summary] Scope
> Owns the process memory layout from user space, covering text, data, bss, heap, memory-mapped regions, and stack segments under address space layout randomization (ASLR).
> Explains mmap flags (MAP_PRIVATE, MAP_SHARED, MAP_ANONYMOUS, MAP_POPULATE), minor versus major page faults, copy-on-write page duplication, and memory metrics (RSS, PSS, USS, VSZ).
> Reaches Explain through Decide at scale, handing physical page frame reclamation to OS foundation notes and binary section mappings to ELF-Mach-O-and-PE-Object-Formats.md.

## Related

- Prerequisites: [[Processes-Fork-Exec-and-File-Descriptors]]
- Next: [[Program-Startup-C-Runtime-Init-and-Auxiliary-Vectors]]
- See also: [[01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L2-Memory-Management|P3L2 Memory Management]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_99_High_Performance_IO/Chapter_99_High_Performance_IO_io_uring_mmap_Zero_Copy|Chapter_99_High_Performance_IO_io_uring_mmap_Zero_Copy]]
- Lab: `code/00-Program-Execution/mmap_rss_vsz/` (requires: linux)
