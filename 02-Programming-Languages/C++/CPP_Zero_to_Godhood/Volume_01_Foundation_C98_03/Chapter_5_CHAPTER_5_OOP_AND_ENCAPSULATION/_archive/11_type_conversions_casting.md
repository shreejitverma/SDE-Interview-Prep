# Chapter 11: Type Conversions and Casting

> *Forcing a square peg into a round hole.*

C++ is a strongly-typed language. An `int` is not a `float`, and a `Cat` is not a `Dog`. The compiler fiercely guards these boundaries to prevent you from doing things that would cause memory corruption or undefined behavior.

However, sometimes you *need* to break the rules. You might have a high-precision `double` that you need to pass to an older graphics API that only accepts an `int`. Or you might have an `Animal*` pointer, and you need to access a feature that only a `Dog` has.

To do this, you must "Cast" the variable from one type to another. 

---

## 11.1 Implicit Conversions (The Silent Killer)

Sometimes, C++ tries to be helpful and performs a cast for you automatically. This is called an **Implicit Conversion**.

```cpp
double pi = 3.14159;
int x = pi; // Implicitly converts double to int. x becomes 3!
```

While convenient, implicit conversions are a massive source of bugs, especially when passing arguments to functions.

```cpp
void take_damage(int amount);

// You accidentally pass a float. 
// The compiler silently chops off the decimal!
take_damage(45.9f); 
```

To prevent the compiler from implicitly converting your custom classes, you can use the `explicit` keyword on your constructors.

```cpp
class Vector {
public:
    // 'explicit' prevents: Vector v = 5;
    explicit Vector(int size) { ... } 
};
```

## 11.2 The C-Style Cast (Why it is Evil)

In older C code, if you wanted to force a conversion, you would just put the new type in parentheses in front of the variable.

```cpp
double pi = 3.14;
int x = (int)pi; // C-style cast
```

**Never do this in modern C++.**

Why? Because a C-style cast is a sledgehammer. It will try *every possible way* to force the conversion, even if it means doing something incredibly dangerous and unsafe (like turning a `float` into a pointer). It is also incredibly hard to search for in a massive codebase (searching for `(int)` will give you thousands of false positives).

To solve this, C++ introduced four highly specific, highly searchable Casting Operators.

## 11.3 `static_cast` (The Workhorse)

This is the cast you will use 90% of the time. It is a "compile-time" cast. It asks the compiler: *"Can you safely convert between these two types based on the rules you already know?"*

If the conversion is unsafe or impossible, the program will refuse to compile.

```cpp
double gravity = 9.81;

// 1. Standard conversions (truncates the decimal)
int g = static_cast<int>(gravity); 

// 2. Class Hierarchy Navigation (Upcasting)
Dog* my_dog = new Dog();
Animal* a = static_cast<Animal*>(my_dog); // Safe: A Dog is an Animal

// 3. Class Hierarchy Navigation (Downcasting - DANGEROUS!)
Animal* some_animal = new Cat();
Dog* d = static_cast<Dog*>(some_animal); 
// COMPILES, BUT DANGEROUS! some_animal is actually a Cat. 
// If you call d->bark(), your program will crash.
```

`static_cast` assumes you know exactly what you are doing. It does no runtime safety checks.

## 11.4 `dynamic_cast` (Safe Downcasting via RTTI)

If you have an `Animal*` pointer, and you don't actually know if it points to a `Dog` or a `Cat`, how do you safely convert it to a `Dog*`?

You use `dynamic_cast`.

`dynamic_cast` uses **Run-Time Type Information (RTTI)**. It looks at the actual object in memory while the program is running. If the cast is valid, it returns the pointer. If the cast is invalid (e.g., trying to turn a Cat into a Dog), it safely returns a `nullptr`.

```cpp
Animal* mystery_animal = get_random_animal();

// Ask the program at runtime: "Are you actually a Dog?"
Dog* d = dynamic_cast<Dog*>(mystery_animal);

if (d != nullptr) {
    std::cout << "It's a dog! Let it bark.\n";
    d->bark();
} else {
    std::cout << "Not a dog. Do nothing.\n";
}
```

> [!CAUTION]
> **Performance Warning**
> `dynamic_cast` is slow. It requires the program to traverse the class inheritance tree during execution. In performance-critical code (like a game engine rendering loop), you should design your architecture to avoid needing `dynamic_cast`.

## 11.5 `const_cast` (Breaking Constness)

What if you have a `const int*` (a pointer to an integer that you promised not to modify), but you *really* need to pass it to an old 3rd-party library function that only accepts a non-const `int*`?

`const_cast` allows you to strip away the `const` qualifier.

```cpp
void legacy_c_function(int* ptr) {
    // This old function promises not to modify ptr, 
    // but the original author forgot to write 'const'.
}

const int my_value = 100;
const int* p = &my_value;

// legacy_c_function(p); // ERROR! Cannot pass const to non-const.

// Strip the const away!
legacy_c_function(const_cast<int*>(p)); 
```

**Warning:** If the `legacy_c_function` actually *does* try to modify the value after you stripped the `const` away, your program will trigger Undefined Behavior and likely crash. Only use `const_cast` when interacting with poorly-written legacy APIs.

## 11.6 `reinterpret_cast` (Raw Memory Reinterpretation)

This is the most dangerous tool in C++. It tells the compiler: *"Take this exact sequence of 1s and 0s in memory, and pretend it is this other type."*

It does not convert values. It does not check inheritance trees. It just blindly reinterprets the bits.

```cpp
int original = 65; // 'A' in ASCII
int* p = &original;

// "Pretend this pointer to an integer is actually a pointer to a character."
char* c = reinterpret_cast<char*>(p);

std::cout << *c << "\n"; // Prints 'A'
```

`reinterpret_cast` is heavily used in networking (taking a raw stream of bytes from the internet and casting them into a structured `Packet` class) and game engines (custom memory allocators). 

If you use it wrong, you will destroy Mem-City.

---

You have now mastered the Core of C++ (Part II). You understand memory, pointers, classes, and how to safely navigate the type system. You are no longer an apprentice.

In Part III, we will step into the Modern Era. We will learn how C++11 revolutionized the language, completely eliminating the need for manual `delete` calls and memory leaks. Welcome to Resource Management.
