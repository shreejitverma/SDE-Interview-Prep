# Appendix T: THE MASTER'S GUIDE TO CMAKE

C++ does not have a standard package manager or build system. CMake won the build system war. If you do not understand CMake, you do not understand C++.

### T.1 The Golden Rule of Modern CMake

**Never use `include_directories()`, `link_libraries()`, or `add_compile_options()`.**
These are global commands. They pollute the entire project. Modern CMake is strictly **Target-Based**.

### T.2 Building a Target

Everything is a target. A target is a node in a dependency graph.


```cmake
# Minimum required version (prevents legacy CMake behavior)

cmake_minimum_required(VERSION 3.20)
project(GodhoodEngine VERSION 1.0 LANGUAGES CXX)

# 1. Create a Library Target

add_library(MathCore src/math.cpp src/trig.cpp)

# 2. Assign Properties to the Target

target_compile_features(MathCore PUBLIC cxx_std_20)

# PUBLIC: MathCore needs 'include/' to compile, and anyone linking 
# to MathCore also needs 'include/' to find its headers.

target_include_directories(MathCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# PRIVATE: MathCore needs extra warnings, but consumers of MathCore don't care.

target_compile_options(MathCore PRIVATE -Wall -Wextra -Werror)

# 3. Create an Executable Target

add_executable(GameEngine src/main.cpp)

# 4. Link them together

target_link_libraries(GameEngine PRIVATE MathCore)
```
When `GameEngine` links to `MathCore`, CMake automatically passes the `include/` directory and the `cxx_std_20` requirement to `GameEngine`. You don't configure the executable; you configure the library, and the properties flow down the graph automatically!

### T.3 Generator Expressions (The Black Magic)

Sometimes you only want a compile flag if you are in Debug mode, or if you are on a specific compiler. `if/else` statements in CMake are evaluated during the *Configure* step. Generator Expressions (`$<...>`) are evaluated during the *Generate* step, allowing per-target logic.

```cmake
# Add -O3 only if it's a Release build

target_compile_options(MathCore PRIVATE $<$<CONFIG:Release>:-O3>)

# Link against a specific library only if on Windows

target_link_libraries(MathCore PRIVATE $<$<PLATFORM_ID:Windows>:ws2_32>)
```
