---
id: leetcode-0084-largest-rectangle-in-histogram
title: "LeetCode 0084: Largest Rectangle in Histogram"
tags:
  - dsa
  - leetcode
  - array
  - stack
  - monotonic-stack
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/largest-rectangle-in-histogram/"
---

# LeetCode 0084: Largest Rectangle in Histogram

## 1. Problem Formalization and Constraints

Given an array of integers `heights` representing the histogram's bar height where the width of each bar is 1, return the area of the largest rectangle in the histogram.

### Constraints
- $1 \le \text{heights.length} \le 10^5$
- $0 \le \text{heights}[i] \le 10^4$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Single-Pass Monotonic Increasing Stack | $O(N)$ | $O(N)$ | Maintains strictly ascending bar indices; computes maximal width on pop. |
| **Tier 2 (Two Arrays)** | Precomputed Left and Right Limits | $O(N)$ | $O(N)$ | Computes nearest smaller element on left and right via two passes. |
| **Tier 3 (Divide & Conquer)** | Minimum Element Pivot (Segment Tree) | $O(N \log N)$ | $O(N)$ | Splits at minimum bar; degrades to $O(N^2)$ without range minimum tree. |
| **Tier 4 (Brute Force)** | Exhaustive Interval Enumeration | $O(N^2)$ | $O(1)$ | Tests every $(i, j)$ pair tracking minimum height; quadratic latency. |

---

## 3. Tier 1: Most Optimal Solution (Single-Pass Monotonic Increasing Stack)

### 3.1 Algorithmic Mechanics and Invariant Proof

For every bar $i$ of height $h = \text{heights}[i]$, the largest rectangle using bar $i$ as its limiting (minimum) height extends:
- As far left as possible until reaching the first bar strictly shorter than $h$.
- As far right as possible until reaching the first bar strictly shorter than $h$.
We maintain a stack of bar indices in monotonically non-decreasing order of height.
When encountering a bar $i$ whose height is strictly smaller than the height of the stack top:
1. The stack top index `top` has found its right boundary at index $i$.
2. We pop `top` with height $h = \text{heights}[\text{top}]$.
3. The left boundary of `top` is the new stack top (or 0 if stack is empty).
4. The maximal rectangle width bounded by height $h$ is $i - 1 - \text{stk.top()}$ (or $i$ if stack is empty).
5. Area $h \times \text{width}$ is evaluated against the running maximum.
By iterating through index $N$ with virtual height 0, all remaining bars in the stack are cleanly resolved.
Each index is pushed and popped at most once, yielding strictly $O(N)$ runtime.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int largestRectangleArea(const std::vector<int>& heights) {
        std::stack<int> stk;
        int max_area = 0;
        const int n = static_cast<int>(heights.size());

        for (int i = 0; i <= n; ++i) {
            const int curr_height = (i == n) ? 0 : heights[i];
            while (!stk.empty() && heights[stk.top()] >= curr_height) {
                const int h = heights[stk.top()];
                stk.pop();
                const int width = stk.empty() ? i : i - 1 - stk.top();
                max_area = std::max(max_area, h * width);
            }
            stk.push(i);
        }
        return max_area;
    }
};
```

#### Python
```python
class Solution:
    def largestRectangleArea(self, heights: list[int]) -> int:
        stack = []
        max_area = 0
        n = len(heights)

        for i in range(n + 1):
            curr_height = 0 if i == n else heights[i]
            while stack and heights[stack[-1]] >= curr_height:
                h = heights[stack.pop()]
                width = i if not stack else i - 1 - stack[-1]
                max_area = max(max_area, h * width)
            stack.append(i)

        return max_area
```

#### Java
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public int largestRectangleArea(int[] heights) {
        int n = heights.length;
        Deque<Integer> stack = new ArrayDeque<>();
        int maxArea = 0;

        for (int i = 0; i <= n; i++) {
            int currHeight = (i == n) ? 0 : heights[i];
            while (!stack.isEmpty() && heights[stack.peek()] >= currHeight) {
                int h = heights[stack.pop()];
                int width = stack.isEmpty() ? i : i - 1 - stack.peek();
                maxArea = Math.max(maxArea, h * width);
            }
            stack.push(i);
        }
        return maxArea;
    }
}
```

#### TypeScript
```typescript
function largestRectangleArea(heights: number[]): number {
    const n = heights.length;
    const stack: number[] = [];
    let maxArea = 0;

    for (let i = 0; i <= n; i++) {
        const currHeight = (i === n) ? 0 : heights[i];
        while (stack.length > 0 && heights[stack[stack.length - 1]] >= currHeight) {
            const h = heights[stack.pop()!];
            const width = stack.length === 0 ? i : i - 1 - stack[stack.length - 1];
            maxArea = Math.max(maxArea, h * width);
        }
        stack.push(i);
    }
    return maxArea;
}
```

#### Golang
```go
package main

func largestRectangleArea(heights []int) int {
	n := len(heights)
	stack := make([]int, 0, n+1)
	maxArea := 0

	for i := 0; i <= n; i++ {
		currHeight := 0
		if i < n {
			currHeight = heights[i]
		}
		for len(stack) > 0 && heights[stack[len(stack)-1]] >= currHeight {
			h := heights[stack[len(stack)-1]]
			stack = stack[:len(stack)-1]
			width := i
			if len(stack) > 0 {
				width = i - 1 - stack[len(stack)-1]
			}
			area := h * width
			if area > maxArea {
				maxArea = area
			}
		}
		stack = append(stack, i)
	}
	return maxArea
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn largest_rectangle_area(heights: Vec<i32>) -> i32 {
        let n = heights.len();
        let mut stack: Vec<usize> = Vec::with_capacity(n + 1);
        let mut max_area: i32 = 0;

        for i in 0..=n {
            let curr_height = if i < n { heights[i] } else { 0 };

            while let Some(&top_idx) = stack.last() {
                if heights[top_idx] >= curr_height {
                    stack.pop();
                    let h = heights[top_idx];
                    let width = match stack.last() {
                        Some(&prev_idx) => (i - 1 - prev_idx) as i32,
                        None => i as i32,
                    };
                    max_area = max_area.max(h * width);
                } else {
                    break;
                }
            }
            stack.push(i);
        }
        max_area
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N)$ amortized runtime; each index is pushed once and popped once.
- **Space Complexity**: $O(N)$ auxiliary space for the stack in monotonically increasing distributions.
- **Cache Locality**: Contiguous vector-backed stacks exhibit high spatial cache locality and predictable branching.
