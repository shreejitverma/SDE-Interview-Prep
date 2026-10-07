---
id: leetcode-0853-car-fleet
title: "LeetCode 0853: Car Fleet"
tags:
  - dsa
  - leetcode
  - array
  - stack
  - sorting
  - monotonic-stack
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/car-fleet/"
---

# LeetCode 0853: Car Fleet

## 1. Problem Formalization and Constraints

There are $n$ cars at given `position` going towards a `target` mile with constant `speed`.
A car cannot pass another car ahead of it, but it can catch up to it and drive bumper-to-bumper at the same speed.
A car fleet is some non-empty set of cars driving at the same position and speed.
A single car is also a car fleet.
If a car catches up to a car fleet right at the destination point, it is still considered as having joined the fleet.
Return the number of car fleets that will arrive at the destination.

### Constraints
- $n == \text{position.length} == \text{speed.length}$
- $1 \le n \le 10^5$
- $0 < \text{target} \le 10^6$
- $0 \le \text{position}[i] < \text{target}$
- All values of `position` are unique.
- $0 < \text{speed}[i] \le 10^6$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Position Sort + Reverse Greedy Scan | $O(N \log N)$ | $O(N)$ | Sorts by position; any car taking $\le$ leader arrival time merges into leader fleet. |
| **Tier 2 (Monotonic Stack)** | Explicit Monotonic Time Stack | $O(N \log N)$ | $O(N)$ | Pushes arrival times; pops cars that catch up to cars ahead; stack size equals fleets. |
| **Tier 3 (Bucket Sort)** | Target Array Bucket Mapping | $O(\text{Target})$ | $O(\text{Target})$ | Eliminates $O(N \log N)$ sorting when $N \approx \text{Target}$; high auxiliary memory. |
| **Tier 4 (Simulation)** | Discrete Time Step Simulation | $O(\text{Time} \cdot N)$ | $O(N)$ | Simulates physics step by step; fails on floating-point catch-ups and large distances. |

---

## 3. Tier 1: Most Optimal Solution (Position Sort + Reverse Greedy Scan)

### 3.1 Algorithmic Mechanics and Invariant Proof

Each car $i$ with initial position $P_i$ and speed $S_i$ would reach `target` in uninterrupted time $T_i = (\text{target} - P_i) / S_i$.
Because cars cannot overtake each other:
1. Consider car $A$ ahead of car $B$ ($P_A > P_B$).
2. If $T_B \le T_A$, car $B$ reaches or catches up to car $A$ on or before the target.
Thus, car $B$ is forced to join car $A$'s fleet and will arrive at time $T_A$.
3. If $T_B > T_A$, car $B$ arrives strictly later than car $A$. Car $B$ cannot be slowed down by car $A$ and starts a new trailing fleet.
By sorting cars in ascending order of initial position $P_i$ and traversing backwards from the car closest to the target:
- We track the maximum arrival time seen so far: $\text{max\_time}$.
- Whenever a car has $T_i > \text{max\_time}$, it cannot catch up to any car ahead of it, so it initiates a new fleet ($\text{fleets} \mathrel{+}= 1$) and sets the new bottleneck time $\text{max\_time} = T_i$.
Sorting takes $O(N \log N)$ and the reverse scan takes $O(N)$, yielding $O(N \log N)$ total time.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int carFleet(int target, const std::vector<int>& position, const std::vector<int>& speed) {
        const size_t n = position.size();
        if (n == 0) return 0;

        std::vector<std::pair<int, double>> cars;
        cars.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            const double time = static_cast<double>(target - position[i]) / speed[i];
            cars.emplace_back(position[i], time);
        }

        std::sort(cars.begin(), cars.end());

        int fleets = 0;
        double max_time = 0.0;

        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            if (cars[i].second > max_time) {
                max_time = cars[i].second;
                ++fleets;
            }
        }
        return fleets;
    }
};
```

#### Python
```python
class Solution:
    def carFleet(self, target: int, position: list[int], speed: list[int]) -> int:
        if not position:
            return 0

        cars = sorted(zip(position, speed), key=lambda x: x[0], reverse=True)
        fleets = 0
        max_time = 0.0

        for pos, spd in cars:
            time = (target - pos) / spd
            if time > max_time:
                max_time = time
                fleets += 1

        return fleets
```

#### Java
```java
import java.util.Arrays;

class Solution {
    public int carFleet(int target, int[] position, int[] speed) {
        int n = position.length;
        if (n == 0) return 0;

        double[][] cars = new double[n][2];
        for (int i = 0; i < n; i++) {
            cars[i][0] = position[i];
            cars[i][1] = (double) (target - position[i]) / speed[i];
        }

        Arrays.sort(cars, (a, b) -> Double.compare(a[0], b[0]));

        int fleets = 0;
        double maxTime = 0.0;

        for (int i = n - 1; i >= 0; i--) {
            if (cars[i][1] > maxTime) {
                maxTime = cars[i][1];
                fleets++;
            }
        }
        return fleets;
    }
}
```

#### TypeScript
```typescript
function carFleet(target: number, position: number[], speed: number[]): number {
    const n = position.length;
    if (n === 0) return 0;

    const cars: [number, number][] = [];
    for (let i = 0; i < n; i++) {
        cars.push([position[i], (target - position[i]) / speed[i]]);
    }

    cars.sort((a, b) => a[0] - b[0]);

    let fleets = 0;
    let maxTime = 0;

    for (let i = n - 1; i >= 0; i--) {
        if (cars[i][1] > maxTime) {
            maxTime = cars[i][1];
            fleets++;
        }
    }
    return fleets;
}
```

#### Golang
```go
package main

import "sort"

type car struct {
	pos  int
	time float64
}

func carFleet(target int, position []int, speed []int) int {
	n := len(position)
	if n == 0 {
		return 0
	}

	cars := make([]car, n)
	for i := 0; i < n; i++ {
		cars[i] = car{
			pos:  position[i],
			time: float64(target-position[i]) / float64(speed[i]),
		}
	}

	sort.Slice(cars, func(i, j int) bool {
		return cars[i].pos < cars[j].pos
	})

	fleets := 0
	var maxTime float64 = 0

	for i := n - 1; i >= 0; i-- {
		if cars[i].time > maxTime {
			maxTime = cars[i].time
			fleets++
		}
	}
	return fleets
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn car_fleet(target: i32, position: Vec<i32>, speed: Vec<i32>) -> i32 {
        let n = position.len();
        if n == 0 {
            return 0;
        }

        let mut cars: Vec<(i32, f64)> = position
            .into_iter()
            .zip(speed.into_iter())
            .map(|(p, s)| (p, (target - p) as f64 / s as f64))
            .collect();

        cars.sort_unstable_by_key(|&(p, _)| p);

        let mut fleets = 0;
        let mut max_time = 0.0;

        for &(_, time) in cars.iter().rev() {
            if time > max_time {
                max_time = time;
                fleets += 1;
            }
        }
        fleets
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N \log N)$ dominated by sorting $N$ cars by starting position.
- **Space Complexity**: $O(N)$ to store combined position-time pairs.
- **Numeric Stability**: Accurate floating point division (`double` / `f64`) prevents round-off errors when determining fleet mergers.
