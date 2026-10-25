---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust GDB, LLDB, Panics, and Backtraces

> [!summary] Scope
> This note owns native interactive debugging and panic diagnostics for Rust binaries using rust-gdb and rust-lldb.
> It reaches the Use and Explain rungs by analyzing v0 symbol demangling, RUST_BACKTRACE unwinding mechanics, panic hooks, and panic=abort versus panic=unwind.
> It provides the foundation for native debugging of compiled Rust code before addressing undefined behavior in unsafe blocks.

## Related

- Prerequisites: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- Next: [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]
