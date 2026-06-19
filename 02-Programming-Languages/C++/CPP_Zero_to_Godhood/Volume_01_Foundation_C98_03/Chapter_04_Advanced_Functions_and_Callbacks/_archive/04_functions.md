# Chapter 4: Functions

> *The engines of your application.*

Imagine you are writing a game, and every time the player takes damage, you need to calculate armor reduction, deduct health, play a sound effect, and update the screen. If you wrote those 20 lines of code every single time an enemy hit the player, your program would become a bloated, unmaintainable nightmare.

**Functions** are how we solve this. A function is a named block of code that performs a specific task. You write it once, and you can "call" it as many times as you want. This is the core of the **DRY Principle: Don't Repeat Yourself**.

---

## 4.1 Anatomy of a Function

To create a function, you must define four things:
1.  **Return Type**: What kind of data does this function give back when it finishes? If it gives nothing back, we use `void`.
2.  **Name**: What is this action called? (e.g., `calculateDamage`).
3.  **Parameters**: What data does this function need to do its job? (e.g., `int damage_amount`).
4.  **Body**: The actual code to execute, wrapped in `{ }`.

```cpp
// return_type  name (parameters)
int add(int a, int b) {
    int result = a + b; // The body
    return result;      // The return statement
}
```

Once defined, you can **call** the function from `main()` (or from other functions):

```cpp
int main() {
    int sum = add(5, 10); // sum becomes 15
    return 0;
}
```

## 4.2 Passing Arguments: Value vs. Reference

This is one of the most critical concepts in C++. When you pass a variable into a function, *how* does it get there?

### 1. Pass by Value (The Copy)
By default, C++ copies the value into the function. 

```cpp
void tryToChange(int x) {
    x = 99; // Only changes the local copy
}

int main() {
    int score = 10;
    tryToChange(score);
    // score is STILL 10! The function only modified a photocopy.
}
```
**Pros**: Safe. The function can't accidentally destroy your original data.
**Cons**: Slow. If you pass a massive 3D model, the CPU has to copy millions of bytes of data just to hand it to the function.

### 2. Pass by Reference (`&`) (The Original)
If you add an ampersand `&` to the parameter type, you pass a **reference**. You are telling the function: *"Don't make a copy. Go look at the exact memory room where the original variable lives."*

```cpp
void actuallyChange(int& x) {
    x = 99; // Modifies the original variable directly!
}

int main() {
    int score = 10;
    actuallyChange(score);
    // score is now 99!
}
```
**Pros**: Lightning fast (no copying) and allows the function to modify the original.
**Cons**: Dangerous if you didn't *want* the function to modify the original.

### 3. Pass by `const` Reference (The Holy Grail)
What if you want the extreme speed of passing by reference, but you want to guarantee the function won't accidentally corrupt your original data? You make the reference `const`.

```cpp
// Fast because it's a reference. Safe because it's const.
void printScore(const std::string& player_name) {
    std::cout << player_name;
    // player_name = "Hacker"; // ERROR! The compiler will stop this.
}
```

> [!TIP]
> **🔥 Godhood Tip: When to use which?**
> *   **Fundamental types** (`int`, `double`, `bool`): Pass by **Value**. They are so small that copying them is actually faster than creating a reference pointer under the hood.
> *   **Large objects** (`std::string`, `std::vector`, Classes): Pass by **`const` Reference**.
> *   **When you need to modify the original**: Pass by **Reference**.

## 4.3 Default Parameters

You can provide default values for parameters. If the caller doesn't provide them, the compiler fills them in automatically.

```cpp
void greet(std::string name = "Traveler", int level = 1) {
    std::cout << "Hello, " << name << " (Lvl " << level << ")\n";
}

int main() {
    greet("Aloy", 50); // Prints: Hello, Aloy (Lvl 50)
    greet("Link");     // Prints: Hello, Link (Lvl 1)
    greet();           // Prints: Hello, Traveler (Lvl 1)
}
```
**Rule:** Default parameters must always be at the *end* of the parameter list.

## 4.4 Function Overloading

In C, if you wanted a function to print an `int` and another to print a `double`, you had to name them `print_int()` and `print_double()`. 

C++ supports **Function Overloading**. You can have multiple functions with the *exact same name*, as long as their parameter lists are different. The compiler is smart enough to figure out which one you meant based on the arguments you pass.

```cpp
void print(int x) {
    std::cout << "Printing an integer: " << x << "\n";
}

void print(double x) {
    std::cout << "Printing a double: " << x << "\n";
}

int main() {
    print(42);    // Calls the int version
    print(3.14);  // Calls the double version
}
```

## 4.5 The `inline` Keyword

Every time you call a function, there is a tiny performance penalty. The CPU has to save its current state, jump to the function's memory address, execute it, and jump back.

For incredibly tiny functions (like a math helper), this jumping around takes more time than the math itself! You can suggest to the compiler to `inline` the function. This tells the compiler to literally copy-paste the function's code directly into wherever it is called, eliminating the jump entirely.

```cpp
inline int square(int x) {
    return x * x;
}

int main() {
    int y = square(5); 
    // The compiler quietly changes the above line to: int y = 5 * 5;
}
```
*Note: `inline` is just a suggestion. Modern compilers (GCC, Clang) are so smart they often ignore your `inline` keyword and make their own decisions based on complex heuristics.*

## 4.6 Recursion

A recursive function is a function that calls itself. It is heavily used in advanced algorithms (like traversing trees or sorting).

Every recursive function MUST have a **Base Case** (a condition that stops the recursion). If it doesn't, it will call itself infinitely.

```cpp
int factorial(int n) {
    if (n <= 1) {
        return 1;  // Base Case: Stop calling yourself!
    }
    return n * factorial(n - 1);  // Recursive Case
}
```

## 4.7 🛋️ Fireside Chat: How the Stack Actually Works

To understand recursion, and to understand why programs crash, you must understand **The Call Stack**.

Think of the Stack as a physical stack of cafeteria trays. 
When your program starts, the OS places a tray representing `main()` on the table.
Inside `main()`, you call `add()`. The OS places a new tray for `add()` on top of `main()`.
If `add()` calls `multiply()`, a `multiply()` tray goes on top of `add()`.

**The Rule of the Stack:** The CPU can only look at, and work on, the tray at the very top. 

When `multiply()` finishes, its tray is popped off the stack and destroyed, revealing the `add()` tray beneath it. The CPU resumes working on `add()`.

Every time a tray is created, it allocates memory for the function's local variables. 

> [!CAUTION]
> **⚠️ Stack Overflow**
> What happens if a recursive function forgets its Base Case?
> It calls itself. A tray is added. It calls itself. Another tray. It calls itself 100,000 times. The stack of trays hits the ceiling of the cafeteria.
> 
> The Operating System panics and instantly kills your program to prevent it from consuming all the computer's RAM. 
> 
> This is called a **Stack Overflow**. It is the most famous error in computer science.

---

You now possess the foundational tools of C++: Variables, Control Flow, and Functions. In Part II, we will peel back the abstraction and look at the actual metal beneath the code: Pointers, Memory, and the true power of C++.
