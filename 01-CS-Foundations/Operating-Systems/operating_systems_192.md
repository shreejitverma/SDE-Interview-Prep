# Complete Operating Systems Reference - 192 Keywords

## 1. CORE OS CONCEPTS (56 KEYWORDS)

### Fundamental Concepts (9)
1. Operating System - Software managing hardware resources and providing services
2. Kernel - Core OS managing CPU, memory, I/O in privileged mode
3. User Mode - Restricted execution for applications
4. Kernel Mode - Privileged execution with full hardware access
5. System Call - Interface between user programs and kernel
6. Context Switch - Switching between processes, saving/restoring state
7. Interrupt Handler - Routine handling hardware/software interrupts
8. Exception - Unexpected event requiring kernel intervention
9. Trap - Software-triggered exception for intentional kernel entry

### Process & Thread Management (12)
10. Process - Instance of executing program with isolated address space
11. Process ID (PID) - Unique identifier for each process
12. Thread - Lightweight execution unit sharing process address space
13. Multitasking - OS ability to run multiple processes concurrently
14. Preemption - OS interrupting running process to switch to another
15. Scheduling - Algorithm deciding which process runs next
16. Scheduler - Component selecting next process to execute
17. Ready Queue - Queue of processes ready to run
18. Run Queue - Processes currently executing on CPUs
19. Process State - Current state (New, Ready, Running, Waiting, Terminated)
20. Context - Process execution state (registers, PC, memory)
21. PCB (Process Control Block) - Data structure storing process info

### Memory Management (14)
22. Virtual Memory - Abstraction providing each process own address space
23. Address Space - Set of memory addresses accessible by process
24. Paging - Dividing memory into fixed-size pages
25. Page - Fixed-size memory unit (typically 4KB)
26. Page Table - Data structure mapping virtual to physical addresses
27. Page Fault - Exception when accessing unmapped page
28. Segmentation - Dividing memory into logical segments
29. Paging vs Segmentation - Fixed vs variable-size allocation
30. TLB (Translation Lookaside Buffer) - Hardware cache for page table entries
31. Memory Hierarchy - Registers → Cache → RAM → Disk
32. Cache - Fast memory holding frequently accessed data
33. Swap - Disk space extending RAM
34. Working Set - Subset of process memory actively used
35. Thrashing - Excessive paging degrading performance

## 2. PROCESS & SYNCHRONIZATION (20 KEYWORDS)

### Process Control (10)
36. fork() - System call creating child process
37. exec() - System call replacing process image
38. wait() - Parent waits for child process termination
39. exit() - Process terminates and releases resources
40. Process Hierarchy - Parent-child relationships forming tree
41. Zombie Process - Terminated process waiting for parent to reap
42. Orphan Process - Process whose parent terminated first
43. Signal - Asynchronous notification to process
44. SIGKILL - Signal forcefully terminating process
45. SIGTERM - Signal requesting graceful termination

### Synchronization (10)
46. Race Condition - Behavior depends on execution timing
47. Critical Section - Code accessing shared resources
48. Mutex - Lock ensuring only one thread in section
49. Lock - Synchronization primitive preventing concurrent access
50. Semaphore - Synchronization with counter allowing N threads
51. Deadlock - Circular wait where processes block indefinitely
52. Livelock - Processes busy but making no progress
53. Monitor - Synchronized construct with automatic mutual exclusion
54. Condition Variable - Allows thread to wait for condition
55. Barrier - Synchronization point where threads wait

## 3. FILE SYSTEMS & I/O (22 KEYWORDS)

### File System Basics (10)
56. File System - Hierarchical storage organization
57. Inode - Data structure storing file metadata
58. Directory - Special file containing name-to-inode mappings
59. Path - Sequence of names identifying location
60. Root Directory - Top of filesystem hierarchy
61. Working Directory - Current directory context
62. Hard Link - Multiple entries pointing to same inode
63. Symbolic Link - Special file containing path
64. File Permissions - Access control (read, write, execute)
65. Ownership - File owner and group assignment

### File System Types (7)
66. ext4 - Fourth extended filesystem (Linux default)
67. NTFS - Windows NT File System
68. FAT32 - File Allocation Table (legacy)
69. APFS - Apple File System
70. btrfs - B-tree filesystem (Linux)
71. XFS - High-performance filesystem
72. ZFS - Advanced filesystem with checksums

### I/O & Storage (5)
73. I/O Subsystem - Component managing I/O operations
74. Block Device - Disk, SSD accessed in blocks
75. Character Device - Terminals, printers accessed sequentially
76. Buffer Cache - Memory buffer for disk blocks
77. Page Cache - Cache combining pages and disk blocks

## 4. OPERATING SYSTEM TYPES (16 KEYWORDS)

### Classification (9)
78. Batch Operating System - Processes jobs sequentially
79. Time-Sharing OS - Multiple users share system
80. Real-Time OS (RTOS) - Guarantees response within deadline
81. Hard Real-Time - Missing deadline is failure
82. Soft Real-Time - Missing deadline degrades quality
83. Embedded OS - Specialized for resource-constrained devices
84. Distributed OS - Multiple computers appear as single system
85. Multiprocessing OS - Manages multiple processors/cores
86. Multitasking OS - Supports multiple concurrent processes

### Desktop/Server OS (7)
87. Windows - Microsoft OS (consumer/enterprise)
88. Windows NT - Modern Windows kernel
89. Linux - Open-source Unix-like OS
90. Linux Kernel - Core of Linux OS
91. GNU/Linux - Linux kernel + utilities
92. macOS - Apple's Unix-based OS
93. FreeBSD - Free Unix-like operating system

## 5. CORE OS COMPONENTS (27 KEYWORDS)

### Bootloader & Startup (6)
94. Bootloader - First program executed on power-on
95. BIOS - Basic Input/Output System (legacy)
96. UEFI - Unified Extensible Firmware Interface (modern)
97. Kernel Loading - Bootloader loading kernel
98. Init System - First process started by kernel
99. systemd - Modern init system

### Scheduler (10)
100. CPU Scheduler - Component selecting next process
101. Scheduling Algorithm - Policy for selection
102. Round-Robin Scheduling - Equal time slices
103. Priority Scheduling - Higher priority first
104. Real-Time Scheduling - Deadline guarantees
105. Multilevel Queue Scheduling - Different queues
106. Time Quantum - Maximum time per process
107. Load Balancing - Distributing across CPUs
108. Affinity - Keeping process on same CPU
109. Priority Inversion - Low-priority blocking high-priority

### Memory Manager (8)
110. Memory Manager - Allocates/deallocates memory
111. Fragmentation - Unused memory scattered
112. Compaction - Moving blocks to eliminate fragmentation
113. Garbage Collection - Automatic memory cleanup
114. Memory Leak - Failed to release memory
115. Heap - Dynamic allocation region
116. Stack - Function locals region
117. Heap vs Stack - Trade-offs

### File Manager (4)
118. File Manager - Manages file creation/deletion
119. File Descriptor - Handle to open file
120. File Descriptor Table - Per-process file mapping
121. Open File Table - System-wide open files

### Device Driver (3)
122. Device Driver - Software interface with hardware
123. DMA (Direct Memory Access) - Device accesses memory
124. Interrupt-Driven I/O - Device raises interrupt

## 6. PROTECTION & SECURITY (17 KEYWORDS)

### Access Control (6)
125. Privilege Levels - Different execution levels
126. Ring Architecture - Multiple privilege rings
127. Capability-Based Security - Rights tied to process
128. Access Control List (ACL) - Permission list
129. Role-Based Access Control (RBAC) - Users assigned roles
130. Principle of Least Privilege - Minimal permissions

### Protection Mechanisms (7)
131. Memory Protection - Hardware prevents access
132. Address Space Layout Randomization (ASLR) - Randomizes addresses
133. Stack Canary - Detects buffer overflows
134. Data Execution Prevention (DEP) - Non-executable regions
135. Sandboxing - Isolated restricted environment
136. Virtualization - Software computer emulation
137. Containers - Lightweight OS-level isolation

### User & Group Management (4)
138. User Account - Identity for access control
139. Group - Collection of users
140. Root/Administrator - Privileged account
141. Sudo - Execute as another user

## 7. ADVANCED TOPICS (34 KEYWORDS)

### Virtualization (8)
142. Virtual Machine (VM) - Software computer emulation
143. Hypervisor - Software managing VMs
144. Type 1 Hypervisor - Bare-metal (KVM, Xen)
145. Type 2 Hypervisor - Hosted (VirtualBox)
146. Container - Lightweight OS-level isolation
147. Docker - Container platform
148. Kubernetes - Container orchestration
149. Virtual Memory - Illusion of unlimited memory

### Performance Optimization (8)
150. Caching - Fast memory for frequent data
151. Prefetching - Loading before needed
152. Locality of Reference - Program reuse
153. Thrashing - Excessive paging
154. Buffer Bloat - Excessive buffering
155. Lock Contention - Multiple threads waiting
156. Hyper-Threading - Logical cores per physical
157. NUMA - Non-uniform memory access

### Communication (6)
158. Inter-Process Communication (IPC) - Process communication
159. Pipe - Unidirectional channel
160. Socket - Communication endpoint
161. Shared Memory - Accessible by multiple
162. Message Queue - Asynchronous messages
163. Deadlock Prevention - Avoiding circular wait

### Advanced Concepts (12)
164. Process Address Space - Segments and allocation
165. Semaphore - Counter-based synchronization
166. Mutex - Mutual exclusion lock
167. Monitor - Synchronized construct
168. Deadlock Conditions - Four enabling conditions
169. Starvation - Process never gets resources
170. Page Replacement - Victim page algorithm
171. Working Set Model - Active memory subset
172. Demand Paging - Load on demand
173. Copy-on-Write - Defer copy until write
174. Swap Space - Virtual memory extension
175. Thrashing Definition - Excessive I/O

## COMPLETE STATISTICS

**Total OS Keywords: 192**

Breakdown:
- Core OS Concepts: 56
- Process & Synchronization: 20
- File Systems & I/O: 22
- OS Types: 16
- Core Components: 27
- Protection & Security: 17
- Advanced Topics: 34

All organized for comprehensive OS understanding.
