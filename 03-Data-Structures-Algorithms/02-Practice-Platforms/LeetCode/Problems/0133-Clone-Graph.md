---
id: leetcode-0133-clone-graph
title: "LeetCode 0133: Clone Graph"
tags:
  - dsa
  - leetcode
  - graph
  - bfs
  - dfs
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/clone-graph/"
---

# LeetCode 0133: Clone Graph

## 1. Problem Formalization and Constraints

Given a reference of a node in a connected undirected graph, return a deep copy (clone) of the graph.
Each node in the graph contains a value `val` (an integer) and a list of its neighbors `List[Node]`.

### Node Definition
```text
class Node {
    public int val;
    public List<Node> neighbors;
}
```

### Constraints
- The number of nodes in the graph is in the range $[0, 100]$.
- $1 \le \text{Node.val} \le 100$
- `Node.val` is unique for each node.
- There are no repeated edges and no self-loops in the graph.
- The graph is connected and all nodes can be visited starting from the given node.

### Examples
- **Example 1**:
  - Input: `adjList = [[2,4],[1,3],[2,4],[1,3]]`
  - Output: `[[2,4],[1,3],[2,4],[1,3]]`
- **Example 2**:
  - Input: `adjList = [[]]`
  - Output: `[[]]`
- **Example 3**:
  - Input: `adjList = []`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Breadth-First Search with Hash Map | $O(V + E)$ | $O(V)$ | Queue-driven traversal; creates clone upon discovery and attaches cloned neighbor edges iteratively. |
| **Tier 2 (Recursive DFS)** | Depth-First Search with Hash Map | $O(V + E)$ | $O(V)$ | Recursively clones nodes; returns memoized clone if already visited, bonding stack frames with edge wiring. |
| **Tier 3 (Iterative DFS)** | Stack-Driven Iterative Traversal | $O(V + E)$ | $O(V)$ | Replaces system call stack with explicit heap-allocated stack; avoids stack overflow risks. |
| **Tier 4 (Brute Force)** | Two-Pass Node & Edge Materialization | $O(V + E)$ | $O(V)$ | First pass discovers and instantiates all isolated cloned vertices; second pass populates adjacency lists. |

---

## 3. Tier 1: Most Optimal Solution (Breadth-First Search with Hash Map)

### 3.1 Algorithmic Mechanics and Invariant Proof

Graph deep cloning requires replicating each vertex exactly once while preserving all original edge incidences.
Because graphs can contain cycles, an unmemoized traversal enters an infinite loop.

Procedure:
1. Handle base case: If input `node == nullptr`, return `nullptr`.
2. Maintain a hash map `visited: Map[OriginalNode*, ClonedNode*]` that maps original memory addresses to their newly allocated clones.
3. Instantiate the root clone: `visited[node] = new Node(node->val)`.
4. Initialize a FIFO queue with `node`.
5. While `queue` is not empty:
   - Dequeue `curr = queue.front()`.
   - For each neighbor `nbr` of `curr`:
     - If `nbr` has not been seen in `visited`:
       - Instantiate its clone: `visited[nbr] = new Node(nbr->val)`.
       - Enqueue `nbr` to schedule its neighbors for future processing.
     - Connect the edge in the cloned graph: `visited[curr]->neighbors.push_back(visited[nbr])`.
6. Return `visited[node]`.

**Invariant Proof**:
Every reachable vertex $v \in V$ enters the queue at most once (guaranteed by the check `!visited.contains(nbr)`).
Each edge $(u, v) \in E$ is traversed twice (once from $u$ and once from $v$ in an undirected graph).
At each step, `visited[u]->neighbors` strictly receives `visited[v]`.
Therefore, the cloned graph isomorphic to $G$ is generated with zero pointer alias leaks to the original graph.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(V + E)$. Every vertex is enqueued and dequeued once, and every edge is examined $2 \times |E|$ times.
- **Auxiliary Space Complexity**: $O(V)$. The `visited` hash map holds $V$ entries, and the queue stores at most $V$ node pointers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>
#include <queue>

/*
class Node {
public:
    int val;
    std::vector<Node*> neighbors;
    Node() : val(0) {}
    Node(int _val) : val(_val) {}
    Node(int _val, std::vector<Node*> _neighbors) : val(_val), neighbors(_neighbors) {}
};
*/

class Solution {
public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;

        std::unordered_map<Node*, Node*> visited;
        std::queue<Node*> q;

        visited[node] = new Node(node->val);
        q.push(node);

        while (!q.empty()) {
            Node* curr = q.front();
            q.pop();

            for (Node* neighbor : curr->neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited[neighbor] = new Node(neighbor->val);
                    q.push(neighbor);
                }
                visited[curr]->neighbors.push_back(visited[neighbor]);
            }
        }

        return visited[node];
    }
};
```

#### Python 3
```python
from collections import deque
from typing import Optional

"""
class Node:
    def __init__(self, val = 0, neighbors = None):
        self.val = val
        self.neighbors = neighbors if neighbors is not None else []
"""

class Solution:
    def cloneGraph(self, node: Optional['Node']) -> Optional['Node']:
        if not node:
            return None

        visited = {}
        queue = deque([node])
        visited[node] = Node(node.val)

        while queue:
            curr = queue.popleft()

            for neighbor in curr.neighbors:
                if neighbor not in visited:
                    visited[neighbor] = Node(neighbor.val)
                    queue.append(neighbor)
                visited[curr].neighbors.append(visited[neighbor])

        return visited[node]
```

#### Java 21
```java
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;

class Solution {
    public Node cloneGraph(Node node) {
        if (node == null) return null;

        Map<Node, Node> visited = new HashMap<>();
        Queue<Node> queue = new LinkedList<>();

        visited.put(node, new Node(node.val));
        queue.offer(node);

        while (!queue.isEmpty()) {
            Node curr = queue.poll();

            for (Node neighbor : curr.neighbors) {
                if (!visited.containsKey(neighbor)) {
                    visited.put(neighbor, new Node(neighbor.val));
                    queue.offer(neighbor);
                }
                visited.get(curr).neighbors.add(visited.get(neighbor));
            }
        }

        return visited.get(node);
    }
}
```

#### TypeScript 5
```typescript
function cloneGraph(node: Node | null): Node | null {
    if (!node) return null;

    const visited: Map<Node, Node> = new Map();
    const queue: Node[] = [node];

    visited.set(node, new Node(node.val));

    while (queue.length > 0) {
        const curr = queue.shift()!;

        for (const neighbor of curr.neighbors) {
            if (!visited.has(neighbor)) {
                visited.set(neighbor, new Node(neighbor.val));
                queue.push(neighbor);
            }
            visited.get(curr)!.neighbors.push(visited.get(neighbor)!);
        }
    }

    return visited.get(node)!;
}
```

#### Go 1.22
```go
package main

func cloneGraph(node *Node) *Node {
	if node == nil {
		return nil
	}

	visited := make(map[*Node]*Node)
	queue := []*Node{node}

	visited[node] = &Node{Val: node.Val}

	for len(queue) > 0 {
		curr := queue[0]
		queue = queue[1:]

		for _, neighbor := range curr.Neighbors {
			if _, exists := visited[neighbor]; !exists {
				visited[neighbor] = &Node{Val: neighbor.Val}
				queue = append(queue, neighbor)
			}
			visited[curr].Neighbors = append(visited[curr].Neighbors, visited[neighbor])
		}
	}

	return visited[node]
}
```

#### Rust 2021
```rust
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::{HashMap, VecDeque};

impl Solution {
    pub fn clone_graph(node: Option<Rc<RefCell<Node>>>) -> Option<Rc<RefCell<Node>>> {
        let Some(start_node) = node else {
            return None;
        };

        let mut visited: HashMap<i32, Rc<RefCell<Node>>> = HashMap::new();
        let mut queue = VecDeque::new();

        let clone_start = Rc::new(RefCell::new(Node::new(start_node.borrow().val)));
        visited.insert(start_node.borrow().val, clone_start.clone());
        queue.push_back(start_node);

        while let Some(curr) = queue.pop_front() {
            let curr_borrow = curr.borrow();
            let curr_clone = visited.get(&curr_borrow.val).unwrap().clone();

            for neighbor in &curr_borrow.neighbors {
                let n_val = neighbor.borrow().val;
                if !visited.contains_key(&n_val) {
                    let neighbor_clone = Rc::new(RefCell::new(Node::new(n_val)));
                    visited.insert(n_val, neighbor_clone);
                    queue.push_back(neighbor.clone());
                }
                let neighbor_clone = visited.get(&n_val).unwrap().clone();
                curr_clone.borrow_mut().neighbors.push(neighbor_clone);
            }
        }

        Some(clone_start)
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Recursive DFS with Hash Map)

### 4.1 Algorithmic Mechanics
Recursive depth-first search resolves neighbor lists during function return:
- If `node == nullptr`, return `nullptr`.
- If `node` exists in `visited`, return `visited[node]`.
- Allocate `clone = new Node(node->val)` and record `visited[node] = clone`.
- Iterate through each neighbor `nbr`, appending `dfs(nbr)` to `clone->neighbors`.
- Return `clone`.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(V + E)$.
- **Space Complexity**: $O(V)$ auxiliary memory ($O(V)$ for hash map plus $O(V)$ call stack depth).

### 4.3 Implementation (C++20)
```cpp
#include <unordered_map>

class Solution {
private:
    std::unordered_map<Node*, Node*> visited;

public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;

        if (visited.find(node) != visited.end()) {
            return visited[node];
        }

        Node* clone = new Node(node->val);
        visited[node] = clone;

        for (Node* neighbor : node->neighbors) {
            clone->neighbors.push_back(cloneGraph(neighbor));
        }

        return clone;
    }
};
```

---

## 5. Tier 3: Iterative DFS with Explicit Node Stack

### 5.1 Algorithmic Mechanics
Replaces recursive call frames with an explicit heap-allocated stack `std::stack<Node*>`.
Pushes discovered nodes to the stack, creating clones upon initial encounter.
Pops nodes to expand their neighbor relationships, guaranteeing bounded memory on operating system threads.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(V + E)$.
- **Space Complexity**: $O(V)$ auxiliary space.

### 5.3 Implementation (C++20)
```cpp
#include <unordered_map>
#include <stack>

class Solution {
public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;

        std::unordered_map<Node*, Node*> visited;
        std::stack<Node*> st;

        visited[node] = new Node(node->val);
        st.push(node);

        while (!st.empty()) {
            Node* curr = st.top();
            st.pop();

            for (Node* neighbor : curr->neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited[neighbor] = new Node(neighbor->val);
                    st.push(neighbor);
                }
                visited[curr]->neighbors.push_back(visited[neighbor]);
            }
        }

        return visited[node];
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Two-Pass Vertex Collection and Edge Cloning)

### 6.1 Algorithmic Mechanics
Pass 1 traverses the graph to collect all original vertices into a list, instantiating an unlinked clone for each node.
Pass 2 iterates through the original vertices and their neighbor lists, translating original pointers to cloned pointers and appending edges.
While correct, this approach requires two distinct passes over the graph.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(V + E)$.
- **Space Complexity**: $O(V)$ auxiliary memory.

### 6.3 Implementation (Python 3)
```python
from collections import deque
from typing import Optional

class Solution:
    def cloneGraph(self, node: Optional['Node']) -> Optional['Node']:
        if not node:
            return None

        # Pass 1: Discover all unique vertices
        nodes = []
        visited = set()
        queue = deque([node])
        visited.add(node)

        while queue:
            curr = queue.popleft()
            nodes.append(curr)
            for nbr in curr.neighbors:
                if nbr not in visited:
                    visited.add(nbr)
                    queue.append(nbr)

        # Allocate copies
        clones = {n: Node(n.val) for n in nodes}

        # Pass 2: Connect cloned edges
        for n in nodes:
            for nbr in n.neighbors:
                clones[n].neighbors.append(clones[nbr])

        return clones[node]
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is a visited hash table mandatory when cloning general graphs?</summary>
Graphs can contain cycles.
Without memoizing visited nodes, traversal continues cycling through adjacent nodes indefinitely, causing infinite loops or call stack overflow.
</details>

<details>
<summary>2. Why does the hash table store original pointer addresses as keys instead of node values?</summary>
Using pointer addresses guarantees identity uniqueness even if node values are duplicated or modified in general graph variants.
In this specific LeetCode problem values are unique, so either pointer addresses or node values work.
</details>

<details>
<summary>3. What happens when the input graph is empty (`node == nullptr`)?</summary>
The function returns `nullptr` immediately without entering traversal loops or allocating containers.
</details>

<details>
<summary>4. What is the behavior for a graph consisting of a single isolated node with 0 neighbors?</summary>
The root clone is instantiated with an empty `neighbors` list.
The queue drains immediately, and the single cloned node is returned correctly.
</details>

<details>
<summary>5. How does Rust enforce cyclic reference ownership in graph structures?</summary>
Rust's borrow checker forbids multiple mutable references and circular ownership through standard references.
`Rc<RefCell<Node>>` provides reference-counted shared ownership and interior mutability to link neighbor vectors.
</details>

<details>
<summary>6. Why must the clone be inserted into `visited` before traversing its neighbors in recursive DFS?</summary>
If the clone is not registered in `visited` before recursing into neighbors, a cycle back to the current node would trigger a second allocation of the same node, destroying graph isomorphism.
</details>

<details>
<summary>7. What is the maximum number of edges in a simple undirected graph with $V$ vertices?</summary>
At most $V(V - 1) / 2$ edges, bounded by $100 \times 99 / 2 = 4950$ edges under the problem constraints.
</details>

<details>
<summary>8. How does deep copying differ from shallow copying in graph data structures?</summary>
A shallow copy creates a new node container whose `neighbors` list points back to the original graph's memory addresses.
A deep copy allocates entirely distinct nodes for every reachable vertex.
</details>

<details>
<summary>9. Can this algorithm handle disconnected graphs?</summary>
The problem states the input graph is connected.
If disconnected components exist, an outer loop iterating over all vertices would be required to clone all components.
</details>

<details>
<summary>10. What is the difference between BFS and DFS performance in this problem?</summary>
Both BFS and DFS visit every node and edge with identical $O(V + E)$ asymptotic time and $O(V)$ space.
BFS traverses level by level, whereas DFS descends depth-first along paths.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/clone-graph.cpp)
- [Python Implementation](../Python/clone-graph.py)
- [Java Implementation](../Java/clone-graph.java)
- [TypeScript Implementation](../TypeScript/clone-graph.ts)
- [Go Implementation](../Golang/clone-graph.go)
- [Rust Implementation](../Rust/clone-graph.rs)
