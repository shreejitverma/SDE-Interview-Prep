---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
course: cs6210
tags: [cs6210]
---

# Part 1 - OS Structure and Virtualization

Lessons 1-3: how to structure an OS for extensibility, protection, and performance (SPIN, Exokernel, L3), and how hypervisors virtualize memory, CPU, and devices (Xen, VMware ESX).

| Note | Concepts | Lab |
| --- | ---: | --- |
| [L01 Introduction to Advanced Operating Systems](L01-Introduction-to-AOS.md) | 6 | [lab-01-syscall-and-context-switch](../labs/lab-01-syscall-and-context-switch/README.md) |
| [L02a OS Structure Overview](L02a-OS-Structure-Overview.md) | 6 | [lab-01-syscall-and-context-switch](../labs/lab-01-syscall-and-context-switch/README.md) |
| [L02b The SPIN Approach](L02b-SPIN-Approach.md) | 10 | [lab-02-extensibility](../labs/lab-02-extensibility/README.md) |
| [L02c The Exokernel Approach](L02c-Exokernel-Approach.md) | 11 | [lab-02-extensibility](../labs/lab-02-extensibility/README.md) |
| [L02d The L3 Microkernel Approach](L02d-L3-Microkernel-Approach.md) | 12 | [lab-01-syscall-and-context-switch](../labs/lab-01-syscall-and-context-switch/README.md) |
| [L03a Introduction to Virtualization](L03a-Introduction-to-Virtualization.md) | 7 | [lab-03-virtualization](../labs/lab-03-virtualization/README.md) |
| [L03b Memory Virtualization](L03b-Memory-Virtualization.md) | 12 | [lab-03-virtualization](../labs/lab-03-virtualization/README.md) |
| [L03c CPU and Device Virtualization](L03c-CPU-and-Device-Virtualization.md) | 9 | [lab-03-virtualization](../labs/lab-03-virtualization/README.md) |

## Papers

- [Extensibility, Safety and Performance in the SPIN Operating System](../Papers/L02-SPIN.md) - SOSP 1995, required
- [Exokernel: An Operating System Architecture for Application-Level Resource Management](../Papers/L02-Exokernel.md) - SOSP 1995, required
- [On Micro-Kernel Construction](../Papers/L02-On-Microkernel-Construction.md) - SOSP 1995, required
- [Improved Address-Space Switching on Pentium Processors by Transparently Multiplexing User Address Spaces](../Papers/L02-Improved-Address-Space-Switching.md) - GMD TR 933, 1995, self-study
- [Xen and the Art of Virtualization](../Papers/L03-Xen.md) - SOSP 2003, required
- [Memory Resource Management in VMware ESX Server](../Papers/L03-VMware-ESX-Memory.md) - OSDI 2002, required

## Practice

- [Practice L01](../Practice/Practice-L01.md)
- [Practice L02](../Practice/Practice-L02.md)
- [Practice L03](../Practice/Practice-L03.md)
