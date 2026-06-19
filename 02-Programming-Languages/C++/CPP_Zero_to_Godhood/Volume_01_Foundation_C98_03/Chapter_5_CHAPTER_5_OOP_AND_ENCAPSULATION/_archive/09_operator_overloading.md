# Chapter 9: Operator Overloading

> *Teaching your objects how to do math.*

In most programming languages, you can add two integers together using the `+` operator. If you want to add two custom objects together—say, two `Vector2D` math objects—you usually have to write a clunky function:

```java
// Java or older languages
Vector2D result = v1.add(v2);
```

C++ believes that user-defined types (Classes) should look and feel exactly like built-in types (like `int` or `float`). If a `Vector2D` is a mathematical concept, you should be able to do math with it.

```cpp
// C++ Operator Overloading
Vector2D result = v1 + v2;
```

This is called **Operator Overloading**. It allows you to redefine how C++ operators (`+`, `-`, `*`, `==`, `<<`) behave when applied to your custom classes.

---

## 9.1 Giving Syntax to Objects

Let's build a `Complex` number class (a number with a real part and an imaginary part) and teach it how to add.

An overloaded operator is just a regular function with a special name: `operator` followed by the symbol you want to overload.

```cpp
class Complex {
private:
    double r; // Real part
    double i; // Imaginary part

public:
    Complex(double real, double imag) : r(real), i(imag) {}

    // Overloading the '+' operator
    Complex operator+(const Complex& other) const {
        // Add the real parts together, and the imaginary parts together
        return Complex(r + other.r, i + other.i);
    }
    
    void print() const {
        std::cout << r << " + " << i << "i\n";
    }
};

int main() {
    Complex c1(1.0, 2.0);
    Complex c2(3.0, 4.0);
    
    // The compiler sees this: c1.operator+(c2)
    Complex c3 = c1 + c2; 
    
    c3.print(); // Prints: 4.0 + 6.0i
}
```

> [!TIP]
> **Why `const Complex&`?**
> We pass `other` by `const` reference because we don't want to copy the entire object just to read its values, and we promise not to change `c2` while adding it to `c1`. The function itself is marked `const` at the end because adding two numbers shouldn't change `c1` either; it just creates a brand new `c3`.

## 9.2 Member vs. Non-Member Functions

What if we want to add a `Complex` number and a regular `double`?

```cpp
Complex c1(1.0, 2.0);
Complex result = c1 + 5.0; // This works! (c1.operator+(5.0))
```

But what if we reverse it?

```cpp
Complex result = 5.0 + c1; // ERROR!
```

This fails because `5.0` is a built-in `double`. It does not have an `operator+` that takes a `Complex` object as an argument. The left side of the `+` dictates who owns the function.

To fix this, we must overload the operator as a **Non-Member Function** (a free-floating function outside the class). If it needs to access `private` data, we make it a `friend`.

```cpp
class Complex {
    double r, i;
public:
    Complex(double r, double i) : r(r), i(i) {}

    // Friend declaration (Non-member)
    friend Complex operator+(double left, const Complex& right);
};

// Implementation outside the class
Complex operator+(double left, const Complex& right) {
    return Complex(left + right.r, right.i);
}
```

## 9.3 Comparison Operators (`==`, `<`)

If you want to sort a list of custom objects (like an array of `Player`s), C++ needs to know how to compare them. Which player is "less than" another player?

```cpp
class Player {
public:
    int score;
    
    Player(int s) : score(s) {}

    // Teach C++ how to check for equality
    bool operator==(const Player& other) const {
        return score == other.score;
    }

    // Teach C++ how to check if one is smaller (useful for sorting!)
    bool operator<(const Player& other) const {
        return score < other.score;
    }
};
```

## 9.4 Overloading the I/O Operators (`<<`, `>>`)

Have you ever tried to print an object directly to `std::cout`?

```cpp
Player p(100);
std::cout << p; // ERROR! cout doesn't know what a Player is.
```

The `<<` symbol is actually an operator (the bitwise left-shift operator) that the C++ standard library overloaded to mean "send to stream". We can overload it again to teach `cout` how to print our `Player`.

Because the left side of the operator is `std::cout` (an `ostream` object), this *must* be a non-member friend function.

```cpp
#include <iostream>

class Player {
private:
    std::string name;
    int score;

public:
    Player(std::string n, int s) : name(n), score(s) {}

    // 1. Return a reference to the ostream
    // 2. Take the ostream and the Object as parameters
    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << "[" << p.name << " - Score: " << p.score << "]";
        return os; // Return the stream so we can chain: cout << p1 << p2;
    }
};

int main() {
    Player p1("Alice", 99);
    std::cout << p1 << "\n"; // Prints: [Alice - Score: 99]
}
```

## 9.5 The Assignment Operator (`=`) and Self-Assignment

As discussed in the Rule of Three, if your class manages memory, you must overload the assignment operator (`=`).

When someone types `a = b;`, you must clean up `a`'s old memory and copy `b`'s memory.

> [!WARNING]
> **⚠️ The Danger Zone: Self-Assignment**
> What happens if a programmer writes `a = a;`?
> If your assignment operator deletes its own memory first, it will delete `a`'s memory. Then, when it tries to copy `a`'s data to the new memory, the data is already gone! 
> You **must** check for self-assignment.

```cpp
class Buffer {
    int* data;
public:
    Buffer() { data = new int[10]; }
    ~Buffer() { delete[] data; }

    // Overload Assignment
    Buffer& operator=(const Buffer& other) {
        // 1. Check for self-assignment!
        if (this == &other) {
            return *this; // Do nothing, just return myself.
        }

        // 2. Clean up old data
        delete[] data;

        // 3. Allocate new memory and copy
        data = new int[10];
        for (int i = 0; i < 10; ++i) {
            data[i] = other.data[i];
        }

        // 4. Return myself
        return *this;
    }
};
```

## 9.6 Functors: Overloading `operator()`

Finally, C++ allows you to overload the parentheses `()`. This allows you to create an object that can be "called" exactly like a function. We call these objects **Functors** (Function Objects).

```cpp
class Multiplier {
private:
    int factor;
public:
    Multiplier(int f) : factor(f) {}

    // Overload the call operator
    int operator()(int value) const {
        return value * factor;
    }
};

int main() {
    Multiplier times_five(5); // Create an object
    
    int result = times_five(10); // Call the object like a function!
    std::cout << result; // Prints 50
}
```

Functors are extremely powerful because, unlike regular functions, they have "state" (like the `factor` variable). We will use them heavily when we explore the Standard Template Library (STL).

---

By overloading operators, you make your classes feel like natural extensions of the C++ language itself. In the next chapter, we will take our Blueprints to the next level by learning how to build new Blueprints based on existing ones through Inheritance.
