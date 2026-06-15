# CHAPTER 26: C20 CONCEPTS


# C++20 CONCEPTS & CONSTRAINTS

Concepts are the first of the "Four Great Pillars" of C++20. They revolutionize template programming by providing a formal way to specify requirements on template arguments.

### 1. The Core Mechanics
*   **Concepts**: Compile-time constraints on template parameters, replacing cryptic SFINAE errors with clear diagnostics.
    ```cpp
    template<typename T>
    concept Addable = requires(T a, T b) { a + b; };
    ```
*   **Requires expressions**: Inline constraint blocks that test the validity of expressions, types, or compound requirements.
    ```cpp
    template<typename T>
    concept Advanced = requires(T x) {
        x++;                        // Simple requirement
        typename T::value_type;      // Type requirement
        {*x} -> std::same_as<int>;   // Compound requirement
    };
    ```
*   **Requires clauses**: Attaches a constraint to a template or function declaration using the `requires` keyword.
    ```cpp
    template<typename T>
    requires Addable<T>
    void f(T a) { /* ... */ }
    ```
*   **Constrained auto**: `auto` parameters in functions and variables can be constrained with a concept.
    ```cpp
    void f(std::integral auto x) { /* x must be an integer type */ }
    ```
*   **Partial ordering by constraints**: Among multiple viable overloads, the most-constrained one is selected automatically.
    ```cpp
    void f(std::integral auto); 
    void f(std::signed_integral auto); 
    f(1); // picks signed_integral (more specific)
    ```

### 2. Standard Concepts
The `<concepts>` header provides a massive library of pre-defined constraints:
*   **Core**: `std::derived_from`, `std::convertible_to`, `std::same_as`, `std::integral`, `std::floating_point`.
*   **Object**: `std::movable`, `std::copyable`, `std::semiregular`, `std::regular`.
*   **Callable**: `std::invocable`, `std::predicate`.
