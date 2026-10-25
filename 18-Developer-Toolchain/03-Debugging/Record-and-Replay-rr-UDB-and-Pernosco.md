---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Record and Replay: rr, UDB, and Pernosco

> [!summary] Scope
> This note owns deterministic user-space record-and-replay debugging systems, analyzing rr, UndoDB (UDB), and cloud-hosted Pernosco.
> It reaches the Use, Explain, and Decide depth rungs by examining PMU retired conditional branch counters, in-process syscallbuf interception that avoids ptrace stops, and hardware watchpoints during reverse execution.
> It distinguishes Linux PMU requirements (bare metal or a VM with PMU virtualization) from Apple Silicon VM limitations and leaves macro replay testing to 14-Low-Latency-Systems.

## Related

- Prerequisites: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- Next: [[Debugging-Distributed-Systems-with-Traces-and-Correlation]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing|Deterministic Replay and Packet Injection Testing]]
