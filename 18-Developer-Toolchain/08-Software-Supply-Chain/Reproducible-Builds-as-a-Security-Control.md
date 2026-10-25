---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Reproducible Builds: Determinism, Diverse Double-Compiling, and diffoscope

> [!summary] Scope
> Source-to-binary bitwise determinism as a foundational control against build-infrastructure compromise.
> Explores determinism hazards including timestamps, SOURCE_DATE_EPOCH, path remapping via compiler flags, archive member ordering, and filesystem entropy.
> Analyzes Thompson Trusting Trust compiler backdoors, Wheeler Diverse Double-Compiling mitigation, and binary forensic inspection with diffoscope.
> Provides verification frameworks for proving that distributed binaries match published source trees.

## Related

- Prerequisites: [[SLSA-Framework-Build-and-Source-Tracks]]
- Next: [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]]
