---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P1L2"
  - "Operating System Concepts, 10th Ed., Silberschatz, Galvin, Gagne"
  - "Linux Kernel Development, 3rd Ed., Robert Love"
  - "Windows Internals, 7th Ed., Russinovich, Solomon, Ionescu"
---

# P1L2: Introduction to Operating Systems

> **Module goal:** Define what an operating system is, understand its dual roles of abstraction and arbitration, learn the user/kernel boundary and system call mechanism, and compare monolithic, microkernel, and hybrid OS organizations.

## Table of Contents

- [1. What is an Operating System?](#1-what-is-an-operating-system)
- [2. OS Roles: Abstraction and Arbitration](#2-os-roles-abstraction-and-arbitration)
- [3. An Operating System is Like...](#3-an-operating-system-is-like)
- [4. Direct Execution vs. Protected Execution](#4-direct-execution-vs-protected-execution)
- [5. User Mode vs. Kernel Mode](#5-user-mode-vs-kernel-mode)
- [6. System Calls and Trap Handling](#6-system-calls-and-trap-handling)
- [7. Crossing the User/Kernel Boundary](#7-crossing-the-uskernel-boundary)
- [8. OS Services Overview](#8-os-services-overview)
- [9. OS Organization: Monolithic Kernels](#9-os-organization-monolithic-kernels)
- [10. OS Organization: Microkernels](#10-os-organization-microkernels)
- [11. Hybrid Architectures (Modular/Layered)](#11-hybrid-architectures-modularlayered)
- [12. Linux Architecture Overview](#12-linux-architecture-overview)
- [13. macOS and Windows Architecture Overview](#13-macos-and-windows-architecture-overview)
- [14. Quizzes and Exercises](#14-quizzes-and-exercises)
- [15. Key Takeaways](#15-key-takeaways)

---

## 1. What is an Operating System?

An **operating system (OS)** is a layer of systems software that sits between hardware and applications.
It provides two fundamental services:

1. **Abstraction** - hides hardware complexity behind clean interfaces
2. **Arbitration** - manages and allocates shared resources among competing applications

```
+---------------------------------------------------+
|                  Applications                      |
|  (web browser, database, compiler, game, shell)    |
+---------------------------------------------------+
|              Operating System                      |
|  +---------------------------------------------+  |
|  | System Libraries (libc, Win32, NTDLL)        |  |
|  +---------------------------------------------+  |
|  | Kernel                                       |  |
|  |  - Process management                        |  |
|  |  - Memory management                         |  |
|  |  - File systems                              |  |
|  |  - Device drivers                            |  |
|  |  - Networking stack                           |  |
|  +---------------------------------------------+  |
+---------------------------------------------------+
|                  Hardware                          |
|  CPU(s)  |  RAM  |  Disk  |  NIC  |  GPU  | ...   |
+---------------------------------------------------+
```

### Formal Definition

An OS is a **resource manager** that:
- Multiplexes hardware resources (CPU time, memory, I/O devices) across applications
- Protects applications from each other and from corrupting the OS itself
- Provides a portable, uniform interface so applications don't depend on specific hardware

### What an OS is NOT

- Not a single program - it's a collection of cooperating components
- Not the same as a shell (bash, PowerShell) - the shell is a user-space program that talks to the OS
- Not the same as a desktop environment (GNOME, Windows Explorer) - that's a user-space application

---

## 2. OS Roles: Abstraction and Arbitration

### Abstraction

The OS hides hardware details behind uniform, well-defined interfaces.

**Example: File I/O**

Without the OS, writing to a disk would require:
- Knowing the disk controller's register addresses
- Computing cylinder/head/sector addresses (or LBA blocks)
- Programming DMA controllers
- Handling interrupts for completion

With the OS, you call `write()`:

**Linux:**
```c
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int fd = open("/tmp/test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    const char *msg = "Hello from Linux\n";
    write(fd, msg, 17);
    close(fd);
    return 0;
}
```

```bash
# Compile and run
gcc -o file_write file_write.c && ./file_write
cat /tmp/test.txt
```

**Windows (Win32 API):**
```c
#include <windows.h>
#include <stdio.h>

int main(void) {
    HANDLE hFile = CreateFileA(
        "C:\\temp\\test.txt",
        GENERIC_WRITE,
        0,                          // no sharing
        NULL,                       // default security
        CREATE_ALWAYS,              // overwrite if exists
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("CreateFile failed: %lu\n", GetLastError());
        return 1;
    }
    const char *msg = "Hello from Windows\r\n";
    DWORD written;
    WriteFile(hFile, msg, 20, &written, NULL);
    CloseHandle(hFile);
    return 0;
}
```

```powershell
# Compile with MSVC
cl /Fe:file_write.exe file_write.c
.\file_write.exe
Get-Content C:\temp\test.txt
```

The application code looks nearly identical despite the underlying hardware potentially being a SATA SSD, NVMe drive, network-attached storage, or even a RAM disk.
The OS abstracts all of that away.

### Arbitration

The OS decides **who** gets **what** resource, **when**, and for **how long**.

| Resource | What the OS Arbitrates |
|----------|----------------------|
| CPU | Which process/thread runs (scheduling) |
| Memory | Which pages are resident, which are swapped (virtual memory) |
| Disk I/O | Order of block requests (disk scheduling) |
| Network | Socket buffer allocation, bandwidth sharing |
| GPU | Context switching between GPU workloads |

**Example: Two processes competing for CPU**

```
Time ------>

CPU:  [  Process A  ][  Process B  ][  Process A  ][  Process B  ]
       quantum=10ms    quantum=10ms   quantum=10ms   quantum=10ms

The OS scheduler arbitrates: each process gets a fair time slice.
Without arbitration, Process A could monopolize the CPU forever.
```

**Linux - see current scheduler:**
```bash
# Check scheduling policy for a process
chrt -p $$                    # current shell's scheduling policy
cat /proc/$$/sched            # detailed scheduler stats

# See all processes' scheduler info
ps -eo pid,cls,pri,ni,comm --sort=-pri | head -20
```

**Windows - see scheduling priority:**
```powershell
# Check process priority
Get-Process -Id $PID | Select-Object ProcessName, PriorityClass, BasePriority

# All processes sorted by priority
Get-Process | Sort-Object -Property BasePriority -Descending |
  Select-Object -First 20 ProcessName, BasePriority, CPU
```

---

## 3. An Operating System is Like...

The course uses a **visual metaphor**: an OS is like a **toy shop manager**.

| Toy Shop Element | OS Element |
|-----------------|------------|
| Toy shop building | Hardware (CPU, memory, disk) |
| Toys being built | Applications / processes |
| Workers | Threads |
| Workbench area | Address space / memory |
| Raw materials | Hardware resources (CPU cycles, memory pages, I/O bandwidth) |
| Shop manager | The OS kernel |

The manager (OS) must:
- **Abstract** the complexity of the shop (workers don't need to know how the supply chain works)
- **Arbitrate** resources (decide which worker gets which workbench, how materials are shared)
- **Protect** workers from each other (one worker's mistake shouldn't ruin another's work)
- **Schedule** workers efficiently (no one should be idle when work is available)

This metaphor recurs throughout the course to motivate each new concept.

---

## 4. Direct Execution vs. Protected Execution

### Direct Execution

In direct execution, applications run directly on the hardware with no restrictions.

```
Application Code -----> CPU (unrestricted)
                         |
                         v
                    All hardware accessible
```

**Problems:**
- A buggy application can overwrite the OS itself
- A malicious application can read any process's memory
- One application can monopolize the CPU indefinitely
- No isolation between applications

**Historical example:** MS-DOS used direct execution. A single application crash could require a full reboot.

### Protected Execution

The OS restricts what applications can do by leveraging **hardware support** for privilege levels.

```
+---------------------------+   Ring 3 (User Mode)
| Application               |   - Cannot access hardware directly
| - computation             |   - Cannot execute privileged instructions
| - library calls           |   - Limited to its own address space
+---------------------------+
         | system call (trap)
         v
+---------------------------+   Ring 0 (Kernel Mode)
| OS Kernel                 |   - Full hardware access
| - validates request       |   - Can execute any instruction
| - performs operation       |   - Manages all address spaces
| - returns result           |
+---------------------------+
         |
         v
     Hardware
```

All modern general-purpose operating systems (Linux, Windows, macOS, BSD) use protected execution.

---

## 5. User Mode vs. Kernel Mode

The CPU hardware supports (at minimum) two privilege levels:

### x86 Protection Rings

```
        +-------------------+
        |     Ring 3        |  <-- User mode (applications)
        |  +-----------+    |
        |  |  Ring 2    |   |  <-- (rarely used on modern OS)
        |  | +-------+  |  |
        |  | | Ring 1 | |  |  <-- (rarely used on modern OS)
        |  | | +---+  | |  |
        |  | | | 0 |  | |  |  <-- Kernel mode (OS kernel)
        |  | | +---+  | |  |
        |  | +-------+  |  |
        |  +-----------+    |
        +-------------------+

Most modern OSes use only Ring 0 (kernel) and Ring 3 (user).
Some hypervisors use Ring -1 (VMX root mode on Intel VT-x).
```

### ARM Exception Levels

```
EL0  -  User applications
EL1  -  OS Kernel
EL2  -  Hypervisor
EL3  -  Secure Monitor (TrustZone)
```

### What Changes Between Modes

| Aspect | User Mode (Ring 3) | Kernel Mode (Ring 0) |
|--------|--------------------|---------------------|
| Privileged instructions | Trap (fault) | Execute normally |
| I/O port access | Trap (unless IOPL set) | Allowed |
| Memory access | Only mapped user pages | All physical + virtual memory |
| Interrupt control | Cannot cli/sti | Can enable/disable interrupts |
| Page table modification | Not allowed | Full control |
| CPU state | Limited view | Full access to all registers |

### How to Check Current Mode

**Linux - from user space, you're always in user mode, but you can inspect:**
```bash
# See which syscalls a program makes (transitions to kernel mode)
strace -c ls /tmp

# See kernel vs user time for a process
time ls -R /usr > /dev/null
# 'real' = wall clock, 'user' = user mode, 'sys' = kernel mode

# Per-process user/kernel time
cat /proc/$$/stat | awk '{print "user:", $14, "kernel:", $15}'
```

**Windows:**
```powershell
# See kernel vs user time
Measure-Command { Get-ChildItem C:\Windows -Recurse -ErrorAction SilentlyContinue }

# Process kernel/user time breakdown
Get-Process explorer | Select-Object ProcessName,
    @{N='UserTime'; E={$_.UserProcessorTime}},
    @{N='KernelTime'; E={$_.PrivilegedProcessorTime}}
```

### The Mode Bit

The CPU maintains a **mode bit** (or equivalent) in a control register:
- **0** = kernel mode (x86: CPL=0 in CS register's low 2 bits)
- **3** = user mode (x86: CPL=3)

**Linux - inspect CPL indirectly:**
```bash
# The kernel exposes ring info in /proc/cpuinfo flags
grep -o 'flags.*' /proc/cpuinfo | head -1

# On x86, look for 'smep' and 'smap' - kernel protections against
# accidentally (or maliciously) executing/reading user-space memory
# from kernel mode
grep -oE '(smep|smap|umip)' /proc/cpuinfo
```

---

## 6. System Calls and Trap Handling

A **system call** (syscall) is the mechanism by which a user-mode process requests a kernel-mode service.

### The System Call Mechanism

```
User Space                              Kernel Space
+-------------------+                   +-------------------+
| Application Code  |                   | Kernel            |
|                   |                   |                   |
| 1. Set up args    |                   |                   |
|    (registers)    |                   |                   |
|                   |                   |                   |
| 2. SYSCALL instr  | ---- trap -----> | 3. Save user state|
|    (or INT 0x80)  |                   |    (to kernel     |
|                   |                   |     stack)        |
|                   |                   |                   |
|                   |                   | 4. Lookup syscall |
|                   |                   |    in sys_call_   |
|                   |                   |    table[]        |
|                   |                   |                   |
|                   |                   | 5. Execute handler|
|                   |                   |    sys_write(),   |
|                   |                   |    sys_read(), ...|
|                   |                   |                   |
| 7. Resume         | <-- SYSRET ----- | 6. Restore user   |
|    execution      |    (or IRET)      |    state, set     |
|    (check retval  |                   |    return value   |
|     in rax)       |                   |                   |
+-------------------+                   +-------------------+
```

### x86-64 Linux System Call Convention

| Register | Purpose |
|----------|---------|
| `rax` | System call number (input) / return value (output) |
| `rdi` | Argument 1 |
| `rsi` | Argument 2 |
| `rdx` | Argument 3 |
| `r10` | Argument 4 |
| `r8` | Argument 5 |
| `r9` | Argument 6 |

**Linux - raw system call in assembly (x86-64):**
```asm
; write(1, msg, 13) -- write "Hello, world\n" to stdout
section .data
    msg db "Hello, world", 10   ; 10 = '\n'

section .text
    global _start

_start:
    mov rax, 1          ; syscall number for write
    mov rdi, 1          ; fd = 1 (stdout)
    mov rsi, msg        ; buffer pointer
    mov rdx, 13         ; count
    syscall             ; trap to kernel

    mov rax, 60         ; syscall number for exit
    xor rdi, rdi        ; exit code 0
    syscall
```

```bash
# Assemble, link, run
nasm -f elf64 hello.asm -o hello.o
ld hello.o -o hello
./hello
```

**Linux - system call from C (using the wrapper):**
```c
#include <unistd.h>
#include <sys/syscall.h>

int main(void) {
    // Using the libc wrapper
    write(STDOUT_FILENO, "via write()\n", 12);

    // Using syscall() directly - bypasses libc wrapper
    syscall(SYS_write, STDOUT_FILENO, "via syscall()\n", 14);

    return 0;
}
```

```bash
# Trace system calls
strace ./a.out
# Output shows:
# write(1, "via write()\n", 12)   = 12
# write(1, "via syscall()\n", 14) = 14
```

### Windows System Call Mechanism

Windows uses a different but analogous mechanism:

```
User Space                           Kernel Space
+--------------------+               +----------------------+
| Application        |               | ntoskrnl.exe         |
|   |                |               |                      |
|   v                |               |                      |
| Win32 API          |               |                      |
| (kernel32.dll)     |               |                      |
|   |                |               |                      |
|   v                |               |                      |
| NTDLL.dll          |               |                      |
| NtWriteFile()      |               |                      |
|   |                |               |                      |
|   | mov eax, 0x08  |               |                      |
|   | syscall        | --- trap ---> | KiSystemCall64()     |
|   |                |               |   |                  |
|   |                |               |   v                  |
|   |                |               | NtWriteFile()        |
|   |                |               |   (kernel-side)      |
|   |                |               |   |                  |
| <-- return ------- | <------------ |   return             |
+--------------------+               +----------------------+
```

**Key difference:** On Windows, applications call Win32 API functions (e.g., `WriteFile`), which call into `ntdll.dll`, which performs the actual `syscall` instruction.
The system call numbers are not stable across Windows versions - they change with every release.
On Linux, syscall numbers are part of the stable ABI.

**Windows - listing system calls:**
```powershell
# Count the number of Nt* functions in ntdll (approximate syscall count)
(Get-Command -Module NtDll -ErrorAction SilentlyContinue).Count

# Using dumpbin to see ntdll exports
dumpbin /exports C:\Windows\System32\ntdll.dll | Select-String "^  " | Measure-Object
```

### Types of Traps

| Trap Type | Cause | Synchronous? | Example |
|-----------|-------|-------------|---------|
| System call | Explicit `SYSCALL`/`INT` instruction | Yes | `write()`, `read()` |
| Exception | Error during instruction execution | Yes | Division by zero, page fault |
| Interrupt | External hardware event | No | Disk I/O complete, timer tick, keypress |

---

## 7. Crossing the User/Kernel Boundary

Every user-to-kernel transition has a **cost**.
Understanding this cost is critical for performance-sensitive systems.

### Cost Breakdown

```
User/Kernel Transition Cost:
+--------------------------------------------+
| 1. Mode switch hardware cost      ~100 ns  |
|    - Privilege level change                 |
|    - TLB/cache impacts                     |
+--------------------------------------------+
| 2. Save/restore register state     ~50 ns  |
|    - All general-purpose registers          |
|    - Segment registers, flags               |
+--------------------------------------------+
| 3. Kernel stack setup              ~20 ns   |
|    - Switch to per-thread kernel stack      |
+--------------------------------------------+
| 4. Security checks                 ~30 ns   |
|    - Validate syscall number                |
|    - Check permissions                      |
+--------------------------------------------+
| 5. Cache/TLB pollution             varies   |
|    - Kernel code evicts user cache lines    |
|    - KPTI adds TLB flush overhead           |
+--------------------------------------------+
| Total: roughly 200-1000 ns per transition   |
+--------------------------------------------+
```

### Measuring Syscall Overhead

**Linux:**
```c
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    const int N = 1000000;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < N; i++) {
        getpid();  // minimal syscall - just returns an integer
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Average syscall latency: %.0f ns\n", elapsed / N * 1e9);
    return 0;
}
```

```bash
gcc -O2 -o syscall_bench syscall_bench.c && ./syscall_bench
# Typical output: "Average syscall latency: 200-400 ns"

# With KPTI (Meltdown mitigation) enabled, expect higher:
cat /sys/devices/system/cpu/vulnerabilities/meltdown
```

**Windows:**
```c
#include <windows.h>
#include <stdio.h>

int main(void) {
    const int N = 1000000;
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    for (int i = 0; i < N; i++) {
        GetCurrentProcessId();  // minimal Win32 syscall
    }
    QueryPerformanceCounter(&end);

    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    printf("Average syscall latency: %.0f ns\n", elapsed / N * 1e9);
    return 0;
}
```

> **Quiz: Crossing the User/Kernel Boundary**
>
> *Which of the following are true about user/kernel mode transitions?*
> - (a) They require a change in the hardware privilege level
> - (b) They always flush the entire TLB
> - (c) They can be triggered by both hardware and software
> - (d) They require switching to a different stack
>
> **Answer:** (a), (c), (d) are true. (b) is false - not all transitions flush the entire TLB.
> With PCID (Process Context Identifiers) on modern CPUs, TLB entries can be tagged
> and retained across some transitions. However, KPTI (Kernel Page Table Isolation)
> does add TLB overhead.

---

## 8. OS Services Overview

An OS provides many services, organized into categories:

```
+---------------------------------------------------------------------+
|                        OS Services                                   |
+---------------------------------------------------------------------+
|                                                                      |
|  Process Management          Memory Management                       |
|  - Process creation/exit     - Virtual memory                        |
|  - Scheduling                - Paging                                |
|  - IPC                       - Page replacement                      |
|  - Synchronization           - Memory-mapped files                   |
|                                                                      |
|  File System Management      I/O Management                         |
|  - File creation/deletion    - Device driver interface               |
|  - Directory management      - Block/character devices               |
|  - Access control (perms)    - Buffering/caching                     |
|  - VFS abstraction           - DMA management                        |
|                                                                      |
|  Security & Protection       Networking                              |
|  - User authentication       - TCP/IP stack                          |
|  - Access control lists      - Socket API                            |
|  - Capabilities              - Routing                               |
|  - Namespaces/sandboxing     - Firewall (netfilter/WFP)              |
|                                                                      |
+---------------------------------------------------------------------+
```

### Linux - inspecting OS services:

```bash
# List all system calls available
ausyscall --dump | wc -l        # ~350+ syscalls on x86-64

# See syscall table
cat /usr/include/asm/unistd_64.h | head -30

# List loaded kernel modules (each provides services)
lsmod | head -20

# See all file systems the kernel supports
cat /proc/filesystems

# See available scheduling policies
chrt -m
# Output:
# SCHED_OTHER min/max priority : 0/0
# SCHED_FIFO  min/max priority : 1/99
# SCHED_RR    min/max priority : 1/99
# SCHED_BATCH min/max priority : 0/0
# SCHED_IDLE  min/max priority : 0/0
# SCHED_DEADLINE min/max priority : 0/0
```

### Windows - inspecting OS services:

```powershell
# List all running services
Get-Service | Where-Object {$_.Status -eq 'Running'} | Measure-Object
# Typically 100-200+ services

# See kernel-mode drivers loaded
Get-CimInstance Win32_SystemDriver | Where-Object {$_.State -eq 'Running'} |
  Select-Object Name, PathName | Format-Table -AutoSize

# File systems
Get-Volume | Select-Object DriveLetter, FileSystemType, Size, SizeRemaining

# See all processes and their memory usage
Get-Process | Sort-Object -Property WorkingSet64 -Descending |
  Select-Object -First 10 Name, @{N='MemMB';E={[math]::Round($_.WorkingSet64/1MB)}}
```

---

## 9. OS Organization: Monolithic Kernels

In a **monolithic kernel**, the entire OS runs as a single large binary in kernel mode (Ring 0).

```
+----------------------------------------------------------+
|  User Space (Ring 3)                                      |
|  +--------+  +--------+  +--------+  +--------+          |
|  | App 1  |  | App 2  |  | App 3  |  | Shell  |          |
|  +--------+  +--------+  +--------+  +--------+          |
+----------------------------------------------------------+
|  Kernel Space (Ring 0) -- ONE BIG BINARY                  |
|  +------------------------------------------------------+ |
|  |                    Monolithic Kernel                  | |
|  |                                                      | |
|  |  Scheduler | Memory Mgr | VFS | Net Stack | Drivers  | |
|  |                                                      | |
|  |  IPC | Crypto | Filesystems (ext4, xfs, btrfs)       | |
|  |                                                      | |
|  |  All components share the same address space          | |
|  |  Any component can call any other directly            | |
|  |  A bug in any component can crash the ENTIRE kernel   | |
|  +------------------------------------------------------+ |
+----------------------------------------------------------+
|  Hardware                                                 |
+----------------------------------------------------------+
```

### Advantages

| Advantage | Explanation |
|-----------|-------------|
| **Performance** | No IPC overhead between kernel components; direct function calls |
| **Simplicity** | No message-passing protocol needed between components |
| **Shared data** | All components can access kernel data structures directly |
| **Mature ecosystem** | Linux, traditional Unix - decades of optimization |

### Disadvantages

| Disadvantage | Explanation |
|-------------|-------------|
| **Large attack surface** | A vulnerability in any driver compromises the entire kernel |
| **Stability risk** | A bug in a device driver (most common source) crashes everything |
| **Hard to maintain** | Linux kernel is 30M+ lines; complex interdependencies |
| **Hard to extend** | Adding a feature requires understanding the whole kernel (mitigated by modules) |

### Examples

- **Linux** (with loadable kernel modules - LKMs)
- **Traditional Unix** (SVR4, BSD)
- **MS-DOS** (technically monolithic but no protection)

### Linux Loadable Kernel Modules (LKMs)

Linux mitigates the "big binary" problem with **loadable modules** that can be inserted/removed at runtime:

```bash
# List loaded modules
lsmod

# Get info about a module
modinfo ext4

# Load a module
sudo modprobe vfat

# Remove a module
sudo modprobe -r vfat

# See module dependencies
modprobe --show-depends ext4

# Write a minimal kernel module
cat << 'EOF' > hello_module.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CS6200");
MODULE_DESCRIPTION("Hello World LKM");

static int __init hello_init(void) {
    printk(KERN_INFO "Hello from kernel module!\n");
    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye from kernel module!\n");
}

module_init(hello_init);
module_exit(hello_exit);
EOF
```

Despite using modules, Linux is still **monolithic**: modules run in Ring 0, share the kernel address space, and a buggy module can still crash the system.

---

## 10. OS Organization: Microkernels

In a **microkernel**, only the absolute minimum runs in kernel mode.
Everything else (file systems, device drivers, networking) runs as **user-space servers**.

```
+----------------------------------------------------------+
|  User Space (Ring 3)                                      |
|                                                           |
|  +--------+ +--------+ +--------+ +--------+ +--------+  |
|  | App 1  | | App 2  | | FS Srv | | Net    | | Driver |  |
|  |        | |        | | (ext4) | | Stack  | | Server |  |
|  +--------+ +--------+ +--------+ +--------+ +--------+  |
|       |          |          ^          ^          ^        |
|       |          |          |          |          |        |
|       +----------+-------- IPC (message passing) --------+|
|                            |                              |
+----------------------------+------------------------------+
|  Kernel Space (Ring 0) -- MINIMAL                         |
|  +------------------------------------------------------+ |
|  |              Microkernel (~10K-50K lines)             | |
|  |                                                      | |
|  |  - IPC (message passing)                             | |
|  |  - Basic scheduling                                  | |
|  |  - Basic memory management (address spaces)          | |
|  |  - Interrupt dispatching                             | |
|  +------------------------------------------------------+ |
+----------------------------------------------------------+
|  Hardware                                                 |
+----------------------------------------------------------+
```

### Advantages

| Advantage | Explanation |
|-----------|-------------|
| **Isolation** | A driver crash doesn't bring down the kernel - just restart the server |
| **Security** | Small kernel = small attack surface, easier to audit and formally verify |
| **Flexibility** | OS components can be replaced independently |
| **Portability** | Small kernel is easier to port to new architectures |

### Disadvantages

| Disadvantage | Explanation |
|-------------|-------------|
| **Performance** | Every OS service request requires IPC (context switch + message copy) |
| **Complexity** | IPC-based architecture is harder to design and debug |
| **Latency** | More context switches mean higher latency for basic operations |

### Performance Impact of IPC

```
Monolithic: Application -> Kernel function call -> Return
            Cost: ~1 context switch (user->kernel->user)

Microkernel: Application -> IPC to FS server -> IPC to driver -> Return
             Cost: ~4+ context switches

Rough overhead comparison:
  Monolithic file read:    ~500 ns
  Microkernel file read:   ~2000-5000 ns  (4-10x slower)
```

### Examples

- **L4 family** (seL4 - formally verified, used in military/avionics)
- **Mach** (basis for macOS/iOS, but Apple uses a hybrid approach)
- **MINIX 3** (fault-tolerant design, runs Intel ME firmware)
- **QNX** (real-time microkernel, used in automotive and medical devices)

> **Quiz: Monolithic vs. Microkernel**
>
> *True or false:*
> 1. In a microkernel, device drivers run in user mode. **True**
> 2. Monolithic kernels are always faster than microkernels. **False** (depends on workload; I/O-heavy workloads suffer more in microkernels)
> 3. A microkernel is easier to formally verify. **True** (smaller codebase)
> 4. Linux is a microkernel. **False** (monolithic with loadable modules)
> 5. A crash in a device driver always crashes a monolithic kernel. **Usually true** (runs in Ring 0, can corrupt kernel state)

---

## 11. Hybrid Architectures (Modular/Layered)

Most production operating systems today are **hybrids** - they take the best ideas from both approaches.

### Layered Architecture

```
+-------------------------------------------------+
| Layer N: User Applications                       |
+-------------------------------------------------+
| Layer N-1: System Programs / Services            |
+-------------------------------------------------+
| Layer N-2: I/O Management                        |
+-------------------------------------------------+
| Layer N-3: Memory Management                     |
+-------------------------------------------------+
| Layer N-4: CPU Scheduling                        |
+-------------------------------------------------+
| Layer 1: Device Drivers                          |
+-------------------------------------------------+
| Layer 0: Hardware                                |
+-------------------------------------------------+

Each layer can only invoke services of the layer below it.
Clean abstraction but rigid - hard to decide layer ordering
(e.g., should memory management be above or below scheduling?
 They depend on each other.)
```

**Problem with pure layering:** Circular dependencies (memory management needs I/O for paging; I/O needs memory management for buffers). Pure layering is therefore **not practical** for full OS design; it's used as a **design principle** rather than a strict architecture.

### Modular Architecture (Linux's actual approach)

```
+-----------------------------------------------------+
| Loadable Kernel Modules (LKMs)                       |
| +--------+ +--------+ +--------+ +--------+         |
| | ext4   | | btrfs  | | nvidia | | wifi   |         |
| | module | | module | | driver | | driver |         |
| +--------+ +--------+ +--------+ +--------+         |
+-----------------------------------------------------+
|                Core Kernel                            |
| +--------------------------------------------------+ |
| | Scheduler | MM | VFS | Net | IPC | Crypto        | |
| |                                                  | |
| | Well-defined internal APIs between subsystems    | |
| +--------------------------------------------------+ |
+-----------------------------------------------------+
|                Hardware                               |
+-----------------------------------------------------+

Key: Modules are loaded into kernel space (Ring 0) but have
defined APIs.  The core is monolithic but extensible.
```

---

## 12. Linux Architecture Overview

```
+===========================================================+
||  USER SPACE                                               ||
||                                                           ||
||  +------+  +------+  +------+  +------+  +------+       ||
||  | bash |  | gcc  |  | httpd|  | sshd |  | app  |       ||
||  +------+  +------+  +------+  +------+  +------+       ||
||       |         |         |         |         |           ||
||  +----v---------v---------v---------v---------v------+   ||
||  |              GNU C Library (glibc)                 |   ||
||  |  printf(), malloc(), pthread_create(), open()      |   ||
||  +---------------------------------------------------+   ||
||       | System Call Interface (SYSCALL instruction)   |   ||
+========|==================================================+
         v
+===========================================================+
||  KERNEL SPACE                                             ||
||                                                           ||
||  +---------------------------------------------------+   ||
||  |          System Call Dispatch (entry_64.S)          |   ||
||  +---------------------------------------------------+   ||
||       |              |              |            |        ||
||  +----v----+   +-----v-----+  +----v----+  +----v----+   ||
||  | Process |   |  Memory   |  |   VFS   |  |   Net   |   ||
||  | Mgmt    |   | Mgmt (MM) |  |         |  |  Stack  |   ||
||  | (sched, |   | (vm, mmap,|  | (inode, |  | (TCP/IP,|   ||
||  |  fork,  |   |  page     |  |  dentry,|  |  socket,|   ||
||  |  exec,  |   |  fault,   |  |  super- |  |  netfil-|   ||
||  |  signal)|   |  slab)    |  |  block) |  |  ter)   |   ||
||  +---------+   +-----------+  +---------+  +---------+   ||
||       |              |              |            |        ||
||  +----v--------------v--------------v------------v----+   ||
||  |              Device Driver Framework               |   ||
||  |  (block drivers, char drivers, network drivers)    |   ||
||  +---------------------------------------------------+   ||
||       |                                                   ||
||  +----v----------------------------------------------+   ||
||  |     Architecture-Dependent Code (arch/x86, arm)    |   ||
||  +---------------------------------------------------+   ||
+===========================================================+
         |
+===========================================================+
||  HARDWARE                                                 ||
||  CPU | RAM | Disk | NIC | GPU | USB | Timer | Interrupt   ||
+===========================================================+
```

### Key Linux Kernel Subsystems

```bash
# See the kernel version and build info
uname -a

# See kernel configuration
zcat /proc/config.gz | head -50   # if available
# or
cat /boot/config-$(uname -r) | grep CONFIG_PREEMPT

# See kernel memory map
cat /proc/iomem | head -30

# See interrupt assignments
cat /proc/interrupts

# See kernel log
dmesg | tail -20

# Count lines in the Linux kernel source (as of 6.x)
# find linux-source/ -name '*.c' -o -name '*.h' | xargs wc -l
# Approximately 30+ million lines
```

---

## 13. macOS and Windows Architecture Overview

### macOS / iOS Architecture (XNU Kernel - Hybrid)

```
+===========================================================+
||  USER SPACE                                               ||
||                                                           ||
||  +---------+  +---------+  +---------+  +---------+      ||
||  | Cocoa/  |  | Carbon  |  | POSIX   |  | Java /  |      ||
||  | Swift   |  | (legacy)|  | Apps    |  | Python  |      ||
||  +---------+  +---------+  +---------+  +---------+      ||
||       |            |             |            |           ||
||  +----v------------v-------------v------------v------+   ||
||  |              Application Frameworks                |   ||
||  |    (AppKit, UIKit, Foundation, Core Foundation)     |   ||
||  +---------------------------------------------------+   ||
||  +---------------------------------------------------+   ||
||  |         Core OS / libSystem (Darwin)                |   ||
||  |   libsystem_kernel.dylib  |  libpthread  |  libc   |   ||
||  +---------------------------------------------------+   ||
+===========================================================+
||  XNU KERNEL (Hybrid: Mach + BSD)                          ||
||                                                           ||
||  +------------------------+  +------------------------+  ||
||  |    Mach Layer           |  |    BSD Layer            |  ||
||  |  - IPC (Mach ports)    |  |  - POSIX API            |  ||
||  |  - Task/thread mgmt    |  |  - VFS, UFS, HFS+       |  ||
||  |  - Virtual memory      |  |  - Networking (TCP/IP)  |  ||
||  |  - Scheduling          |  |  - Security (kauth)     |  ||
||  +------------------------+  +------------------------+  ||
||                  |                       |                 ||
||  +---------------v-----------------------v-----------+   ||
||  |              I/O Kit (Driver Framework)             |   ||
||  +---------------------------------------------------+   ||
||  +---------------------------------------------------+   ||
||  |         Platform Expert (Hardware Abstraction)      |   ||
||  +---------------------------------------------------+   ||
+===========================================================+
```

XNU is a **hybrid**: it has a Mach microkernel at its core but runs BSD and I/O Kit in the same address space (kernel mode), making it effectively monolithic in practice.

### Windows NT Architecture

```
+===========================================================+
||  USER SPACE                                               ||
||                                                           ||
||  +--------+ +--------+ +--------+ +---------+ +--------+||
||  | Win32  | | .NET   | | WSL2   | | Service | | UWP    |||
||  | App    | | App    | | (Linux)| | Host    | | App    |||
||  +--------+ +--------+ +--------+ +---------+ +--------+||
||       |          |           |          |          |      ||
||  +----v----------v-----------v----------v----------v--+  ||
||  |         Subsystem DLLs                              |  ||
||  | kernel32.dll | user32.dll | ws2_32.dll | advapi32   |  ||
||  +--------------------------------------------------- +  ||
||  +---------------------------------------------------+   ||
||  |              NTDLL.DLL                              |   ||
||  |  (Native API - Nt*/Zw* functions, syscall stubs)   |   ||
||  +---------------------------------------------------+   ||
+===========================================================+
||  KERNEL SPACE                                             ||
||                                                           ||
||  +---------------------------------------------------+   ||
||  |           Executive (ntoskrnl.exe)                  |   ||
||  |                                                    |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  |  | I/O Mgr   | | Obj Mgr   | | Security |          |   ||
||  |  | (IRP-     | | (handles, | | Ref Mon  |          |   ||
||  |  |  based)   | |  names)   | | (ACLs)   |          |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  |  | Memory    | | Process   | | Plug &   |          |   ||
||  |  | Manager   | | Manager   | | Play Mgr |          |   ||
||  |  | (VADs,    | | (EPROCESS,| |          |          |   ||
||  |  |  section) | |  ETHREAD) | |          |          |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  |  | Cache     | | Config    | | Power    |          |   ||
||  |  | Manager   | | Manager   | | Manager  |          |   ||
||  |  |           | | (Registry)| |          |          |   ||
||  |  +-----------+ +-----------+ +----------+          |   ||
||  +---------------------------------------------------+   ||
||  +---------------------------------------------------+   ||
||  |                    Kernel (ke)                      |   ||
||  |  Scheduling, interrupt dispatch, synchronization   |   ||
||  +---------------------------------------------------+   ||
||  +---------------------------------------------------+   ||
||  |        Hardware Abstraction Layer (HAL)             |   ||
||  +---------------------------------------------------+   ||
+===========================================================+
||  HARDWARE                                                 ||
+===========================================================+
```

**Windows - inspecting kernel architecture:**
```powershell
# See kernel version
[System.Environment]::OSVersion

# See loaded kernel drivers
Get-CimInstance Win32_SystemDriver | Where-Object {$_.State -eq 'Running'} |
  Measure-Object

# See kernel modules (using driverquery)
driverquery /v | Select-Object -First 20

# See memory layout
systeminfo | Select-String "Total Physical Memory","Available Physical Memory"

# See interrupt assignments (requires admin)
Get-CimInstance Win32_IRQResource | Select-Object IRQNumber, Name
```

### Comparison Summary

| Feature | Linux | Windows NT | macOS (XNU) |
|---------|-------|-----------|-------------|
| Kernel type | Monolithic (modular) | Hybrid | Hybrid (Mach+BSD) |
| Kernel language | C (some Rust) | C | C, C++ |
| Syscall stability | Stable ABI | Unstable (version-dependent) | Semi-stable (libsystem) |
| Driver model | LKMs | WDM/WDF/KMDF | I/O Kit (C++ based) |
| Source | Open source (GPL) | Closed (partial source available) | Partially open (Darwin) |
| Preemption | Fully preemptible | Fully preemptible | Fully preemptible |

---

## 14. Quizzes and Exercises

### Quiz 1: OS Fundamentals

> **Q:** An application wants to read data from a file.
> Which OS role is primarily being exercised - abstraction or arbitration?
>
> **A:** **Abstraction** - the OS is hiding the complexity of the disk hardware behind the `read()` interface.
> If multiple applications are reading simultaneously, then arbitration is also involved (disk scheduling).

### Quiz 2: Mode Transitions

> **Q:** Rank these operations by the number of user/kernel transitions required (fewest to most):
> 1. Adding two integers in a register
> 2. Calling `printf("hello")`
> 3. Reading from a file and writing to another file
>
> **A:**
> 1. **Zero transitions** - purely user-mode computation
> 2. **One transition** - printf eventually calls write() which is one syscall
> 3. **Two transitions** (minimum) - one for read(), one for write()

### Quiz 3: Architecture Identification

> **Q:** For each OS, identify its kernel architecture:
>
> | OS | Architecture |
> |----|-------------|
> | Linux 6.x | Monolithic with loadable modules |
> | Windows 11 | Hybrid (microkernel-inspired but monolithic in practice) |
> | QNX | Microkernel |
> | MINIX 3 | Microkernel |
> | macOS Sonoma | Hybrid (Mach + BSD in same address space) |
> | seL4 | Microkernel (formally verified) |

### Exercise: Syscall Counting

```bash
# Linux: Count syscalls made by different programs
strace -c ls /tmp 2>&1 | tail -15
strace -c cat /etc/hostname 2>&1 | tail -15
strace -c python3 -c "print('hello')" 2>&1 | tail -15

# Compare the number of syscalls and time spent in kernel mode.
# Python makes far more syscalls due to interpreter initialization.
```

```powershell
# Windows: Use Process Monitor (procmon) from Sysinternals
# Download: https://learn.microsoft.com/en-us/sysinternals/downloads/procmon
# Filter by process name, observe ReadFile, WriteFile, etc.

# Or use ETW (Event Tracing for Windows) from command line:
# xperf -on DiagEasy   (requires admin + Windows Performance Toolkit)
```

---

## 15. Key Takeaways

1. An OS provides **abstraction** (hides hardware complexity) and **arbitration** (manages shared resources).
2. **User/kernel mode** separation is enforced by hardware (CPU privilege rings / exception levels).
3. **System calls** are the gateway from user mode to kernel mode - they are expensive (~200-1000 ns).
4. **Monolithic kernels** (Linux) put everything in Ring 0 for performance; **microkernels** (seL4, QNX) minimize Ring 0 code for reliability and security.
5. **Hybrid kernels** (Windows NT, macOS XNU) borrow ideas from both - they are microkernel-inspired but run most components in kernel mode.
6. **Loadable modules** give monolithic kernels some of the flexibility of microkernels without the IPC performance cost.
7. Production OS choice depends on the trade-off between **performance** (monolithic) and **reliability/security** (microkernel).

---

**Previous:** [P1L1: Course Overview & Logistics](P1L1-Course-Overview.md) | **Next:** [P2L1: Processes and Process Management](../Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md)
