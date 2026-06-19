# Chapter 36: Advanced Debugging and Tooling

> *Finding the needle in the megabyte haystack.*

Even if you follow the C++ Core Guidelines perfectly, bugs will happen. Memory will be corrupted, threads will deadlock, and variables will mysteriously change values. 

When `std::cout << "got here"` stops working, you must rely on professional debugging tools.

---

## 36.1 GDB and LLDB

The GNU Debugger (`gdb`) and the LLVM Debugger (`lldb`) are command-line tools that allow you to pause execution, inspect memory, and step through assembly code.

To use them, you must compile your code with the `-g` flag, which tells the compiler to embed debug symbols (mapping memory addresses back to your C++ variable names and line numbers).

```bash
g++ -g main.cpp -o my_app
gdb ./my_app
```

### Basic Commands
*   `run` (`r`): Start the program.
*   `break main.cpp:42` (`b`): Pause execution at line 42.
*   `next` (`n`): Execute the current line and step over functions.
*   `step` (`s`): Execute the current line and step *into* functions.
*   `continue` (`c`): Resume execution until the next breakpoint.
*   `print var` (`p`): Print the value of a variable.
*   `backtrace` (`bt`): Show the call stack that led to the current line.

## 36.2 Advanced Breakpoints

Often, a bug only happens on the 10,000th iteration of a loop. You can't press `continue` 10,000 times.

**Conditional Breakpoints:**
Tell the debugger to only pause if a specific C++ condition is met.
```text
(gdb) break main.cpp:100 if i == 9999
```

**Watchpoints (Hardware Breakpoints):**
Sometimes a variable changes, but you have no idea *which* function changed it. A Watchpoint asks the CPU hardware to monitor a specific memory address and pause execution the exact microsecond any assembly instruction writes to it.
```text
(gdb) watch my_global_variable
```

## 36.3 The Sanitizers

Debugging memory corruption (like a Buffer Overflow) in GDB is incredibly difficult because the crash usually happens millions of instructions *after* the actual corruption occurred.

Modern compilers (GCC and Clang) include **Sanitizers**. These are compiler flags that inject tracking code into your application. They slow your program down by 2x-5x, but they catch bugs the exact moment they happen.

### AddressSanitizer (ASan)
Compile with `-fsanitize=address`.
ASan poisons the memory surrounding your arrays and heap allocations. If your code tries to read or write 1 byte past the end of an array, or tries to use a pointer after it has been deleted (Use-After-Free), ASan instantly halts the program and prints an exact stack trace of the violation.

### ThreadSanitizer (TSan)
Compile with `-fsanitize=thread`.
Data races are the hardest bugs to track down because they are non-deterministic. TSan tracks every memory access across every thread. If two threads access the same variable without a mutex, and at least one is writing, TSan halts the program and prints the stack traces of both offending threads.

### UndefinedBehaviorSanitizer (UBSan)
Compile with `-fsanitize=undefined`.
Catches things like signed integer overflow, division by zero, and unaligned memory access.

## 36.4 Valgrind

Before Sanitizers existed, there was Valgrind (specifically the Memcheck tool). 
Unlike ASan, which requires you to recompile your code, Valgrind runs your pre-compiled executable inside a virtual machine. 

```bash
valgrind --leak-check=full ./my_app
```
It tracks every single `malloc` and `free`. When your program exits, Valgrind prints a detailed report of any memory that was not freed, completely eliminating memory leaks. *(Note: Valgrind is much slower than ASan, often slowing execution by 20x).*

## 36.5 Post-Mortem Debugging: Core Dumps

What happens if your application crashes in a production environment where you can't attach a debugger?

Linux supports **Core Dumps**. When an application crashes (e.g., Segfault), the OS can freeze the exact state of the program's RAM and write it to a file on disk (the "core" file).

You can then load that file into GDB on your local machine:
```bash
gdb ./my_app ./core
```
GDB will instantly put you at the exact line of code that caused the crash, allowing you to inspect the variables as they existed at the moment of failure.

## 36.6 Modern Stack Traces (C++23)

For decades, if a C++ program threw an unhandled exception or hit a fatal error, the terminal would just print `Aborted (core dumped)`. 

Other languages (like Java or Python) print beautiful stack traces. Finally, C++23 introduced `<stacktrace>`.

```cpp
#include <iostream>
#include <stacktrace>

void crash_handler() {
    std::cout << "CRASH! Stack trace:\n";
    std::cout << std::stacktrace::current() << '\n';
    std::abort();
}
```
You can tie this into a custom `std::set_terminate` handler to ensure that your application always prints a stack trace before it dies.

---

With our code architected cleanly and our bugs squashed, there is only one piece of the puzzle left. We must understand how the code we write actually turns into a running executable. We move to the final technical phase: **Part X: Compilation and Systems**.
