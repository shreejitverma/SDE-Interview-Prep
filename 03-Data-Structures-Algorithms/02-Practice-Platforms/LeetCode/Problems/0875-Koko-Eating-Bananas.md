---
id: leetcode-0875-koko-eating-bananas
title: "LeetCode 0875: Koko Eating Bananas"
tags:
  - dsa
  - leetcode
  - array
  - binary-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/koko-eating-bananas/"
---

# LeetCode 0875: Koko Eating Bananas

## 1. Problem Formalization and Constraints

Koko loves to eat bananas.
There are $n$ piles of bananas, the $i$-th pile has `piles[i]` bananas.
The guards have gone and will come back in $h$ hours.
Koko can decide her bananas-per-hour eating speed of $k$.
Each hour, she chooses some pile of bananas and eats $k$ bananas from that pile.
If the pile has less than $k$ bananas, she eats all of them instead and will not eat any more bananas during this hour.
Koko likes to eat slowly but still wants to finish eating all the bananas before the guards return.
Return the minimum integer $k$ such that she can eat all the bananas within $h$ hours.

### Constraints
- $1 \le \text{piles.length} \le 10^4$
- $\text{piles.length} \le h \le 10^9$
- $1 \le \text{piles}[i] \le 10^9$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Binary Search on Answer Space | $O(N \log(\max(\text{piles})))$ | $O(1)$ | Exploits monotonic feasibility of speed $k$; finds first valid speed. |
| **Tier 2 (Alternative Range)** | Average-Bounded Binary Search | $O(N \log(\max(\text{piles})))$ | $O(1)$ | Tightens lower bound to $\lceil \sum \text{piles} / h \rceil$; reduces search steps. |
| **Tier 3 (Linear Scan)** | Linear Speed Increment | $O(N \cdot \max(\text{piles}))$ | $O(1)$ | Tries $k = 1, 2, 3, \dots$ until feasibility condition met; TLE. |
| **Tier 4 (Brute Force)** | Complete Simulation | $O(N \cdot \sum \text{piles})$ | $O(1)$ | Simulates hour-by-hour consumption for each speed; catastrophic latency. |

---

## 3. Tier 1: Most Optimal Solution (Binary Search on Answer Space)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $f(k) = \sum_{i=1}^n \lceil \text{piles}[i] / k \rceil$ denote the total hours required to consume all piles at speed $k$.
The function $f(k)$ is monotonically non-increasing: as $k$ increases, $\lceil \text{pile} / k \rceil$ never increases.
The feasibility condition $f(k) \le h$ divides the integer domain $[1, \max(\text{piles})]$ into two contiguous zones:
- An infeasible prefix where $f(k) > h$ (false)
- A feasible suffix where $f(k) \le h$ (true)
Standard binary search over $k \in [1, \max(\text{piles})]$ finds the transition boundary (the minimum feasible $k$) in $\lceil \log_2(\max(\text{piles})) \rceil \le 30$ evaluations.
To avoid 32-bit integer overflow during accumulation, the hour sum is stored in a 64-bit integer (`long long` in C++, `int64` in Go, `long` in Java, `i64` in Rust).

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int minEatingSpeed(const std::vector<int>& piles, int h) {
        int left = 1;
        int right = *std::max_element(piles.begin(), piles.end());

        while (left <= right) {
            const int mid = left + (right - left) / 2;
            if (canFinish(piles, h, mid)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }

private:
    bool canFinish(const std::vector<int>& piles, int h, int k) {
        long long hours = 0;
        for (const int pile : piles) {
            hours += (static_cast<long long>(pile) + k - 1) / k;
        }
        return hours <= h;
    }
};
```

#### Python
```python
class Solution:
    def minEatingSpeed(self, piles: list[int], h: int) -> int:
        left, right = 1, max(piles)

        def can_finish(k: int) -> bool:
            hours = sum((p + k - 1) // k for p in piles)
            return hours <= h

        while left <= right:
            mid = (left + right) // 2
            if can_finish(mid):
                right = mid - 1
            else:
                left = mid + 1

        return left
```

#### Java
```java
class Solution {
    public int minEatingSpeed(int[] piles, int h) {
        int left = 1;
        int right = 1;
        for (int pile : piles) {
            if (pile > right) right = pile;
        }

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (canFinish(piles, h, mid)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }

    private boolean canFinish(int[] piles, int h, int k) {
        long hours = 0;
        for (int pile : piles) {
            hours += (pile + k - 1L) / k;
        }
        return hours <= h;
    }
}
```

#### TypeScript
```typescript
function minEatingSpeed(piles: number[], h: number): number {
    let left = 1;
    let right = Math.max(...piles);

    const canFinish = (k: number): boolean => {
        let hours = 0;
        for (const pile of piles) {
            hours += Math.ceil(pile / k);
        }
        return hours <= h;
    };

    while (left <= right) {
        const mid = Math.floor(left + (right - left) / 2);
        if (canFinish(mid)) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return left;
}
```

#### Golang
```go
package leetcode

func minEatingSpeed(piles []int, h int) int {
	left := 1
	right := 1
	for _, pile := range piles {
		if pile > right {
			right = pile
		}
	}

	canFinish := func(k int) bool {
		var hours int64 = 0
		for _, pile := range piles {
			hours += int64((pile + k - 1) / k)
		}
		return hours <= int64(h)
	}

	for left <= right {
		mid := left + (right-left)/2
		if canFinish(mid) {
			right = mid - 1
		} else {
			left = mid + 1
		}
	}
	return left
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn min_eating_speed(piles: Vec<i32>, h: i32) -> i32 {
        let mut left: i32 = 1;
        let mut right: i32 = *piles.iter().max().unwrap_or(&1);

        let can_finish = |k: i32| -> bool {
            let mut hours: i64 = 0;
            for &pile in &piles {
                hours += ((pile as i64) + (k as i64) - 1) / (k as i64);
            }
            hours <= (h as i64)
        };

        while left <= right {
            let mid = left + (right - left) / 2;
            if can_finish(mid) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        left
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N \log(\max(\text{piles})))$ where $N \le 10^4$ and $\max(\text{piles}) \le 10^9$.
The logarithmic factor performs $\approx 30$ iterations, each scanning $N$ items.
- **Space Complexity**: Strictly $O(1)$ auxiliary storage.
- **Cache Optimization**: In each feasibility check, the array `piles` is traversed linearly, maximizing CPU hardware prefetching and L1 cache hits.
