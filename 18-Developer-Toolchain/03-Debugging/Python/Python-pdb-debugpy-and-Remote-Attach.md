---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python pdb, debugpy, and Remote Attach

> [!summary] Scope
> This note owns interactive and remote Python debugging workflows via the standard pdb module, debugpy, and PEP 768.
> It covers the Use, Explain, and Debug rungs, explaining post-mortem inspection, DAP protocol architecture, and safe live attach via python -m pdb -p PID in Python 3.14+.
> It replaces unsafe ptrace code injection with first-class standard library remote debugging.

## Related

- Prerequisites: none
- Next: [[Python-sys-monitoring-and-Tracing]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]], [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_41_Testing_Debugging_and_Quality_Assurance|Chapter_41_Testing_Debugging_and_Quality_Assurance]]
