---
id: leetcode-0022-generate-parentheses
title: "LeetCode 0022: Generate Parentheses"
tags:
  - dsa
  - leetcode
  - backtracking
  - string
  - dynamic-programming
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/generate-parentheses/"
---

# LeetCode 0022: Generate Parentheses

## 1. Problem Formalization and Constraints

Given `n` pairs of parentheses, write a function to generate all combinations of well-formed parentheses.

### Constraints
- $1 \le n \le 8$

### Examples
- **Example 1**:
  - Input: `n = 3`
  - Output: `["((()))","(()())","(())()","()(())","()()()"]`
- **Example 2**:
  - Input: `n = 1`
  - Output: `["()"]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Constrained Backtracking DFS | $O\left(\frac{4^n}{\sqrt{n}}\right)$ | $O(n)$ auxiliary | Prunes illegal branches ahead of time; appends '(' when open $< n$ and ')' when close $<$ open. |
| **Tier 2 (Catalan DP)** | Closure Decomposition Dynamic Programming | $O\left(\frac{4^n}{\sqrt{n}}\right)$ | $O\left(\sum C_i\right)$ | Decomposes valid strings into `(left)right` pairs; caches sub-problems bottom-up. |
| **Tier 3 (Iterative)** | Explicit Call-Frame State Stack | $O\left(\frac{4^n}{\sqrt{n}}\right)$ | $O(n)$ | Emulates recursive backtracking using an explicit stack; eliminates compiler call-frame overhead. |
| **Tier 4 (Brute Force)** | Exhaustive $2^{2n}$ Binary Generation and Validation | $O(2^{2n} \cdot n)$ | $O(n)$ | Generates all $2^{2n}$ combinations of length $2n$ blindly, filtering well-formed candidates with a counter. |

---

## 3. Tier 1: Most Optimal Solution (Constrained Backtracking DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

Every well-formed parenthesis string of length $2n$ obeys two mathematical invariants:
1. **Total Count Invariant**: The string contains exactly $n$ opening parentheses `'('` and $n$ closing parentheses `')'`.
2. **Prefix Balance Invariant (Dyck Language)**: In every prefix of the string, the number of opening parentheses must be greater than or equal to the number of closing parentheses ($\text{count}('(') \ge \text{count}(')')$).

Rather than generating invalid strings and validating them afterward, backtracking prunes invalid branches before exploring them:
- An opening parenthesis `'('` can be placed if and only if $\text{open} < n$.
- A closing parenthesis `')'` can be placed if and only if $\text{close} < \text{open}$.
- When $\text{open} == n$ and $\text{close} == n$, the buffer contains a verified valid string of length $2n$; append it to the result set.

**Invariant Proof**:
The number of valid parenthesis strings with $n$ pairs is given by the $n$-th Catalan number:
$$C_n = \frac{1}{n+1} \binom{2n}{n} \sim \frac{4^n}{n^{3/2} \sqrt{\pi}}$$
By construction, every path explored in the decision tree satisfies $\text{close} \le \text{open} \le n$ at all steps.
Because branch termination triggers immediately whenever either condition would be violated, zero invalid strings are ever formed.
Furthermore, each step adds either `'('` or `')'` deterministically, so all $C_n$ distinct paths are reached exactly once without duplicates.

### 3.2 Complexity Analysis
- **Time Complexity**: $O\left(\frac{4^n}{\sqrt{n}}\right)$. The algorithm visits $2 C_n$ states in the decision tree, and each leaf copies a string of length $2n$.
- **Auxiliary Space Complexity**: $O(n)$ auxiliary space. The recursion call stack depth and the reused mutable path buffer have size $2n \le 16$.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <string>

class Solution {
public:
    std::vector<std::string> generateParenthesis(int n) {
        std::vector<std::string> result;
        std::string path;
        backtrack(n, 0, 0, path, result);
        return result;
    }

private:
    void backtrack(int n, int open_count, int close_count, std::string& path, std::vector<std::string>& result) {
        if (open_count == n && close_count == n) {
            result.push_back(path);
            return;
        }

        if (open_count < n) {
            path.push_back('(');
            backtrack(n, open_count + 1, close_count, path, result);
            path.pop_back();
        }

        if (close_count < open_count) {
            path.push_back(')');
            backtrack(n, open_count, close_count + 1, path, result);
            path.pop_back();
        }
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def generateParenthesis(self, n: int) -> List[str]:
        result: List[str] = []
        path: List[str] = []

        def backtrack(open_count: int, close_count: int) -> None:
            if open_count == n and close_count == n:
                result.append("".join(path))
                return

            if open_count < n:
                path.append('(')
                backtrack(open_count + 1, close_count)
                path.pop()

            if close_count < open_count:
                path.append(')')
                backtrack(open_count, close_count + 1)
                path.pop()

        backtrack(0, 0)
        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

public class Solution {
    public List<String> generateParenthesis(int n) {
        List<String> result = new ArrayList<>();
        StringBuilder path = new StringBuilder();
        backtrack(n, 0, 0, path, result);
        return result;
    }

    private void backtrack(int n, int openCount, int closeCount, StringBuilder path, List<String> result) {
        if (openCount == n && closeCount == n) {
            result.add(path.toString());
            return;
        }

        if (openCount < n) {
            path.append('(');
            backtrack(n, openCount + 1, closeCount, path, result);
            path.deleteCharAt(path.length() - 1);
        }

        if (closeCount < openCount) {
            path.append(')');
            backtrack(n, openCount, closeCount + 1, path, result);
            path.deleteCharAt(path.length() - 1);
        }
    }
}
```

#### TypeScript 5
```typescript
function generateParenthesis(n: number): string[] {
    const result: string[] = [];
    const path: string[] = [];

    function backtrack(openCount: number, closeCount: number): void {
        if (openCount === n && closeCount === n) {
            result.push(path.join(""));
            return;
        }

        if (openCount < n) {
            path.push("(");
            backtrack(openCount + 1, closeCount);
            path.pop();
        }

        if (closeCount < openCount) {
            path.push(")");
            backtrack(openCount, closeCount + 1);
            path.pop();
        }
    }

    backtrack(0, 0);
    return result;
}
```

#### Go 1.22
```go
package main

func generateParenthesis(n int) []string {
	var result []string
	path := make([]byte, 2*n)

	var backtrack func(openCount, closeCount int)
	backtrack = func(openCount, closeCount int) {
		if openCount == n && closeCount == n {
			result = append(result, string(path))
			return
		}

		if openCount < n {
			path[openCount+closeCount] = '('
			backtrack(openCount+1, closeCount)
		}

		if closeCount < openCount {
			path[openCount+closeCount] = ')'
			backtrack(openCount, closeCount+1)
		}
	}

	backtrack(0, 0)
	return result
}
```

#### Rust 1.75
```rust
pub struct Solution;

impl Solution {
    pub fn generate_parenthesis(n: i32) -> Vec<String> {
        let mut result = Vec::new();
        let mut path = String::with_capacity((2 * n) as usize);
        Self::backtrack(n, 0, 0, &mut path, &mut result);
        result
    }

    fn backtrack(n: i32, open_count: i32, close_count: i32, path: &mut String, result: &mut Vec<String>) {
        if open_count == n && close_count == n {
            result.push(path.clone());
            return;
        }

        if open_count < n {
            path.push('(');
            Self::backtrack(n, open_count + 1, close_count, path, result);
            path.pop();
        }

        if close_count < open_count {
            path.push(')');
            Self::backtrack(n, open_count, close_count + 1, path, result);
            path.pop();
        }
    }
}
```

---

## 4. Tier 2: Closure Decomposition Dynamic Programming

### 4.1 Implementation Mechanism
Every non-empty well-formed parenthesis sequence can be uniquely factored as:
$$S = (A)B$$
where $A$ and $B$ are valid (possibly empty) parenthesis sequences.
If $A$ has $c$ pairs, then $B$ must have $n - 1 - c$ pairs, with $0 \le c \le n - 1$.
We build the solutions for each $k \in [0, n]$ inductively:

```cpp
class SolutionDP {
public:
    std::vector<std::string> generateParenthesis(int n) {
        std::vector<std::vector<std::string>> dp(n + 1);
        dp[0] = {""};

        for (int i = 1; i <= n; ++i) {
            for (int c = 0; c < i; ++c) {
                for (const auto& left : dp[c]) {
                    for (const auto& right : dp[i - 1 - c]) {
                        dp[i].push_back("(" + left + ")" + right);
                    }
                }
            }
        }

        return dp[n];
    }
};
```

### 4.2 Trade-offs
- Eliminates recursion entirely.
- Generates and stores all intermediate subsets $dp[1 \dots n-1]$, consuming significant additional heap memory compared to the $O(n)$ auxiliary space of backtracking.

---

## 5. Tier 3: Explicit State Machine Iterative Stack Traversal

### 5.1 Algorithmic Structure
We simulate DFS call frames using an explicit stack of action tuples `(step, left_rem, right_rem, char)`.
- Step 1: Evaluate state; push unwind and child transition steps.
- Step 2: Append character to buffer.
- Step 3: Pop character from buffer.

```cpp
class SolutionIterative {
public:
    std::vector<std::string> generateParenthesis(int n) {
        std::vector<std::string> result;
        std::string curr;
        std::vector<std::tuple<int, int, int, char>> stk = {{1, n, n, 0}};

        while (!stk.empty()) {
            auto [step, left, right, c] = stk.back();
            stk.pop_back();

            if (step == 1) {
                if (left == 0 && right == 0) {
                    result.push_back(curr);
                }
                if (left < right) {
                    stk.push_back({3, 0, 0, 0});
                    stk.push_back({1, left, right - 1, 0});
                    stk.push_back({2, 0, 0, ')'});
                }
                if (left > 0) {
                    stk.push_back({3, 0, 0, 0});
                    stk.push_back({1, left - 1, right, 0});
                    stk.push_back({2, 0, 0, '('});
                }
            } else if (step == 2) {
                curr.push_back(c);
            } else if (step == 3) {
                curr.pop_back();
            }
        }
        return result;
    }
};
```

### 5.2 Trade-offs
- Non-recursive control flow guarantees zero recursion stack overflow risks.
- Stack management incurs minor tuple creation overhead.

---

## 6. Tier 4: Brute Force Baseline (Exhaustive $2^{2n}$ Binary Generation)

### 6.1 Mechanical Description
Generate all $2^{2n}$ possible strings consisting of `'('` and `')'`.
For each string, iterate through its characters to check if the prefix balance is always non-negative and ends with total balance zero.

### 6.2 Complexity
- **Time Complexity**: $O(2^{2n} \cdot n)$. For $n = 8$, $2^{16} = 65,536$ candidates are generated, compared to $C_8 = 1,430$ valid solutions, performing $45 \times$ more work than Tier 1.
- **Space Complexity**: $O(n)$ auxiliary memory.
- **Verdict**: Inefficient and scales poorly for larger $n$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Stack Allocation of Buffers**: For $n \le 8$, the maximum string length is $2n \le 16$ bytes.
2. In C++, a `std::string` of length 16 fits directly within Small String Optimization (SSO) storage, requiring zero heap allocations during traversal.
3. **Branch Elimination via Pre-Allocated Arrays**: In Go, allocating `make([]byte, 2*n)` directly overwrites indices at `openCount + closeCount` without calling slice append or reallocation routines.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Minimal Constraint | `n = 1` | Returns `["()"]` | Base recursion emits single pair immediately. |
| Maximum Constraint | `n = 8` | Returns 1,430 strings | Fast DFS finishes in $< 1$ millisecond. |
| Invalid Close Append | `closeCount >= openCount` | Branch pruned | `if (closeCount < openCount)` prevents illegal suffixes. |
| Memory Exhaustion | Arbitrary $n > 20$ | Guarded by problem constraints | Memory bounds stay within hardware registers for $n \le 8$. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What mathematical sequence describes the number of valid parentheses?
The sequence is the Catalan numbers $C_n = \frac{1}{n+1} \binom{2n}{n}$. For $n = 1, 2, 3, 4, 5, 6, 7, 8$, the counts are $1, 2, 5, 14, 42, 132, 429, 1430$.

### 2. Why does the condition `closeCount < openCount` guarantee well-formedness?
A closing parenthesis cannot precede its matching opening parenthesis. Ensuring that the number of closed parentheses never exceeds open parentheses guarantees non-negative prefix balance.

### 3. How does Catalan Dynamic Programming in Tier 2 prevent duplicate strings?
By fixing the first matching pair $(A)$ and requiring $A$ to be the unique substring enclosed by the outermost initial parenthesis, every string has a unique decomposition.

### 4. What is the maximum depth of the recursion tree?
The maximum recursion depth is strictly $2n$. For $n = 8$, the depth is 16 call frames, taking less than 1 KB of stack memory.

### 5. Why is string pre-allocation (`with_capacity` in Rust) important?
Pre-allocating $2n$ bytes ensures that string extensions never cause internal buffer reallocation during DFS operations.

### 6. Can this problem be framed as finding paths in a grid?
Yes. It is equivalent to finding monotonic paths on an $n \times n$ grid from $(0, 0)$ to $(n, n)$ that do not cross above the diagonal line $y = x$ (Dyck paths).

### 7. How does this compare with LeetCode 20 (Valid Parentheses)?
LeetCode 20 verifies whether a given string is valid in $O(N)$ time using a stack. LeetCode 22 generates all valid strings using backtracking.

### 8. Does the order of placing `'('` before `')'` affect correctness?
No, the order only affects the lexicographical permutation order of the output strings. Placing `'('` first generates strings in lexicographical order.

### 9. Why is `StringBuilder.deleteCharAt` used in Java?
`deleteCharAt` pops the last appended character in $O(1)$ time, allowing the same `StringBuilder` instance to be reused across all recursive branches.

### 10. Can bit manipulation generate valid parentheses?
Yes, by interpreting integers from $0$ to $2^{2n}-1$ as binary bitmaps where bit 0 is `'('` and bit 1 is `')'`, though filtering invalid strings takes $O(2^{2n} \cdot n)$ time.

---

## 10. Related Problems and Systematic Progression Links

- [[0020-Valid-Parentheses]]: Validating bracket balance using a stack.
- [[0017-Letter-Combinations-of-a-Phone-Number]]: Cartesian product decision tree backtracking.
- [[0078-Subsets]]: Combinatorial search and subset enumeration.
- LeetCode 32 (Longest Valid Parentheses): Dynamic programming and stack tracking for longest valid substring.
- LeetCode 241 (Different Ways to Add Parentheses): Divide-and-conquer expression evaluation via Catalan parenthesization.
- LeetCode 301 (Remove Invalid Parentheses): BFS / DFS minimum removals to produce valid parentheses.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/generate-parentheses.cpp)
- [Python Implementation](../Python/generate-parentheses.py)
- [Java Implementation](../Java/generate-parentheses.java)
- [TypeScript Implementation](../TypeScript/generate-parentheses.ts)
- [Go Implementation](../Golang/generate-parentheses.go)
- [Rust Implementation](../Rust/generate-parentheses.rs)
