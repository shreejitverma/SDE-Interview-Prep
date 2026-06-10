# Enums and Topological Sorts (`enum`, `graphlib`)


This chapter explores advanced data categorization and dependency resolution modules that leverage Python's metaclassing and algorithmic strengths.

### 47.1 `enum`: Metaprogramming Constant Mappings

As introduced in Chapter 10, `enum` is more than a simple constant list.

#### 1. `IntEnum` and `IntFlag`
*   **`IntEnum`**: Subclasses both `int` and `Enum`. It allows comparisons with raw integers (`Color.RED == 1`).
*   **`IntFlag`**: Supports bitwise operations (`|`, `&`, `^`, `~`). It is useful for representing hardware registers or permission bitmasks.
*   **Internals**: `IntFlag` members are combined using a specialized version of the `EnumMeta` metaclass that ensures bitwise results are still valid members of the Flag class.

#### 2. The `auto()` Helper
`auto()` is a sentinel object. During class construction, the metaclass detects `auto()` and assigns an appropriate value (usually an incrementing integer).

### 47.2 `graphlib`: Dependency Resolution

Python 3.9 introduced `graphlib` to provide a standard way to perform topological sorting of graphs.

#### 1. Topological Sorting
A topological sort is a linear ordering of vertices such that for every directed edge $(u, v)$, $u$ comes before $v$. This is the foundation of build systems (e.g., `make`, `ninja`) and task schedulers.

#### 2. `TopologicalSorter` Internals
The `TopologicalSorter` class implements a **Kahn's Algorithm** variant.
1.  It calculates the "in-degree" (number of incoming edges) for every node.
2.  It maintains a queue of nodes with in-degree 0 (those with no dependencies).
3.  As nodes are "prepared" and "done", it decrements the in-degree of their neighbors, adding new 0-degree nodes to the queue.
4.  **Cycle Detection**: If the graph is exhausted but nodes remain with in-degree > 0, it raises a `CycleError`.

#### 3. Parallel Execution
`graphlib` is designed for parallel runners. You can call `get_ready()` to get all nodes that can be executed immediately, work on them in separate threads/processes, and call `done()` as they finish to unlock the next tier of dependencies.

---


