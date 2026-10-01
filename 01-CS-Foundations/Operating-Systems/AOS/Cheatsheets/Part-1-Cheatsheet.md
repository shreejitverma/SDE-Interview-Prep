---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
tags: [cs6210, cs6210/cheatsheet]
---

# Part 1 Cheat Sheet: OS Structure and Virtualization

## OS Architecture Trade-offs

The core tension in OS design is balancing extensibility, protection, and performance.

| Architecture | Extensibility | Protection | Performance | Key Philosophy and Mechanism |
| :--- | :--- | :--- | :--- | :--- |
| **Monolithic** | Low (rigid boundary) | High (isolated from user) | High | All services run in a single privileged address space. |
| **DOS-like** | High (no boundary) | Low (no isolation) | Highest | Application and OS share one address space. |
| **Microkernel (Mach)** | High (user-space servers) | Highest (servers isolated) | Low | Services run as user-level processes causing heavy border crossings. |
| **Microkernel (L3/L4)**| High (user-space servers) | Highest (servers isolated) | High | Minimal abstractions rely on hardware-specific assembly and segment multiplexing to avoid TLB flushes. |
| **SPIN** | Excellent (in-kernel) | High (logical domains) | High | Modula-3 type safety allows dynamic in-kernel co-location via events and handlers. |
| **Exokernel** | Excellent (LibOS) | High (secure bindings) | High | Separation of protection from management allows applications to safely multiplex raw hardware via custom Library OSes. |

## Border Crossings and Context Switches

A border crossing is the transition between privilege levels or address spaces.
Explicit costs are the direct CPU cycles needed to execute a mode switch, save registers, and validate arguments.
Implicit costs are the delayed performance penalties due to cache and TLB misses after the switch.
Implicit costs are usually the dominant factor in degraded performance.

### TLB Flush Overhead Formula

When an untagged TLB is flushed during an address space switch, the penalty scales linearly with the new working set.
Total Cost = $C_{explicit} + (N_{misses} \times C_{penalty})$
$C_{explicit}$ represents the base explicit instruction cost.
$N_{misses}$ represents the number of TLB misses for the new working set.
$C_{penalty}$ represents the processor cycles required per TLB miss.

### Exokernel Software TLB

Exokernel caches virtual-to-physical mappings in a software TLB for each Library OS.
During a context switch, the kernel preloads the hardware TLB from the incoming LibOS's software TLB.
This technique significantly mitigates the cold start implicit costs of context switching.

## Virtualization Approaches

A hypervisor multiplexes hardware to run multiple independent virtual machines safely.

| Virtualization Type | Guest OS Mod? | CPU Discontinuities | Memory and Page Tables | Device I/O Data Transfer |
| :--- | :--- | :--- | :--- | :--- |
| **Full Virtualization** | No | Trap-and-emulate or dynamic binary translation. | Hypervisor manages shadow page tables. | Emulated hardware registers incur high overhead. |
| **Paravirtualization** | Yes | Hypercalls and registered exception handlers run guest in ring 1. | Guest manages tables while hypervisor validates updates. | Shared memory asynchronous I/O rings eliminate data copies. |
| **Hardware-Assisted** | No | Hardware handles traps securely via VT-x or AMD-V. | Hardware nested paging uses EPT or NPT. | Virtual Functions map directly via IOMMU using SR-IOV. |

## Xen Paravirtualization Mechanics

Xen uses a proportional-share or fair-share scheduler to multiplex the physical CPU.
Control transfer relies on synchronous hypercalls from the guest and asynchronous event channels from Xen.
Data transfer uses asynchronous I/O rings organized as circular buffers of pointers.
Data is never copied during transfer.
Xen ensures safety by pinning the machine pages during the physical transfer process.

## Memory Overcommitment (VMware ESX)

Hypervisors allocate more virtual memory than available machine memory using intelligent reclamation.
Ballooning is a cooperative memory reclamation technique.
A hypervisor-controlled pseudo-driver inside the guest allocates guest pages to force the guest OS to gracefully page out idle applications.
The hypervisor then reclaims the underlying machine pages without causing double paging.
Content-based page sharing reduces memory footprint by hashing machine pages and merging identical ones across VMs as Copy-On-Write.

### Idle Memory Tax Formula

The idle memory tax balances strict proportional sharing with working-set-based utilization.
It charges virtual machines more for retaining idle memory to optimize overall throughput.

- Adjusted Pages = Active Pages + (Idle Pages $\times$ (1 - Tax Rate))
- Effective Share Price = Assigned Shares / Adjusted Pages

When memory pressure rises, the hypervisor revokes pages from the VM with the lowest effective share price.
A high tax rate artificially deflates the denominator for idle pages.
This drastically raises their price and shifts the policy towards a pure working-set allocation.
