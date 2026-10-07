---
title: LeetCode - 0150 - Evaluate Reverse Polish Notation
tags:
  - leetcode
  - problem
  - stack
  - array
  - math
difficulty: medium
source: leetcode
problem_number: "0150"
topics:
  - Stack
  - Array
  - Math
---

# LeetCode 0150: Evaluate Reverse Polish Notation

## Problem Breakdown

You are given an array of strings `tokens` that represents an arithmetic expression in a Reverse Polish Notation (postfix notation).
Evaluate the expression.
Return an integer that represents the value of the expression.
The valid operators are `'+'`, `'-'`, `'*'`, and `'/'`.
Each operand may be an integer or another expression.
Division between two integers always truncates toward zero.
There will not be any division by zero.
The input represents a valid arithmetic expression in reverse polish notation.

### Postfix Evaluation Invariant

- In Reverse Polish Notation, every operator immediately follows its two operands.
- This evaluation rule aligns with a Last-In, First-Out (LIFO) stack discipline.
- When an operand is encountered, it is placed onto the evaluation stack.
- When an operator is encountered, the top two elements are popped: the first popped element is the right operand `b`, and the second popped element is the left operand `a`.
- The binary operation is evaluated as `a op b`, and the scalar result is pushed back onto the stack.
- Upon consuming all tokens, the final result resides at the top of the stack.

## Optimal Approaches

### Stack-Based Simulation

```
Tokens: ["2", "1", "+", "3", "*"]

Step 1: Token "2" -> Push 2. Stack: [2]
Step 2: Token "1" -> Push 1. Stack: [2, 1]
Step 3: Token "+" -> Pop 1 (b), Pop 2 (a). Eval 2 + 1 = 3. Push 3. Stack: [3]
Step 4: Token "3" -> Push 3. Stack: [3, 3]
Step 5: Token "*" -> Pop 3 (b), Pop 3 (a). Eval 3 * 3 = 9. Push 9. Stack: [9]

Result = 9
```

1. Initialize an empty integer stack.
2. For each string token in `tokens`:
   - If token is an operator (`+`, `-`, `*`, `/`):
     - Pop `b = stack.pop()`.
     - Pop `a = stack.pop()`.
     - Apply operation: `a + b`, `a - b`, `a * b`, or `trunc(a / b)`.
     - Push result back to stack.
   - Otherwise:
     - Parse token as an integer and push it to stack.
3. Return the top of the stack.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | Each token is pushed and popped at most once with $O(1)$ arithmetic operations. |
| **Space Complexity** | $O(n)$ | Auxiliary stack holding intermediate operands up to $(n + 1) / 2$ numbers. |

## Common Traps & Edge Cases

- **Operand Order in Subtraction and Division**: Subtraction and division are non-commutative; the first popped element is `b` (divisor/subtrahend) and the second is `a` (dividend/minuend).
- **Truncation Toward Zero**: In Python, standard floor division `//` rounds toward negative infinity; `int(a / b)` must be used to ensure truncation toward zero.
- **Negative Integer Tokens**: Tokens like `"-11"` start with a minus sign but represent negative operands, not operators; checking string equality against `"-"` disambiguates operands from operators.
