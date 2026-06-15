# CHAPTER 33: C23 STD PRINT


# C++23: THE END OF IOSTREAM AND PRINTF

### 1. `std::print` and `std::println`
C++23 finally fixes standard output. `std::cout` is slow and verbose, while `printf` is not type-safe. `std::print` bridges the gap using the C++20 `std::format` engine.

*   **Type-safe and Fast**: It writes directly to the underlying OS file descriptor without creating an intermediate `std::string` allocation.
    ```cpp
    #include <print>
    
    std::println("User {} has {} points.", user.name, user.score);
    std::println(stderr, "Error: connection failed");
    ```

### 2. Formatting Ranges
`std::print` natively understands standard ranges and containers.
```cpp
std::vector<int> v = {1, 2, 3};
std::println("{}", v); // Output: [1, 2, 3]
```
