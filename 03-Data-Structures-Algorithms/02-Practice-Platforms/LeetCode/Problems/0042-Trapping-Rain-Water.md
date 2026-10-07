---
id: leetcode-0042-trapping-rain-water
title: "LeetCode 0042: Trapping Rain Water"
tags:
  - dsa
  - leetcode
  - two-pointers
  - monotonic-stack
  - dynamic-programming
  - array
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/trapping-rain-water/"
---

# LeetCode 0042: Trapping Rain Water

## 1. Problem Formalization and Constraints

Given `n` non-negative integers representing an elevation map where the width of each bar is 1, compute how much water it can trap after raining.

### Constraints
- $n == \text{height.length}$
- $1 \le n \le 2 \times 10^4$
- $0 \le \text{height}[i] \le 10^5$

### Examples
- **Example 1**:
  - Input: `height = [0,1,0,2,1,0,1,3,2,1,2,1]`
  - Output: `6`
  - Explanation: The above elevation map (black section) is represented by array `[0,1,0,2,1,0,1,3,2,1,2,1]`. In this case, 6 units of rain water (blue section) are being trapped.
- **Example 2**:
  - Input: `height = [4,2,0,3,2,5]`
  - Output: `9`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Two Pointers with Invariant Bounds | $O(N)$ | $O(1)$ | Advances inward pointer with smaller boundary; computes vertical trapped water without auxiliary arrays. |
| **Tier 2 (Monotonic Stack)** | Monotonic Decreasing Stack | $O(N)$ | $O(N)$ | Calculates water horizontally in slabs between current bar and bounded left wall. |
| **Tier 3 (Dynamic Programming)** | Prefix and Suffix Max Tables | $O(N)$ | $O(N)$ | Pre-computes `leftMax[i]` and `rightMax[i]`; water trapped at $i$ is $\min(\text{leftMax}[i], \text{rightMax}[i]) - \text{height}[i]$. |
| **Tier 4 (Brute Force)** | Per-Element Boundary Scanning | $O(N^2)$ | $O(1)$ | Scans left and right for every individual bar to find enclosing boundaries independently. |

---

## 3. Tier 1: Most Optimal Solution (Two Pointers with Invariant Bounds)

### 3.1 Algorithmic Mechanics and Invariant Proof

Water trapped above any index $i$ is governed by the bottleneck:
$$\text{water}[i] = \max(0, \min(\max_{0 \le k \le i} \text{height}[k], \max_{i \le k < n} \text{height}[k]) - \text{height}[i])$$

Instead of computing both global maxima explicitly, two pointers `left = 0` and `right = n - 1` maintain running prefix and suffix bounds `leftMax` and `rightMax`:
1. If `height[left] <= height[right]`:
   - The global right boundary for `left` is guaranteed to be at least `height[right] >= height[left]`.
   - Therefore, the bottleneck for `left` is strictly bounded by `leftMax`, regardless of whether `rightMax` is the absolute true global maximum further inward.
   - If `height[left] >= leftMax`, update `leftMax = height[left]`.
   - Otherwise, trap `leftMax - height[left]` water.
   - Advance `left++`.
2. Otherwise (`height[left] > height[right]`):
   - The bottleneck for `right` is strictly bounded by `rightMax`.
   - If `height[right] >= rightMax`, update `rightMax = height[right]`.
   - Otherwise, trap `rightMax - height[right]` water.
   - Advance `right--`.

**Invariant Proof**:
At every step where `height[left] <= height[right]`, we know $\text{leftMax} \le \text{height}[right] \le \max_{k \ge \text{left}} \text{height}[k]$.
Thus, $\min(\text{leftMax}, \text{globalRightMax}) = \text{leftMax}$.
The volume of water supported above `left` depends only on `leftMax`, rendering the exact value of the inward right maximum irrelevant.
Symmetry holds identically for `right` when `height[left] > height[right]`.
Every column is processed exactly once, yielding an optimal $O(N)$ time and $O(1)$ space algorithm.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. The two pointers start at opposite ends and converge inward, decrementing the distance $right - left$ by 1 at every iteration.
- **Auxiliary Space Complexity**: $O(1)$. Uses only four scalar integer variables (`left`, `right`, `leftMax`, `rightMax`).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int trap(std::vector<int>& height) {
        if (height.empty()) return 0;

        int left = 0;
        int right = static_cast<int>(height.size()) - 1;
        int leftMax = 0;
        int rightMax = 0;
        int totalWater = 0;

        while (left < right) {
            if (height[left] <= height[right]) {
                if (height[left] >= leftMax) {
                    leftMax = height[left];
                } else {
                    totalWater += leftMax - height[left];
                }
                ++left;
            } else {
                if (height[right] >= rightMax) {
                    rightMax = height[right];
                } else {
                    totalWater += rightMax - height[right];
                }
                --right;
            }
        }

        return totalWater;
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def trap(self, height: List[int]) -> int:
        if not height:
            return 0

        left, right = 0, len(height) - 1
        left_max, right_max = 0, 0
        total_water = 0

        while left < right:
            if height[left] <= height[right]:
                if height[left] >= left_max:
                    left_max = height[left]
                else:
                    total_water += left_max - height[left]
                left += 1
            else:
                if height[right] >= right_max:
                    right_max = height[right]
                else:
                    total_water += right_max - height[right]
                right -= 1

        return total_water
```

#### Java 21
```java
class Solution {
    public int trap(int[] height) {
        if (height == null || height.length == 0) return 0;

        int left = 0;
        int right = height.length - 1;
        int leftMax = 0;
        int rightMax = 0;
        int totalWater = 0;

        while (left < right) {
            if (height[left] <= height[right]) {
                if (height[left] >= leftMax) {
                    leftMax = height[left];
                } else {
                    totalWater += leftMax - height[left];
                }
                left++;
            } else {
                if (height[right] >= rightMax) {
                    rightMax = height[right];
                } else {
                    totalWater += rightMax - height[right];
                }
                right--;
            }
        }

        return totalWater;
    }
}
```

#### TypeScript 5
```typescript
function trap(height: number[]): number {
    if (!height || height.length === 0) return 0;

    let left = 0;
    let right = height.length - 1;
    let leftMax = 0;
    let rightMax = 0;
    let totalWater = 0;

    while (left < right) {
        if (height[left] <= height[right]) {
            if (height[left] >= leftMax) {
                leftMax = height[left];
            } else {
                totalWater += leftMax - height[left];
            }
            left++;
        } else {
            if (height[right] >= rightMax) {
                rightMax = height[right];
            } else {
                totalWater += rightMax - height[right];
            }
            right--;
        }
    }

    return totalWater;
}
```

#### Go 1.22
```go
package main

func trap(height []int) int {
	if len(height) == 0 {
		return 0
	}

	left, right := 0, len(height)-1
	leftMax, rightMax := 0, 0
	totalWater := 0

	for left < right {
		if height[left] <= height[right] {
			if height[left] >= leftMax {
				leftMax = height[left]
			} else {
				totalWater += leftMax - height[left]
			}
			left++
		} else {
			if height[right] >= rightMax {
				rightMax = height[right]
			} else {
				totalWater += rightMax - height[right]
			}
			right--
		}
	}

	return totalWater
}
```

#### Rust 2021
```rust
impl Solution {
    pub fn trap(height: Vec<i32>) -> i32 {
        if height.is_empty() {
            return 0;
        }

        let mut left = 0;
        let mut right = height.len() - 1;
        let mut left_max = 0;
        let mut right_max = 0;
        let mut total_water = 0;

        while left < right {
            if height[left] <= height[right] {
                if height[left] >= left_max {
                    left_max = height[left];
                } else {
                    total_water += left_max - height[left];
                }
                left += 1;
            } else {
                if height[right] >= right_max {
                    right_max = height[right];
                } else {
                    total_water += right_max - height[right];
                }
                right -= 1;
            }
        }

        total_water
    }
}
```

---

## 4. Tier 2: Space-Optimized Monotonic Stack (Horizontal Layer Accumulation)

### 4.1 Algorithmic Mechanics
Instead of calculating trapped water vertically column-by-column, a monotonic decreasing stack computes water horizontally slab-by-slab:
- Maintain a stack of bar indices in strictly non-increasing order of height.
- When `height[curr] > height[stack.top()]`:
  - Pop `mid = stack.top()`.
  - If the stack becomes empty, there is no left wall to bound water; break.
  - The left wall is `left = stack.top()`.
  - The bounded distance is `distance = curr - left - 1`.
  - The bounded height is `boundedHeight = min(height[left], height[curr]) - height[mid]`.
  - Accumulate `water += distance * boundedHeight`.
- Push `curr` onto the stack.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each index is pushed and popped at most once.
- **Space Complexity**: $O(N)$ auxiliary stack space.

### 4.3 Implementation (C++20)
```cpp
#include <vector>
#include <stack>
#include <algorithm>

class Solution {
public:
    int trap(std::vector<int>& height) {
        std::stack<int> st;
        int totalWater = 0;

        for (int i = 0; i < static_cast<int>(height.size()); ++i) {
            while (!st.empty() && height[i] > height[st.top()]) {
                int mid = st.top();
                st.pop();

                if (st.empty()) break;

                int left = st.top();
                int distance = i - left - 1;
                int boundedHeight = std::min(height[left], height[i]) - height[mid];

                totalWater += distance * boundedHeight;
            }
            st.push(i);
        }

        return totalWater;
    }
};
```

---

## 5. Tier 3: Dynamic Programming Prefix/Suffix Bounds

### 5.1 Algorithmic Mechanics
Allocate two auxiliary arrays `leftMax` and `rightMax` of length $N$:
1. Forward pass: `leftMax[i] = max(leftMax[i-1], height[i])`.
2. Backward pass: `rightMax[i] = max(rightMax[i+1], height[i])`.
3. Compute water column-wise: `water += min(leftMax[i], rightMax[i]) - height[i]`.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ three sequential linear passes.
- **Space Complexity**: $O(N)$ auxiliary memory for the two precomputed arrays.

### 5.3 Implementation (C++20)
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int trap(std::vector<int>& height) {
        int n = static_cast<int>(height.size());
        if (n == 0) return 0;

        std::vector<int> leftMax(n);
        std::vector<int> rightMax(n);

        leftMax[0] = height[0];
        for (int i = 1; i < n; ++i) {
            leftMax[i] = std::max(leftMax[i - 1], height[i]);
        }

        rightMax[n - 1] = height[n - 1];
        for (int i = n - 2; i >= 0; --i) {
            rightMax[i] = std::max(rightMax[i + 1], height[i]);
        }

        int totalWater = 0;
        for (int i = 0; i < n; ++i) {
            totalWater += std::min(leftMax[i], rightMax[i]) - height[i];
        }

        return totalWater;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Per-Column Min-Max Scan)

### 6.1 Algorithmic Mechanics
For every bar $i$ from $0$ to $N - 1$:
- Scan left from $i$ down to $0$ to find `maxLeft = max(height[0..i])`.
- Scan right from $i$ up to $N - 1$ to find `maxRight = max(height[i..N-1])`.
- Add `min(maxLeft, maxRight) - height[i]` to `totalWater`.
This incurs quadratic operations without memory caching.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(1)$ memory.

### 6.3 Implementation (Python 3)
```python
from typing import List

class Solution:
    def trap(self, height: List[int]) -> int:
        n = len(height)
        total_water = 0

        for i in range(n):
            max_left = max(height[:i + 1])
            max_right = max(height[i:])
            total_water += min(max_left, max_right) - height[i]

        return total_water
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why does the condition `height[left] <= height[right]` justify moving `left` instead of `right`?</summary>
When `height[left] <= height[right]`, `height[right]` acts as a sufficiently tall right wall.
The effective water height at `left` cannot exceed `leftMax`.
Hence, the water trapped above `left` is determined exclusively by `leftMax - height[left]`.
</details>

<details>
<summary>2. How does the Monotonic Stack approach differ conceptually from the Two Pointers approach?</summary>
Two pointers compute water vertically column by column.
The monotonic stack computes water horizontally slab by slab across bounded elevation basins.
</details>

<details>
<summary>3. What is the minimum length of `height` required to trap any water?</summary>
At least 3 bars are required ($n \ge 3$).
With $n < 3$, no basin can form between two outer walls, yielding 0 trapped water.
</details>

<details>
<summary>4. What happens if all bars are sorted in strictly increasing or strictly decreasing order?</summary>
Water cannot be contained because no enclosing trough exists.
The algorithm calculates 0 trapped water in $O(N)$ time.
</details>

<details>
<summary>5. How does memory caching favor Two Pointers over Dynamic Programming?</summary>
Two pointers read elements sequentially inward from both ends of the array, maximizing cache line utilization with zero heap buffer allocations.
The DP approach allocates two large vectors and requires multiple memory passes.
</details>

<details>
<summary>6. How does Trapping Rain Water differ from Container With Most Water (LeetCode 11)?</summary>
Container With Most Water chooses exactly two lines to form a single rectangular container.
Trapping Rain Water calculates the aggregate fluid retained across all intermediate terrain depressions.
</details>

<details>
<summary>7. What occurs if `height` contains all zeros?</summary>
`leftMax` and `rightMax` remain 0 throughout execution, correctly resulting in 0 trapped water.
</details>

<details>
<summary>8. How can this algorithm be extended to 3D Trapping Rain Water (LeetCode 407)?</summary>
In 2D elevation grids, water spills toward the boundary along 4 directions.
Solving 3D requires a Min-Heap initialized with all boundary cells, progressively expanding inward via Dijkstra-like BFS.
</details>

<details>
<summary>9. What is the maximum possible value of total trapped water under the constraints?</summary>
With $N = 2 \times 10^4$ and bar heights up to $10^5$, maximum water can reach $(2 \times 10^4 - 2) \times 10^5 \approx 2 \times 10^9$.
This fits comfortably inside a standard 32-bit signed integer (`i32`).
</details>

<details>
<summary>10. Why is strictly decreasing order preserved in the monotonic stack?</summary>
A strictly decreasing stack records falling elevation steps that form the left wall of a potential water container.
When a taller bar appears, it triggers pops that fill the basin up to the level of the preceding wall.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/trapping-rain-water.cpp)
- [Python Implementation](../Python/trapping-rain-water.py)
- [Java Implementation](../Java/trapping-rain-water.java)
- [TypeScript Implementation](../TypeScript/trapping-rain-water.ts)
- [Go Implementation](../Golang/trapping-rain-water.go)
- [Rust Implementation](../Rust/trapping-rain-water.rs)
