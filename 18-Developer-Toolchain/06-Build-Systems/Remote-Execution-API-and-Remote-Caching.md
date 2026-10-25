---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Remote Execution API and Remote Caching Architecture

> [!summary] Scope
> Owns distributed compilation, remote action caching, and content-addressable storage protocols.
> Reaches Explain, Debug, and Decide at Scale rungs by detailing the Linux Foundation and Bazel Remote Execution API v2, covering ActionCache, ContentAddressableStorage, and Execution gRPC services.
> Analyzes Merkle tree digests, ByteStream chunking, Build without the Bytes bandwidth optimization, and cache poisoning mitigation strategies.
> Establishes the boundary with local wrapper compiler caches.

## Related

- Prerequisites: [[Hermetic-and-Reproducible-Builds]], [[Bazel-Skyframe-Bzlmod-and-Execution-Model]]
- Next: [[Compiler-Caches-ccache-and-sccache]]
- Lab: `code/06-Build-Systems/reapi_client_demo/`
