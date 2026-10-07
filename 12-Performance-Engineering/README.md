---
type: moc
track: [low-latency, sde]
level:
status: draft
last_reviewed:
sources: []
---

# Performance Engineering

Performance engineering: CPU architecture effects and profiling.

```mermaid
flowchart TD
    Measure["Profile the running program"] --> Find["Find the hot line"]
    Find --> Cause{"Cache, allocation, lock, or algorithm?"}
    Cause --> Change["Change one thing"]
    Change --> Measure
```

[[profiling_guide]] is the measurement step.
[[false_sharing.cpp]] is one concrete cause: two cores writing different fields that share a cache line.
Do not start at the change box.
