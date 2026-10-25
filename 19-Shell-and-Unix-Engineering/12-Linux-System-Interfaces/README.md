---
type: moc
track: [sde, distinguished, low-latency, cloud-ai]
level:
status: draft
last_reviewed:
sources: []
---

# Linux System Interfaces

Containers, services, and boot are shell-visible kernel interfaces.
These notes cover system calls, namespaces, cgroups, capabilities, seccomp, init systems, and kernel tunables from the shell.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [System Calls and strace](System-Calls-and-strace.md)
2. [Namespaces from the Shell: unshare and nsenter](Namespaces-from-the-Shell-unshare-and-nsenter.md)
3. [cgroups v2 from the Shell](cgroups-v2-from-the-Shell.md)
4. [Linux Capabilities](Linux-Capabilities.md)
5. [seccomp](seccomp.md)
6. [Init Systems SysV and systemd](Init-Systems-SysV-and-systemd.md)
7. [systemd Units for Scripts](systemd-Units-for-Scripts.md)
8. [journald](journald.md)
9. [initramfs and Boot Scripts](initramfs-and-Boot-Scripts.md)
10. [sysctl and /proc/sys](sysctl-and-proc-sys.md)
11. [Kernel Modules and dmesg](Kernel-Modules-and-dmesg.md)
