---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# 🔄 SDE Common Patterns & Frameworks

---

## Coding Patterns

### 1. Sliding Window
**When:** Contiguous subarray/substring with constraint
**Template:** Expand right pointer, shrink left when invalid
**Problems:** Max subarray sum of size K, longest substring without repeating, minimum window substring

### 2. Two Pointers
**When:** Sorted array, pair finding, partitioning
**Template:** Left at 0, right at n-1, move based on comparison
**Problems:** Two Sum (sorted), 3Sum, container with most water, remove duplicates

### 3. Fast & Slow Pointers
**When:** Cycle detection, middle of linked list, happy number
**Template:** Slow moves 1 step, fast moves 2 steps

### 4. BFS/DFS on Graphs
**When:** Connected components, shortest path (unweighted), traversal
**BFS Template:** Queue + visited set. Level-order = track level size.
**DFS Template:** Stack or recursion + visited set.

### 5. Topological Sort (Kahn's)
**When:** Prerequisites, build order, dependency resolution
**Template:** In-degree map → queue with in-degree 0 → process and reduce neighbors

### 6. Dynamic Programming Framework
1. **Define state:** What does `dp[i]` represent?
2. **Recurrence:** How does `dp[i]` relate to previous states?
3. **Base case:** What are the trivial solutions?
4. **Order:** Bottom-up or top-down?
5. **Optimize:** Can we reduce space?

### 7. Binary Search Variations
**When:** Sorted array, monotonic function, search space reduction
**Key insight:** Define `condition(mid)` that is monotonic
**Template:** `lo, hi = 0, n-1; while lo < hi: mid = lo + (hi-lo)//2`

### 8. Union-Find
**When:** Connected components, dynamic connectivity, MST (Kruskal)
**Template:** Path compression + union by rank → near O(1) per operation

### 9. Monotonic Stack
**When:** Next greater/smaller element, histogram problems
**Template:** Stack stores indices, pop when current > stack top

### 10. Backtracking
**When:** All combinations/permutations, constraint satisfaction
**Template:** Choose → Explore → Unchoose (backtrack)

---

## System Design Framework (45 min)

### Step 1: Requirements (5 min)
- Functional: What must the system do?
- Non-functional: Scale, latency, availability, consistency?
- Constraints: DAU, QPS, storage estimates

### Step 2: API Design (5 min)
- REST endpoints with request/response schemas
- Consider pagination, authentication, rate limiting

### Step 3: Data Model (5 min)
- Tables/collections and relationships
- SQL vs NoSQL decision with justification
- Indexing strategy

### Step 4: High-Level Architecture (10 min)
- Draw major components: clients, LB, services, DB, cache, queue
- Data flow for read and write paths

### Step 5: Deep Dives (15 min)
- Scaling bottlenecks and solutions
- Caching strategy (read-through, write-behind)
- Consistency vs availability trade-offs
- Failure handling and monitoring

### Step 6: Wrap-Up (5 min)
- Summarize trade-offs made
- Discuss future improvements
- Monitoring, alerting, observability

---

## Behavioral Framework

### STAR Method (2 min max per story)
- **S**ituation: 1 sentence context
- **T**ask: What was YOUR goal?
- **A**ction: 3-4 specific things YOU did (use "I")
- **R**esult: Quantified outcome (numbers!)

### Amazon LP Mapping
| LP | What They Want | Signal |
|----|---------------|--------|
| Customer Obsession | Start with customer, work backward | "The user needed..." |
| Ownership | Long-term thinking, never "not my job" | "I took ownership of..." |
| Invent and Simplify | Innovation, simplification | "I proposed a simpler..." |
| Dive Deep | Know details, audit | "I investigated and found..." |
| Bias for Action | Speed matters, calculated risks | "Rather than wait, I..." |
| Deliver Results | Focus on outcomes | "We shipped on time and..." |
