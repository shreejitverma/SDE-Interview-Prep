---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Automated Git Bisect Across Non-Linear Merge History

> [!summary] Scope
> Owns a practical bisection drill isolating a microsecond latency regression across 500 complex merge commits using automated bisection testing.
> Reaches the Use and Debug rungs by providing a harness script containing build breakages and transient test failures, teaching the engineer how to write a robust test script that uses exit code 125 for untestable commits and handles merge-base ambiguities.
> Grounds the concepts of bisection in automated CI/CD practices.

## Related

- Prerequisites: [[Git-Archaeology-Log-Blame-and-Bisect]]
- Lab: `code/02-Version-Control/drill_bisect_regression/`
