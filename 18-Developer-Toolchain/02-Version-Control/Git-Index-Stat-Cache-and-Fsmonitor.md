---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Index, Stat Cache, and Fsmonitor

> [!summary] Scope
> Owns the binary .git/index file format across versions 2, 3, and 4, the operating system stat cache, and working-tree modification detection.
> Reaches Explain, Debug, and Rebuild rungs by analyzing cached filesystem metadata, racy-Git timestamp resolution, conflict stage entries, and extensions including tree cache, untracked cache, sparse-index, and fsmonitor daemon integration.
> Sets the boundary with working-tree checkout operations and sparse-checkout cone-mode mechanics.

## Related

- Prerequisites: [[Git-Object-Model-and-Content-Addressing]]
- Next: [[Git-Packfiles-and-Delta-Compression]]
- See also: [[19-Shell-and-Unix-Engineering/10-Files-Filesystems-and-Permissions/inotify-and-fswatch|inotify and fswatch]]
- Lab: `code/02-Version-Control/index_parser/`
