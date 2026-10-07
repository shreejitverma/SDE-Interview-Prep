---
type: moc
track: [sde]
level:
status: draft
last_reviewed:
sources: []
---

# Security And Cryptography

Security: common vulnerabilities and secure coding.

```mermaid
flowchart LR
    Request["Request"] --> Parse["Parse and validate"]
    Parse --> Auth["Authenticate, then authorize"]
    Auth --> Action["Touch data or run a command"]
    Action --> Out["Encode the response"]
```

[[owasp_top_10]] is the catalog of what goes wrong on those arrows.
[[cpp_safety.cpp]] is the memory-safety side of the same idea in C++.
