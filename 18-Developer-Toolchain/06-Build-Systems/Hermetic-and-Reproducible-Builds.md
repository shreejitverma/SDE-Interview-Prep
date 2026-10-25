---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Hermetic and Reproducible Builds

> [!summary] Scope
> Owns bit-for-bit build determinism and host environment isolation principles.
> Reaches Explain, Debug, and Decide at Scale rungs by detailing timestamp normalization via SOURCE_DATE_EPOCH, path remapping, deterministic archive packing, locale and randomness normalization, and verification using diffoscope.
> Establishes the boundary with operating system execution sandboxing and supply chain attestation.

## Related

- Prerequisites: [[Build-System-Theory-Schedulers-and-Rebuilders]]
- Next: [[Sandboxing-and-Hermetic-Execution-Internals]]
- See also: [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Nix-Builders-and-stdenv-Phases|Nix Builders and stdenv Phases]]
- Lab: `code/06-Build-Systems/reproducible_build/`
