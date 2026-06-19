# Chapter 02: Memory Types and Pointers

> *The building blocks of every program.*

If Chapter 1 taught you how to start the engine, this chapter teaches you how to steer. Programs exist to manipulate data. Whether you are rendering a 3D model, calculating a mortgage rate, or sending an HTTP request, you are just moving data around.

To move data, you need to store it. To store it, you need variables.

***

## 2.1 What Is a Variable? (The Hotel Room Analogy)

> [!NOTE]
> **🛋️ Fireside Chat: The Memory Hotel**
> Imagine your computer's RAM (Random Access Memory) is a massive hotel with billions of rooms.
>
> When you declare a **variable**, you are walking up to the front desk and saying: *"I need a room. I want to name it `score`, and I am going to put an integer inside it."*
>
> The computer assigns you a specific room (a memory address, like `0x7ffee9b`), sticks the label `score` on the door, and puts your integer inside. Whenever you ask for `score`, the computer runs to that room and retrieves what's inside.

In C++, you must explicitly declare a variable before using it. You must tell the compiler its **type** and its **name**.

```cpp
int score;      // Declaration: "Get me a room for an integer named 'score'"
score = 100;    // Assignment: "Put 100 inside that room"

int lives = 3;  // Declaration & Initialization: "Get a room, name it 'lives', put 3 in it"
```

## 2.2 Fundamental Types

Because C++ is statically typed, you must declare exactly what kind of data goes into the room. A room designed for a single character is much smaller than a room designed for a massive, highly-precise decimal number.

| Type | Description | Typical Size | Example |
| :--- | :--- | :--- | :--- |
| `int` | Integer (whole numbers) | 4 bytes | `42`, `-5` |
| `double` | Double-precision floating point (decimals) | 8 bytes | `3.14159` |
| `float` | Single-precision floating point | 4 bytes | `3.14f` |
| `char` | A single character | 1 byte | `'A'`, `'?'` |
| `bool` | Boolean (true or false) | 1 byte | `true`, `false` |

```cpp
int age = 25;
double temperature = 98.6;
char grade = 'A';
bool is_game_over = false;
```

## 2.3 Fixed-Width Integers `[C++11]`

The "typical size" in the table above is a lie. The C++ standard guarantees *minimum* sizes for `int`, but an `int` might be 2 bytes on an old embedded microcontroller, 4 bytes on your laptop, and 8 bytes on a supercomputer.

When you need exact precision (like in networking or binary file parsing), use the `<cstdint>` header to get fixed-width integers introduced in C++11:

```cpp
#include <cstdint>

int8_t   tiny_num = 120;        // Exactly 1 byte (8 bits)
int16_t  small_num = 30000;     // Exactly 2 bytes (16 bits)
int32_t  normal_num = 1000000;  // Exactly 4 bytes (32 bits)
int64_t  huge_num = 5000000000; // Exactly 8 bytes (64 bits)

uint32_t unsigned_num = 4000;   // Unsigned (can't be negative, but holds twice as high positive values)
```

## 2.4 Literals

A literal is a raw value typed directly into the code.

* **Integer**: `42`, `-10`
* **Floating-Point**: `3.14`, `0.5`, `1.2e5` (Scientific notation for 120,000)
* **Character**: `'A'`, `'\n'` (Newline character)
* **String**: `"Hello"` (Text wrapped in double quotes)
* **Boolean**: `true`, `false`

You can also use prefixes and suffixes to change how literals are read:

* `0x2A` (Hexadecimal for 42)
* `0b101010` (Binary for 42) `[C++14]`
* `3.14f` (Forces the compiler to treat this as a `float` instead of a `double`)
* `1'000'000` (Digit separators for readability) `[C++14]`

## 2.5 `const` and `constexpr` — Values That Never Change

If a variable shouldn't change, explicitly lock it down using `const`. This prevents you from accidentally overwriting it, and allows the compiler to optimize your code better.

```cpp
const double PI = 3.14159;
// PI = 4.0; // ERROR: Compiler will block this
```

In C++11, `constexpr` was introduced. While `const` just says "this value won't change," `constexpr` says "this value is known *right now, while compiling*."

```cpp
constexpr int SECONDS_IN_HOUR = 60 * 60; // Calculated by compiler, not at runtime
```

## 2.6 Initialization: Copy, Direct, Uniform, and Designated

C++ has a notoriously complex initialization history.

```cpp
// 1. Copy Initialization (C-Style)
int a = 5;

// 2. Direct Initialization (C++98)
int b(5);

// 3. Uniform Initialization (C++11) - Use this!
int c{5};
```

> [!TIP]
> **🔥 Godhood Tip: Use Uniform Initialization `{}`**
> The `{}` syntax prevents "narrowing conversions". If you try `int a = 3.14;`, the compiler will silently chop off the `.14` and make `a = 3`. If you try `int a{3.14};`, the compiler will throw a hard error and save you from a nasty bug.

## 2.7 `auto` Type Deduction `[C++11]`

If the compiler already knows the type based on the right side of the equals sign, you can use `auto` to save typing.

```cpp
auto age = 25;           // Compiler deduces 'int'
auto name = "Alice";     // Compiler deduces 'const char*' (string literal)
auto price = 19.99;      // Compiler deduces 'double'
```

Do not abuse `auto`. Use it when the type is glaringly obvious or hideously long.

## 2.8 Arithmetic Operators

Math in C++ works exactly as you learned in elementary school.

```cpp
int a = 10, b = 3;

a + b;  // 13
a - b;  // 7
a * b;  // 30
a / b;  // 3  (Integer division truncates the decimal!)
a % b;  // 1  (Modulo: remainder of division)
```

**Compound Assignment & Increment/Decrement:**

```cpp
int x = 5;
x += 3;  // x is now 8 (same as x = x + 3)
x++;     // x is now 9 (post-increment)
--x;     // x is now 8 (pre-decrement)
```

## 2.9 Comparison and Logical Operators

Comparisons return a `bool` (`true` or `false`).

```cpp
int a = 10, b = 5;

a == b; // false (Equal to)
a != b; // true  (Not equal to)
a > b;  // true  (Greater than)
a <= b; // false (Less than or equal to)

bool is_adult = true;
bool has_ticket = false;

is_adult && has_ticket; // false (Logical AND: both must be true)
is_adult || has_ticket; // true  (Logical OR: one must be true)
!is_adult;              // false (Logical NOT: flips the value)
```

## 2.10 Bitwise Operators

At the lowest level, everything is bits. Bitwise operators let you manipulate them directly. These are essential for embedded systems, graphics, and high-performance hashing.

* `&` (AND): Both bits must be 1.
* `|` (OR): At least one bit must be 1.
* `^` (XOR): Bits must be different.
* `~` (NOT): Flip all bits.
* `<<` (Left Shift): Shift bits left (effectively multiplies by 2^N).
* `>>` (Right Shift): Shift bits right (effectively divides by 2^N).

> [!TIP]
> **🔥 Godhood Tip: Fast Power of 2 Check**
> A legendary bitwise trick to check if a number is a power of 2:
> `bool isPowerOf2 = (x > 0) && ((x & (x - 1)) == 0);`

## 2.11 Type Conversions: Implicit Narrowing vs Widening

When you mix types, C++ tries to be helpful via **implicit conversion**.

* **Widening (Safe)**: Storing a smaller type in a larger type. `double d = 5;` (The `int` 5 becomes `5.0`).
* **Narrowing (Dangerous)**: Storing a larger type in a smaller type. `int i = 3.14;` (The `.14` is silently deleted).

We will cover explicit casting (`static_cast`) in Chapter 11. For now, avoid mixing signed (`int`) and unsigned (`uint32_t`) integers, as it leads to bizarre bugs.

## 2.12 Scope and Lifetime of Variables

A variable only lives inside the block `{ }` where it was created. This is called its **scope**.

```cpp
int global = 100; // Lives forever

int main() {
    int local = 5; // Lives until the end of main()

    {
        int nested = 10; // Lives only inside these braces
    }
    // std::cout << nested; // ERROR: nested is dead here!
}
```

## 2.13 🔥 Data Representation Deep Dive

To achieve Godhood, you must know what your variables actually look like in memory.

### Two's Complement (How Integers Work)

Most computers use "Two's Complement" to represent negative numbers. The highest bit (the leftmost bit) is the sign bit.

* `0000 0101` in binary = `5`
* To make it `-5`: Invert the bits (`1111 1010`), and add 1 (`1111 1011`).

If you add 1 to the maximum possible positive integer, it flips the sign bit and wraps around to the lowest possible negative number. This is called **Integer Overflow**, and it has caused rockets to explode.

### IEEE 754 (How Floats Work)

Floating-point numbers are not exact. They are approximations based on scientific notation, storing a *Sign*, an *Exponent*, and a *Mantissa/Fraction*.

> [!WARNING]
> **⚠️ The Danger Zone: Float Equality**
> Never do this: `if (0.1 + 0.2 == 0.3)`.
> Because 0.1 cannot be perfectly represented in binary, the left side actually evaluates to something like `0.30000000000000004`. The statement will be `false`.
> Instead, check if the difference is smaller than a tiny tolerance (epsilon).

***

Variables are the nouns of our code. Operators are the verbs. In the next chapter, we will learn how to write the grammar of control flow to make our code make decisions.

***

# Chapter 02: Memory Types & Pointers

### 1.1 The Address Space: The Map of Mem-City

Every computer program lives in a virtual "Address Space." Imagine this as a giant, infinite row of mailboxes, each with a unique number (the address).

#### The Layout of your Program in Memory

When your program starts, the Operating System divides Mem-City into several "Zoning Districts." Each district has its own rules, speed limits, and rent costs.

| District | The Zoning Rules | Who Lives Here? |
| :--- | :--- | :--- |
| **The Text Segment** | **Read-Only Park**: No one is allowed to change the ground. | Your compiled code (the machine instructions). It's fixed forever. |
| **The Data Segment** | **The Town Square**: Fixed-size statues that stay forever. | Global variables (`int g = 10;`) and `static` variables. |
| **The BSS Segment** | **The Empty Lot**: Reserved space for future statues. | Uninitialized global variables. The OS zero-initializes these lot for you. |
| **The Stack** | **The Quick-Start Desk**: A desk that grows and shrinks. | Local variables, function parameters, and the "Return Address" (how the CPU knows where to go back to after a function). |
| **The Heap** | **The Industrial Warehouse**: Massive space you rent by the square foot. | Dynamic memory (`new`, `malloc`). It's big, but you have to manage it. |

***

### Fireside Chat: Why Pointers Break Your Brain

**Student**: "I just don't get it. If I have a variable `int x = 10;`, why can't I just use `x`? Why do I need `int* p = &x;`?"

**The Architect**: "Think about a huge library. If you want to tell your friend about a great book, you have two choices.

1. You can photocopy every single page of the book and hand them the pile of paper (**Pass by Value**).
2. You can just hand them a slip of paper with the shelf location: 'Floor 2, Row 10, Shelf 4' (**Pass by Pointer/Reference**)."

**Student**: "Okay, the location is easier. But what if the librarian moves the book?"

**The Architect**: "That's exactly why Pointers are dangerous! If the book moves but you still have the old address, you're looking at an empty shelf—or worse, a different book entirely. That's a **Dangling Pointer**."

***

### Step-by-Step: The Life of a Pointer

Let's trace a pointer's life in the CPU registers and RAM.

```cpp
int main() {
    int secret_number = 42;    // 1. Build a house
    int* spy = &secret_number; // 2. Write down the address
    *spy = 100;                // 3. Go to the address and change the contents
}
```

1. **Step 1**: The CPU asks the OS for 4 bytes on the **Stack**. The OS gives it address `0x1000`. The CPU writes the bits for `42` into that location.
2. **Step 2**: The CPU asks for another 8 bytes (on a 64-bit system) for the pointer `spy`. It stores the value `0x1000` into this new house.
3. **Step 3**: The CPU looks at the value in `spy` (`0x1000`), jumps to that location in RAM, and overwrites the `42` with `100`.

***

### 1.2 Common Pointer "Street Gangs" (Traps)

| The Trap | What it is | How to avoid it |
| :--- | :--- | :--- |
| **The Ghost (Wild Pointer)** | A pointer that was never initialized. It's pointing at a random house in the city. | Always initialize to `nullptr`. |
| **The Zombie (Dangling Pointer)** | You deleted the house, but you still have the address. | Set to `nullptr` immediately after `delete`. |
| **The Squatter (Memory Leak)** | You rented a warehouse locker, threw away the key, and never returned it. | Use **Smart Pointers** (RAII). |

***

### Deep Dive: Pointer Arithmetic (Walking the Streets)

Pointers are just numbers (addresses), so you can add or subtract from them. But C++ is smart—it knows the "size" of the houses.

* If you have an `int* p` pointing at address `100`, and you do `p++`, it doesn't go to `101`.
* It jumps to `104` (because an `int` is 4 bytes).

**It's like walking down a street where every house is exactly 4 meters wide. Taking one step forward always puts you at the front door of the next neighbor.**

***

# MEMORY, TYPES, AND POINTERS

Welcome to the heart of C++. Most languages (Java, Python, JS) try to hide memory from you. C++ hands you the keys to the city and says, "Don't burn it down."

### The City of Memory Analogy

Imagine your computer's RAM is a giant city called **Mem-City**.

1. **Memory Addresses**: Every house in Mem-City has a unique street address (e.g., `0x7ffee6b5a`).
2. **Variables**: A variable is just a **House**. When you say `int x = 5;`, the Mayor (the OS) builds a house, puts the number `5` inside it, and names the house "x".
3. **Pointers**: A pointer is a **GPS Device**. It doesn't hold a value like `5`; it holds the **Street Address** of a house.

#### Why do we care?

In other languages, if you want to give someone your house, you have to *clone* the entire house and give them the copy. In C++, you just give them the **Street Address** (a pointer). It’s faster, more efficient, and allows two people to look at the same house at the same time.

***

### The Two Districts: Stack vs. Heap

Mem-City is divided into two main districts where variables can live:

| District | Analogy: The Work Space | Lifetime | Speed |
| :--- | :--- | :--- | :--- |
| **The Stack** | **The Desk**: Think of this as your immediate office desk. You put things on it as you need them. When you leave the office (function ends), the cleaning crew automatically wipes the desk clean. | Automatic (ends with `}`) | **Ultra Fast**. Just like grabbing a pen from your desk. |
| **The Heap** | **The Warehouse**: A giant storage facility across town. If you need to store something huge or keep it forever, you call the Warehouse Manager (`new`) and ask for a locker. | Manual (You must `delete` it) | **Slower**. You have to travel to the warehouse and talk to the manager. |

> **There are no dumb questions...**
>
> **Q: What happens if I forget to clean out my Warehouse locker (Heap memory)?**
> **A:** You get a **Memory Leak**. The locker stays "rented" forever, even if your program isn't using it. If you keep doing this, Mem-City runs out of space and the whole computer crashes.
>
> **Q: Why don't I just put everything on the Stack (The Desk)?**
> **A:** Because your desk is small! If you try to put a 1,000-page book on a tiny desk, you get a **Stack Overflow**. Use the Warehouse for the big stuff.

***

# ADVANCED POINTERS & MEMORY

## 1.1 Pointer to Const vs Const Pointer

```cpp
#include <iostream>

using namespace std;

int main() {
    int x = 5, y = 10;

    // Pointer to const - can't modify data
    const int* ptr1 = &x;
    // *ptr1 = 10;  // ERROR
    ptr1 = &y;      // OK - can change pointer

    // Const pointer - can't modify pointer
    int* const ptr2 = &x;
    *ptr2 = 10;     // OK - can change data
    // ptr2 = &y;   // ERROR

    // Const pointer to const - can't modify either
    const int* const ptr3 = &x;
    // *ptr3 = 10;  // ERROR
    // ptr3 = &y;   // ERROR

    return 0;
}
```

## 1.2 Void Pointers

```cpp
#include <iostream>

using namespace std;

int main() {
    int x = 42;
    double y = 3.14;

    // Void pointer can point to any type
    void* ptr = &x;
    cout << *(int*)ptr << endl;  // 42

    ptr = &y;
    cout << *(double*)ptr << endl;  // 3.14

    // Generic function using void*
    void print_value(void* ptr, char type) {
        if (type == 'i') {
            cout << *(int*)ptr << endl;
        } else if (type == 'd') {
            cout << *(double*)ptr << endl;
        }
    }

    print_value(&x, 'i');  // 42
    print_value(&y, 'd');  // 3.14

    return 0;
}
```

***

### Professional Insights: Pointers & References

#### 1. Arrays as Pointers

In C++, an array name decays to a pointer to its first element in most contexts.

* **Accessing**: `arr[i]` is equivalent to `*(arr + i)`.
* **Size**: `sizeof(arr)` returns the total size in bytes, whereas `sizeof(ptr)` returns the size of the pointer (usually 4 or 8 bytes).

#### 2. References vs. Pointers

* **References**: Must be initialized upon declaration. Cannot be NULL. Cannot be reseated (re-pointed).
* **Pointers**: Can be initialized later. Can be NULL. Can point to different objects over time.

**Godhood Tip**: Use references for function parameters to avoid copying and for operator overloading. Use pointers for dynamic memory management and optional parameters.

#### 3. Pointers to Members

Pointers to members are a specialized feature allowing you to point to a data member or function inside a class without an instance.

```cpp
struct Point { int x, y; };
int Point::*p_x = &Point::x; // Pointer to member x

Point p = {10, 20};
std::cout << p.*p_x << std::endl; // Accessing via pointer to member
```

#### 4. The `this` Pointer

Inside every non-static member function, `this` is a hidden pointer to the current instance.

* **Type**: `T* const` (or `const T* const` in const methods).
* **Usage**: Returning `*this` allows for method chaining (e.g., in a Fluent API).

***

### Professional Insights: Language Boundary & Storage

#### 1. C Incompatibilities: The Parent's Shadow

While C++ evolved from C, they are distinct languages.

* **`void*` Conversion**: C allows `int* p = malloc(10);`. C++ requires `int* p = static_cast<int*>(malloc(10));`.
* **Enumerations**: In C, enums are effectively integers. In C++, they are distinct types.
* **Tentative Definitions**: C allows `int x; int x;` at file scope. C++ considers the second one a redefinition (ODR violation).

#### 2. Storage Class Specifiers

* **`static`**: Internal linkage for globals; persistent lifetime for locals.
* **`extern`**: External linkage. Tells the compiler the variable is defined elsewhere.
* **`thread_local` (C++11)**: Unique instance per thread.
* **`register`**: (Deprecated in C++11, removed in C++17) Hint to use a CPU register.
* **`auto`**: (C++98) Automatic storage. (C++11) Type deduction.

#### 3. Digit Separators and Binary Literals (C++14)

Use `'` to separate digits for readability: `int x = 1'000'000;`. Use `0b` for binary: `int b = 0b1101;`.

***

## 1.3 Null Pointer Safety

```cpp
#include <iostream>

using namespace std;

int main() {
    int* ptr = NULL;  // Set to null

    // Always check before dereferencing
    if (ptr != NULL) {
        cout << *ptr << endl;
    } else {
        cout << "Pointer is NULL" << endl;
    }

    // Safer approach
    ptr = new int(42);
    if (ptr) {
        cout << *ptr << endl;
        delete ptr;
        ptr = NULL;
    }

    return 0;
}
```

## 1.4 Memory Layout & Alignment

```cpp
#include <iostream>

using namespace std;

int main() {
    struct Data {
        char a;     // 1 byte
        int b;      // 4 bytes
        double c;   // 8 bytes
    };

    cout << "Size of Data: " << sizeof(Data) << endl;
    // Likely 16 or 24 (due to alignment padding)

    cout << "Size of char: " << sizeof(char) << endl;      // 1
    cout << "Size of int: " << sizeof(int) << endl;        // 4
    cout << "Size of double: " << sizeof(double) << endl;  // 8

    Data data;
    cout << "Address of a: " << (void*)&data.a << endl;
    cout << "Address of b: " << (void*)&data.b << endl;
    cout << "Address of c: " << (void*)&data.c << endl;

    return 0;
}
```

***

# ADVANCED ARRAYS

## 4.1 Dynamic Arrays

```cpp
#include <iostream>

using namespace std;

int main() {
    // 1D dynamic array
    int size = 5;
    int* arr = new int[size];

    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }

    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    delete[] arr;
    arr = NULL;

    // 2D dynamic array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }

    // Fill matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }

    // Delete matrix
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;

    return 0;
}
```

## 4.2 Variable Length Arrays (Non-standard)

```cpp
#include <iostream>

using namespace std;

int main() {
    int size;
    cout << "Enter size: ";
    cin >> size;

    // VLA - not standard but supported by many compilers
    int arr[size];  // GCC extension

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
```

## 4.3 Array Bounds & Safety

```cpp
#include <iostream>

using namespace std;

int main() {
    int arr[5] = {10, 20, 30, 40, 50};

    // No bounds checking in C++
    cout << arr[0] << endl;   // 10 (OK)
    cout << arr[10] << endl;  // Undefined behavior!

    // Manual bounds checking
    int index = 5;
    if (index >= 0 && index < 5) {
        cout << arr[index] << endl;
    } else {
        cout << "Index out of bounds" << endl;
    }

    return 0;
}
```

***

<!-- Merged content from Chapter_12_CONST__VOLATILE.md -->

# CONST & VOLATILE

## 11.1 Const Correctness

```cpp
#include <iostream>

using namespace std;

int main() {
    int x = 10;

    // const variable
    const int constant = 5;
    // constant = 10;  // ERROR

    // pointer to const
    const int* ptr1 = &x;
    // *ptr1 = 20;  // ERROR
    ptr1 = &constant;  // OK

    // const pointer
    int* const ptr2 = &x;
    *ptr2 = 20;  // OK
    // ptr2 = &constant;  // ERROR

    // const reference
    const int& ref = x;
    // ref = 20;  // ERROR

    cout << x << endl;
    cout << *ptr1 << endl;
    cout << *ptr2 << endl;
    cout << ref << endl;

    return 0;
}
```

## 11.2 Volatile Keyword

```cpp
#include <iostream>

using namespace std;

int main() {
    // volatile - tells compiler value may change unexpectedly
    volatile int sensor_reading = 0;  // From hardware

    // Compiler won't optimize away reads
    while (sensor_reading < 100) {
        // Check actual value each time, not cached
    }

    // Common use: hardware registers
    volatile int* hardware_register = (volatile int*)0x1000;

    // Each access reads from actual location
    int val1 = *hardware_register;
    int val2 = *hardware_register;

    // Without volatile, compiler might optimize one read

    return 0;
}
```

***

<!-- Merged content from Chapter_9_TYPE_CASTING.md -->

# TYPE CASTING

## 8.1 C-Style Casting

```cpp
#include <iostream>
#include <cmath>

using namespace std;

int main() {
    double d = 3.14;

    // C-style cast (avoid in modern C++)
    int i = (int)d;  // 3
    cout << i << endl;

    int x = 65;
    char c = (char)x;  // 'A'
    cout << c << endl;

    float f = (float)d;
    cout << f << endl;

    return 0;
}
```

## 8.2 Implicit Conversions

```cpp
#include <iostream>

using namespace std;

int main() {
    // Implicit conversions
    int x = 5;
    double d = x;  // int to double (automatic)
    cout << d << endl;  // 5.0

    double d2 = 3.9;
    int y = d2;  // double to int (loses precision)
    cout << y << endl;  // 3

    // Char arithmetic
    char c = 'A';
    int code = c;  // char to int (gets ASCII)
    cout << code << endl;  // 65

    return 0;
}
```

***

<!-- Merged content from Chapter_11_ENUMERATION__UNIONS.md -->

# ENUMERATION & UNIONS

## 10.1 Enumerations

```cpp
#include <iostream>

using namespace std;

// Enum definition
enum Color { RED, GREEN, BLUE };

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

int main() {
    Color c = RED;
    cout << c << endl;  // 0

    Color colors[3] = {RED, GREEN, BLUE};

    // Switching on enum
    switch (c) {
        case RED:
            cout << "Red" << endl;
            break;
        case GREEN:
            cout << "Green" << endl;
            break;
        case BLUE:
            cout << "Blue" << endl;
            break;
    }

    // Iterate through enum values
    for (int dir = NORTH; dir <= WEST; dir++) {
        cout << "Direction: " << dir << endl;
    }

    return 0;
}
```

## 10.2 Unions

```cpp
#include <iostream>

using namespace std;

// Union - all members share same memory
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data data;

    cout << "Size of Data: " << sizeof(data) << endl;  // 4 (size of largest member)

    data.i = 10;
    cout << "data.i: " << data.i << endl;     // 10
    cout << "data.f: " << data.f << endl;     // Garbage (overwrites data.i)

    data.f = 3.14;
    cout << "data.i: " << data.i << endl;     // Garbage (overwrites by data.f)
    cout << "data.f: " << data.f << endl;     // 3.14

    // Union useful for memory-constrained systems
    union Variant {
        int int_val;
        double double_val;
        char char_val;
    };

    cout << "Size of Variant: " << sizeof(Variant) << endl;

    return 0;
}
```

***

<!-- Merged content from Chapter_7_BITWISE_OPERATIONS.md -->

# BITWISE OPERATIONS

## 6.1 Bitwise Operators

```cpp
#include <iostream>

using namespace std;

int main() {
    unsigned char a = 5;   // 0101
    unsigned char b = 3;   // 0011

    // AND
    cout << (a & b) << endl;  // 0001 = 1

    // OR
    cout << (a | b) << endl;  // 0111 = 7

    // XOR
    cout << (a ^ b) << endl;  // 0110 = 6

    // NOT (bitwise complement)
    cout << (~a) << endl;     // 1010 = 250 (for unsigned char)

    // Left shift
    cout << (a << 1) << endl; // 1010 = 10

    // Right shift
    cout << (b >> 1) << endl; // 0001 = 1

    return 0;
}
```

## 6.2 Bit Manipulation Techniques

```cpp
#include <iostream>

using namespace std;

int main() {
    unsigned int num = 5;  // 0101

    // Check if bit is set
    int bit_pos = 2;
    bool is_set = (num >> bit_pos) & 1;
    cout << "Bit " << bit_pos << " is: " << is_set << endl;

    // Set a bit
    num |= (1 << 1);  // Set bit 1
    cout << "After setting bit 1: " << num << endl;  // 7 (0111)

    // Clear a bit
    num &= ~(1 << 1);  // Clear bit 1
    cout << "After clearing bit 1: " << num << endl;  // 5 (0101)

    // Toggle a bit
    num ^= (1 << 0);  // Toggle bit 0
    cout << "After toggling bit 0: " << num << endl;  // 4 (0100)

    // Count set bits
    unsigned int count = 0;
    unsigned int temp = num;
    while (temp) {
        count += temp & 1;
        temp >>= 1;
    }
    cout << "Number of set bits: " << count << endl;

    return 0;
}
```

***

## Professional Insights: Arrays

Arrays are elements of the same type placed in adjoining memory locations. The elements can be individually
referenced by a unique identiﬁer with an added index.
This allows you to declare multiple variable values of a speciﬁc type and access them individually without needing
to declare a variable for each value.
Section 8.1: Array initialization
An array is just a block of sequential memory locations for a speciﬁc type of variable. Arrays are allocated the same
way as normal variables, but with square brackets appended to its name [] that contain the number of elements
that ﬁt into the array memory.
The following example of an array uses the typ int, the variable name arrayOfInts, and the number of elements
 that the array has space for:
int arrayOfInts;
An array can be declared and initialized at the same time like this
int arrayOfInts = {10, 20, 30, 40, 50};
When initializing an array by listing all of its members, it is not necessary to include the number of elements inside
the square brackets. It will be automatically calculated by the compiler. In the following example, it's 5:
int arrayOfInts[] = {10, 20, 30, 40, 50};
It is also possible to initialize only the ﬁrst elements while allocating more space. In this case, deﬁning the length in
brackets is mandatory. The following will allocate an array of length 5 with partial initialization, the compiler
initializes all remaining elements with the standard value of the element type, in this case zero.
int arrayOfInts = {10,20}; // means 10, 20, 0, 0, 0
Arrays of other basic data types may be initialized in the same way.
char arrayOfChars; // declare the array and allocate the memory, don't initialize
char arrayOfChars = { 'a', 'b', 'c', 'd', 'e' } ; //declare and initialize
double arrayOfDoubles = {1.14159, 2.14159, 3.14159, 4.14159, 5.14159};
string arrayOfStrings = { "C++", "is", "super", "duper", "great!"};
It is also important to take note that when accessing array elements, the array's element index(or position) starts
from 0.
int array = { 10/*Element no.0*/, 20/*Element no.1*/, 30, 40, 50/*Element no.4*/};

```cpp
std::cout << array[4]; //outputs 50
std::cout << array[0]; //outputs 10

Section 8.2: A ﬁxed size raw array matrix (that is, a 2D raw
array)
// A fixed size raw array matrix (that is, a 2D raw array).
#include <iostream>
#include <iomanip>

using namespace std;
auto main() -> int
{
    int const   n_rows  = 3;
    int const   n_cols  = 7;
    int const   m[n_rows][n_cols] =             // A raw array matrix.
    {
        {  1,  2,  3,  4,  5,  6,  7 },
        {  8,  9, 10, 11, 12, 13, 14 },
        { 15, 16, 17, 18, 19, 20, 21 }
    };
    for( int y = 0; y < n_rows; ++y )
    {
        for( int x = 0; x < n_cols; ++x )
        {
            cout << setw( 4 ) << m[y][x];       // Note: do NOT use m[y,x]!
        }
        cout << '\\n';
    }
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
C++ doesn't support special syntax for indexing a multi-dimensional array. Instead such an array is viewed as an
array of arrays (possibly of arrays, and so on), and the ordinary single index notation [i] is used for each level. In
the example above m[y] refers to row y of m, where y is a zero-based index. Then this row can be indexed in turn,
e.g. m[y][x], which refers to the xth item – or column – of row y.
I.e. the last index varies fastest, and in the declaration the range of this index, which here is the number of columns
per row, is the last and “innermost” size speciﬁed.
Since C++ doesn't provide built-in support for dynamic size arrays, other than dynamic allocation, a dynamic size
matrix is often implemented as a class. Then the raw array matrix indexing notation m[y][x] has some cost, either
by exposing the implementation (so that e.g. a view of a transposed matrix becomes practically impossible) or by
adding some overhead and slight inconvenience when it's done by returning a proxy object from operator[]. And
so the indexing notation for such an abstraction can and will usually be diﬀerent, both in look-and-feel and in the
order of indices, e.g. m(x,y) or m.at(x,y) or m.item(x,y).
Section 8.3: Dynamically sized raw array
// Example of raw dynamic size array. It's generally better to use std::vector.
#include <algorithm>            // std::sort
#include <iostream>

using namespace std;
auto int_from( istream& in ) -> int { int x; in >> x; return x; }

auto main()
    -> int
{
    cout << "Sorting n integers provided by you.\\\\n";
    cout << "n? ";
    int const   n   = int_from( cin );
    int*        a   = new int[n];       // ← Allocation of array of n items.
    for( int i = 1; i <= n; ++i )
    {
        cout << "The #" << i << " number, please: ";
        a[i-1] = int_from( cin );
    }
    sort( a, a + n );
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\\\n';
    delete[] a;
}
A program that declares an array T a[n]; where n is determined a run-time, can compile with certain compilers
that support C99 variadic length arrays (VLAs) as a language extension. But VLAs are not supported by standard C++.
This example shows how to manually allocate a dynamic size array via a new[]-expression,
int*        a   = new int[n];       // ← Allocation of array of n items.
… then use it, and ﬁnally deallocate it via a delete[]-expression:
delete[] a;
The array allocated here has indeterminate values, but it can be zero-initialized by just adding an empty
parenthesis (), like this: new int[n](). More generally, for arbitrary item type, this performs a value-initialization.
As part of a function down in a call hierarchy this code would not be exception safe, since an exception before the
delete[] expression (and after the new[]) would cause a memory leak. One way to address that issue is to
automate the cleanup via e.g. a std::unique_ptr smart pointer. But a generally better way to address it is to just
use a std::vector: that's what std::vector is there for.
Section 8.4: Array size: type safe at compile time
#include      // size_t, ptrdiff_t

//----------------------------------- Machinery:
using Size = ptrdiff_t;
template< class Item, size_t n >
constexpr auto n_items( Item (&)[n] ) noexcept
-> Size
{ return n; }
//----------------------------------- Usage:
#include

using namespace std;
auto main()

-> int
{
int const   a[]     = {3, 1, 4, 1, 5, 9, 2, 6, 5, 4};
Size const  n       = n_items( a );
int         b[n]    = {};       // An array of the same size as a.
(void) b;
cout <}
The C idiom for array size, sizeof(a)/sizeof(a[0]), will accept a pointer as argument and will then generally yield
an incorrect result.
For C++11
using C++11 you can do:
std::extent<decltype(MyArray)>::value;
Example:
char MyArray[] = { 'X','o','c','e' };
const auto n = std::extent<decltype(MyArray)>::value;
std::cout << n << "\\n"; // Prints 4
Up till C++17 (forthcoming as of this writing) C++ had no built-in core language or standard library utility to obtain
the size of an array, but this can be implemented by passing the array by reference to a function template, as shown
above. Fine but important point: the template size parameter is a size_t, somewhat inconsistent with the signed
Size function result type, in order to accommodate the g++ compiler which sometimes insists on size_t for
template matching.
With C++17 and later one may instead use std::size, which is specialized for arrays.
Section 8.5: Expanding dynamic size array by using
std::vector
// Example of std::vector as an expanding dynamic size array.
#include <algorithm>            // std::sort
#include <iostream>
#include <vector>               // std::vector

using namespace std;
int int_from( std::istream& in ) { int x = 0; in >> x; return x; }
int main()
{
    cout << "Sorting integers provided by you.\\n";
    cout << "You can indicate EOF via F6 in Windows or Ctrl+D in Unix-land.\\n";
    vector<int> a;      // ← Zero size by default.
    while( cin )
    {
        cout << "One number, please, or indicate EOF: ";
        int const x = int_from( cin );
        if( !cin.fail() ) { a.push_back( x ); }  // Expands as necessary.
    }
    sort( a.begin(), a.end() );

    int const n = a.size();
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\n';
}
std::vector is a standard library class template that provides the notion of a variable size array. It takes care of all
the memory management, and the buﬀer is contiguous so a pointer to the buﬀer (e.g. &v[0] or v.data()) can be
passed to API functions requiring a raw array. A vector can even be expanded at run time, via e.g. the push_back
member function that appends an item.
The complexity of the sequence of n push_back operations, including the copying or moving involved in the vector
expansions, is amortized O(n). “Amortized”: on average.
```

Internally this is usually achieved by the vector doubling its buﬀer size, its capacity, when a larger buﬀer is needed.
E.g. for a buﬀer starting out as size 1, and being repeatedly doubled as needed for n=17 push_back calls, this
involves 1 + 2 + 4 + 8 + 16 = 31 copy operations, which is less than 2×n = 34. And more generally the sum of this
sequence can't exceed 2×n.
Compared to the dynamic size raw array example, this vector-based code does not require the user to supply (and
know) the number of items up front. Instead the vector is just expanded as necessary, for each new item value
speciﬁed by the user.
Section 8.6: A dynamic size matrix using std::vector for
storage
Unfortunately as of C++14 there's no dynamic size matrix class in the C++ standard library. Matrix classes that
support dynamic size are however available from a number of 3rd party libraries, including the Boost Matrix library
(a sub-library within the Boost library).
If you don't want a dependency on Boost or some other library, then one poor man's dynamic size matrix in C++ is
just like
vector<vector<int>> m( 3, vector<int>( 7 ) );
… where vector is std::vector. The matrix is here created by copying a row vector n times where n is the number
of rows, here 3. It has the advantage of providing the same m[y][x] indexing notation as for a ﬁxed size raw array
matrix, but it's a bit ineﬃcient because it involves a dynamic allocation for each row, and it's a bit unsafe because
it's possible to inadvertently resize a row.
A more safe and eﬃcient approach is to use a single vector as storage for the matrix, and map the client code's (x, y)
to a corresponding index in that vector:
// A dynamic size matrix using std::vector for storage.
//--------------------------------------------- Machinery:
# include         // std::copy
# include          // assert
# include  // std::initializer_list
# include            // std::vector
# include          // ptrdiff_t

namespace my {
using Size = ptrdiff_t;

```cpp
using std::initializer_list;
using std::vector;
template< class Item >
class Matrix
{
private:
vector    items_;
Size            n_cols_;
auto index_for( Size const x, Size const y ) const
-> Size
{ return y*n_cols_ + x; }
public:
auto n_rows() const -> Size { return items_.size()/n_cols_; }
auto n_cols() const -> Size { return n_cols_; }
auto item( Size const x, Size const y )
-> Item&
{ return items_[index_for(x, y)]; }
auto item( Size const x, Size const y ) const
-> Item const&
{ return items_[index_for(x, y)]; }
Matrix(): n_cols_( 0 ) {}
Matrix( Size const n_cols, Size const n_rows )
: items_( n_cols*n_rows )
, n_cols_( n_cols )
{}
Matrix( initializer_list< initializer_list > const& values )
: items_()
, n_cols_( values.size() == 0? 0 : values.begin()->size() )
{
for( auto const& row : values )
{
assert( Size( row.size() ) == n_cols_ );
items_.insert( items_.end(), row.begin(), row.end() );
}
}
};
}  // namespace my
//--------------------------------------------- Usage:
using my::Matrix;
auto some_matrix()
-> Matrix
{
return
{
{  1,  2,  3,  4,  5,  6,  7 },
{  8,  9, 10, 11, 12, 13, 14 },
{ 15, 16, 17, 18, 19, 20, 21 }
};
}
#include
#include

using namespace std;
auto main() -> int
{
Matrix const m = some_matrix();
assert( m.n_cols() == 7 );

assert( m.n_rows() == 3 );
for( int y = 0, y_end = m.n_rows(); y < y_end; ++y )
{
for( int x = 0, x_end = m.n_cols(); x < x_end; ++x )
{
cout <← Note: not `m[y][x]`!
}
cout <}
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
The above code is not industrial grade: it's designed to show the basic principles, and serve the needs of students
learning C++.
```

For example, one may deﬁne operator() overloads to simplify the indexing notation.

***

### Professional Notes: Pointers & References

#### 1. Arrays as Pointers

In C++, an array name decays to a pointer to its first element in most contexts.

* **Accessing**: `arr[i]` is equivalent to `*(arr + i)`.
* **Size**: `sizeof(arr)` returns the total size in bytes, whereas `sizeof(ptr)` returns the size of the pointer (usually 4 or 8 bytes).

#### 2. References vs. Pointers

* **References**: Must be initialized upon declaration. Cannot be NULL. Cannot be reseated (re-pointed).
* **Pointers**: Can be initialized later. Can be NULL. Can point to different objects over time.

**Godhood Tip**: Use references for function parameters to avoid copying and for operator overloading. Use pointers for dynamic memory management and optional parameters.

#### 3. Pointers to Members

Pointers to members are a specialized feature allowing you to point to a data member or function inside a class without an instance.

```cpp
struct Point { int x, y; };
int Point::*p_x = &Point::x; // Pointer to member x

Point p = {10, 20};
std::cout << p.*p_x << std::endl; // Accessing via pointer to member
```

#### 4. The `this` Pointer

Inside every non-static member function, `this` is a hidden pointer to the current instance.

* **Type**: `T* const` (or `const T* const` in const methods).
* **Usage**: Returning `*this` allows for method chaining (e.g., in a Fluent API).

***

***

### Professional Notes: Language Boundary & Storage

#### 1. C Incompatibilities: The Parent's Shadow

While C++ evolved from C, they are distinct languages.

* **`void*` Conversion**: C allows `int* p = malloc(10);`. C++ requires `int* p = static_cast<int*>(malloc(10));`.
* **Enumerations**: In C, enums are effectively integers. In C++, they are distinct types.
* **Tentative Definitions**: C allows `int x; int x;` at file scope. C++ considers the second one a redefinition (ODR violation).

#### 2. Storage Class Specifiers

* **`static`**: Internal linkage for globals; persistent lifetime for locals.
* **`extern`**: External linkage. Tells the compiler the variable is defined elsewhere.
* **`thread_local` (C++11)**: Unique instance per thread.
* **`register`**: (Deprecated in C++11, removed in C++17) Hint to use a CPU register.
* **`auto`**: (C++98) Automatic storage. (C++11) Type deduction.

#### 3. Digit Separators and Binary Literals (C++14)

Use `'` to separate digits for readability: `int x = 1'000'000;`. Use `0b` for binary: `int b = 0b1101;`.

***

## 1.3 Null Pointer Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = NULL;  // Set to null
    
    // Always check before dereferencing
    if (ptr != NULL) {
        cout << *ptr << endl;
    } else {
        cout << "Pointer is NULL" << endl;
    }
    
    // Safer approach
    ptr = new int(42);
    if (ptr) {
        cout << *ptr << endl;
        delete ptr;
        ptr = NULL;
    }
    
    return 0;
}
```

## 1.4 Memory Layout & Alignment

```cpp
#include <iostream>
using namespace std;

int main() {
    struct Data {
        char a;     // 1 byte
        int b;      // 4 bytes
        double c;   // 8 bytes
    };
    
    cout << "Size of Data: " << sizeof(Data) << endl;
    // Likely 16 or 24 (due to alignment padding)
    
    cout << "Size of char: " << sizeof(char) << endl;      // 1
    cout << "Size of int: " << sizeof(int) << endl;        // 4
    cout << "Size of double: " << sizeof(double) << endl;  // 8
    
    Data data;
    cout << "Address of a: " << (void*)&data.a << endl;
    cout << "Address of b: " << (void*)&data.b << endl;
    cout << "Address of c: " << (void*)&data.c << endl;
    
    return 0;
}
```

***

# ADVANCED ARRAYS

## 4.1 Dynamic Arrays

```cpp
#include <iostream>
using namespace std;

int main() {
    // 1D dynamic array
    int size = 5;
    int* arr = new int[size];
    
    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    delete[] arr;
    arr = NULL;
    
    // 2D dynamic array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    
    // Fill matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }
    
    // Delete matrix
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    
    return 0;
}
```

## 4.2 Variable Length Arrays (Non-standard)

```cpp
#include <iostream>
using namespace std;

int main() {
    int size;
    cout << "Enter size: ";
    cin >> size;
    
    // VLA - not standard but supported by many compilers
    int arr[size];  // GCC extension
    
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 4.3 Array Bounds & Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    
    // No bounds checking in C++
    cout << arr[0] << endl;   // 10 (OK)
    cout << arr[10] << endl;  // Undefined behavior!
    
    // Manual bounds checking
    int index = 5;
    if (index >= 0 && index < 5) {
        cout << arr[index] << endl;
    } else {
        cout << "Index out of bounds" << endl;
    }
    
    return 0;
}
```

***

<!-- Merged content from Chapter_12_CONST__VOLATILE.md -->

# CONST & VOLATILE

## 11.1 Const Correctness

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10;
    
    // const variable
    const int constant = 5;
    // constant = 10;  // ERROR
    
    // pointer to const
    const int* ptr1 = &x;
    // *ptr1 = 20;  // ERROR
    ptr1 = &constant;  // OK
    
    // const pointer
    int* const ptr2 = &x;
    *ptr2 = 20;  // OK
    // ptr2 = &constant;  // ERROR
    
    // const reference
    const int& ref = x;
    // ref = 20;  // ERROR
    
    cout << x << endl;
    cout << *ptr1 << endl;
    cout << *ptr2 << endl;
    cout << ref << endl;
    
    return 0;
}
```

## 11.2 Volatile Keyword

```cpp
#include <iostream>
using namespace std;

int main() {
    // volatile - tells compiler value may change unexpectedly
    volatile int sensor_reading = 0;  // From hardware
    
    // Compiler won't optimize away reads
    while (sensor_reading < 100) {
        // Check actual value each time, not cached
    }
    
    // Common use: hardware registers
    volatile int* hardware_register = (volatile int*)0x1000;
    
    // Each access reads from actual location
    int val1 = *hardware_register;
    int val2 = *hardware_register;
    
    // Without volatile, compiler might optimize one read
    
    return 0;
}
```

***

<!-- Merged content from Chapter_9_TYPE_CASTING.md -->

# TYPE CASTING

## 8.1 C-Style Casting

```cpp
#include <iostream>
#include <cmath>
using namespace std;

int main() {
    double d = 3.14;
    
    // C-style cast (avoid in modern C++)
    int i = (int)d;  // 3
    cout << i << endl;
    
    int x = 65;
    char c = (char)x;  // 'A'
    cout << c << endl;
    
    float f = (float)d;
    cout << f << endl;
    
    return 0;
}
```

## 8.2 Implicit Conversions

```cpp
#include <iostream>
using namespace std;

int main() {
    // Implicit conversions
    int x = 5;
    double d = x;  // int to double (automatic)
    cout << d << endl;  // 5.0
    
    double d2 = 3.9;
    int y = d2;  // double to int (loses precision)
    cout << y << endl;  // 3
    
    // Char arithmetic
    char c = 'A';
    int code = c;  // char to int (gets ASCII)
    cout << code << endl;  // 65
    
    return 0;
}
```

***

<!-- Merged content from Chapter_11_ENUMERATION__UNIONS.md -->

# ENUMERATION & UNIONS

## 10.1 Enumerations

```cpp
#include <iostream>
using namespace std;

// Enum definition
enum Color { RED, GREEN, BLUE };

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

int main() {
    Color c = RED;
    cout << c << endl;  // 0
    
    Color colors[3] = {RED, GREEN, BLUE};
    
    // Switching on enum
    switch (c) {
        case RED:
            cout << "Red" << endl;
            break;
        case GREEN:
            cout << "Green" << endl;
            break;
        case BLUE:
            cout << "Blue" << endl;
            break;
    }
    
    // Iterate through enum values
    for (int dir = NORTH; dir <= WEST; dir++) {
        cout << "Direction: " << dir << endl;
    }
    
    return 0;
}
```

## 10.2 Unions

```cpp
#include <iostream>
using namespace std;

// Union - all members share same memory
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data data;
    
    cout << "Size of Data: " << sizeof(data) << endl;  // 4 (size of largest member)
    
    data.i = 10;
    cout << "data.i: " << data.i << endl;     // 10
    cout << "data.f: " << data.f << endl;     // Garbage (overwrites data.i)
    
    data.f = 3.14;
    cout << "data.i: " << data.i << endl;     // Garbage (overwrites by data.f)
    cout << "data.f: " << data.f << endl;     // 3.14
    
    // Union useful for memory-constrained systems
    union Variant {
        int int_val;
        double double_val;
        char char_val;
    };
    
    cout << "Size of Variant: " << sizeof(Variant) << endl;
    
    return 0;
}
```

***

<!-- Merged content from Chapter_7_BITWISE_OPERATIONS.md -->

# BITWISE OPERATIONS

## 6.1 Bitwise Operators

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned char a = 5;   // 0101
    unsigned char b = 3;   // 0011
    
    // AND
    cout << (a & b) << endl;  // 0001 = 1
    
    // OR
    cout << (a | b) << endl;  // 0111 = 7
    
    // XOR
    cout << (a ^ b) << endl;  // 0110 = 6
    
    // NOT (bitwise complement)
    cout << (~a) << endl;     // 1010 = 250 (for unsigned char)
    
    // Left shift
    cout << (a << 1) << endl; // 1010 = 10
    
    // Right shift
    cout << (b >> 1) << endl; // 0001 = 1
    
    return 0;
}
```

## 6.2 Bit Manipulation Techniques

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned int num = 5;  // 0101
    
    // Check if bit is set
    int bit_pos = 2;
    bool is_set = (num >> bit_pos) & 1;
    cout << "Bit " << bit_pos << " is: " << is_set << endl;
    
    // Set a bit
    num |= (1 << 1);  // Set bit 1
    cout << "After setting bit 1: " << num << endl;  // 7 (0111)
    
    // Clear a bit
    num &= ~(1 << 1);  // Clear bit 1
    cout << "After clearing bit 1: " << num << endl;  // 5 (0101)
    
    // Toggle a bit
    num ^= (1 << 0);  // Toggle bit 0
    cout << "After toggling bit 0: " << num << endl;  // 4 (0100)
    
    // Count set bits
    unsigned int count = 0;
    unsigned int temp = num;
    while (temp) {
        count += temp & 1;
        temp >>= 1;
    }
    cout << "Number of set bits: " << count << endl;
    
    return 0;
}
```

***

# Professional Notes: Chapter 8: Arrays

Arrays are elements of the same type placed in adjoining memory locations. The elements can be individually
referenced by a unique identiﬁer with an added index.
This allows you to declare multiple variable values of a speciﬁc type and access them individually without needing
to declare a variable for each value.
Section 8.1: Array initialization
An array is just a block of sequential memory locations for a speciﬁc type of variable. Arrays are allocated the same
way as normal variables, but with square brackets appended to its name [] that contain the number of elements
that ﬁt into the array memory.
The following example of an array uses the typ int, the variable name arrayOfInts, and the number of elements
 that the array has space for:
int arrayOfInts;
An array can be declared and initialized at the same time like this
int arrayOfInts = {10, 20, 30, 40, 50};
When initializing an array by listing all of its members, it is not necessary to include the number of elements inside
the square brackets. It will be automatically calculated by the compiler. In the following example, it's 5:
int arrayOfInts[] = {10, 20, 30, 40, 50};
It is also possible to initialize only the ﬁrst elements while allocating more space. In this case, deﬁning the length in
brackets is mandatory. The following will allocate an array of length 5 with partial initialization, the compiler
initializes all remaining elements with the standard value of the element type, in this case zero.
int arrayOfInts = {10,20}; // means 10, 20, 0, 0, 0
Arrays of other basic data types may be initialized in the same way.
char arrayOfChars; // declare the array and allocate the memory, don't initialize
char arrayOfChars = { 'a', 'b', 'c', 'd', 'e' } ; //declare and initialize
double arrayOfDoubles = {1.14159, 2.14159, 3.14159, 4.14159, 5.14159};
string arrayOfStrings = { "C++", "is", "super", "duper", "great!"};
It is also important to take note that when accessing array elements, the array's element index(or position) starts
from 0.
int array = { 10/*Element no.0*/, 20/*Element no.1*/, 30, 40, 50/*Element no.4*/};
std::cout << array[1]; //outputs 50
std::cout << array; //outputs 10
Section 8.2: A ﬁxed size raw array matrix (that is, a 2D raw
array)
// A fixed size raw array matrix (that is, a 2D raw array).
# include <iostream>
# include <iomanip>
using namespace std;
auto main() -> int
{
    int const   n_rows  = 3;
    int const   n_cols  = 7;
    int const   m[n_rows][n_cols] =             // A raw array matrix.
    {
        {  1,  2,  3,  4,  5,  6,  7 },
        {  8,  9, 10, 11, 12, 13, 14 },
        { 15, 16, 17, 18, 19, 20, 21 }
    };
    for( int y = 0; y < n_rows; ++y )
    {
        for( int x = 0; x < n_cols; ++x )
        {
            cout << setw( 4 ) << m[y][x];       // Note: do NOT use m[y,x]!
        }
        cout << '\n';
    }
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
C++ doesn't support special syntax for indexing a multi-dimensional array. Instead such an array is viewed as an
array of arrays (possibly of arrays, and so on), and the ordinary single index notation [i] is used for each level. In
the example above m[y] refers to row y of m, where y is a zero-based index. Then this row can be indexed in turn,
e.g. m[y][x], which refers to the xth item – or column – of row y.
I.e. the last index varies fastest, and in the declaration the range of this index, which here is the number of columns
per row, is the last and “innermost” size speciﬁed.
Since C++ doesn't provide built-in support for dynamic size arrays, other than dynamic allocation, a dynamic size
matrix is often implemented as a class. Then the raw array matrix indexing notation m[y][x] has some cost, either
by exposing the implementation (so that e.g. a view of a transposed matrix becomes practically impossible) or by
adding some overhead and slight inconvenience when it's done by returning a proxy object from operator[]. And
so the indexing notation for such an abstraction can and will usually be diﬀerent, both in look-and-feel and in the
order of indices, e.g. m(x,y) or m.at(x,y) or m.item(x,y).
Section 8.3: Dynamically sized raw array
// Example of raw dynamic size array. It's generally better to use std::vector.
# include <algorithm>            // std::sort
# include <iostream>
using namespace std;
auto int_from( istream& in ) -> int { int x; in >> x; return x; }
auto main()
    -> int
{
    cout << "Sorting n integers provided by you.\\n";
    cout << "n? ";
    int const   n   = int_from( cin );
    int*a   = new int[n];       // ← Allocation of array of n items.
    for( int i = 1; i <= n; ++i )
    {
        cout << "The #" << i << " number, please: ";
        a[i-1] = int_from( cin );
    }
    sort( a, a + n );
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\n';
    delete[] a;
}
A program that declares an array T a[n]; where n is determined a run-time, can compile with certain compilers
that support C99 variadic length arrays (VLAs) as a language extension. But VLAs are not supported by standard C++.
This example shows how to manually allocate a dynamic size array via a new[]-expression,
int*        a   = new int[n];       // ← Allocation of array of n items.
… then use it, and ﬁnally deallocate it via a delete[]-expression:
delete[] a;
The array allocated here has indeterminate values, but it can be zero-initialized by just adding an empty
parenthesis (), like this: new int[n](). More generally, for arbitrary item type, this performs a value-initialization.
As part of a function down in a call hierarchy this code would not be exception safe, since an exception before the
delete[] expression (and after the new[]) would cause a memory leak. One way to address that issue is to
automate the cleanup via e.g. a std::unique_ptr smart pointer. But a generally better way to address it is to just
use a std::vector: that's what std::vector is there for.
Section 8.4: Array size: type safe at compile time
# include      // size_t, ptrdiff_t
//----------------------------------- Machinery:
using Size = ptrdiff_t;
template< class Item, size_t n >
constexpr auto n_items( Item [&](n) ) noexcept
-> Size
{ return n; }
//----------------------------------- Usage:
# include
using namespace std;
auto main()
-> int
{
int const   a[]     = {3, 1, 4, 1, 5, 9, 2, 6, 5, 4};
Size const  n       = n_items( a );
int         b[n]    = {};       // An array of the same size as a.
(void) b;
cout <}
The C idiom for array size, sizeof(a)/sizeof(a), will accept a pointer as argument and will then generally yield
an incorrect result.
For C++11
using C++11 you can do:
std::extent<decltype(MyArray)>::value;
Example:
char MyArray[] = { 'X','o','c','e' };
const auto n = std::extent<decltype(MyArray)>::value;
std::cout << n << "\n"; // Prints 4
Up till C++17 (forthcoming as of this writing) C++ had no built-in core language or standard library utility to obtain
the size of an array, but this can be implemented by passing the array by reference to a function template, as shown
above. Fine but important point: the template size parameter is a size_t, somewhat inconsistent with the signed
Size function result type, in order to accommodate the g++ compiler which sometimes insists on size_t for
template matching.
With C++17 and later one may instead use std::size, which is specialized for arrays.
Section 8.5: Expanding dynamic size array by using
std::vector
// Example of std::vector as an expanding dynamic size array.
# include <algorithm>            // std::sort
# include <iostream>
# include <vector>               // std::vector
using namespace std;
int int_from( std::istream& in ) { int x = 0; in >> x; return x; }
int main()
{
    cout << "Sorting integers provided by you.\n";
    cout << "You can indicate EOF via F6 in Windows or Ctrl+D in Unix-land.\n";
    vector<int> a;      // ← Zero size by default.
    while( cin )
    {
        cout << "One number, please, or indicate EOF: ";
        int const x = int_from( cin );
        if( !cin.fail() ) { a.push_back( x ); }  // Expands as necessary.
    }
    sort( a.begin(), a.end() );
    int const n = a.size();
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\n';
}
std::vector is a standard library class template that provides the notion of a variable size array. It takes care of all
the memory management, and the buﬀer is contiguous so a pointer to the buﬀer (e.g. &v or v.data()) can be
passed to API functions requiring a raw array. A vector can even be expanded at run time, via e.g. the push_back
member function that appends an item.
The complexity of the sequence of n push_back operations, including the copying or moving involved in the vector
expansions, is amortized O(n). “Amortized”: on average.
Internally this is usually achieved by the vector doubling its buﬀer size, its capacity, when a larger buﬀer is needed.
E.g. for a buﬀer starting out as size 1, and being repeatedly doubled as needed for n=17 push_back calls, this
involves 1 + 2 + 4 + 8 + 16 = 31 copy operations, which is less than 2×n = 34. And more generally the sum of this
sequence can't exceed 2×n.
Compared to the dynamic size raw array example, this vector-based code does not require the user to supply (and
know) the number of items up front. Instead the vector is just expanded as necessary, for each new item value
speciﬁed by the user.
Section 8.6: A dynamic size matrix using std::vector for
storage
Unfortunately as of C++14 there's no dynamic size matrix class in the C++ standard library. Matrix classes that
support dynamic size are however available from a number of 3rd party libraries, including the Boost Matrix library
(a sub-library within the Boost library).
If you don't want a dependency on Boost or some other library, then one poor man's dynamic size matrix in C++ is
just like
vector<vector<int>> m( 3, vector<int>( 7 ) );
… where vector is std::vector. The matrix is here created by copying a row vector n times where n is the number
of rows, here 3. It has the advantage of providing the same m[y][x] indexing notation as for a ﬁxed size raw array
matrix, but it's a bit ineﬃcient because it involves a dynamic allocation for each row, and it's a bit unsafe because
it's possible to inadvertently resize a row.
A more safe and eﬃcient approach is to use a single vector as storage for the matrix, and map the client code's (x, y)
to a corresponding index in that vector:
// A dynamic size matrix using std::vector for storage.
//--------------------------------------------- Machinery:
# include         // std::copy
# include          // assert
# include  // std::initializer_list
# include            // std::vector
# include          // ptrdiff_t
namespace my {
using Size = ptrdiff_t;
using std::initializer_list;
using std::vector;
template< class Item >
class Matrix
{
private:
vector    items_;
Size            n_cols_;
auto index_for( Size const x, Size const y ) const
-> Size
{ return y*n_cols_ + x; }
public:
auto n_rows() const -> Size { return items_.size()/n_cols_; }
auto n_cols() const -> Size { return n_cols_; }
auto item( Size const x, Size const y )
-> Item&
{ return items_[index_for(x, y)]; }
auto item( Size const x, Size const y ) const
-> Item const&
{ return items_[index_for(x, y)]; }
Matrix(): n_cols_( 0 ) {}
Matrix( Size const n_cols, Size const n_rows )
: items_( n_cols*n_rows )
, n_cols_( n_cols )
{}
Matrix( initializer_list< initializer_list > const& values )
: items_()
, n_cols_( values.size() == 0? 0 : values.begin()->size() )
{
for( auto const& row : values )
{
assert( Size( row.size() ) == n_cols_);
items_.insert( items_.end(), row.begin(), row.end() );
}
}
};
}  // namespace my
//--------------------------------------------- Usage:
using my::Matrix;
auto some_matrix()
-> Matrix
{
return
{
{  1,  2,  3,  4,  5,  6,  7 },
{  8,  9, 10, 11, 12, 13, 14 },
{ 15, 16, 17, 18, 19, 20, 21 }
};
}
# include
# include
using namespace std;
auto main() -> int
{
Matrix const m = some_matrix();
assert( m.n_cols() == 7 );
assert( m.n_rows() == 3 );
for( int y = 0, y_end = m.n_rows(); y < y_end; ++y )
{
for( int x = 0, x_end = m.n_cols(); x < x_end; ++x )
{
cout <← Note: not `m[y][x]`!
}
cout <}
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
The above code is not industrial grade: it's designed to show the basic principles, and serve the needs of students
learning C++.
For example, one may deﬁne operator() overloads to simplify the indexing notation.

***

# MEMORY TYPES AND POINTERS

***

# CHAPTER 2: MEMORY TYPES AND POINTERS

### 1.1 The Address Space: The Map of Mem-City

Every computer program lives in a virtual "Address Space." Imagine this as a giant, infinite row of mailboxes, each with a unique number (the address).

#### The Layout of your Program in Memory

When your program starts, the Operating System divides Mem-City into several "Zoning Districts." Each district has its own rules, speed limits, and rent costs.

| District | The Zoning Rules | Who Lives Here? |
| :--- | :--- | :--- |
| **The Text Segment** | **Read-Only Park**: No one is allowed to change the ground. | Your compiled code (the machine instructions). It's fixed forever. |
| **The Data Segment** | **The Town Square**: Fixed-size statues that stay forever. | Global variables (`int g = 10;`) and `static` variables. |
| **The BSS Segment** | **The Empty Lot**: Reserved space for future statues. | Uninitialized global variables. The OS zero-initializes these lot for you. |
| **The Stack** | **The Quick-Start Desk**: A desk that grows and shrinks. | Local variables, function parameters, and the "Return Address" (how the CPU knows where to go back to after a function). |
| **The Heap** | **The Industrial Warehouse**: Massive space you rent by the square foot. | Dynamic memory (`new`, `malloc`). It's big, but you have to manage it. |

***

### Fireside Chat: Why Pointers Break Your Brain

**Student**: "I just don't get it. If I have a variable `int x = 10;`, why can't I just use `x`? Why do I need `int* p = &x;`?"

**The Architect**: "Think about a huge library. If you want to tell your friend about a great book, you have two choices.

1. You can photocopy every single page of the book and hand them the pile of paper (**Pass by Value**).
2. You can just hand them a slip of paper with the shelf location: 'Floor 2, Row 10, Shelf 4' (**Pass by Pointer/Reference**)."

**Student**: "Okay, the location is easier. But what if the librarian moves the book?"

**The Architect**: "That's exactly why Pointers are dangerous! If the book moves but you still have the old address, you're looking at an empty shelfor worse, a different book entirely. That's a **Dangling Pointer**."

***

### Step-by-Step: The Life of a Pointer

Let's trace a pointer's life in the CPU registers and RAM.

```cpp
int main() {
    int secret_number = 42;    // 1. Build a house
    int* spy = &secret_number; // 2. Write down the address
    *spy = 100;                // 3. Go to the address and change the contents
}
```

1. **Step 1**: The CPU asks the OS for 4 bytes on the **Stack**. The OS gives it address `0x1000`. The CPU writes the bits for `42` into that location.
2. **Step 2**: The CPU asks for another 8 bytes (on a 64-bit system) for the pointer `spy`. It stores the value `0x1000` into this new house.
3. **Step 3**: The CPU looks at the value in `spy` (`0x1000`), jumps to that location in RAM, and overwrites the `42` with `100`.

***

### 1.2 Common Pointer "Street Gangs" (Traps)

| The Trap | What it is | How to avoid it |
| :--- | :--- | :--- |
| **The Ghost (Wild Pointer)** | A pointer that was never initialized. It's pointing at a random house in the city. | Always initialize to `nullptr`. |
| **The Zombie (Dangling Pointer)** | You deleted the house, but you still have the address. | Set to `nullptr` immediately after `delete`. |
| **The Squatter (Memory Leak)** | You rented a warehouse locker, threw away the key, and never returned it. | Use **Smart Pointers** (RAII). |

***

### Deep Dive: Pointer Arithmetic (Walking the Streets)

Pointers are just numbers (addresses), so you can add or subtract from them. But C++ is smartit knows the "size" of the houses.

* If you have an `int* p` pointing at address `100`, and you do `p++`, it doesn't go to `101`.
* It jumps to `104` (because an `int` is 4 bytes).

**It's like walking down a street where every house is exactly 4 meters wide. Taking one step forward always puts you at the front door of the next neighbor.**

***

***

# Chapter 5: Arrays, Pointers, and References

> *Handing over the keys to the city.*

Welcome to the heart of C++. Most modern languages—like Java, Python, or C#—try to hide memory from you. They handle the allocation, the cleanup, and the addresses automatically.

C++ does not hide the memory. C++ hands you the keys to the city and says, *"Don't burn it down."*

This level of control is exactly why C++ is the language of choice for game engines, operating systems, and high-frequency trading. It is also the reason why C++ is famous for crashing spectacularly if you don't know what you are doing.

To master C++, you must stop thinking about variables as abstract concepts and start thinking about them as physical locations inside a silicon chip.

***

## 5.1 🛋️ Fireside Chat: The Memory City Analogy

Imagine your computer's RAM (Random Access Memory) is a giant metropolis called **Mem-City**.

1. **Memory Addresses**: Every house in Mem-City has a unique street address (e.g., `0x7ffee6b5a`).
2. **Variables**: A variable is just a **House**. When you type `int x = 5;`, the Mayor (the Operating System) builds a house, puts the number `5` inside it, and hangs a sign on the door that says `"x"`.
3. **Pointers**: A pointer is a **GPS Device**. It doesn't hold a value like `5`; it holds the *Street Address* of a house.

### Why do we care?

If you want to give a massive 10-Gigabyte 3D model to a function in Python, the language has to do a lot of background magic. In C++, if you want a function to look at your 3D model, you don't copy the model. You just hand the function the GPS coordinates (a Pointer). It is lightning fast.

## 5.2 The Stack vs. The Heap

Mem-City is divided into two main districts. Where your variable lives determines how fast it is, and who is responsible for tearing down the house when you are done.

| District | Analogy: The Work Space | Lifetime | Speed |
| :--- | :--- | :--- | :--- |
| **The Stack** | **Your Office Desk**: You put things on it as you need them. When you leave the office (the function ends), the cleaning crew automatically wipes the desk perfectly clean. | Automatic (ends at `}`) | **Ultra Fast**. Just like grabbing a pen right in front of you. |
| **The Heap** | **The Industrial Warehouse**: A giant storage facility across town. You call the Manager to rent a locker. *You* must remember to return the key when you are done. | Manual (Until you call `delete`) | **Slower**. You have to travel to the warehouse and talk to the manager. |

If you forget to return the key to your warehouse locker, you get a **Memory Leak**. The locker stays "rented" forever. If you do this in a loop, Mem-City runs out of space, and your game crashes.

## 5.3 C-Style Arrays and Decay

Before we look at pointers, we must look at arrays. A raw C-style array is simply a block of houses built right next to each other on the same street.

```cpp
int scores[5] = {10, 20, 30, 40, 50};

std::cout << scores[0]; // Prints 10
std::cout << scores[4]; // Prints 50
```

> [!WARNING]
> **⚠️ The Danger Zone: Array Bounds**
> C++ does absolutely **zero bounds checking**. If you ask for `scores[100]`, C++ won't stop you. It will just walk 100 houses down the street, break into whoever lives there, and read their data. This causes **Undefined Behavior**.

**Array Decay:**
When you pass an array to a function, C++ doesn't copy the whole array. The array instantly "decays" into a pointer to its first element.
If you type `std::cout << scores;`, it won't print "10 20 30". It will print something like `0x7ffeb3a`, the memory address of the first house.

## 5.4 What is a Pointer? (`*` and `&`)

To work with memory addresses directly, we need two new operators.

* **`&` (Address-Of Operator)**: "Tell me the address of this variable."
* **`*` (Dereference Operator)**: "Go to this address and look inside."

```cpp
int secret_number = 42;           // Build a house. Put 42 inside.
int* spy = &secret_number;        // Get the address of the house. Store it in a pointer.

std::cout << spy << "\n";         // Prints the address (e.g., 0x1000)
std::cout << *spy << "\n";        // Dereferences the address. Prints 42.

*spy = 100;                       // Go to the address and overwrite the contents.
std::cout << secret_number;       // Prints 100!
```

## 5.5 Pointer Arithmetic (Walking the Streets)

Because pointers are just numbers (addresses), you can add or subtract from them.
But C++ is smart—it knows how wide the houses are.

```cpp
int arr[3] = {10, 20, 30};
int* p = arr; // p points to the 10

p++; // Steps forward to the NEXT element
std::cout << *p; // Prints 20
```

If an `int` is 4 bytes wide, `p++` doesn't add 1 to the memory address; it adds 4. Taking one step forward always puts you at the front door of the next neighbor.

## 5.6 The Traps: Dangling, Wild, and Leaks

Pointers are the number one cause of bugs in C++. Here are the street gangs of Mem-City:

1. **The Ghost (Wild Pointer)**: A pointer you declared but didn't initialize. `int* p;`. It is pointing at a random, unpredictable house in the city. Dereferencing it will instantly crash your program. Always initialize pointers to `nullptr`!
2. **The Zombie (Dangling Pointer)**: You deleted the memory, but you kept the address. If you try to visit the house later, it might have been bulldozed and replaced by a different program.
3. **The Squatter (Memory Leak)**: You rented heap memory but lost the pointer to it before calling `delete`. That memory is gone forever until the program restarts.

## 5.7 References `&` vs Pointers `*`

Because Pointers are so dangerous, C++ introduced **References**.
A reference is just an alias—a second name for an existing variable.

```cpp
int original = 100;
int& alias = original; // 'alias' is now permanently bound to 'original'

alias = 500;
std::cout << original; // Prints 500
```

| Feature | Reference (`&`) | Pointer (`*`) |
| :--- | :--- | :--- |
| **Initialization** | MUST be initialized immediately. | Can be initialized later. |
| **Re-seating** | Cannot be changed to point to something else. | Can be pointed to a new address at any time. |
| **Null Safety** | Cannot be null (guaranteed to be safe). | Can be `nullptr` (must check before using). |

**Rule of Thumb:** Use References (`&`) everywhere you possibly can. Use Pointers (`*`) only when you absolutely must (like when building dynamic data structures like Linked Lists).

## 5.8 Dynamic Memory: `new` and `delete`

If you don't know how much memory you need until the program is actually running (e.g., asking the user how many enemies to spawn), you cannot use the Stack. You must use the Heap.

```cpp
// 1. Rent space on the Heap
int* player_score = new int; 
*player_score = 999;

// 2. Return the space when done!
delete player_score; 
player_score = nullptr; // Good practice to prevent Zombie pointers
```

**Allocating Arrays on the Heap:**

```cpp
int num_enemies = 100;
int* enemies = new int[num_enemies]; // Rent 100 integers

delete[] enemies; // Notice the []. You must use this to delete arrays!
```

## 5.9 `const` with Pointers

Mixing `const` with pointers creates a notorious syntax puzzle. Read it backwards (from right to left).

```cpp
int x = 10;
int y = 20;

// 1. Pointer to a Const Int
const int* p1 = &x; 
// *p1 = 50; // ERROR: You cannot change the value through this pointer.
p1 = &y;     // OK: You CAN point it at a different house.

// 2. Const Pointer to an Int
int* const p2 = &x;
*p2 = 50;    // OK: You CAN change the value inside the house.
// p2 = &y;  // ERROR: You cannot point it at a different house.

// 3. Const Pointer to a Const Int
const int* const p3 = &x;
// *p3 = 50; // ERROR
// p3 = &y;  // ERROR
```

***

You now understand the fabric of the Matrix. You can allocate memory, navigate addresses, and manipulate data exactly how the CPU sees it. In the next chapter, we will look at how C++ handles text—a concept that is surprisingly complex when you are working directly with memory arrays.
