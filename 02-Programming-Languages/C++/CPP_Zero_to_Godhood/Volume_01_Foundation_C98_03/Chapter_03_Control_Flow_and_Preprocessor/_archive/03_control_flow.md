# Chapter 3: Control Flow

> *Making decisions and repeating actions.*

Code that runs straight from top to bottom is boring. It does the exact same thing every single time. To make software truly useful—to make it react to user input, process data files, or run game loops—your code needs to make decisions. It needs to branch. It needs to repeat.

This is **Control Flow**. 

---

## 3.1 `if`, `else if`, `else`

The `if` statement is the most fundamental decision-making tool in programming. It evaluates a boolean condition (is it `true` or `false`?) and executes a block of code only if the condition is `true`.

```cpp
int health = 45;

if (health > 50) {
    std::cout << "You feel fine.\n";
} else if (health > 20) {
    std::cout << "You are injured. Find a medkit.\n";
} else {
    std::cout << "Critical warning! Health is dangerously low.\n";
}
```

The conditions are evaluated from top to bottom. As soon as one condition evaluates to `true`, its block executes, and the rest of the chain is completely skipped. 

## 3.2 The Ternary Operator `?:`

Often, you just want to assign a value to a variable based on a simple condition. Writing a full `if/else` block for this can feel needlessly verbose. 

Enter the **Ternary Operator**. It takes three arguments: a condition, a result if true, and a result if false.

```cpp
int player_score = 8500;
int high_score = 10000;

// Syntax: condition ? value_if_true : value_if_false;
int new_high_score = (player_score > high_score) ? player_score : high_score;
```

> [!CAUTION]
> **⚠️ The Danger Zone: Nested Ternaries**
> Just because you *can* chain ternaries together doesn't mean you *should*. 
> `std::string status = (age < 18) ? "Minor" : (age < 65) ? "Adult" : "Senior";`
> This is difficult to read. Code is read ten times more often than it is written. Use `if/else` instead.

## 3.3 `switch` and `case`

When you have a single integer or character, and you want to check it against many possible exact values, a `switch` statement is cleaner (and often faster) than a massive chain of `else if` statements.

```cpp
int day = 3;

switch (day) {
    case 1:
        std::cout << "Monday\n";
        break;
    case 2:
        std::cout << "Tuesday\n";
        break;
    case 3:
        std::cout << "Wednesday\n";
        break;
    default:
        std::cout << "Unknown day\n";
        break;
}
```

### The `[[fallthrough]]` Attribute `[C++17]`
Notice the `break;` statement at the end of every case? If you forget to include it, C++ will "fall through" and execute the code for the *next* case as well, even if the value doesn't match! This historical quirk of C has caused billions of dollars in software bugs.

Sometimes, you actually *want* cases to fall through (e.g., stacking multiple cases together). In C++17, you should explicitly tell the compiler this is intentional to silence warnings:

```cpp
char grade = 'B';

switch (grade) {
    case 'A':
    case 'B':
    case 'C':
        std::cout << "You passed!\n";
        break;
    case 'D':
        std::cout << "Barely passed.\n";
        [[fallthrough]]; // Tells compiler: "I know what I'm doing"
    case 'F':
        std::cout << "Please see the professor.\n";
        break;
}
```

## 3.4 `while` and `do-while` Loops

Use a `while` loop when you want to repeat an action, but you don't know exactly how many times it will happen. You just know it should stop when a condition becomes `false`.

```cpp
int ammo = 3;

while (ammo > 0) {
    std::cout << "Bang!\n";
    ammo--;
}
std::cout << "Click. Out of ammo.\n";
```

A **`do-while`** loop is identical, except the condition is checked at the *end* of the loop, not the beginning. This guarantees the code will run **at least once**, even if the condition is `false` from the start.

```cpp
int choice;
do {
    std::cout << "Press 1 to start, 0 to exit: ";
    std::cin >> choice;
} while (choice != 0 && choice != 1);
```

## 3.5 `for` Loops

When you know exactly how many times you want to iterate (e.g., "count to 10"), you use a `for` loop. It packs the initialization, the condition, and the increment into a single, clean line.

```cpp
// for (initialization; condition; increment)
for (int i = 0; i < 5; i++) {
    std::cout << "Count: " << i << "\n";
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

## 3.6 Range-Based `for` Loops `[C++11]`

If you want to look at every item in a collection (like a list or an array), the traditional `for` loop is unnecessarily verbose. C++11 introduced the **Range-Based `for` loop**, which drastically simplifies iteration.

```cpp
#include <vector>

std::vector<double> prices = {19.99, 5.50, 42.00};

// Read as: "For each 'price' in 'prices'"
for (double price : prices) {
    std::cout << "Price: $" << price << "\n";
}
```

When combined with `auto`, it becomes beautifully generic:

```cpp
for (auto price : prices) {
    // The compiler figures out 'price' should be a double
}
```

## 3.7 `break`, `continue`, and Labels

You can interrupt the normal flow of a loop using two powerful keywords:
*   **`break`**: Instantly destroys the loop. Execution jumps to the first line *after* the loop block.
*   **`continue`**: Instantly skips the rest of the current iteration and jumps back up to the loop's condition check to start the next iteration.

```cpp
for (int i = 1; i <= 10; i++) {
    if (i == 3) {
        continue; // Skips printing 3. Jumps to i++
    }
    if (i == 8) {
        break;    // Destroys the loop completely. 8, 9, 10 are never evaluated.
    }
    std::cout << i << " ";
}
// Output: 1 2 4 5 6 7
```

### The `goto` Statement (Avoid!)
C++ still supports the ancient `goto` statement, which jumps execution to an arbitrary label.
```cpp
loop_start:
    // do something
    goto loop_start; // Creates an infinite loop
```
**Never use `goto`**. It creates "spaghetti code" that is impossible to follow. The *only* professionally acceptable use for `goto` is in deep C-style systems programming to jump to an error cleanup block, but in modern C++, we use RAII and Exceptions (Chapter 12) to handle this safely instead.

## 3.8 `if` and `switch` with Initializers `[C++17]`

Often, you call a function that returns a value, check that value, and then never use the value again.

```cpp
// The old way
int status = connect_to_server();
if (status == 200) {
    std::cout << "Success!";
}
// 'status' is still alive down here, polluting your variable scope.
```

C++17 allows you to put an initialization statement directly inside the `if` or `switch` condition! 

```cpp
// The C++17 way
if (int status = connect_to_server(); status == 200) {
    std::cout << "Success!";
} else {
    std::cout << "Failed with code: " << status;
}
// 'status' is officially DEAD here. Clean scope!
```
This is a phenomenal feature for keeping your variable scope as tight as possible.

## 3.9 Nested Control Flow and Code Readability

You can put loops inside of loops, and `if` statements inside of `if` statements.

```cpp
for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
        if (x == y) {
            std::cout << "X";
        } else {
            std::cout << ".";
        }
    }
    std::cout << "\n";
}
```

> [!NOTE]
> **📋 Professional Notes: The Arrow Anti-Pattern**
> Be highly wary of deeply nested control flow. If your code looks like a giant sideways arrow `>` because of so many nested `if` and `for` blocks, your code is unreadable. 
> 
> The solution? 
> 1. Use **early returns**. If a condition fails, `return` or `continue` immediately rather than putting the entire rest of the function inside a massive `if` block.
> 2. Break the inner loops out into separate **Functions**. 

Which leads us perfectly to the next chapter.
