---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Shipping Python Applications: zipapp, PEX, PyInstaller, and uv tool

> [!summary] Scope
> Architecture and runtime trade-offs for distributing standalone Python applications without external interpreter setup.
> Compares zipapp archives, PEX and shiv self-extracting virtualenv executables, PyInstaller bootloaders, and uv tool isolated environments with PEP 723 script metadata.
> Evaluates startup latency, disk footprint, and binary portability across platforms.
> Guides architectural selection between single-file binaries and containerized deployment.

## Related

- Prerequisites: [[Python-Packaging-History-and-pyproject-toml]]
- Next: [[Python-Entry-Points-Plugins-and-Namespace-Packages]]
