---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Sandboxing and Hermetic Execution Internals

> [!summary] Scope
> Owns the operating system primitives and filesystem abstractions used to guarantee action isolation in modern build systems.
> Reaches Explain and Debug rungs by contrasting Linux namespaces and chroot, macOS Seatbelt, FUSE action trees, and containerization.
> Details how undeclared inputs and network leaks are intercepted, analyzes the I/O throughput penalty of sandbox overlay mounts, and explains differences in execution behavior between local developer laptops and remote CI workers.

## Related

- Prerequisites: [[Hermetic-and-Reproducible-Builds]]
- Next: [[Make-and-Ninja-Mechanics]]
- See also: [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock|Agent Shell Sandboxing Seatbelt and Landlock]]
- Lab: `code/06-Build-Systems/linux_sandbox/` (requires: linux)
