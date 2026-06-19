# Chapter 03: Control Flow & Preprocessor

> *Making decisions, repeating actions, and bending the compiler to your will before it even runs.*

Every program you have written so far has been a straight line—executing from top to bottom, never deviating. Real software does not work this way. A trading engine must react to market signals in microseconds. A game must loop sixty times per second, branching on player input. An operating system must route interrupts to the correct handler without a single wasted cycle. This chapter equips you with the complete control-flow toolkit of C++98/03 and the preprocessor machinery that operates *before* your code ever reaches the compiler.

---

## Table of Contents

- [3.1 Conditional Branching: `if`, `else if`, `else`](#31-conditional-branching-if-else-if-else)
- [3.2 The Ternary Operator `?:`](#32-the-ternary-operator-)
- [3.3 `switch` and `case`](#33-switch-and-case)
- [3.4 `while` and `do-while` Loops](#34-while-and-do-while-loops)
- [3.5 `for` Loops](#35-for-loops)
- [3.6 Loop Control: `break` and `continue`](#36-loop-control-break-and-continue)
- [3.7 The `goto` Statement](#37-the-goto-statement)
- [3.8 Nested Control Flow and the Arrow Anti-Pattern](#38-nested-control-flow-and-the-arrow-anti-pattern)
- [3.9 Operator Precedence and Short-Circuit Evaluation](#39-operator-precedence-and-short-circuit-evaluation)
- [3.10 Bitwise Operators](#310-bitwise-operators)
- [3.11 Preprocessor Directives: `#define` and `#include`](#311-preprocessor-directives-define-and-include)
- [3.12 Conditional Compilation](#312-conditional-compilation)
- [3.13 Pragma Directives](#313-pragma-directives)
- [3.14 Inline Functions vs. Macros](#314-inline-functions-vs-macros)
- [3.15 Professional Insights: Loop Mechanics In Depth](#315-professional-insights-loop-mechanics-in-depth)
- [3.16 Professional Insights: Flow Control Keywords Reference](#316-professional-insights-flow-control-keywords-reference)
- [3.17 Forward Reference: C++11/17 Control Flow Enhancements](#317-forward-reference-c1117-control-flow-enhancements)

---

## 3.1 Conditional Branching: `if`, `else if`, `else`

The `if` statement is the most fundamental decision-making tool in programming. It evaluates a boolean condition and executes a block of code only if the condition is `true`.

```cpp
// Listing 3.1: Basic if-else chain
#include <iostream>

int main() {
    int health = 45;

    if (health > 50) {
        std::cout << "You feel fine." << std::endl;
    } else if (health > 20) {
        std::cout << "You are injured. Find a medkit." << std::endl;
    } else {
        std::cout << "Critical warning! Health is dangerously low." << std::endl;
    }

    return 0;
}
```

The conditions are evaluated from top to bottom. As soon as one condition evaluates to `true`, its block executes, and the rest of the chain is completely skipped. The `else` block is a catch-all that executes only when every preceding condition evaluated to `false`.

### 3.1.1 The Boolean Nature of Conditions

In C++, the `if` condition can be any expression convertible to `bool`. Integers are truthy if non-zero, and pointers are truthy if non-null:

```cpp
// Listing 3.2: Truthiness of non-boolean types
int x = 42;
if (x) {
    std::cout << "x is non-zero" << std::endl;  // This executes
}

int* ptr = NULL;
if (ptr) {
    std::cout << "ptr is valid" << std::endl;
} else {
    std::cout << "ptr is null" << std::endl;      // This executes
}
```

---

## 3.2 The Ternary Operator `?:`

When you need to assign a value based on a simple condition, a full `if/else` block can feel needlessly verbose. The **ternary operator** condenses the pattern into a single expression.

```cpp
// Listing 3.3: Ternary operator for conditional assignment
int player_score = 8500;
int high_score = 10000;

// Syntax: condition ? value_if_true : value_if_false;
int new_high_score = (player_score > high_score) ? player_score : high_score;
```

> [!CAUTION]
> **⚠️ The Danger Zone: Nested Ternaries**
> Just because you *can* chain ternaries does not mean you *should*.
> `std::string status = (age < 18) ? "Minor" : (age < 65) ? "Adult" : "Senior";`
> This is difficult to read. Code is read ten times more often than it is written. Use `if/else` instead.

```cpp
// Listing 3.4: Ternary operator examples
#include <iostream>

int main() {
    int x = 10, y = 5;

    int max = (x > y) ? x : y;
    std::cout << "Max: " << max << std::endl;  // 10

    std::cout << (x % 2 == 0 ? "Even" : "Odd") << std::endl;  // Even

    return 0;
}
```

---

## 3.3 `switch` and `case`

When you have a single integer or character and want to check it against many possible exact values, a `switch` statement is cleaner—and often faster—than a massive chain of `else if` statements. The compiler can optimize a `switch` into a **jump table**, yielding O(1) dispatch.

```cpp
// Listing 3.5: Basic switch statement
#include <iostream>

int main() {
    int day = 3;

    switch (day) {
        case 1:
            std::cout << "Monday" << std::endl;
            break;
        case 2:
            std::cout << "Tuesday" << std::endl;
            break;
        case 3:
            std::cout << "Wednesday" << std::endl;
            break;
        default:
            std::cout << "Unknown day" << std::endl;
            break;
    }

    return 0;
}
```

### 3.3.1 The `break` Imperative and Fall-Through

Notice the `break;` statement at the end of every case. Without it, C++ will **fall through** and execute the code for the *next* case, even if the value does not match. This historical quirk inherited from C has caused billions of dollars in software bugs.

Sometimes, you actually *want* cases to fall through—for example, stacking multiple cases together:

```cpp
// Listing 3.6: Intentional fall-through in switch
char grade = 'B';

switch (grade) {
    case 'A':
    case 'B':
    case 'C':
        std::cout << "You passed!" << std::endl;
        break;
    case 'F':
        std::cout << "Please see the professor." << std::endl;
        break;
    default:
        std::cout << "Invalid grade." << std::endl;
        break;
}
```

### 3.3.2 The `default` Label

The `default` label introduces a block that will be jumped to if the condition's value does not equal any of the `case` labels' values. It is the switch equivalent of `else`. The condition must be an expression or declaration of integer or enumeration type, or a class type with a conversion function to integer or enumeration type.

```cpp
// Listing 3.7: Switch with character input
char c = getchar();
bool confirmed;
switch (c) {
    case 'y':
        confirmed = true;
        break;
    case 'n':
        confirmed = false;
        break;
    default:
        std::cout << "invalid response!" << std::endl;
        break;
}
```

---

## 3.4 `while` and `do-while` Loops

Use a `while` loop when you want to repeat an action but do not know exactly how many times—you only know it should stop when a condition becomes `false`.

```cpp
// Listing 3.8: While loop
#include <iostream>

int main() {
    int ammo = 3;

    while (ammo > 0) {
        std::cout << "Bang!" << std::endl;
        ammo--;
    }
    std::cout << "Click. Out of ammo." << std::endl;

    return 0;
}
```

A **`do-while`** loop is identical, except the condition is checked at the *end* of the loop body, not the beginning. This guarantees the code will run **at least once**, even if the condition is `false` from the start.

```cpp
// Listing 3.9: Do-while loop for input validation
#include <iostream>

int main() {
    int choice;
    do {
        std::cout << "Press 1 to start, 0 to exit: ";
        std::cin >> choice;
    } while (choice != 0 && choice != 1);

    return 0;
}
```

### 3.4.1 `while` vs. `do-while` Semantics

The distinction is critical. The following `while` loop prints nothing because the condition is `false` at the start:

```cpp
// Listing 3.10: While loop that never executes
int i = 0;
while (i < 0) {
    std::cout << i;
    ++i;
}
// Output: (nothing)
```

The equivalent `do-while` loop prints `0` because the body executes once before the condition is checked:

```cpp
// Listing 3.11: Do-while always executes at least once
int i = 0;
do {
    std::cout << i;
    ++i;
} while (i < 0);
// Output: 0
```

> [!WARNING]
> Do not forget the semicolon at the end of `while(condition);` in the `do-while` construct. Omitting it is a compilation error.

---

## 3.5 `for` Loops

When you know exactly how many times you want to iterate, use a `for` loop. It packs initialization, condition, and increment into a single, clean line.

```cpp
// Listing 3.12: Basic for loop
for (int i = 0; i < 5; i++) {
    std::cout << "Count: " << i << std::endl;
}
// Note: The variable 'i' dies here. It only exists inside the loop!
```

> [!IMPORTANT]
> **🧠 Brain Power: How a `for` Loop Actually Executes**
> 1. `int i = 0;` runs exactly once.
> 2. `i < 5;` is checked. If true, proceed to step 3. If false, exit the loop.
> 3. The body `std::cout...` runs.
> 4. `i++` runs.
> 5. Jump back to step 2.

### 3.5.1 Advanced `for` Loop Patterns

The `for` loop header supports multiple variable declarations and multiple iteration expressions:

```cpp
// Listing 3.13: Multi-variable for loop
for (int a = 0, b = 10, c = 20; (a + b + c < 100); c--, b++, a += c) {
    std::cout << a << " " << b << " " << c << std::endl;
}
```

### 3.5.2 Variable Hiding in `for` Loops

A variable declared in the `for` initialization hides any variable of the same name in the enclosing scope:

```cpp
// Listing 3.14: Variable hiding
int i = 99;
for (int i = 0; i < 10; i++) {
    // i ranges from 0 to 9 during loop execution
}
// After the loop, i is still 99
```

To use an already-declared variable without hiding it, omit the declaration:

```cpp
// Listing 3.15: Reusing existing variable
int i = 99;
for (i = 0; i < 10; i++) {
    // i ranges from 0 to 9
}
// After the loop, i is 10
```

### 3.5.3 The `for` Loop as a `while` Loop

Every `for` loop is equivalent to a `while` loop:

```cpp
// Listing 3.16: for-while equivalence
// for (initialization; condition; increment) { body; }
// is equivalent to:
/* initialization */
while (/* condition */) {
    /* body */
    /* increment */
}
```

### 3.5.4 Infinite Loops

An empty `for` header creates an infinite loop:

```cpp
// Listing 3.17: Infinite loops
for (;;) {
    std::cout << "Never ending!" << std::endl;
}

// Equivalent:
while (true) {
    std::cout << "Never ending!" << std::endl;
}
```

### 3.5.5 Iterator-Based Loops (C++98)

Before C++11, iterating over STL containers required explicit iterator declarations:

```cpp
// Listing 3.18: Iterator-based loop over std::vector
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> names;
    names.push_back("Albert Einstein");
    names.push_back("Stephen Hawking");
    names.push_back("Michael Ellis");

    for (std::vector<std::string>::iterator it = names.begin();
         it != names.end(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}
```

---

## 3.6 Loop Control: `break` and `continue`

You can interrupt the normal flow of a loop with two powerful keywords:

- **`break`**: Instantly destroys the loop. Execution jumps to the first line *after* the loop block.
- **`continue`**: Instantly skips the rest of the current iteration and jumps back to the loop's condition check.

```cpp
// Listing 3.19: break and continue
for (int i = 1; i <= 10; i++) {
    if (i == 3) {
        continue;  // Skips printing 3
    }
    if (i == 8) {
        break;     // Destroys the loop at 8
    }
    std::cout << i << " ";
}
// Output: 1 2 4 5 6 7
```

### 3.6.1 Rewriting Loops to Avoid `break` and `continue`

Because control flow changes are sometimes difficult for humans to reason about, `break` and `continue` should be used sparingly. A more straightforward implementation is usually easier to read:

```cpp
// Listing 3.20: Equivalent loop without break
for (int i = 0; i < 4; i++) {
    std::cout << i << std::endl;
}

// Listing 3.21: Equivalent loop without continue
for (int i = 0; i < 6; i++) {
    if (i % 2 != 0) {
        std::cout << i << " is an odd number" << std::endl;
    }
}
```

---

## 3.7 The `goto` Statement

C++ still supports the `goto` statement, which jumps execution to an arbitrary label within the current function.

```cpp
// Listing 3.22: goto creating a loop (DO NOT DO THIS)
int x = 0;
loop_start:
    std::cout << x << " ";
    x++;
    if (x < 5) {
        goto loop_start;
    }
```

**Never use `goto` for control flow.** It creates "spaghetti code" that is impossible to follow and debug.

### 3.7.1 The One Acceptable Use: Error Cleanup in C-Style Code

In legacy C-style resource management (before RAII), `goto` was sometimes used to jump to a single cleanup block:

```cpp
// Listing 3.23: goto for error cleanup (C-style pattern)
#include <iostream>
#include <cstdlib>
#include <cstdio>

int main() {
    FILE* file = NULL;
    char* buffer = NULL;

    file = fopen("test.txt", "r");
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
        goto cleanup;
    }

    buffer = new char[100];
    if (!buffer) {
        std::cout << "Memory allocation failed" << std::endl;
        goto cleanup;
    }

    // Do work...

cleanup:
    if (buffer) delete[] buffer;
    if (file) fclose(file);

    return 0;
}
```

> [!TIP]
> **🔥 Godhood Tip**: In modern C++, RAII (Resource Acquisition Is Initialization) and exceptions make `goto` entirely unnecessary. Smart pointers and scope-based resource managers eliminate the need for manual cleanup blocks. See Chapter 9 for details.

---

## 3.8 Nested Control Flow and the Arrow Anti-Pattern

You can nest loops inside loops and `if` statements inside `if` statements:

```cpp
// Listing 3.24: Nested loops — diagonal pattern
for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
        if (x == y) {
            std::cout << "X";
        } else {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}
```

> [!NOTE]
> **📋 Professional Note: The Arrow Anti-Pattern**
> Be wary of deeply nested control flow. If your code looks like a giant sideways arrow `>` because of many nested `if` and `for` blocks, your code is unreadable.
>
> The solutions:
> 1. Use **early returns**. If a condition fails, `return` or `continue` immediately rather than putting the rest of the function inside a massive `if` block.
> 2. Break inner loops out into separate **functions**.

---

## 3.9 Operator Precedence and Short-Circuit Evaluation

Understanding operator precedence is not optional for professional C++ engineering. Bugs born from incorrect assumptions about evaluation order are among the hardest to diagnose.

### 3.9.1 The Precedence Hierarchy

C++ has a strict hierarchy for operator evaluation:

| Priority | Operators | Associativity |
|:---------|:----------|:-------------|
| Highest | `()`, `[]`, `->`, `::`, `++`/`--` (postfix) | Left-to-right |
| High | `++`/`--` (prefix), `!`, `~`, unary `+`/`-`, `*` (deref), `&` (addr) | Right-to-left |
| Medium-High | `*`, `/`, `%` | Left-to-right |
| Medium | `+`, `-` | Left-to-right |
| Medium-Low | `<<`, `>>` | Left-to-right |
| Low | `<`, `<=`, `>`, `>=` | Left-to-right |
| Lower | `==`, `!=` | Left-to-right |
| Lower | `&` (bitwise AND), then `^`, then `|` | Left-to-right |
| Near-Bottom | `&&`, then `||` | Left-to-right |
| Bottom | `=`, `+=`, `-=`, etc. | Right-to-left |

### 3.9.2 Short-Circuit Evaluation

C++ uses **short-circuit evaluation** in `&&` and `||` to avoid unnecessary work:

- `&&`: If the left operand is `false`, the right operand is *never evaluated*.
- `||`: If the left operand is `true`, the right operand is *never evaluated*.

`&&` has higher precedence than `||`, which determines how parentheses are implicitly placed:

```cpp
// Listing 3.25: Short-circuit evaluation and precedence
#include <iostream>
#include <string>

bool True(std::string id) {
    std::cout << "True" << id << std::endl;
    return true;
}

bool False(std::string id) {
    std::cout << "False" << id << std::endl;
    return false;
}

int main() {
    bool result;

    // Equivalent to: False("A") || (False("B") && False("C"))
    result = False("A") || False("B") && False("C");
    // Output: FalseA, FalseB
    // C is skipped: B is false, so (B && C) is false without evaluating C.

    // Equivalent to: True("A") || (False("B") && False("C"))
    result = True("A") || False("B") && False("C");
    // Output: TrueA
    // B and C are both skipped: A is true, so the entire || is true.

    return 0;
}
```

### 3.9.3 Unary Operators and Postfix Semantics

**Unary operators** act on a single operand and have high precedence. When used in **postfix** form, the side-effect occurs only *after* the entire expression is evaluated:

```cpp
// Listing 3.26: Prefix vs. postfix increment
int a = 1;
++a;            // a is now 2
a--;            // a is now 1
int minusa = -a;  // minusa is -1

a = 4;
int c = a++ / 2;  // c = 4/2 = 2, then a becomes 5
int d = ++a / 2;  // a becomes 6, then d = 6/2 = 3
```

> [!WARNING]
> **Undefined Behavior Warning**: Never write expressions like `a = b++ + ++b;`. Modifying a variable twice between sequence points is **Undefined Behavior**.

### 3.9.4 Arithmetic Precedence

Arithmetic operators follow mathematical precedence: multiplication and division before addition and subtraction, all with left-to-right associativity.

```cpp
// Listing 3.27: Arithmetic precedence
int a = 2 + 4 / 2;       // 2 + (4/2) = 4
int b = (3 + 3) / 2;     // (3+3)/2 = 3
int c = 3 + 4 / 2 * 6;   // 3 + ((4/2)*6) = 15
int d = 3 * (3 + 6) / 9; // (3*(3+6))/9 = 3
int g = 3 - 3 % 1;       // 3 - (3%1) = 3 - 0 = 3
```

### 3.9.5 Logical AND/OR Precedence

AND (`&&`) binds tighter than OR (`||`):

```cpp
// Listing 3.28: AND before OR
// You can drive with a foreign license for up to 60 days
bool can_drive = has_domestic_license || has_foreign_license && num_days <= 60;

// Equivalent to:
bool can_drive = has_domestic_license || (has_foreign_license && num_days <= 60);
```

Adding explicit parentheses does not change behavior but dramatically improves readability. In professional code, always parenthesize mixed `&&`/`||` expressions.

---

## 3.10 Bitwise Operators

Bitwise operators manipulate individual bits of integer types. They are essential for embedded systems, graphics programming, cryptography, and network protocol implementation.

### 3.10.1 `|` — Bitwise OR

Each bit in the result is `1` if *either* corresponding bit is `1`:

```cpp
// Listing 3.29: Bitwise OR
int a = 5;      // 0101b
int b = 12;     // 1100b
int c = a | b;  // 1101b = 13
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
// Output: a = 5, b = 12, c = 13
```

The compound assignment form is `|=`:

```cpp
int a = 5;   // 0101b
a |= 12;     // a = 0101b | 1100b = 1101b = 13
```

### 3.10.2 `^` — Bitwise XOR (Exclusive OR)

Each bit in the result is `1` if the corresponding bits are *different*:

```cpp
// Listing 3.30: Bitwise XOR
int a = 5;      // 0101b
int b = 9;      // 1001b
int c = a ^ b;  // 1100b = 12
```

**XOR Swap** — a classic trick that swaps two variables without a temporary:

```cpp
// Listing 3.31: XOR swap (demonstration only)
void doXORSwap(int& a, int& b) {
    if (&a != &b) {  // CRITICAL: swapping a variable with itself zeros it!
        a ^= b;
        b ^= a;
        a ^= b;
    }
}
```

> [!NOTE]
> The XOR swap is a historical curiosity. On modern CPUs, it is *slower* than using a temporary variable due to pipeline stalls and data dependencies. Use `std::swap()` instead.

### 3.10.3 `&` — Bitwise AND

Each bit in the result is `1` only if *both* corresponding bits are `1`:

```cpp
// Listing 3.32: Bitwise AND
int a = 6;      // 0110b
int b = 10;     // 1010b
int c = a & b;  // 0010b = 2
```

### 3.10.4 `<<` — Left Shift

Shifts all bits left by N positions, padding the least significant bits with zeros. Equivalent to multiplying by 2^N:

```cpp
// Listing 3.33: Left shift
int a = 1;       // 0001b
int b = a << 1;  // 0010b = 2
int c = a << 4;  // 00010000b = 16
```

> [!WARNING]
> Left-shifting a signed number so that the sign bit is affected is **Undefined Behavior**. Shifting by a negative amount or by more bits than the type can hold is also UB.

### 3.10.5 `>>` — Right Shift

Shifts all bits right by N positions. For unsigned types, zeros are shifted in. For signed negative numbers, the behavior is **implementation-defined**:

```cpp
// Listing 3.34: Right shift
int a = 8;       // 1000b
int b = a >> 1;  // 0100b = 4
int c = a >> 2;  // 0010b = 2
```

```cpp
// Listing 3.35: Right shift of negative numbers — implementation-defined
int a = -2;
int b = a >> 1;  // Result depends on the compiler!
```

---

## 3.11 Preprocessor Directives: `#define` and `#include`

The **preprocessor** operates on your source code *before* the compiler sees it. It performs textual substitution, file inclusion, and conditional compilation. Every line beginning with `#` is a preprocessor directive.

### 3.11.1 `#define` — Object-Like and Function-Like Macros

```cpp
// Listing 3.36: Basic preprocessor macros
#define PI 3.14159
#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))

// Conditional debug logging
#define DEBUG

#ifdef DEBUG
    #define LOG(msg) std::cout << msg << std::endl
#else
    #define LOG(msg)  // Expands to nothing in release builds
#endif

#include <iostream>

int main() {
    std::cout << "PI = " << PI << std::endl;

    int arr[MAX_SIZE];
    std::cout << "Array size: " << sizeof(arr) << std::endl;

    std::cout << "Square of 5: " << SQUARE(5) << std::endl;

    LOG("Debug message");

    return 0;
}
```

> [!WARNING]
> **Macro Pitfall**: Macros perform textual substitution. Always parenthesize macro parameters: `#define SQUARE(x) ((x) * (x))`. Without the extra parentheses, `SQUARE(1+2)` would expand to `(1+2 * 1+2)` = 5, not 9.

---

## 3.12 Conditional Compilation

Conditional compilation directives allow you to include or exclude blocks of code at compile time. This is essential for platform-specific code, feature flags, and debug/release builds.

```cpp
// Listing 3.37: Platform-specific conditional compilation
#include <iostream>

#ifdef _WIN32
    #define OS "Windows"
#elif __APPLE__
    #define OS "macOS"
#elif __linux__
    #define OS "Linux"
#else
    #define OS "Unknown"
#endif

int main() {
    std::cout << "Running on: " << OS << std::endl;

#if defined(DEBUG)
    std::cout << "Debug mode" << std::endl;
#else
    std::cout << "Release mode" << std::endl;
#endif

    return 0;
}
```

### 3.12.1 Include Guards

Every header file must use include guards to prevent multiple inclusion:

```cpp
// Listing 3.38: Include guard pattern
#ifndef MY_HEADER_H
#define MY_HEADER_H

// Header contents here

#endif // MY_HEADER_H
```

---

## 3.13 Pragma Directives

**`#pragma`** directives provide compiler-specific instructions. They are not standardized (except `#pragma once` in practice), so portability must be considered.

```cpp
// Listing 3.39: Pragma directives
#include <iostream>

// Disable specific warnings (MSVC)
#pragma warning(disable: 4996)

// Pack structure — remove padding
#pragma pack(1)

struct PackedData {
    char a;      // 1 byte
    int b;       // 4 bytes
    double c;    // 8 bytes
};

#pragma pack()   // Restore default packing

int main() {
    std::cout << "Size of PackedData: " << sizeof(PackedData) << std::endl;
    // Without pragma pack: likely 24 (with alignment padding)
    // With pragma pack(1): 13 (tightly packed)

    return 0;
}
```

> [!TIP]
> **🔥 Godhood Tip**: `#pragma pack` is critical when your struct must match an external binary protocol (network packets, hardware registers, file formats). But packed structs may cause performance penalties on architectures requiring aligned access.

---

## 3.14 Inline Functions vs. Macros

Macros and inline functions both aim to eliminate function-call overhead, but they are fundamentally different mechanisms.

```cpp
// Listing 3.40: Macro vs. inline function
#include <iostream>

// Macro function — preprocessor substitution (no type safety)
#define ADD_MACRO(a, b) ((a) + (b))

// Inline function — type-safe, debuggable
inline int add_inline(int a, int b) {
    return a + b;
}

int main() {
    std::cout << ADD_MACRO(5, 3) << std::endl;    // 8
    std::cout << add_inline(5, 3) << std::endl;   // 8

    // Macro danger: side effects with increment operators
    int x = 5, y = 3;
    std::cout << ADD_MACRO(x++, y++) << std::endl;  // Evaluates ((x++) + (y++))
    std::cout << "x = " << x << ", y = " << y << std::endl;  // x = 6, y = 4

    // Inline function is safer — arguments evaluated exactly once
    x = 5; y = 3;
    std::cout << add_inline(x++, y++) << std::endl;  // 8
    std::cout << "x = " << x << ", y = " << y << std::endl;  // x = 6, y = 4

    return 0;
}
```

| Feature | Macro | Inline Function |
|:--------|:------|:---------------|
| Type Safety | None | Full |
| Debugging | Invisible to debugger | Normal step-through |
| Side Effects | Double evaluation risk | Arguments evaluated once |
| Scope | Global (no namespace) | Respects namespaces |
| Recommendation | Avoid for functions | Preferred |

### 3.14.1 The `do { ... } while(0)` Macro Idiom

Multi-statement macros must be wrapped in `do { ... } while(0)` to behave correctly in all contexts:

```cpp
// Listing 3.41: Safe multi-statement macro
#define BAD_MACRO(x)  f1(x); f2(x); f3(x);

// Dangerous:
if (cond) BAD_MACRO(var);  // Only f1 is inside the if!

#define GOOD_MACRO(x)  do { f1(x); f2(x); f3(x); } while(0)

// Safe:
if (cond) GOOD_MACRO(var);  // All three calls are inside the if
```

---

## 3.15 Professional Insights: Loop Mechanics In Depth

### 3.15.1 Variable Declaration in Loop Conditions

In `for` and `while` loops, you may declare a variable in the condition. The variable is in scope until the loop ends and persists through each iteration:

```cpp
// Listing 3.42: Variable declaration in conditions
for (int i = 0; i < 5; ++i) {
    // i is in scope here
}
// i is no longer in scope

// while loop with declaration (C++98 — pointer check)
while (Node* p = get_next_node()) {
    p->process();
}
// p is no longer in scope
```

This is *not* permitted in `do-while` loops, because the body executes before the condition is reached.

### 3.15.2 The `for` Loop Execution Model

The `for` loop has three optional clauses:

1. **Initialization**: Executed exactly once. May declare multiple variables of one type.
2. **Condition**: Evaluated before each iteration. If false, the loop terminates.
3. **Increment**: Executed after each iteration, before the next condition check.

Any or all clauses may be omitted. Omitting all three creates an infinite loop: `for (;;)`.

### 3.15.3 Breaking Out of Nested Loops

`break` only exits the *innermost* enclosing loop. To break out of multiple nested loops, use one of these patterns:

1. Extract the nested loops into a function and use `return`.
2. Use a flag variable checked by the outer loop.
3. In extreme low-level code, `goto` to a label after the outer loop (the one legitimate use).

---

## 3.16 Professional Insights: Flow Control Keywords Reference

### 3.16.1 `try`, `catch`, and `throw`

Exception handling keywords provide structured error propagation. They are covered in depth in Chapter 9, but the mechanics are summarized here:

```cpp
// Listing 3.43: Exception handling basics
#include <iostream>
#include <vector>
#include <stdexcept>

void print_asterisks(int count) {
    if (count < 0) {
        throw std::invalid_argument("count cannot be negative!");
    }
    while (count--) { putchar('*'); }
}

int main() {
    try {
        std::vector<int> v(1000000000);  // May throw std::bad_alloc
    } catch (const std::bad_alloc&) {
        std::cout << "failed to allocate memory!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "runtime error: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception!" << std::endl;
        throw;  // Rethrow to caller
    }

    return 0;
}
```

### 3.16.2 `throw` in Expressions

A `throw` expression has type `void` and can be nested in ternary expressions:

```cpp
// Listing 3.44: throw in ternary expression
unsigned int predecessor(unsigned int x) {
    return (x > 0) ? (x - 1) : (throw std::invalid_argument("0 has no predecessor"));
}
```

---

## 3.17 Forward Reference: C++11/17 Control Flow Enhancements

> [!NOTE]
> The following features are **not available in C++98/03** but are included as forward references for completeness. They will be covered in full in the C++11 and C++17 volumes.

### 3.17.1 Range-Based `for` Loops [C++11]

C++11 introduced the **range-based `for` loop**, which drastically simplifies iteration over containers:

```cpp
// Listing 3.45: Range-based for loop [C++11]
#include <vector>
#include <iostream>

int main() {
    std::vector<double> prices;
    prices.push_back(19.99);
    prices.push_back(5.50);
    prices.push_back(42.00);

    for (double price : prices) {
        std::cout << "Price: $" << price << std::endl;
    }

    return 0;
}
```

The range-based `for` works with:
- C-style arrays (fixed size, not dynamically allocated)
- Any type with `begin()` and `end()` member functions
- Any type with non-member `begin()`/`end()` found via ADL

### 3.17.2 `if` and `switch` with Initializers [C++17]

C++17 allows initialization statements directly inside `if` and `switch` conditions:

```cpp
// Listing 3.46: if-initializer [C++17]
if (int status = connect_to_server(); status == 200) {
    std::cout << "Success!" << std::endl;
} else {
    std::cout << "Failed with code: " << status << std::endl;
}
// 'status' is out of scope here — clean!
```

### 3.17.3 `[[fallthrough]]` Attribute [C++17]

```cpp
// Listing 3.47: [[fallthrough]] attribute [C++17]
switch (grade) {
    case 'D':
        std::cout << "Barely passed." << std::endl;
        [[fallthrough]];  // Tells compiler: intentional fall-through
    case 'F':
        std::cout << "Please see the professor." << std::endl;
        break;
}
```

---

*You now command the full control-flow arsenal of C++98/03: branching, looping, the preprocessor, and operator precedence. In the next chapter, we move beyond basic function calls into the advanced mechanics of function pointers, callbacks, and the machinery that makes C++ a language of composable abstractions.*
