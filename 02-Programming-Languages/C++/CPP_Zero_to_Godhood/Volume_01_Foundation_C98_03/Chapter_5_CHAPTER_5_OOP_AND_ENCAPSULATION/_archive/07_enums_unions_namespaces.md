# Chapter 7: Enumerations, Unions, and Namespaces

> *Organizing data and preventing chaos.*

As your programs grow from a few dozen lines to thousands of lines, you will run into organizational problems. How do you represent a specific set of states (like "Menu", "Playing", "Game Over") without just using random integers? How do you save memory when a variable could be one of three different types? And how do you ensure that your `Player` class doesn't conflict with a `Player` class from an external audio library?

This chapter covers the three primary tools for organizing data types and scope.

---

## 7.1 Enumerations (`enum`)

When you need a variable to represent a specific, limited set of states, you should use an **Enumeration**. 

### The Old Way: C-Style Enums
In older C++, you would define an enum like this:

```cpp
enum GameState {
    MENU,       // Automatically assigned 0
    PLAYING,    // Automatically assigned 1
    PAUSED,     // Automatically assigned 2
    GAME_OVER   // Automatically assigned 3
};

GameState current = PLAYING;

if (current == 1) {
    // This is valid, but terrible practice!
    std::cout << "We are playing.\n";
}
```

> [!WARNING]
> **⚠️ The Danger Zone: Global Leakage**
> C-style enums are notoriously leaky. The names `MENU`, `PLAYING`, etc., leak out into the surrounding scope. If you try to create another enum later like `enum VideoState { PLAYING, STOPPED };`, the compiler will throw a massive error because the word `PLAYING` has already been taken by `GameState`. Furthermore, C-style enums will implicitly convert to integers, defeating the purpose of strict typing.

### The Modern Way: Scoped Enums (`enum class`) `[C++11]`
C++11 fixed these issues by introducing scoped enumerations. You add the word `class` (or `struct`).

```cpp
enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver
};

// You MUST use the scope resolution operator ::
GameState current = GameState::Playing;

// if (current == 1) // ERROR! Will not compile. Strict type safety.

if (current == GameState::Playing) {
    std::cout << "We are playing.\n";
}
```
Always use `enum class`. It guarantees that your names stay contained and prevents accidental math operations on your game states.

## 7.2 Unions — Shared Memory Layout

A `union` is a special data structure where all members share the *exact same memory location*. 

```cpp
union PacketData {
    int as_integer;
    float as_float;
    char as_bytes[4];
};

int main() {
    PacketData packet;
    
    // The size of this union is 4 bytes (the size of the largest member)
    std::cout << "Size: " << sizeof(packet) << "\n"; 
    
    packet.as_integer = 42;
    std::cout << packet.as_integer << "\n"; // Prints 42
    
    packet.as_float = 3.14f; 
    // WARNING: 'as_integer' has just been completely overwritten!
    std::cout << packet.as_integer << "\n"; // Prints pure garbage data
}
```

Unions are heavily used in low-level systems programming (like network drivers or embedded microcontrollers) where you need to interpret the exact same 4 bytes of memory as an integer sometimes, and as a float other times, without copying the data. 

However, they are highly dangerous. The compiler does not know which type is currently "active" inside the union.

## 7.3 `std::variant` — The Safe Union `[C++17]`

To solve the safety issues of raw Unions, C++17 introduced `<variant>`. A `std::variant` is a "type-safe union." It remembers exactly which type it is currently holding.

```cpp
#include <iostream>
#include <variant>
#include <string>

int main() {
    // This variable can hold an int, a float, OR a string.
    std::variant<int, float, std::string> data;

    data = 42;
    std::cout << std::get<int>(data) << "\n"; // Prints 42

    data = "Hello";
    std::cout << std::get<std::string>(data) << "\n"; // Prints Hello

    // std::cout << std::get<int>(data); 
    // CRASH! It safely throws an exception because it currently holds a string!
}
```

If you are building modern software, use `std::variant`. Reserve raw `union`s strictly for hardware-level bit manipulation.

## 7.4 Namespaces

As you pull in third-party libraries (a graphics engine, an audio library, a physics engine), you will inevitably run into **Naming Collisions**. What happens if both the graphics library and the physics library have a class named `Vector3D`?

C++ solves this with `namespace`. A namespace is a named declarative region that provides a scope to the identifiers inside it.

```cpp
namespace Physics {
    class Vector3D { /* ... */ };
    void calculate_gravity();
}

namespace Graphics {
    class Vector3D { /* ... */ };
    void draw();
}

int main() {
    // We use the scope resolution operator (::) to specify which one we want
    Physics::Vector3D gravity_vector;
    Graphics::Vector3D render_vector;
    
    Physics::calculate_gravity();
}
```

### The `using` Directive
You have been typing `std::cout` and `std::string`. The `std::` simply means those tools live inside the "Standard" namespace.

You can tell the compiler to automatically look inside a namespace so you don't have to type it every time:

```cpp
using namespace std; // Pulls EVERYTHING from 'std' into the global scope
```

> [!CAUTION]
> **⚠️ The Danger Zone: `using namespace std;` in Headers**
> While it is fine to write `using namespace std;` in a `.cpp` file for a small homework assignment, **NEVER** put it in a header (`.h`) file. 
> 
> If you put it in a header, every single file that `#include`s your header will violently be forced to dump the entire standard library into their global namespace, causing massive naming collisions and ruining the compilation of massive codebases.

## 7.5 Anonymous Namespaces

Sometimes you write a helper function in a `.cpp` file, and you want to guarantee that no other file in the entire program can accidentally call it or collide with its name. 

You can use an **Anonymous Namespace**:

```cpp
// math_helpers.cpp

namespace {
    // This function is invisible to every other file in the program!
    int secret_internal_calculation(int x) {
        return x * x;
    }
}

int public_math_function(int x) {
    return secret_internal_calculation(x);
}
```

This is the modern C++ equivalent of the C-style `static` function. It is heavily used by Senior Engineers to encapsulate internal logic and prevent the global namespace from becoming polluted.

---

With your data states and naming scopes organized, you are finally ready to bundle data and functions together into single entities. In the next chapter, we enter the world of Object-Oriented Programming (OOP) with Classes.
