---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Containers and VMs for macOS Development

> [!summary] Scope
> Owns the engineering mechanics and performance costs of executing Linux containers and virtual machines on macOS hosts.
> Analyzes hypervisor virtualization (Apple Virtualization.framework, OrbStack, Lima, Colima, Docker Desktop), filesystem bind-mount I/O overhead (VirtioFS vs 9p vs Mutagen), Rosetta 2 x86-64 binary translation in Linux guests, and virtual networking bridge latencies.
> Reaches Use through Decide at scale, handing cross-platform toolchain nuances to 00-Program-Execution/macOS-vs-Linux-Toolchain-Mechanics.md.

## Related

- Prerequisites: [[macOS-vs-Linux-Toolchain-Mechanics]]
- Next: [[CLI-Design-and-Agent-Friendly-Interfaces-AXI]]
- See also: [[04-System-Design/00-Concepts/Virtual-Machines-vs-Containers|Virtual Machines vs Containers]], [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_01_Hypervisors_MicroVMs_and_Sandboxing|Chapter_01_Hypervisors_MicroVMs_and_Sandboxing]]
- Lab: `code/01-Command-Line/container_vm_io_bench/` (requires: docker)
