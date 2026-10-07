---
id: leetcode-0739-daily-temperatures
title: "LeetCode 0739: Daily Temperatures"
tags:
  - dsa
  - leetcode
  - array
  - stack
  - monotonic-stack
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/daily-temperatures/"
---

# LeetCode 0739: Daily Temperatures

## 1. Problem Formalization and Constraints

Given an array of integers `temperatures` represents the daily temperatures, return an array `answer` such that `answer[i]` is the number of days you have to wait after the $i$-th day to get a warmer temperature.
If there is no future day for which this is possible, keep `answer[i] == 0` instead.

### Constraints
- $1 \le \text{temperatures.length} \le 10^5$
- $30 \le \text{temperatures}[i] \le 100$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Monotonic Decreasing Stack | $O(N)$ | $O(N)$ | Stores unfulfilled candidate indices; resolves next warmer day in amortized $O(1)$. |
| **Tier 2 (Space-Optimized Tabulation)** | Backward Jump Optimization | $O(N)$ | $O(1)$ extra | Uses output array itself to jump forward skipping cooler intermediate days. |
| **Tier 3 (Next Array)** | Temperature Bucket Array | $O(N \cdot |\Sigma|)$ | $O(|\Sigma|)$ | Tracks nearest occurrences of each temperature in $[30, 100]$; $|\Sigma| = 71$. |
| **Tier 4 (Brute Force)** | Forward Nested Scan | $O(N^2)$ | $O(1)$ | Scans future indices until warmer temperature found; quadratic latency. |

---

## 3. Tier 1: Most Optimal Solution (Monotonic Decreasing Stack)

### 3.1 Algorithmic Mechanics and Invariant Proof

We maintain a stack storing indices whose next warmer temperature has not yet been discovered.
The stack maintains the invariant: temperatures at stored indices are monotonically non-increasing from bottom to top.
When processing day $i$ with temperature $T_i$:
1. While the stack is non-empty and $T_{\text{top}} < T_i$, index $\text{top}$ has found its immediate next warmer day at index $i$.
2. We pop $\text{top}$ and record `answer[top] = i - top`.
3. After resolving all cooler past days, we push index $i$ onto the stack.
Because every index is pushed onto the stack exactly once and popped at most once, the total number of stack operations across all $N$ elements is at most $2N$.
This guarantees strict $O(N)$ linear execution time.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    std::vector<int> dailyTemperatures(const std::vector<int>& temperatures) {
        const size_t n = temperatures.size();
        std::vector<int> result(n, 0);
        std::stack<int> stk;

        for (size_t i = 0; i < n; ++i) {
            while (!stk.empty() && temperatures[stk.top()] < temperatures[i]) {
                const int prev = stk.top();
                stk.pop();
                result[prev] = static_cast<int>(i) - prev;
            }
            stk.push(static_cast<int>(i));
        }
        return result;
    }
};
```

#### Python
```python
class Solution:
    def dailyTemperatures(self, temperatures: list[int]) -> list[int]:
        n = len(temperatures)
        result = [0] * n
        stack = []

        for i in range(n):
            while stack and temperatures[stack[-1]] < temperatures[i]:
                prev_idx = stack.pop()
                result[prev_idx] = i - prev_idx
            stack.append(i)

        return result
```

#### Java
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public int[] dailyTemperatures(int[] temperatures) {
        int n = temperatures.length;
        int[] result = new int[n];
        Deque<Integer> stack = new ArrayDeque<>();

        for (int i = 0; i < n; i++) {
            while (!stack.isEmpty() && temperatures[stack.peek()] < temperatures[i]) {
                int prev = stack.pop();
                result[prev] = i - prev;
            }
            stack.push(i);
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function dailyTemperatures(temperatures: number[]): number[] {
    const n = temperatures.length;
    const result: number[] = new Array(n).fill(0);
    const stack: number[] = [];

    for (let i = 0; i < n; i++) {
        while (stack.length > 0 && temperatures[stack[stack.length - 1]] < temperatures[i]) {
            const prev = stack.pop()!;
            result[prev] = i - prev;
        }
        stack.push(i);
    }
    return result;
}
```

#### Golang
```go
package leetcode

func dailyTemperatures(temperatures []int) []int {
	n := len(temperatures)
	result := make([]int, n)
	stack := make([]int, 0, n)

	for i := 0; i < n; i++ {
		for len(stack) > 0 && temperatures[stack[len(stack)-1]] < temperatures[i] {
			prev := stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			result[prev] = i - prev
		}
		stack = append(stack, i)
	}
	return result
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn daily_temperatures(temperatures: Vec<i32>) -> Vec<i32> {
        let n = temperatures.len();
        let mut result = vec![0; n];
        let mut stack: Vec<usize> = Vec::with_capacity(n);

        for i in 0..n {
            while let Some(&prev) = stack.last() {
                if temperatures[prev] < temperatures[i] {
                    stack.pop();
                    result[prev] = (i - prev) as i32;
                } else {
                    break;
                }
            }
            stack.push(i);
        }
        result
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N)$ amortized linear time across all iterations.
- **Space Complexity**: $O(N)$ worst-case auxiliary space for the monotonic index stack (e.g. on monotonically non-increasing inputs).
- **Cache Locality**: Elements are accessed sequentially, with stack operations maintaining high temporal locality in L1 cache.
