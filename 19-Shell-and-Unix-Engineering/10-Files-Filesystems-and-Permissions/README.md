---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Files, Filesystems, and Permissions

Scripts live and die by filesystem semantics: links, permissions, atomic rename, durability, and path races.
These notes cover the filesystem behavior a script relies on, from inodes to mount namespaces.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Inodes and Links](Inodes-and-Links.md)
2. [Permissions, umask, and Special Bits](Permissions-umask-and-Special-Bits.md)
3. [ACLs and Extended Attributes](ACLs-and-Extended-Attributes.md)
4. [setuid and Interpreted Scripts](setuid-and-Interpreted-Scripts.md)
5. [Atomic Rename and fsync](Atomic-Rename-and-fsync.md)
6. [Sparse Files and Holes](Sparse-Files-and-Holes.md)
7. [Mounts and Mount Namespaces](Mounts-and-Mount-Namespaces.md)
8. [Filesystem Hierarchy Standard](Filesystem-Hierarchy-Standard.md)
9. [inotify and fswatch](inotify-and-fswatch.md)
10. [File Locking Semantics](File-Locking-Semantics.md)
11. [Path Resolution and Symlink Races](Path-Resolution-and-Symlink-Races.md)
