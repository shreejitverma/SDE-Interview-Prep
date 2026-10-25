---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# macOS Packaging: Universal Binaries, Code Signing, and Notarization

> [!summary] Scope
> macOS distribution toolchain mechanics including Mach-O universal2 fat binary synthesis via lipo, codesign signatures, and LINKEDIT superblobs.
> Details Hardened Runtime entitlements, automated submission to Apple via xcrun notarytool, and offline ticket stapling with stapler.
> Resolves common Gatekeeper rejection modes and dynamic linker library path validation failures.
> Covers quarantine attributes and distribution outside the Mac App Store.

## Related

- Prerequisites: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]]
