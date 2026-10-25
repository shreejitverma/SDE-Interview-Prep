---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Hardened Release Builds and Separate Debug Symbols

> [!summary] Scope
> OpenSSF compiler and linker hardening flags including FORTIFY_SOURCE level 3, stack protection, full RELRO, and standard library assertions.
> Details debug symbol stripping via objcopy, gnu_debuglink embedding, Build ID generation, symbol archive indexing, and debuginfod network symbol servers.
> Balances runtime defense-in-depth protections against binary execution overhead in latency-critical environments.
> Covers post-crash core dump symbol symbolication without shipping multi-gigabyte debug binaries to customer hosts.

## Related

- Prerequisites: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]]
- Next: [[Cpp-macOS-Universal-Binaries-Code-Signing-and-Notarization]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
