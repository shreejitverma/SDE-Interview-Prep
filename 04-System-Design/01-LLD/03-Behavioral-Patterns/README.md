---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Behavioral Patterns

Behavioral patterns decide who talks to whom at runtime, and what they are allowed to assume.

- [[Strategy-and-State]] swaps an algorithm, or swaps behavior because the object's state changed.
- [[Observer-and-Pub-Sub]] fans an event out to dependents. The distributed version of this idea is [[Pub-Sub-Architecture]].
- [[Command-and-Chain-of-Responsibility]] turns a request into an object, or walks it down a line of handlers.
- [[Iterator-Visitor-and-Mediator]] walks a structure, adds an operation without editing the structure, or stops a set of objects from pointing at each other.
