---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Coreutils and the Userland

A script is only as portable as the utilities it calls.
These notes compare the GNU, BSD and macOS, BusyBox, and uutils userlands and go deep on the utilities behind most production bugs.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Four Userlands: GNU, BSD, BusyBox, and uutils](Four-Userlands-GNU-BSD-BusyBox-uutils.md)
2. [Userland Portability Traps](Userland-Portability-Traps.md)
3. [find Expressions](find-Expressions.md)
4. [find -print0 and -exec +](find-print0-and-exec-Plus.md)
5. [xargs and Argument Batching](xargs-and-Argument-Batching.md)
6. [sort: Keys and Stability](sort-Keys-and-Stability.md)
7. [External Merge Sort](External-Merge-Sort.md)
8. [uniq and Counting](uniq-and-Counting.md)
9. [cp, mv, and Rename Semantics](cp-mv-and-Rename-Semantics.md)
10. [Reflinks and copy_file_range](Reflinks-and-copy_file_range.md)
11. [dd and Block I/O](dd-and-Block-IO.md)
12. [tee and Fan-Out](tee-and-Fan-Out.md)
13. [install and Atomic Deployment](install-and-Atomic-Deployment.md)
14. [stdio Buffering and stdbuf](stdio-Buffering-and-stdbuf.md)
15. [timeout](timeout.md)
16. [flock](flock.md)
17. [nohup and setsid](nohup-and-setsid.md)
18. [nice, ionice, and Priorities](nice-ionice-and-Priorities.md)
19. [env and Shebang Tricks](env-and-Shebang-Tricks.md)
20. [date and Time Zones](date-and-Time-Zones.md)
21. [mktemp](mktemp.md)
22. [tar and Archives](tar-and-Archives.md)
23. [Compression: gzip, zstd, and xz](Compression-gzip-zstd-xz.md)
24. [Checksums and Integrity](Checksums-and-Integrity.md)
25. [rsync: Algorithm and Usage](rsync-Algorithm-and-Usage.md)
