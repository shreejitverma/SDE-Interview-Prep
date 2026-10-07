---
title: LeetCode - 0134 - Gas Station
tags:
  - leetcode
  - problem
  - greedy
  - array
difficulty: medium
source: leetcode
problem_number: "0134"
topics:
  - Greedy
  - Array
---

# LeetCode 0134: Gas Station

## Problem Breakdown

There are `n` gas stations along a circular route, where the amount of gas at the `i`-th station is `gas[i]`.
You have a car with an unlimited gas tank and it costs `cost[i]` of gas to travel from the `i`-th station to its next `(i + 1)`-th station.
You begin the journey with an empty tank at one of the gas stations.
Given two integer arrays `gas` and `cost`, return the starting gas station's index if you can travel around the circuit once in the clockwise direction, otherwise return `-1`.
If there exists a solution, it is guaranteed to be unique.

### Mathematical Invariants

1. **Global Solvability Condition**: If the sum of all fuel is less than the sum of all costs ($\sum gas[i] < \sum cost[i]$), then the total energy deficit makes it impossible to complete a full lap from any station.
2. **Local Elimination Invariant**: If you start at station $A$ and run out of gas at station $B$ (meaning accumulated fuel drops below zero arriving at or trying to leave $B$), then **no station $k$ between $A$ and $B$** ($A \le k \le B$) can be a valid starting point either.
   - Proof: Since station $A$ successfully reached $k$, the fuel tank upon arriving at $k$ was $\ge 0$.
   - Starting fresh from $k$ with tank equal to $0$ would only yield strictly less or equal fuel than arriving at $k$ with surplus fuel.
   - Hence, if $A$ failed to pass $B$, starting at $k$ would fail at or before $B$ as well.
   - Therefore, the next viable starting candidate must be strictly after $B$, namely $B + 1$.

## Optimal Approaches

### Single-Pass Greedy Algorithm

```
Stations:   0      1      2      3      4
Gas:       [1,     2,     3,     4,     5]
Cost:      [3,     4,     5,     1,     2]
Diff:      [-2,   -2,    -2,    +3,    +3]

Index 0: diff = -2 -> current_tank = -2 < 0 -> reset start = 1, current_tank = 0
Index 1: diff = -2 -> current_tank = -2 < 0 -> reset start = 2, current_tank = 0
Index 2: diff = -2 -> current_tank = -2 < 0 -> reset start = 3, current_tank = 0
Index 3: diff = +3 -> current_tank = +3 >= 0
Index 4: diff = +3 -> current_tank = +6 >= 0
Total sum = -2 + -2 + -2 + 3 + 3 = 0 >= 0
Result = start = 3
```

1. Maintain `total_tank = 0`, `current_tank = 0`, and `start_index = 0`.
2. Iterate `i` from `0` to `n - 1`:
   - Compute `balance = gas[i] - cost[i]`.
   - Accumulate `total_tank += balance` and `current_tank += balance`.
   - If `current_tank < 0`, reset `start_index = i + 1` and `current_tank = 0`.
3. If `total_tank >= 0`, return `start_index`; otherwise return `-1`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | Strictly single pass across the $n$ stations without redundant scans. |
| **Space Complexity** | $O(1)$ | Uses three primitive integer registers for tracking state. |

## Common Traps & Edge Cases

- **Global Net Deficit**: Always verify that `total_tank >= 0` at the conclusion; otherwise no station can complete the circuit.
- **Starting at the Last Station**: If `start_index` becomes `n - 1` and completes the check, it correctly returns `n - 1`.
- **Single Element Arrays**: For `n = 1`, if `gas[0] >= cost[0]`, returns `0`; otherwise returns `-1`.
