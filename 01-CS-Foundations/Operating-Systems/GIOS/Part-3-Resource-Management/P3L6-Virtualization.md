---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L6"
  - "Virtual Machines, Smith & Nair"
  - "Hardware and Software Support for Virtualization, Bugnion et al."
---

# P3L6: Virtualization

> **Module goal:** Master virtualization concepts - hypervisor types (Type 1/Type 2), full virtualization, paravirtualization, hardware-assisted virtualization (VT-x/AMD-V), binary translation, trap-and-emulate, containers, and the overhead tradeoffs.

## Table of Contents

- [1. What is Virtualization?](#1-what-is-virtualization)
- [2. Hypervisor Types: Type 1 and Type 2](#2-hypervisor-types-type-1-and-type-2)
- [3. Trap-and-Emulate Virtualization](#3-trap-and-emulate-virtualization)
- [4. The x86 Virtualization Problem](#4-the-x86-virtualization-problem)
- [5. Full Virtualization and Binary Translation](#5-full-virtualization-and-binary-translation)
- [6. Paravirtualization](#6-paravirtualization)
- [7. Hardware-Assisted Virtualization (VT-x, AMD-V)](#7-hardware-assisted-virtualization-vt-x-amd-v)
- [8. Memory Virtualization: Shadow and EPT](#8-memory-virtualization-shadow-and-ept)
- [9. I/O Virtualization](#9-io-virtualization)
- [10. Containers vs. Virtual Machines](#10-containers-vs-virtual-machines)
- [11. Quizzes and Exercises](#11-quizzes-and-exercises)
- [12. Key Takeaways](#12-key-takeaways)

---

## 1. What is Virtualization?

Virtualization is the creation of a **virtual version** of hardware, allowing multiple operating systems to share the same physical hardware.

```
Without Virtualization:          With Virtualization:
+------------------+             +--------+--------+--------+
|   Application    |             | App 1  | App 2  | App 3  |
+------------------+             +--------+--------+--------+
| Operating System |             | OS 1   | OS 2   | OS 3   |
+------------------+             | (Linux)| (Win)  | (Linux)|
|    Hardware      |             +--------+--------+--------+
+------------------+             |    Hypervisor (VMM)       |
                                 +---------------------------+
                                 |      Hardware             |
                                 +---------------------------+
```

**Key properties (Popek & Goldberg, 1974):**
1. **Fidelity:** Software runs identically in VM and on bare metal
2. **Safety:** The hypervisor controls all physical resources
3. **Performance:** Minimal overhead vs. native execution

---

## 2. Hypervisor Types: Type 1 and Type 2

### Type 1 (Bare-Metal)

```
+--------+--------+--------+
| VM 1   | VM 2   | VM 3   |
| (Linux)| (Win)  | (Linux)|
+--------+--------+--------+
|     Type 1 Hypervisor     |  <-- Runs directly on hardware
|     (VMM)                 |
+---------------------------+
|        Hardware            |
+---------------------------+

Examples: VMware ESXi, Xen, Microsoft Hyper-V, KVM*

*KVM is technically a kernel module that turns Linux into
a Type 1 hypervisor. It runs inside the host Linux kernel
but the guest OS runs at CPU privilege level as if on bare metal.
```

### Type 2 (Hosted)

```
+--------+--------+
| VM 1   | VM 2   |
| (Linux)| (Win)  |
+--------+--------+
|  Type 2 Hypervisor |  <-- Runs as an application
|  (VMM application) |
+--------------------+
|   Host OS          |  <-- Regular operating system
+--------------------+
|   Hardware         |
+--------------------+

Examples: VMware Workstation/Fusion, VirtualBox, QEMU, Parallels
```

| Aspect | Type 1 | Type 2 |
|--------|--------|--------|
| Performance | Better (no host OS overhead) | Worse (additional layer) |
| Complexity | More complex (must include drivers) | Simpler (reuses host drivers) |
| Use case | Data centers, cloud | Development, testing |
| Boot | Boots directly | Requires host OS to boot first |

---

## 3. Trap-and-Emulate Virtualization

The classic virtualization technique for architectures where privileged instructions trap:

```
Guest OS (thinks it's in Ring 0, actually in Ring 1 or user mode):
  |
  +-> executes privileged instruction (e.g., modify page table)
      |
      v
  CPU TRAPS (because guest is not in Ring 0)
      |
      v
  Hypervisor catches the trap (in Ring 0):
    - Decodes the instruction
    - Emulates the effect on virtual hardware
    - Updates guest's virtual state
    - Returns to guest
      |
      v
  Guest continues, unaware anything happened
```

**This works cleanly on architectures where ALL sensitive instructions are privileged** (trap when not in Ring 0).

---

## 4. The x86 Virtualization Problem

Pre-VT-x x86 had **17 instructions** that were **sensitive but not privileged** - they silently behaved differently in user mode instead of trapping:

```
Problematic instructions (examples):
  SGDT / SIDT:   Store GDT/IDT register
                  Reveals real hardware state (no trap)
  PUSHF/POPF:    Push/pop flags register
                  IF flag (interrupt enable) silently ignored in user mode
  LAR/LSL:       Load access rights / segment limit
                  Returns different values based on privilege level
  VERR/VERW:     Verify segment for read/write

Problem: Guest OS executes POPF to disable interrupts,
         but it's running in Ring 1/3, so the instruction
         silently does nothing. Guest thinks interrupts
         are disabled, but they're not!

This violates the Popek-Goldberg virtualization requirement.
```

**Solutions developed:**
1. **Binary Translation** (VMware's approach)
2. **Paravirtualization** (Xen's approach)
3. **Hardware assist** (Intel VT-x, AMD-V)

---

## 5. Full Virtualization and Binary Translation

VMware's approach: scan guest code and **rewrite** problematic instructions.

```
Guest OS code (before translation):
  mov cr3, eax        ; privileged: will trap (good)
  pushf                ; sensitive but NOT privileged (bad!)
  sgdt [ebx]           ; sensitive but NOT privileged (bad!)
  add eax, 1           ; normal instruction (fine)

After binary translation:
  mov cr3, eax        ; kept (will trap to hypervisor)
  call vmm_pushf      ; replaced with hypervisor call
  call vmm_sgdt       ; replaced with hypervisor call
  add eax, 1           ; kept (harmless)

Translation cache: translated code is cached for reuse.
```

**Performance:** User-mode code runs at native speed (no translation needed). Only kernel code (Ring 0 in guest) is translated. Overhead: ~2-10% for CPU-intensive workloads.

---

## 6. Paravirtualization

The guest OS is **modified** to call the hypervisor explicitly instead of executing privileged instructions.

```
Traditional OS:                  Paravirtualized OS:
  asm("mov cr3, eax");           hypercall(SET_PAGE_TABLE, ...);
  asm("cli");                     hypercall(DISABLE_INTERRUPTS);
  asm("hlt");                     hypercall(YIELD_CPU);

Hypercalls are like system calls, but from guest OS to hypervisor:
  Guest OS --hypercall--> Hypervisor (similar to syscall trap)
```

### Xen Paravirtualization

```
Xen architecture:
+--------+--------+--------+
| DomU   | DomU   | DomU   |  <-- Unprivileged guest VMs
| (PV    | (PV    | (HVM   |      (paravirtualized or HVM)
| Linux) | Linux) | Win)   |
+--------+--------+--------+
| Dom0 (Privileged Domain) |  <-- Runs device drivers,
| (Linux with Xen patches) |      management tools
+---------------------------+
|     Xen Hypervisor        |  <-- Thin, bare-metal hypervisor
+---------------------------+
|        Hardware            |
+---------------------------+
```

| Aspect | Full Virtualization | Paravirtualization |
|--------|-------------------|--------------------|
| Guest modification | None (unmodified OS) | Required (OS aware of hypervisor) |
| Performance | Good with HW assist | Best (minimal overhead) |
| Compatibility | Any OS | Only modified OSes |
| x86 sensitive instructions | Binary translation or VT-x | Replaced with hypercalls |

---

## 7. Hardware-Assisted Virtualization (VT-x, AMD-V)

Intel VT-x (2005) and AMD-V (2006) added a **new CPU privilege mode** for hypervisors:

```
Without VT-x:                With VT-x:
Ring 0: Hypervisor            VMX Root mode: Hypervisor (Ring 0)
Ring 1: Guest OS (problematic) VMX Non-root mode: Guest OS (Ring 0!)
Ring 3: Guest apps            Guest apps run in Ring 3 as usual

Guest OS runs in Ring 0 of VMX non-root mode.
ALL sensitive instructions trap to VMX root mode.
No binary translation or OS modification needed!
```

### VMCS (Virtual Machine Control Structure)

```
VMCS: per-VM data structure that stores:
  - Guest state (registers, CR3, IDTR, etc.)
  - Host state (hypervisor's registers)
  - VM-execution control fields (which events cause VM exits)
  - VM-exit information (why the exit occurred)

Key transitions:
  VM Entry: hypervisor -> guest (load guest state from VMCS)
  VM Exit:  guest -> hypervisor (save guest state to VMCS)

VM Exit triggers:
  - External interrupts
  - Guest executes CPUID, HLT, IN/OUT
  - Guest accesses CR3 (page table change)
  - EPT violation (page fault in nested paging)
```

```bash
# Check VT-x/AMD-V support
grep -E "vmx|svm" /proc/cpuinfo
# vmx = Intel VT-x
# svm = AMD-V

# Check KVM is available
lsmod | grep kvm
# kvm_intel or kvm_amd should be loaded
```

```powershell
# Windows: check Hyper-V support
systeminfo | findstr "Hyper-V"
# Or:
Get-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V
```

---

## 8. Memory Virtualization: Shadow and EPT

### The Two-Level Address Translation Problem

```
Guest Virtual Address (GVA)
    |
    v  Guest page table (managed by guest OS)
Guest Physical Address (GPA)
    |
    v  ??? (must translate to real physical address)
Host Physical Address (HPA)
```

### Shadow Page Tables

```
Hypervisor maintains "shadow" page tables that map GVA -> HPA directly:

Guest page table:     Shadow page table (maintained by hypervisor):
GVA -> GPA            GVA -> HPA

Guest modifies its page table -> VM exit
Hypervisor intercepts, updates shadow table accordingly
CR3 points to shadow table (hardware uses it directly)

Cost: Every guest page table update causes a VM exit (~1-5 us each)
```

### Extended Page Tables (EPT) / Nested Page Tables (NPT)

```
Hardware performs two-level page table walk:

GVA -> Guest Page Table -> GPA -> EPT/NPT -> HPA

Both translations happen in HARDWARE.
No VM exits for page table updates!

Cost: Page table walk is longer (more memory accesses)
      but TLB hides this for hot pages.

            Guest PT          EPT/NPT
GVA ------> GPA -------> HPA
  (4 levels)   (4 levels)
  = up to 24 memory accesses for a TLB miss!
  (4 guest levels * (4 EPT levels + 1) + 4 EPT levels)
  But TLB hit rate is typically >99%, so this rarely matters.
```

```bash
# Check EPT/NPT support
cat /sys/module/kvm_intel/parameters/ept    # 1 = enabled (Intel)
cat /sys/module/kvm_amd/parameters/npt      # 1 = enabled (AMD)
```

---

## 9. I/O Virtualization

### Emulated Devices

```
Guest OS writes to I/O port:
  1. VM exit (trapped by hypervisor)
  2. Hypervisor emulates the device behavior
  3. Translates to real I/O operations on host
  4. Returns result to guest

Example: Guest thinks it has an Intel e1000 NIC.
Hypervisor emulates e1000 register interface.
Actual I/O goes through host's real NIC driver.

Overhead: HIGH (VM exit per I/O operation)
```

### Virtio (Paravirtualized I/O)

```
Guest knows it's virtualized and uses virtio drivers:

Guest OS                     Hypervisor/Host
+------------------+         +------------------+
| virtio-net driver|         | vhost-net        |
| virtio-blk driver|         | vhost-blk        |
+--------+---------+         +--------+---------+
         |                            |
     virtqueue (shared memory ring buffer)
         |                            |
  Notifications (doorbell) via MMIO or MSI
```

### SR-IOV (Single Root I/O Virtualization)

```
Physical NIC with SR-IOV:
+------------------------------------------+
| Physical Function (PF)                    |
|   Full NIC driver in host                 |
+------------------------------------------+
| VF 0  | VF 1  | VF 2  | VF 3  | ...    |
| (VM 0)| (VM 1)| (VM 2)| (VM 3)|         |
+-------+-------+-------+-------+---------+

Each VM gets a Virtual Function (VF) - direct hardware access.
No hypervisor involvement for data path!
Near-native performance.
```

---

## 10. Containers vs. Virtual Machines

```
Virtual Machines:                    Containers:
+--------+--------+--------+        +--------+--------+--------+
| App A  | App B  | App C  |        | App A  | App B  | App C  |
+--------+--------+--------+        +--------+--------+--------+
| Bins/  | Bins/  | Bins/  |        | Bins/  | Bins/  | Bins/  |
| Libs   | Libs   | Libs   |        | Libs   | Libs   | Libs   |
+--------+--------+--------+        +--------+--------+--------+
| Guest  | Guest  | Guest  |        |    Container Engine       |
| OS     | OS     | OS     |        |    (Docker, containerd)   |
+--------+--------+--------+        +---------------------------+
|    Hypervisor             |        |    Host OS Kernel         |
+---------------------------+        +---------------------------+
|    Hardware               |        |    Hardware               |
+---------------------------+        +---------------------------+
```

| Aspect | VM | Container |
|--------|-----|-----------|
| Isolation | Strong (separate kernel) | Weaker (shared kernel) |
| Overhead | High (full OS per VM) | Low (shared kernel) |
| Boot time | Minutes | Seconds |
| Memory | GBs per VM | MBs per container |
| Security | Hardware-level isolation | Namespace/cgroup-based |
| OS diversity | Different OS per VM | Same kernel, different userspace |

### Linux Containers: Namespaces and Cgroups

```
Namespaces (isolation):
  PID namespace:   Container sees only its own processes (PID 1 inside)
  Mount namespace: Container has its own filesystem view
  Network namespace: Container has its own IP, routing table
  UTS namespace:   Container has its own hostname
  User namespace:  Container can map UIDs (root inside != root outside)
  IPC namespace:   Separate message queues, semaphores

Cgroups (resource limits):
  CPU:     Limit CPU time (cpu.max)
  Memory:  Limit memory usage (memory.max)
  I/O:     Limit disk bandwidth (io.max)
  PIDs:    Limit number of processes (pids.max)
```

```bash
# See namespaces of a process
ls -la /proc/$PID/ns/

# Create a simple namespace (unshare)
sudo unshare --pid --fork --mount-proc bash
# Now in a new PID namespace; only bash and its children are visible

# See cgroup limits
cat /sys/fs/cgroup/system.slice/docker-$CONTAINER_ID.scope/memory.max
cat /sys/fs/cgroup/system.slice/docker-$CONTAINER_ID.scope/cpu.max
```

---

## 11. Quizzes and Exercises

> **Quiz: Virtualization Approaches**
>
> *Match each approach to its key mechanism:*
> | Approach | Mechanism |
> |----------|-----------|
> | Full virtualization (VMware) | Binary translation of sensitive instructions |
> | Paravirtualization (Xen) | Hypercalls replacing privileged instructions |
> | Hardware-assisted (KVM) | VT-x/AMD-V VMX non-root mode |

> **Quiz: EPT vs. Shadow Page Tables**
>
> *EPT/NPT has higher TLB miss cost but lower overall overhead. Why?*
>
> **Answer:** Shadow page tables require a **VM exit on every guest page table modification** (expensive: ~1-5 us each). EPT eliminates these exits entirely. Although an EPT TLB miss requires up to 24 memory accesses (nested 4-level walk), TLB hit rates are >99%, so this penalty is rarely paid. The savings from avoiding VM exits far outweigh the occasional longer TLB miss.

---

## 12. Key Takeaways

1. **Type 1 hypervisors** run on bare metal (ESXi, Xen, KVM); Type 2 run on a host OS (VirtualBox, QEMU).
2. **Trap-and-emulate** works when all sensitive instructions are privileged; x86 pre-VT-x had 17 that weren't.
3. **Binary translation** (VMware) rewrites problematic instructions; **paravirtualization** (Xen) modifies the guest OS to use hypercalls.
4. **VT-x/AMD-V** added VMX root/non-root modes, solving the x86 problem in hardware and enabling guest OS to run in Ring 0.
5. **EPT/NPT** provides hardware two-level page table translation, eliminating the overhead of shadow page tables.
6. **Virtio** provides efficient paravirtualized I/O; **SR-IOV** gives near-native I/O by assigning hardware directly to VMs.
7. **Containers** (Docker, Kubernetes) share the host kernel via namespaces/cgroups - lighter than VMs but weaker isolation.

---

## 13. KVM Internals and QEMU Integration

KVM (Kernel-based Virtual Machine) is a Linux kernel module that exposes `/dev/kvm` for user-space hypervisors.

```
KVM Architecture:
+----------------------------------+
|         QEMU (user space)        |  Manages VM: devices, migration, snapshots
|   +-------+   +--------+        |
|   | vCPU  |   | Device |        |
|   | thread|   | models |        |
|   +---+---+   +--------+        |
+-------|---------------------------+
        | ioctl(/dev/kvm)
+-------|----------------------------+
|       v   Linux Kernel            |
|   +--------+                      |
|   |  KVM   |  VM exits → handler  |
|   | module |  EPT management      |
|   +---+----+  interrupt injection  |
+-------|----------------------------+
        | VMX instructions (hardware)
+-------|----------------------------+
|  CPU: | VMX root (KVM) /          |
|       | VMX non-root (guest OS)   |
+----------------------------------+
```

```bash
# Check KVM support
egrep -c '(vmx|svm)' /proc/cpuinfo   # > 0 means supported
lsmod | grep kvm

# Load KVM modules
modprobe kvm
modprobe kvm_intel  # or kvm_amd

# KVM capabilities
cat /proc/cpuinfo | grep -E "vmx|svm|ept|npt"

# List VMs (libvirt)
virsh list --all

# Create a KVM VM with QEMU directly
qemu-system-x86_64 \
    -enable-kvm \
    -cpu host \                      # Pass through host CPU features
    -m 4096 \                        # 4GB RAM
    -smp 4 \                         # 4 vCPUs
    -drive file=disk.qcow2,format=qcow2,if=virtio \  # Virtio disk
    -netdev user,id=net0 \
    -device virtio-net,netdev=net0 \ # Virtio NIC
    -vga virtio \
    -nographic \
    -serial mon:stdio

# VM exit statistics (per-VCPU)
# Install qemu debug build or use perf
perf kvm stat live     # Live VM exit stats (requires perf kvm support)
perf kvm stat report   # After perf kvm record

# KVM VM exit types (what causes exits):
# EXTERNAL_INTERRUPT - host interrupt
# HLT               - guest executed HLT (idle)
# IO_INSTRUCTION    - PIO access
# CPUID             - guest executed CPUID
# EPT_VIOLATION     - page fault in EPT (cold mapping)
```

### vCPU Overcommit and Scheduling

```bash
# vCPU-to-pCPU ratio
# Typical recommendation: 2-4:1 for general workloads
# High compute: 1:1 (no overcommit)

# Pin vCPUs to specific physical CPUs
virsh vcpupin <domain> <vcpu> <cpu-list>
virsh vcpupin myvm 0 0-3    # vCPU 0 → pCPUs 0,1,2,3

# Emulator pin (QEMU threads, not vCPUs)
virsh emulatorpin myvm 4-7

# NUMA topology for VMs (critical for memory performance)
virsh numatune myvm --nodeset 0 --mode strict

# Check NUMA node for VM memory
numastat -p $(pgrep qemu)

# Transparent Huge Pages for VMs (reduces EPT walker overhead)
echo always > /sys/kernel/mm/transparent_hugepage/enabled  # Host
# In VM: same setting
```

---

## 14. VM Live Migration Deep Dive

Live migration moves a running VM to another host without downtime.

```
Live Migration Phases (Pre-copy algorithm):

Phase 1: Setup
  Source                          Destination
    |                                 |
    |--- establish connection -------->|
    |--- send VM config -------------->|
    |                                 |

Phase 2: Memory Pre-copy (iterative)
    |--- send dirty pages ------------>|
    |   (while guest runs)            |
    |--- send newly-dirtied pages ---->|  (repeat until small enough)
    |                                 |

Phase 3: Stop-and-copy (brief pause)
    |--- STOP guest  ----------------->|
    |--- send remaining dirty pages -->|
    |--- send CPU state (registers) -->|
    |--- send device state ----------->|
    |                                 |

Phase 4: Resume
    |                   RESUME guest  |
    |--- redirect network traffic ---->|
```

```bash
# Live migration with virsh
virsh migrate --live myvm qemu+ssh://destination-host/system

# Verbose migration status
virsh domjobinfo myvm

# Postcopy migration (switch to on-demand page fetching)
virsh migrate --live --postcopy myvm qemu+ssh://dest/system

# Migration with compression (slower but less bandwidth)
virsh migrate --live --compressed myvm qemu+ssh://dest/system

# Set bandwidth limit
virsh migrate-setmaxdowntime myvm 100  # Max 100ms pause
virsh migrate-setspeed myvm 1024       # 1 GB/s max bandwidth
```

---

## 15. Container Internals: Namespaces and Cgroups v2

### Linux Namespaces

Each namespace type isolates a specific aspect of the system:

| Namespace | Flag | Isolates | `/proc` view |
|-----------|------|----------|-------------|
| `pid` | CLONE_NEWPID | Process tree | `/proc/<pid>/status` |
| `net` | CLONE_NEWNET | Network stack | `/proc/<pid>/net/` |
| `mnt` | CLONE_NEWNS | Mount points | `/proc/<pid>/mounts` |
| `uts` | CLONE_NEWUTS | Hostname, domainname | `uname -n` |
| `ipc` | CLONE_NEWIPC | SysV IPC, POSIX MQ | `ipcs` |
| `user` | CLONE_NEWUSER | UID/GID mapping | `/proc/<pid>/uid_map` |
| `cgroup` | CLONE_NEWCGROUP | cgroup root | `/proc/<pid>/cgroup` |
| `time` | CLONE_NEWTIME | Clock offsets | `/proc/<pid>/timens_offsets` |

```bash
# Create a new namespace (unshare)
unshare --pid --fork --mount-proc bash
echo "My PID: $$"    # Will be 1 inside the namespace!
ps aux               # Only sees processes in this namespace

# Full container-like environment
unshare --pid --fork --mount-proc --net --uts --ipc bash

# Inspect a process's namespaces
ls -la /proc/$(pgrep docker)/ns/
# lrwxrwxrwx pid  → pid:[4026531836]
# lrwxrwxrwx net  → net:[4026531999]
# lrwxrwxrwx mnt  → mnt:[4026531840]

# Enter a running container's namespace
nsenter --target $(pgrep nginx) --pid --net --mnt -- bash
# Now you're inside nginx's namespace!

# lsns: list all namespaces
lsns

# View container PID in host namespace
docker inspect --format='{{.State.Pid}}' <container>

# Create a minimal container by hand
ip netns add mynet                   # Create network namespace
ip link add veth0 type veth peer name veth1
ip link set veth1 netns mynet
ip netns exec mynet ip addr add 10.0.0.1/24 dev veth1
ip netns exec mynet ip link set veth1 up
ip netns exec mynet bash             # Enter namespace
```

### Cgroups v2 (Unified Hierarchy)

```bash
# Cgroups v2 unified hierarchy
mount | grep cgroup2
# cgroup2 on /sys/fs/cgroup type cgroup2

# Create a cgroup
mkdir /sys/fs/cgroup/myapp

# Set limits
echo "2048M" > /sys/fs/cgroup/myapp/memory.max       # 2GB RAM limit
echo "max"   > /sys/fs/cgroup/myapp/memory.swap.max  # Unlimited swap
echo "500000 1000000" > /sys/fs/cgroup/myapp/cpu.max # 50% CPU (500ms per 1s period)
echo "100"   > /sys/fs/cgroup/myapp/cpu.weight        # CPU shares (1-10000)
echo "rbps=104857600 wbps=104857600" > /sys/fs/cgroup/myapp/io.max  # 100MB/s R+W

# Add process to cgroup
echo $$ > /sys/fs/cgroup/myapp/cgroup.procs

# Monitor cgroup statistics
cat /sys/fs/cgroup/myapp/memory.current   # Current usage
cat /sys/fs/cgroup/myapp/memory.stat      # Detailed breakdown
cat /sys/fs/cgroup/myapp/cpu.stat         # CPU usage
cat /sys/fs/cgroup/myapp/io.stat          # I/O usage

# PSI (Pressure Stall Information) - detect resource pressure
cat /sys/fs/cgroup/myapp/memory.pressure
# some avg10=0.00 avg60=0.00 avg300=0.00 total=0
# full avg10=0.00 avg60=0.00 avg300=0.00 total=0
# "full" = ALL tasks stalled; "some" = at least one stalled

cat /proc/pressure/cpu      # System-wide CPU pressure
cat /proc/pressure/memory   # System-wide memory pressure
cat /proc/pressure/io       # System-wide I/O pressure

# Docker uses cgroups automatically
docker stats                                      # Live stats
docker inspect --format='{{.HostConfig}}' mycontainer | python3 -m json.tool
cat /sys/fs/cgroup/system.slice/docker-$(docker inspect --format='{{.Id}}' mycontainer).scope/memory.current

# Kubernetes resource limits → cgroup limits
kubectl describe pod mypod | grep -A5 Limits
# resources:
#   limits:
#     cpu: "2"         → cpu.max = 200000 100000
#     memory: 512Mi    → memory.max = 536870912
```

### OCI Container Runtime Internals

```bash
# runc: the reference container runtime
# See what Docker does under the hood:
strace -f docker run -it --rm ubuntu bash 2>&1 | grep -E "clone|unshare|setns"

# runc directly (from OCI bundle)
mkdir -p bundle/rootfs
docker export $(docker create ubuntu) | tar -xC bundle/rootfs
cd bundle
runc spec  # Generate config.json
runc run mycontainer

# Runtime lifecycle:
# 1. runc creates namespaces (clone with CLONE_NEW*)
# 2. Sets up rootfs (pivot_root or chroot)
# 3. Applies seccomp profile
# 4. Drops capabilities (cap_drop_all + cap_add specific)
# 5. Sets resource limits (cgroups)
# 6. Exec's the container entrypoint

# Security: seccomp filter (syscall whitelist)
docker run --security-opt seccomp=profile.json ubuntu bash

# Capabilities
docker run --cap-drop ALL --cap-add NET_BIND_SERVICE nginx
# Check capabilities of a running process:
cat /proc/<pid>/status | grep Cap
capsh --decode=$(cat /proc/<pid>/status | grep CapEff | awk '{print $2}')
```

---

## 16. eBPF: Observability for Virtualization

eBPF programs run safely in the kernel and are essential for VM/container observability:

```bash
# Install bcc-tools (eBPF toolkit)
apt install bpfcc-tools linux-headers-$(uname -r)

# Trace all execve calls (see what containers spawn)
execsnoop-bpfcc

# Trace open() calls with latency
opensnoop-bpfcc -p $(pgrep -f "docker")

# File I/O latency histogram for all processes
fileslower-bpfcc 10    # Show I/O taking > 10ms

# TCP connections being made
tcpconnect-bpfcc

# Network packet tracing
tcptracer-bpfcc

# OOM killer events
oomkill-bpfcc

# Custom eBPF: trace VM exits (requires KVM tracepoints)
sudo bpftrace -e '
    tracepoint:kvm:kvm_exit {
        @exits[args->exit_reason] = count();
    }
    interval:s:5 {
        print(@exits);
        clear(@exits);
    }'

# eBPF for container isolation verification
# Ensure container cannot access host PIDs:
sudo bpftrace -e '
    tracepoint:syscalls:sys_enter_kill {
        if (args->sig != 0) {
            printf("PID %d trying to kill PID %d\n", pid, args->pid);
        }
    }'
```

---

## 17. Hyper-V Internals and Windows Virtualization

Microsoft Hyper-V is a Type-1 hypervisor built into Windows Server and client.

```
Hyper-V Architecture:

+---------------------------------------------------+
|  Parent Partition (Root Partition)                 |
|  +-----------+  +-----------+  +-----------+      |
|  | Windows   |  | VMBus     |  | VMWP      |      |
|  | OS        |  | (virt bus)|  | (VM Worker|      |
|  +-----------+  +-----------+  | Process)  |      |
+---------|-----------|-------|----|----------|------+
          │           │       │    │          │
+---------▼-----------▼-------▼----▼----------▼-----+
|              Hyper-V Hypervisor (Ring -1)           |
|   VT-x/AMD-V hardware virtualization support        |
+---------|-----------|-----------|-----------|-------+
          │           │           │           │
+----+----▼---+  +----▼---+  +---▼----+  +---▼----+
| Child       |  | Child  |  | Child  |  | Child  |
| Partition 1 |  | Part 2 |  | Part 3 |  | Part 4 |
| (VM1)       |  | (VM2)  |  | (VM3)  |  | (VM4)  |
+-------------+  +--------+  +--------+  +--------+

Key differences from KVM:
- Hyper-V: runs in Ring -1 (VMX root), Windows in Ring 0 (VMX non-root parent)
- KVM: Linux kernel IS the hypervisor; VMs are processes
- Hyper-V integrates tighter with Windows hardware abstraction layer (HAL)
```

```powershell
# Hyper-V management (PowerShell)
# Enable Hyper-V feature
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All

# Create a VM
New-VM -Name "UbuntuVM" `
       -MemoryStartupBytes 4GB `
       -Generation 2 `
       -NewVHDPath "C:\VMs\ubuntu.vhdx" `
       -NewVHDSizeBytes 50GB `
       -SwitchName "Default Switch"

# Configure VM
Set-VM -Name "UbuntuVM" -ProcessorCount 4
Set-VMMemory -VMName "UbuntuVM" -DynamicMemoryEnabled $true `
             -MinimumBytes 1GB -MaximumBytes 8GB

# Start/Stop
Start-VM -Name "UbuntuVM"
Stop-VM -Name "UbuntuVM"

# Live migration (between Hyper-V hosts)
Move-VM -Name "UbuntuVM" -DestinationHost "server2.domain.com" `
        -DestinationStoragePath "C:\VMs\"

# Checkpoints (snapshots)
Checkpoint-VM -Name "UbuntuVM" -SnapshotName "BeforeUpgrade"
Restore-VMCheckpoint -VMName "UbuntuVM" -Name "BeforeUpgrade"
Remove-VMCheckpoint -VMName "UbuntuVM" -Name "BeforeUpgrade"

# VM performance counters
Get-VM -Name "UbuntuVM" | Measure-VM
# ComputerName  VMName   AvgCPU AvgRam AvgDiskRead AvgDiskWrite AvgNetInbound AvgNetOutbound

# Hyper-V events
Get-WinEvent -LogName "Microsoft-Windows-Hyper-V-Worker-Admin" | Select-Object -First 20
Get-WinEvent -LogName "Microsoft-Windows-Hyper-V-VMMS-Admin"   | Select-Object -First 20

# Nested virtualization (VM inside VM)
Set-VMProcessor -VMName "UbuntuVM" -ExposeVirtualizationExtensions $true

# SR-IOV for network passthrough
Set-VMNetworkAdapter -VMName "UbuntuVM" -IovWeight 100

# vNUMA topology exposure
Set-VMMemory -VMName "UbuntuVM" -MaximumAmountPerNumaNodeBytes 4GB
```

```bash
# On Linux guest inside Hyper-V: detect and optimize
dmesg | grep -i hyperv             # Detect Hyper-V environment
lsmod | grep hv_                   # Hyper-V drivers
# hv_vmbus: VMBus communication
# hv_storvsc: virtual SCSI (storage)
# hv_netvsc: virtual network
# hv_utils: heartbeat, timesync

# Hyper-V clock source (more accurate than TSC in VMs)
cat /sys/devices/system/clocksource/clocksource0/current_clocksource
# hyperv_clocksource_tsc_page  (best option on Hyper-V)

# Check VMBus channel assignments
ls /sys/bus/vmbus/devices/
cat /sys/bus/vmbus/devices/*/class_id
```

---

## 18. SR-IOV: Single Root I/O Virtualization

SR-IOV allows a single physical NIC/GPU to appear as multiple PCIe devices - bypassing the hypervisor for VMs.

```
SR-IOV Architecture:

Physical NIC (PF: Physical Function)
    │
    ├── VF0 (Virtual Function 0) → VM 1 (direct DMA, no hypervisor copy!)
    ├── VF1 (Virtual Function 1) → VM 2
    ├── VF2 (Virtual Function 2) → VM 3
    └── VF3 (Virtual Function 3) → VM 4

Performance comparison (10Gbps NIC):
  virtio (emulated):  ~5Gbps, ~50μs latency
  SR-IOV (passthrough): ~9.5Gbps, ~5μs latency  (10x lower latency!)
```

```bash
# Check SR-IOV support
lspci | grep -i net
ethtool -i eth0 | grep driver       # Check driver

# Check if device supports SR-IOV
lspci -v -s <device_id> | grep "SR-IOV"
cat /sys/bus/pci/devices/<device>/sriov_totalvfs    # Max VFs supported

# Create Virtual Functions
echo 4 > /sys/bus/pci/devices/0000:01:00.0/sriov_numvfs   # Create 4 VFs
ip link show dev eth0                                       # See VFs
lspci | grep "Virtual Function"

# Assign VF to VM (KVM)
virsh attach-device myvm --file vf_device.xml
# vf_device.xml:
# <hostdev mode='subsystem' type='pci' managed='yes'>
#   <source><address domain='0' bus='1' slot='0' function='1'/></source>
# </hostdev>

# VFIO: kernel driver for safe passthrough
modprobe vfio-pci
echo "1234 5678" > /sys/bus/pci/drivers/vfio-pci/new_id  # vendor/device
```

---

## 19. Live Migration: Deep Dive

Live migration moves a running VM between hypervisor hosts with near-zero downtime.

```
Pre-Copy Algorithm (most common):

Phase 1: ENABLE LOGGING
  - Enable dirty page tracking on source VM
  - VM continues running at full speed

Phase 2: ITERATIVE PRE-COPY
  Iteration 1: Copy ALL memory (say 8GB) to destination
  Iteration 2: Copy pages dirtied during iteration 1 (~20% = 1.6GB)
  Iteration 3: Copy pages dirtied during iteration 2 (~20% = 320MB)
  ...
  Stop when: dirty_pages < threshold (e.g., < 50MB) OR iterations > max

Phase 3: STOP-AND-COPY (blackout phase)
  - Pause source VM (downtime START)
  - Copy remaining dirty pages (~50MB)
  - Transfer CPU state, device state, network state
  - Redirect network to destination
  - Resume destination VM (downtime END)
  - Total blackout: 50MB / 10Gbps network = ~40ms downtime!

Phase 4: CLEANUP
  - Free source VM memory

Factors affecting migration time:
  - Memory size: larger = longer iterative phase
  - Dirty rate: high CPU/write VMs dirty more pages
  - Network bandwidth: 10Gbps vs 1Gbps = 10x faster
  - Compression: WAN migrations compress dirty pages
```

```bash
# KVM live migration with QEMU monitor
# On source host:
(qemu) migrate tcp:destination-host:4444
(qemu) info migrate
# Migration status: active
# transferred ram: 3456 MB / 8192 MB  (42%)
# remaining ram: 4736 MB
# dirty pages rate: 52423 pages/second

# Wait for completion
(qemu) info migrate
# Migration status: completed
# transferred ram: 8320 MB  (more than RAM due to dirty pages re-sent)
# downtime: 38 ms

# virsh live migration
virsh migrate --live myvm qemu+ssh://destination/system

# With compression (for WAN migration)
virsh migrate --live --compressed myvm qemu+ssh://dest/system

# Post-copy migration (opposite of pre-copy)
# - Immediately start VM on destination with no memory
# - Page faults trigger network fetch from source
# - Lower downtime but risk of failure if network drops
virsh migrate --live --postcopy myvm qemu+ssh://dest/system

# Migration bandwidth limit (to avoid saturating production network)
(qemu) migrate_set_speed 500M   # Limit to 500 MB/s

# Monitor migration progress
watch -n1 'virsh domjobinfo myvm'
# Job type:         Unbounded
# Operation:        Outgoing Migration
# Time elapsed:     12345   ms
# Data processed:   4096 MiB
# Data remaining:   3800 MiB
# Memory total:     8192 MiB
# Memory processed: 6234 MiB
# Memory dirty rate:  48765 pages/second
# Expected downtime:    38 ms
```

---

## 20. OCI Container Runtime and Kubernetes Internals

```
Kubernetes → CRI (Container Runtime Interface)
                    │
            ┌───────┴───────┐
            │               │
        containerd       CRI-O
            │               │
        runc (OCI)       runc (OCI)
            │               │
       Linux Kernel    Linux Kernel
   (namespaces, cgroups, seccomp)

OCI Runtime Spec: defines lifecycle hooks
  create → start → running → (kill signal) → stopped → delete
```

```bash
# containerd: the standard CRI implementation
# Inspect running containers
ctr containers list
ctr tasks list                     # Running processes

# containerd snapshots (union filesystem layers)
ctr snapshots list
ctr snapshots info <sha256>

# Pull and run an OCI image
ctr image pull docker.io/library/nginx:latest
ctr run --rm docker.io/library/nginx:latest nginx

# runc: the low-level OCI runtime
# Create and start a container from an OCI bundle
mkdir -p /run/mycontainer/rootfs
# Populate rootfs...
cd /run/mycontainer
runc spec                          # Generate config.json template
runc create mycontainer            # Create (allocate namespaces, not started)
runc start mycontainer             # Start (run entrypoint)
runc state mycontainer             # Check status
runc kill mycontainer SIGTERM      # Send signal
runc delete mycontainer            # Clean up

# Inspect container namespaces from host
PID=$(runc state mycontainer | jq -r .pid)
ls -la /proc/$PID/ns/
# cgroup   ipc    mnt    net    pid    user   uts
nsenter --target $PID --net -- ip addr show   # Enter network namespace
nsenter --target $PID --mnt -- ls /proc       # Enter mount namespace

# cgroup v2: inspect container resource usage
find /sys/fs/cgroup -name "*.service" | xargs grep . 2>/dev/null | head -20
cat /sys/fs/cgroup/system.slice/containerd.service/memory.current
cat /sys/fs/cgroup/system.slice/containerd.service/cpu.stat

# Kubernetes: inspect pod resources
kubectl get pods -n kube-system -o yaml | grep -A 10 resources
kubectl top pods --all-namespaces      # CPU/memory actual usage
kubectl describe node <nodename>       # Node capacity and allocations

# Kubernetes cgroup hierarchy
# /sys/fs/cgroup/kubepods/
#   burstable/          (pods with requests < limits)
#     pod<uid>/         (per-pod cgroup)
#       <container>/    (per-container cgroup)
#   guaranteed/         (requests == limits)
#   besteffort/         (no requests set)

# Seccomp: restrict syscalls in containers
docker run --security-opt seccomp=/path/to/profile.json nginx

# Default Docker seccomp profile blocks ~44 syscalls
# Including: kexec, mount, swapon, reboot, pivot_root

# AppArmor: MAC for containers
cat /sys/kernel/security/apparmor/profiles | grep docker

# OCI image inspection
skopeo inspect docker://nginx:latest
# Shows: layers, config, architecture, OS

# Container image layers (union filesystem)
docker history nginx:latest
docker save nginx:latest | tar -xv  # See layer tarballs
# Each layer is a tar.gz of filesystem changes
```

---

**Previous:** [P3L5: I/O Management](P3L5-IO-Management.md)
**Next:** [P4L1: Remote Procedure Calls](../Part-4-Distributed-Systems/P4L1-Remote-Procedure-Calls.md)


