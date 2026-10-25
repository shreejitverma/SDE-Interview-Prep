---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# OCI Images: Layers, Multi-Stage Builds, and Distroless Roots

> [!summary] Scope
> Internal mechanics of the OCI Image Format Specification v1.1.1, content-addressable blobs, tar and zstd filesystem diff layers, image manifests, and multi-architecture image indexes.
> Explores multi-stage compilation patterns, whiteout file deletion mechanics, and minimal attack surface distroless base configurations.
> Covers bit-for-bit reproducible container image generation and layer caching invariants.
> Contrasts developer container ergonomics with lean production runtime environments.

## Related

- Prerequisites: [[Lockfiles-Vendoring-and-Offline-Mirrors]]
- Next: [[Progressive-Delivery-Rollbacks-and-Feature-Flags]]
- See also: [[04-System-Design/00-Concepts/Virtual-Machines-vs-Containers|Virtual Machines vs Containers]], [[04-System-Design/07-Infrastructure-and-Orchestration/Docker-and-Container-Runtimes|Docker and Container Runtimes]]
