# Functional Breadth: `map`, `filter`, and `reduce`


### 94.1 The `operator` Module (Integration)
As seen in Chapter 34, combining `map` with the `operator` module is often faster than lambdas.
```python
from operator import add
result = list(map(add, [1, 2, 3], [4, 5, 6])) # [5, 7, 9]
```

### 94.2 `reduce` and `accumulate`
*   **`functools.reduce`**: Collapses a sequence to a single value by applying a binary function cumulatively.
*   **`itertools.accumulate`**: Similar to reduce, but yields every intermediate result.

---

## Phase XXII: Visualization and Interface Engineering

# Chapter 95: Turtle Graphics: The Educational Engine

The `turtle` module is a built-in toolkit for turtle graphics, providing an excellent way to visualize algorithms and teach geometry.

### 95.1 The Virtual Screen and the Turtle
*   **The Turtle**: A stateful cursor that maintains a position, a heading, and a "pen" (up or down).
*   **The Screen**: A window where the turtle draws.

### 95.2 Recursive Fractals with Turtle
Because the turtle's state is easily managed, it is perfect for drawing recursive structures like the Koch Snowflake or the Sierpinski Triangle.

---
