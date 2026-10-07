---
id: leetcode-0207-course-schedule
title: "LeetCode 0207: Course Schedule"
tags:
  - dsa
  - leetcode
  - graph
  - topological-sort
  - bfs
  - dfs
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/course-schedule/"
---

# LeetCode 0207: Course Schedule

## 1. Problem Formalization and Constraints

There are a total of `numCourses` courses you have to take, labeled from `0` to `numCourses - 1`.
You are given an array `prerequisites` where `prerequisites[i] = [a_i, b_i]` indicates that you must take course $b_i$ first if you want to take course $a_i$.
For example, the pair `[0, 1]` indicates that to take course 0 you have to first take course 1.
Return `true` if you can finish all courses. Otherwise, return `false`.

### Constraints
- $1 \le \text{numCourses} \le 2000$
- $0 \le \text{prerequisites.length} \le 5000$
- $\text{prerequisites}[i].\text{length} == 2$
- $0 \le a_i, b_i < \text{numCourses}$
- All the pairs prerequisites[i] are unique.

### Examples
- **Example 1**:
  - Input: `numCourses = 2, prerequisites = [[1,0]]`
  - Output: `true`
  - Explanation: There are a total of 2 courses to take. To take course 1 you should have finished course 0. So it is possible.
- **Example 2**:
  - Input: `numCourses = 2, prerequisites = [[1,0],[0,1]]`
  - Output: `false`
  - Explanation: There are a total of 2 courses to take. To take course 1 you should have finished course 0, and to take course 0 you should also have finished course 1. So it is impossible.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Kahn's Algorithm (BFS In-Degree Peeling) | $O(V + E)$ | $O(V + E)$ auxiliary | Builds adjacency list and in-degree array; processes zero-in-degree nodes in a queue; counts processed vertices. |
| **Tier 2 (Three-Color DFS)** | Depth-First Search Cycle Detection | $O(V + E)$ | $O(V + E)$ auxiliary | Traverses graph coloring nodes as 0 (unvisited), 1 (visiting / on stack), and 2 (visited / finished); detects back-edges. |
| **Tier 3 (Adjacency Matrix BFS)** | Dense Graph Tabulation | $O(V^2 + E)$ | $O(V^2)$ auxiliary | Uses a $V \times V$ boolean matrix; incurs $O(V^2)$ initialization and scan overhead. |
| **Tier 4 (Brute Force Path Tracing)** | Simple DFS Path Traversal | $O(V \cdot (V + E))$ | $O(V)$ auxiliary | Runs DFS from each node tracking visited nodes along the path without cross-path caching. |

*Notation*: $V = \text{numCourses}$ and $E = \text{prerequisites.length}$.

---

## 3. Tier 1: Most Optimal Solution (Kahn's Algorithm / BFS In-Degree Peeling)

### 3.1 Algorithmic Mechanics and Invariant Proof

The problem is equivalent to detecting whether the directed graph $G = (V, E)$ is a Directed Acyclic Graph (DAG).
Courses are vertices, and each prerequisite pair $[a, b]$ represents a directed edge $b \to a$ ($b$ must precede $a$).
1. Construct adjacency list `adj` where `adj[b]` contains all courses that depend on $b$.
2. Compute the in-degree of every vertex $a$, representing the count of unfinished prerequisites for $a$.
3. Enqueue all vertices with $\text{in\_degree} == 0$ (courses with no prerequisites).
4. While the queue is non-empty:
   - Dequeue a course $u$.
   - Increment `visited_count`.
   - For each neighbor $v \in \text{adj}[u]$:
     - Decrement $\text{in\_degree}[v]$.
     - If $\text{in\_degree}[v]$ becomes 0, enqueue $v$.
5. Return `visited_count == numCourses`. If vertices remain with non-zero in-degree, they belong to or depend on a directed cycle.

**Invariant Proof**:
Let $G = (V, E)$ be a directed graph.
A directed graph is acyclic if and only if every non-empty induced subgraph contains at least one vertex with in-degree 0.
Base Case: At step 0, vertices with in-degree 0 have no prerequisites and can be completed immediately.
Inductive Step: Suppose vertices $U \subset V$ have been processed.
Removing $U$ and their outgoing edges produces the induced subgraph $G[V \setminus U]$.
The in-degree of each remaining vertex $v \in V \setminus U$ accurately reflects its remaining prerequisites.
If $G$ is a DAG, $G[V \setminus U]$ must contain at least one vertex with in-degree 0 until all vertices are consumed, yielding `visited_count == |V|`.
Conversely, if $G$ contains a directed cycle $C$, every vertex in $C$ has at least one incoming edge from another vertex in $C$.
No vertex in $C$ can ever have in-degree 0, so no vertex in $C$ is ever enqueued.
Hence `visited_count < |V|`, proving exact correctness.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(V + E)$. Initializing degrees and adjacency lists takes $O(V + E)$. Each vertex is enqueued and dequeued once ($O(V)$), and each directed edge is traversed once ($O(E)$).
- **Auxiliary Space Complexity**: $O(V + E)$ auxiliary space to store the adjacency list, in-degree array, and BFS queue.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <queue>

class Solution {
public:
    bool canFinish(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        std::vector<std::vector<int>> adj(numCourses);
        std::vector<int> in_degree(numCourses, 0);

        for (const auto& pre : prerequisites) {
            int course = pre[0];
            int prerequisite = pre[1];
            adj[prerequisite].push_back(course);
            in_degree[course]++;
        }

        std::queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (in_degree[i] == 0) {
                q.push(i);
            }
        }

        int visited_count = 0;
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            visited_count++;

            for (int neighbor : adj[curr]) {
                if (--in_degree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }

        return visited_count == numCourses;
    }
};
```

#### Python 3
```python
from collections import deque
from typing import List


class Solution:
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        adj: List[List[int]] = [[] for _ in range(numCourses)]
        in_degree: List[int] = [0] * numCourses

        for course, prereq in prerequisites:
            adj[prereq].append(course)
            in_degree[course] += 1

        queue: deque[int] = deque([i for i in range(numCourses) if in_degree[i] == 0])
        visited_count = 0

        while queue:
            curr = queue.popleft()
            visited_count += 1

            for neighbor in adj[curr]:
                in_degree[neighbor] -= 1
                if in_degree[neighbor] == 0:
                    queue.append(neighbor)

        return visited_count == numCourses
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;

class Solution {
    public boolean canFinish(int numCourses, int[][] prerequisites) {
        List<List<Integer>> adj = new ArrayList<>();
        for (int i = 0; i < numCourses; i++) {
            adj.add(new ArrayList<>());
        }
        int[] inDegree = new int[numCourses];

        for (int[] pre : prerequisites) {
            int course = pre[0];
            int prerequisite = pre[1];
            adj.get(prerequisite).add(course);
            inDegree[course]++;
        }

        Queue<Integer> queue = new ArrayDeque<>();
        for (int i = 0; i < numCourses; i++) {
            if (inDegree[i] == 0) {
                queue.offer(i);
            }
        }

        int visitedCount = 0;
        while (!queue.isEmpty()) {
            int curr = queue.poll();
            visitedCount++;

            for (int neighbor : adj.get(curr)) {
                if (--inDegree[neighbor] == 0) {
                    queue.offer(neighbor);
                }
            }
        }

        return visitedCount == numCourses;
    }
}
```

#### TypeScript
```typescript
function canFinish(numCourses: number, prerequisites: number[][]): boolean {
    const adj: number[][] = Array.from({ length: numCourses }, () => []);
    const inDegree: number[] = new Array(numCourses).fill(0);

    for (const [course, prereq] of prerequisites) {
        adj[prereq].push(course);
        inDegree[course]++;
    }

    const queue: number[] = [];
    for (let i = 0; i < numCourses; i++) {
        if (inDegree[i] === 0) {
            queue.push(i);
        }
    }

    let head = 0;
    let visitedCount = 0;

    while (head < queue.length) {
        const curr = queue[head++];
        visitedCount++;

        for (const neighbor of adj[curr]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] === 0) {
                queue.push(neighbor);
            }
        }
    }

    return visitedCount === numCourses;
}
```

#### Go
```go
package main

func canFinish(numCourses int, prerequisites [][]int) bool {
	adj := make([][]int, numCourses)
	inDegree := make([]int, numCourses)

	for _, pre := range prerequisites {
		course := pre[0]
		prereq := pre[1]
		adj[prereq] = append(adj[prereq], course)
		inDegree[course]++
	}

	queue := make([]int, 0, numCourses)
	for i := 0; i < numCourses; i++ {
		if inDegree[i] == 0 {
			queue = append(queue, i)
		}
	}

	visitedCount := 0
	for len(queue) > 0 {
		curr := queue[0]
		queue = queue[1:]
		visitedCount++

		for _, neighbor := range adj[curr] {
			inDegree[neighbor]--
			if inDegree[neighbor] == 0 {
				queue = append(queue, neighbor)
			}
		}
	}

	return visitedCount == numCourses
}
```

#### Rust
```rust
use std::collections::VecDeque;

pub struct Solution;

impl Solution {
    pub fn can_finish(num_courses: i32, prerequisites: Vec<Vec<i32>>) -> bool {
        let n = num_courses as usize;
        let mut adj = vec![Vec::new(); n];
        let mut in_degree = vec![0i32; n];

        for pre in prerequisites {
            let course = pre[0] as usize;
            let prereq = pre[1] as usize;
            adj[prereq].push(course);
            in_degree[course] += 1;
        }

        let mut queue = VecDeque::new();
        for i in 0..n {
            if in_degree[i] == 0 {
                queue.push_back(i);
            }
        }

        let mut visited_count = 0;
        while let Some(curr) = queue.pop_front() {
            visited_count += 1;

            for &neighbor in &adj[curr] {
                in_degree[neighbor] -= 1;
                if in_degree[neighbor] == 0 {
                    queue.push_back(neighbor);
                }
            }
        }

        visited_count == n
    }
}
```

---

## 4. Tier 2: Three-Color DFS Cycle Detection

### 4.1 Mechanical Description
Maintain a `state` array of size $V$:
- `0`: UNVISITED
- `1`: VISITING (currently in recursion call stack)
- `2`: VISITED (subgraph fully explored and verified acyclic)

For each node $i \in [0, V-1]$:
If `state[i] == 0`, run `dfs(i)`.
Inside `dfs(u)`:
Set `state[u] = 1`.
For each neighbor $v$:
If `state[v] == 1`, a back-edge to an ancestor is detected; cycle exists, return false.
If `state[v] == 0` and `!dfs(v)`, return false.
Set `state[u] = 2` upon returning; return true.

```python
def canFinishDFS(numCourses: int, prerequisites: list[list[int]]) -> bool:
    adj: list[list[int]] = [[] for _ in range(numCourses)]
    for c, p in prerequisites:
        adj[p].append(c)

    state = [0] * numCourses

    def has_cycle(u: int) -> bool:
        if state[u] == 1:
            return True
        if state[u] == 2:
            return False
        state[u] = 1
        for v in adj[u]:
            if has_cycle(v):
                return True
        state[u] = 2
        return False

    for i in range(numCourses):
        if state[i] == 0 and has_cycle(i):
            return False
    return True
```

### 4.2 Trade-offs
- Avoids queue allocations and in-degree counts.
- Incurs $O(V)$ recursion stack depth, risking stack overflow in deep chains if recursion limits are low.

---

## 5. Tier 3: Adjacency Matrix BFS

### 5.1 Mechanical Description
Represent the graph as a 2D boolean matrix `matrix[u][v]`.
Incurs $O(V^2)$ initialization space and requires scanning all $V$ potential neighbors for each popped vertex.

### 5.2 Trade-offs
- Extremely cache friendly for dense graphs ($E \approx V^2$).
- Prohibitively slow for sparse graphs where $E \ll V^2$.

---

## 6. Tier 4: Brute Force Path Tracing

### 6.1 Mechanical Description
For every vertex, launch DFS without marking nodes globally visited (only tracking visited nodes along the active path).
Re-visits shared subgraphs repeatedly, taking $O(V \cdot (V + E))$ worst-case time.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Adjacency Vector Layout**: In C++, `std::vector<std::vector<int>>` stores neighbor indices in contiguous blocks, maximizing L1 cache line utilization during edge traversals.
2. **Fixed In-Degree Buffer**: Sizing `in_degree` to $V$ statically avoids dynamic hash map overhead.
3. **Queue Pointer Optimization**: In TypeScript and Go, using an index pointer or pre-allocated slice avoids costly array reshuffling.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Zero prerequisites | `numCourses = 3, prerequisites = []` | Returns `true` | All vertices have in-degree 0; all are enqueued and counted. |
| Self loop | `prerequisites = [[0, 0]]` | Returns `false` | In-degree of 0 is 1; never added to queue; returns false. |
| Disconnected components | Two independent trees | Returns `true` | Kahn's algorithm processes multiple zero in-degree components simultaneously. |
| Mutual dependency cycle | `prerequisites = [[0, 1], [1, 0]]` | Returns `false` | Both have in-degree 1; queue is empty at start; returns false. |
| Large linear chain | $0 \to 1 \to 2 \dots \to N-1$ | Returns `true` | Dequeues sequentially without cycle. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What does a cycle in prerequisites imply?
A cycle means a circular dependency (e.g. course A requires B, and B requires A), making it impossible to satisfy all prerequisites.

### 2. Why is edge direction $b \to a$ rather than $a \to b$?
`[a, b]` means $b$ must be taken before $a$. Modeling the prerequisite $b$ as the source allows peeling nodes forward in completion order.

### 3. What is the difference between Course Schedule and Course Schedule II (LeetCode 210)?
Course Schedule only asks if a topological sort exists (boolean), whereas Course Schedule II requires returning an actual valid ordering of all courses.

### 4. Can Kahn's algorithm detect which courses are part of a cycle?
Yes. All courses that have `in_degree > 0` after Kahn's algorithm completes are either part of a cycle or depend on a cycle.

### 5. Why is Kahn's algorithm preferred over DFS in production?
Kahn's algorithm is iterative, eliminating the possibility of call-stack overflow crashes on very deep dependency graphs.

### 6. What happens if duplicate prerequisite pairs exist?
The problem constraints state that all pairs are unique. If duplicates existed, in-degrees would double-count unless duplicates were deduped.

### 7. What is the maximum number of vertices and edges?
$V \le 2000$ and $E \le 5000$, which is very sparse ($E \ll V^2$), making adjacency list representation ideal.

### 8. How does Go handle queue resizing in Kahn's algorithm?
Pre-allocating `make([]int, 0, numCourses)` ensures the slice backing array never needs reallocation.

### 9. Why does Rust's `VecDeque` work well for BFS?
`VecDeque` implements a ring buffer with $O(1)$ amortized `push_back` and `pop_front`, avoiding vector shift reallocations.

### 10. Does the output topological order have to be unique?
No. There can be many valid topological orders for a DAG. Kahn's algorithm explores any valid topological sorting.

---

## 10. Related Problems and Systematic Progression Links

- [[0133-Clone-Graph]]: Deep cloning graph structures with visited maps.
- [[0200-Number-of-Islands]]: Graph connected component exploration.
- LeetCode 210 (Course Schedule II): Returning the explicit topological sort order.
- LeetCode 269 (Alien Dictionary): Topological sorting on character precedence graphs.
- LeetCode 630 (Course Schedule III): Greedy scheduling with course duration constraints.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/course-schedule.cpp)
- [Python Implementation](../Python/course-schedule.py)
- [Java Implementation](../Java/course-schedule.java)
- [TypeScript Implementation](../TypeScript/course-schedule.ts)
- [Go Implementation](../Golang/course-schedule.go)
- [Rust Implementation](../Rust/course-schedule.rs)
