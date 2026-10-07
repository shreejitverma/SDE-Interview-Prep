---
id: leetcode-0056-merge-intervals
title: "LeetCode 0056: Merge Intervals"
tags:
  - dsa
  - leetcode
  - array
  - sorting
  - intervals
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/merge-intervals/"
---

# LeetCode 0056: Merge Intervals

## 1. Problem Formalization and Constraints

Given an array of `intervals` where `intervals[i] = [start_i, end_i]`, merge all overlapping intervals, and return an array of the non-overlapping intervals that cover all the intervals in the input.

### Constraints
- $1 \le \text{intervals.length} \le 10^4$
- $\text{intervals}[i]\text{.length} == 2$
- $0 \le start_i \le end_i \le 10^4$

### Examples
- **Example 1**:
  - Input: `intervals = [[1,3],[2,6],[8,10],[15,18]]`
  - Output: `[[1,6],[8,10],[15,18]]`
  - Explanation: Since intervals `[1,3]` and `[2,6]` overlap, merge them into `[1,6]`.
- **Example 2**:
  - Input: `intervals = [[1,4],[4,5]]`
  - Output: `[[1,5]]`
  - Explanation: Intervals `[1,4]` and `[4,5]` are considered overlapping.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Sort + Single-Pass Greedy Merge | $O(N \log N)$ | $O(1)$ | Sorts intervals by start time; compares current start with running interval end. |
| **Tier 2 (Space-Optimized)** | In-Place Input Buffer Compaction | $O(N \log N)$ | $O(1)$ | Sorts and writes merged intervals directly over the input buffer; zero new allocations. |
| **Tier 3 (Time-Optimized Alternative)** | Sweep-Line Coordinate Discretization | $O(N \log N)$ | $O(N)$ | Decomposes intervals into entry (+1) and exit (-1) events to track active spans. |
| **Tier 4 (Brute Force)** | Connected Components Graph Search | $O(N^2)$ | $O(N^2)$ | Constructs graph where overlapping pairs share edges; merges connected components via BFS. |

---

## 3. Tier 1: Most Optimal Solution (Sort + Single-Pass Greedy Merge)

### 3.1 Algorithmic Mechanics and Invariant Proof

1. Sort all intervals by their start time in ascending order.
If two intervals share identical start times, their relative order is irrelevant because both will be merged.
2. Initialize the result array with the first sorted interval.
3. Iterate through subsequent intervals:
   - Let the last merged interval be `[prev_start, prev_end]` and the current interval be `[curr_start, curr_end]`.
   - If `curr_start <= prev_end`, an overlap exists.
   Update `prev_end = max(prev_end, curr_end)`.
   - Otherwise, `curr_start > prev_end`, indicating no overlap.
   Append `[curr_start, curr_end]` to the result as a new disjoint interval.

**Invariant Proof**:
Because intervals are sorted by start time, for any interval $j > i$, we know $\text{start}_j \ge \text{start}_i$.
Thus, interval $j$ can only overlap with the immediately preceding active merged interval.
If interval $j$ does not overlap with the active merged interval ($\text{start}_j > \text{end}_{\text{merged}}$), no future interval $k > j$ can overlap with it either, because $\text{start}_k \ge \text{start}_j > \text{end}_{\text{merged}}$.
Hence, local decisions are globally optimal.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$. Sorting dominates the runtime; the subsequent linear pass takes $O(N)$ time.
- **Space Complexity**: $O(1)$ auxiliary space (ignoring the memory required for sorting and output storage).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        if (intervals.empty()) return {};

        std::sort(intervals.begin(), intervals.end());
        std::vector<std::vector<int>> merged;
        merged.push_back(intervals[0]);

        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] <= merged.back()[1]) {
                merged.back()[1] = std::max(merged.back()[1], intervals[i][1]);
            } else {
                merged.push_back(intervals[i]);
            }
        }
        return merged;
    }
};
```

#### Python 3.12
```python
class Solution:
    def merge(self, intervals: list[list[int]]) -> list[list[int]]:
        if not intervals:
            return []

        intervals.sort(key=lambda x: x[0])
        merged: list[list[int]] = [intervals[0]]

        for i in range(1, len(intervals)):
            curr = intervals[i]
            if curr[0] <= merged[-1][1]:
                merged[-1][1] = max(merged[-1][1], curr[1])
            else:
                merged.append(curr)

        return merged
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
    public int[][] merge(int[][] intervals) {
        if (intervals == null || intervals.length <= 1) {
            return intervals;
        }

        Arrays.sort(intervals, (a, b) -> Integer.compare(a[0], b[0]));
        List<int[]> merged = new ArrayList<>();
        merged.add(intervals[0]);

        for (int i = 1; i < intervals.length; i++) {
            int[] last = merged.get(merged.size() - 1);
            int[] curr = intervals[i];

            if (curr[0] <= last[1]) {
                last[1] = Math.max(last[1], curr[1]);
            } else {
                merged.add(curr);
            }
        }

        return merged.toArray(new int[merged.size()][]);
    }
}
```

#### TypeScript
```typescript
function merge(intervals: number[][]): number[][] {
    if (intervals.length <= 1) return intervals;

    intervals.sort((a, b) => a[0] - b[0]);
    const result: number[][] = [intervals[0]];

    for (let i = 1; i < intervals.length; i++) {
        const curr = intervals[i];
        const last = result[result.length - 1];

        if (curr[0] <= last[1]) {
            last[1] = Math.max(last[1], curr[1]);
        } else {
            result.push(curr);
        }
    }

    return result;
}
```

#### Go
```go
package main

import "sort"

func merge(intervals [][]int) [][]int {
    if len(intervals) <= 1 {
        return intervals
    }

    sort.Slice(intervals, func(i, j int) bool {
        return intervals[i][0] < intervals[j][0]
    })

    var result [][]int
    result = append(result, intervals[0])

    for i := 1; i < len(intervals); i++ {
        last := &result[len(result)-1]
        curr := intervals[i]

        if curr[0] <= (*last)[1] {
            if curr[1] > (*last)[1] {
                (*last)[1] = curr[1]
            }
        } else {
            result = append(result, curr)
        }
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn merge(mut intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        if intervals.len() <= 1 {
            return intervals;
        }

        intervals.sort_unstable_by_key(|item| item[0]);
        let mut result = Vec::with_capacity(intervals.len());
        result.push(intervals[0].clone());

        for i in 1..intervals.len() {
            let last = result.last_mut().unwrap();
            let curr = &intervals[i];

            if curr[0] <= last[1] {
                last[1] = last[1].max(curr[1]);
            } else {
                result.push(curr.clone());
            }
        }

        result
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (In-Place Input Buffer Compaction)

### 4.1 Algorithmic Mechanics and Invariant Proof

Instead of instantiating an auxiliary collection or vector for the output, overwrite the input buffer in-place.
1. Sort the input array `intervals` by start time.
2. Maintain a write index `idx = 0`.
3. For each interval from index $1$ to $N - 1$:
   - If `intervals[i][0] <= intervals[idx][1]`, merge into `intervals[idx][1] = max(intervals[idx][1], intervals[i][1])`.
   - Otherwise, advance `idx++` and copy `intervals[idx] = intervals[i]`.
4. Truncate the array to length `idx + 1`.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ sorting time plus $O(N)$ compaction.
- **Space Complexity**: $O(1)$ auxiliary space beyond in-place sort stack memory.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        if (intervals.empty()) return {};

        std::sort(intervals.begin(), intervals.end());
        int idx = 0;

        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] <= intervals[idx][1]) {
                intervals[idx][1] = std::max(intervals[idx][1], intervals[i][1]);
            } else {
                ++idx;
                intervals[idx] = intervals[i];
            }
        }
        intervals.resize(idx + 1);
        return intervals;
    }
};
```

#### Python 3.12
```python
class Solution:
    def merge(self, intervals: list[list[int]]) -> list[list[int]]:
        if not intervals:
            return []

        intervals.sort(key=lambda x: x[0])
        idx = 0

        for i in range(1, len(intervals)):
            if intervals[i][0] <= intervals[idx][1]:
                intervals[idx][1] = max(intervals[idx][1], intervals[i][1])
            else:
                idx += 1
                intervals[idx] = intervals[i]

        return intervals[: idx + 1]
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int[][] merge(int[][] intervals) {
        if (intervals == null || intervals.length <= 1) return intervals;

        Arrays.sort(intervals, (a, b) -> Integer.compare(a[0], b[0]));
        int idx = 0;

        for (int i = 1; i < intervals.length; i++) {
            if (intervals[i][0] <= intervals[idx][1]) {
                intervals[idx][1] = Math.max(intervals[idx][1], intervals[i][1]);
            } else {
                idx++;
                intervals[idx] = intervals[i];
            }
        }

        return Arrays.copyOfRange(intervals, 0, idx + 1);
    }
}
```

#### TypeScript
```typescript
function merge(intervals: number[][]): number[][] {
    if (intervals.length <= 1) return intervals;

    intervals.sort((a, b) => a[0] - b[0]);
    let idx = 0;

    for (let i = 1; i < intervals.length; i++) {
        if (intervals[i][0] <= intervals[idx][1]) {
            intervals[idx][1] = Math.max(intervals[idx][1], intervals[i][1]);
        } else {
            idx++;
            intervals[idx] = intervals[i];
        }
    }

    intervals.length = idx + 1;
    return intervals;
}
```

#### Go
```go
package main

import "sort"

func merge(intervals [][]int) [][]int {
    if len(intervals) <= 1 {
        return intervals
    }

    sort.Slice(intervals, func(i, j int) bool {
        return intervals[i][0] < intervals[j][0]
    })

    idx := 0
    for i := 1; i < len(intervals); i++ {
        if intervals[i][0] <= intervals[idx][1] {
            if intervals[i][1] > intervals[idx][1] {
                intervals[idx][1] = intervals[i][1]
            }
        } else {
            idx++
            intervals[idx] = intervals[i]
        }
    }

    return intervals[:idx+1]
}
```

#### Rust
```rust
impl Solution {
    pub fn merge(mut intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        if intervals.len() <= 1 {
            return intervals;
        }

        intervals.sort_unstable_by_key(|item| item[0]);
        let mut idx = 0;

        for i in 1..intervals.len() {
            if intervals[i][0] <= intervals[idx][1] {
                intervals[idx][1] = intervals[idx][1].max(intervals[i][1]);
            } else {
                idx += 1;
                intervals[idx] = intervals[i].clone();
            }
        }

        intervals.truncate(idx + 1);
        intervals
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Sweep-Line Coordinate Discretization)

### 5.1 Algorithmic Mechanics and Invariant Proof

Split each interval $[start, end]$ into two events:
- A start event at coordinate $start$ with value $+1$.
- An end event at coordinate $end$ with value $-1$.
Sort all events by coordinate.
When coordinates coincide, prioritize start events before end events to maintain continuity of contiguous intervals.
Iterate through the events and maintain a running prefix sum of active intervals:
- When the prefix sum transitions from $0$ to positive, mark the start of a merged interval.
- When the prefix sum returns to $0$, mark the end of the merged interval.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ to sort the $2N$ endpoints.
- **Space Complexity**: $O(N)$ to store event objects.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        std::vector<std::pair<int, int>> events;
        for (const auto& iv : intervals) {
            events.emplace_back(iv[0], -1); // -1 represents start
            events.emplace_back(iv[1], 1);   // 1 represents end
        }

        std::sort(events.begin(), events.end());
        std::vector<std::vector<int>> result;
        int active = 0, start = 0;

        for (const auto& ev : events) {
            if (active == 0) {
                start = ev.first;
            }
            active += (ev.second == -1) ? 1 : -1;
            if (active == 0) {
                result.push_back({start, ev.first});
            }
        }
        return result;
    }
};
```

#### Python 3.12
```python
class Solution:
    def merge(self, intervals: list[list[int]]) -> list[list[int]]:
        events: list[tuple[int, int]] = []
        for s, e in intervals:
            events.append((s, -1))
            events.append((e, 1))

        events.sort()
        result: list[list[int]] = []
        active = 0
        start = 0

        for pt, typ in events:
            if active == 0:
                start = pt
            active += 1 if typ == -1 else -1
            if active == 0:
                result.append([start, pt])

        return result
```

#### Java 21
```java
import java.util.*;

class Solution {
    public int[][] merge(int[][] intervals) {
        List<int[]> events = new ArrayList<>();
        for (int[] iv : intervals) {
            events.add(new int[]{iv[0], -1});
            events.add(new int[]{iv[1], 1});
        }

        events.sort((a, b) -> a[0] != b[0] ? Integer.compare(a[0], b[0]) : Integer.compare(a[1], b[1]));
        List<int[]> result = new ArrayList<>();
        int active = 0;
        int start = 0;

        for (int[] ev : events) {
            if (active == 0) {
                start = ev[0];
            }
            active += (ev[1] == -1) ? 1 : -1;
            if (active == 0) {
                result.add(new int[]{start, ev[0]});
            }
        }

        return result.toArray(new int[result.size()][]);
    }
}
```

#### TypeScript
```typescript
function merge(intervals: number[][]): number[][] {
    const events: [number, number][] = [];
    for (const [s, e] of intervals) {
        events.push([s, -1]);
        events.push([e, 1]);
    }

    events.sort((a, b) => a[0] !== b[0] ? a[0] - b[0] : a[1] - b[1]);
    const result: number[][] = [];
    let active = 0;
    let start = 0;

    for (const [pt, typ] of events) {
        if (active === 0) {
            start = pt;
        }
        active += typ === -1 ? 1 : -1;
        if (active === 0) {
            result.push([start, pt]);
        }
    }

    return result;
}
```

#### Go
```go
package main

import "sort"

type Event struct {
    point int
    kind  int
}

func merge(intervals [][]int) [][]int {
    events := make([]Event, 0, len(intervals)*2)
    for _, iv := range intervals {
        events = append(events, Event{point: iv[0], kind: -1})
        events = append(events, Event{point: iv[1], kind: 1})
    }

    sort.Slice(events, func(i, j int) bool {
        if events[i].point != events[j].point {
            return events[i].point < events[j].point
        }
        return events[i].kind < events[j].kind
    })

    var result [][]int
    active := 0
    start := 0

    for _, ev := range events {
        if active == 0 {
            start = ev.point
        }
        if ev.kind == -1 {
            active++
        } else {
            active--
        }
        if active == 0 {
            result = append(result, []int{start, ev.point})
        }
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn merge(intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let mut events: Vec<(i32, i32)> = Vec::with_capacity(intervals.len() * 2);
        for iv in &intervals {
            events.push((iv[0], -1));
            events.push((iv[1], 1));
        }

        events.sort_unstable();
        let mut result = Vec::new();
        let mut active = 0;
        let mut start = 0;

        for (pt, typ) in events {
            if active == 0 {
                start = pt;
            }
            active += if typ == -1 { 1 } else { -1 };
            if active == 0 {
                result.push(vec![start, pt]);
            }
        }

        result
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Connected Components Graph Search)

### 6.1 Algorithmic Mechanics and Invariant Proof

Build an undirected graph where each vertex represents an input interval.
Add an edge between interval $u$ and interval $v$ if they overlap:
$$\max(u_{\text{start}}, v_{\text{start}}) \le \min(u_{\text{end}}, v_{\text{end}})$$
Use BFS or DFS to identify connected components.
For each connected component, the merged interval has start equal to the minimum start of all component nodes and end equal to the maximum end.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Testing all pairs of intervals takes $O(N^2)$ time, and graph traversal runs in $O(V + E) = O(N + N^2)$ time.
- **Space Complexity**: $O(N^2)$ to store the adjacency list representation of the overlap graph.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>
#include <queue>

class Solution {
public:
    std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        int n = static_cast<int>(intervals.size());
        std::vector<std::vector<int>> adj(n);

        auto overlaps = [](const std::vector<int>& a, const std::vector<int>& b) {
            return std::max(a[0], b[0]) <= std::min(a[1], b[1]);
        };

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (overlaps(intervals[i], intervals[j])) {
                    adj[i].push_back(j);
                    adj[j].push_back(i);
                }
            }
        }

        std::vector<bool> visited(n, false);
        std::vector<std::vector<int>> result;

        for (int i = 0; i < n; ++i) {
            if (visited[i]) continue;
            visited[i] = true;
            std::queue<int> q;
            q.push(i);
            int min_s = intervals[i][0];
            int max_e = intervals[i][1];

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                min_s = std::min(min_s, intervals[u][0]);
                max_e = std::max(max_e, intervals[u][1]);

                for (int v : adj[u]) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
            result.push_back({min_s, max_e});
        }
        std::sort(result.begin(), result.end());
        return result;
    }
};
```

#### Python 3.12
```python
from collections import deque

class Solution:
    def merge(self, intervals: list[list[int]]) -> list[list[int]]:
        n = len(intervals)
        adj: list[list[int]] = [[] for _ in range(n)]

        def overlaps(a: list[int], b: list[int]) -> bool:
            return max(a[0], b[0]) <= min(a[1], b[1])

        for i in range(n):
            for j in range(i + 1, n):
                if overlaps(intervals[i], intervals[j]):
                    adj[i].append(j)
                    adj[j].append(i)

        visited = [False] * n
        result: list[list[int]] = []

        for i in range(n):
            if visited[i]:
                continue
            visited[i] = True
            q = deque([i])
            min_s = intervals[i][0]
            max_e = intervals[i][1]

            while q:
                u = q.popleft()
                min_s = min(min_s, intervals[u][0])
                max_e = max(max_e, intervals[u][1])
                for v in adj[u]:
                    if not visited[v]:
                        visited[v] = True
                        q.append(v)

            result.append([min_s, max_e])

        result.sort()
        return result
```

#### Java 21
```java
import java.util.*;

class Solution {
    public int[][] merge(int[][] intervals) {
        int n = intervals.length;
        List<List<Integer>> adj = new ArrayList<>();
        for (int i = 0; i < n; i++) adj.add(new ArrayList<>());

        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (Math.max(intervals[i][0], intervals[j][0]) <= Math.min(intervals[i][1], intervals[j][1])) {
                    adj.get(i).add(j);
                    adj.get(j).add(i);
                }
            }
        }

        boolean[] visited = new boolean[n];
        List<int[]> result = new ArrayList<>();

        for (int i = 0; i < n; i++) {
            if (visited[i]) continue;
            visited[i] = true;
            Queue<Integer> q = new ArrayDeque<>();
            q.add(i);
            int minS = intervals[i][0];
            int maxE = intervals[i][1];

            while (!q.isEmpty()) {
                int u = q.poll();
                minS = Math.min(minS, intervals[u][0]);
                maxE = Math.max(maxE, intervals[u][1]);
                for (int v : adj.get(u)) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.add(v);
                    }
                }
            }
            result.add(new int[]{minS, maxE});
        }

        result.sort((a, b) -> Integer.compare(a[0], b[0]));
        return result.toArray(new int[result.size()][]);
    }
}
```

#### TypeScript
```typescript
function merge(intervals: number[][]): number[][] {
    const n = intervals.length;
    const adj: number[][] = Array.from({ length: n }, () => []);

    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            if (Math.max(intervals[i][0], intervals[j][0]) <= Math.min(intervals[i][1], intervals[j][1])) {
                adj[i].push(j);
                adj[j].push(i);
            }
        }
    }

    const visited = new Uint8Array(n);
    const result: number[][] = [];

    for (let i = 0; i < n; i++) {
        if (visited[i]) continue;
        visited[i] = 1;
        const q: number[] = [i];
        let minS = intervals[i][0];
        let maxE = intervals[i][1];

        let head = 0;
        while (head < q.length) {
            const u = q[head++];
            minS = Math.min(minS, intervals[u][0]);
            maxE = Math.max(maxE, intervals[u][1]);
            for (const v of adj[u]) {
                if (!visited[v]) {
                    visited[v] = 1;
                    q.push(v);
                }
            }
        }
        result.push([minS, maxE]);
    }

    result.sort((a, b) => a[0] - b[0]);
    return result;
}
```

#### Go
```go
package main

import "sort"

func merge(intervals [][]int) [][]int {
    n := len(intervals)
    adj := make([][]int, n)

    for i := 0; i < n; i++ {
        for j := i + 1; j < n; j++ {
            maxStart := intervals[i][0]
            if intervals[j][0] > maxStart {
                maxStart = intervals[j][0]
            }
            minEnd := intervals[i][1]
            if intervals[j][1] < minEnd {
                minEnd = intervals[j][1]
            }
            if maxStart <= minEnd {
                adj[i] = append(adj[i], j)
                adj[j] = append(adj[j], i)
            }
        }
    }

    visited := make([]bool, n)
    var result [][]int

    for i := 0; i < n; i++ {
        if visited[i] {
            continue
        }
        visited[i] = true
        q := []int{i}
        minS := intervals[i][0]
        maxE := intervals[i][1]

        for len(q) > 0 {
            u := q[0]
            q = q[1:]
            if intervals[u][0] < minS {
                minS = intervals[u][0]
            }
            if intervals[u][1] > maxE {
                maxE = intervals[u][1]
            }
            for _, v := range adj[u] {
                if !visited[v] {
                    visited[v] = true
                    q = append(q, v)
                }
            }
        }
        result = append(result, []int{minS, maxE})
    }

    sort.Slice(result, func(i, j int) bool {
        return result[i][0] < result[j][0]
    })
    return result
}
```

#### Rust
```rust
use std::collections::VecDeque;

impl Solution {
    pub fn merge(intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let n = intervals.len();
        let mut adj = vec![Vec::new(); n];

        for i in 0..n {
            for j in (i + 1)..n {
                let max_s = intervals[i][0].max(intervals[j][0]);
                let min_e = intervals[i][1].min(intervals[j][1]);
                if max_s <= min_e {
                    adj[i].push(j);
                    adj[j].push(i);
                }
            }
        }

        let mut visited = vec![false; n];
        let mut result = Vec::new();

        for i in 0..n {
            if visited[i] {
                continue;
            }
            visited[i] = true;
            let mut q = VecDeque::new();
            q.push_back(i);
            let mut min_s = intervals[i][0];
            let mut max_e = intervals[i][1];

            while let Some(u) = q.pop_front() {
                min_s = min_s.min(intervals[u][0]);
                max_e = max_e.max(intervals[u][1]);
                for &v in &adj[u] {
                    if !visited[v] {
                        visited[v] = true;
                        q.push_back(v);
                    }
                }
            }
            result.push(vec![min_s, max_e]);
        }

        result.sort_unstable();
        result
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is sorting by start time sufficient to detect all overlaps in a single pass?</summary>
Sorting ensures that whenever interval $B$ follows interval $A$, its start time is at least $A$'s start time.
Therefore, $B$ can only overlap with $A$ if $B$'s start is less than or equal to $A$'s end.
If it does not overlap, no subsequent interval can overlap with $A$ either.
</details>

<details>
<summary>2. How are intervals that touch at a single point (e.g. `[1, 4]` and `[4, 5]`) handled?</summary>
The condition `curr_start <= prev_end` evaluates $4 \le 4$ to true.
The intervals merge into `[1, 5]`, correctly adhering to the problem definition.
</details>

<details>
<summary>3. What happens when an interval is completely contained inside another (e.g. `[1, 6]` followed by `[2, 4]`)?</summary>
The condition `curr_start <= prev_end` holds ($2 \le 6$).
The new end becomes $\max(6, 4) = 6$.
The contained interval is subsumed without extending the outer boundary.
</details>

<details>
<summary>4. What is the advantage of using in-place compaction over allocating a new vector?</summary>
In-place compaction avoids heap allocations, improving spatial cache locality and reducing garbage collection pressure in managed runtimes.
</details>

<details>
<summary>5. Why must start events precede end events in the sweep-line approach when coordinates match?</summary>
If interval $[1, 2]$ meets $[2, 3]$, both events occur at coordinate 2.
Processing the start event (+1) before the end event (-1) ensures the active counter does not drop to 0 prematurely, correctly merging them into $[1, 3]$.
</details>

<details>
<summary>6. How can an Interval Tree or Segment Tree solve interval merging?</summary>
An Interval Tree can query overlapping ranges dynamically in $O(\log N + k)$ time, making it suitable for streaming interval arrivals.
</details>

<details>
<summary>7. What is the runtime difference between `sort_unstable` and `sort` in Rust for this problem?</summary>
`sort_unstable` uses pattern-defeating quicksort (pdqsort), avoiding memory allocation and running faster than stable merge sort.
Since relative order of identical intervals does not affect the merged result, `sort_unstable` is optimal.
</details>

<details>
<summary>8. How does the graph connected component approach fail at scale?</summary>
Constructing the $O(N^2)$ graph for $N = 10^4$ requires $10^8$ overlap comparisons and up to $10^8$ edges, exceeding memory limits and causing timeouts.
</details>

<details>
<summary>9. What edge case occurs when all intervals in the input are identical?</summary>
Every subsequent interval satisfies `curr[0] <= last[1]` and `curr[1] <= last[1]`.
The end value remains unchanged, condensing all duplicate intervals into a single interval.
</details>

<details>
<summary>10. What edge case occurs when the input array has length 1?</summary>
The algorithm returns the single interval immediately without performing any loop iterations.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/merge-intervals.cpp)
- [Python Implementation](../Python/merge-intervals.py)
- [Java Implementation](../Java/merge-intervals.java)
- [TypeScript Implementation](../TypeScript/merge-intervals.ts)
- [Go Implementation](../Golang/merge-intervals.go)
- [Rust Implementation](../Rust/merge-intervals.rs)
