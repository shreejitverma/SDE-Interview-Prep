---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python sys.monitoring and Tracing

> [!summary] Scope
> This note owns Python execution event monitoring using PEP 669 sys.monitoring and legacy sys.settrace hooks.
> It covers the Explain and Rebuild rungs by examining tool identifier allocation, bytecode instruction instrumentation, local versus global events, and the near-zero-overhead DISABLE fast path.
> It establishes the tracing mechanism utilized by modern Python debuggers, profilers, and coverage tools.

## Related

- Prerequisites: [[Python-pdb-debugpy-and-Remote-Attach]]
- Next: [[Python-faulthandler-and-Crash-Diagnostics]]
- Lab: `code/03-Debugging/sys_monitoring_tracer/`
