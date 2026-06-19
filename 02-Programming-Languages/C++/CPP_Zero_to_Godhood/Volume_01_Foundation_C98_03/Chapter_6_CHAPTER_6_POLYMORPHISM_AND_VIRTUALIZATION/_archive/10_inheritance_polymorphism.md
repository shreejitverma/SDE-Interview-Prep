# Chapter 10: Inheritance and Polymorphism

> *Building upon the work of others.*

You have learned how to create a Blueprint (a Class). But what if you want to build a `SportsCar`? A Sports Car is just a `Car`, but with a bigger engine and a spoiler. 

Do you copy and paste the entire `Car` blueprint, rename it `SportsCar`, and add the new features? Absolutely not. Copy-pasting code leads to unmaintainable nightmares. If you find a bug in the brakes of the `Car`, you would have to remember to fix it in the `SportsCar` blueprint too.

Instead, C++ allows you to say: *"A SportsCar is exactly like a Car, plus these few extra things."*

This is called **Inheritance**.

---

## 10.1 "Is-A" Relationships (Base and Derived Classes)

Inheritance models an "Is-A" relationship. A Dog *is an* Animal. A Sword *is a* Weapon.

```cpp
#include <iostream>

// 1. The Base Class (The Parent)
class Animal {
public:
    void eat() {
        std::cout << "Eating food...\n";
    }
};

// 2. The Derived Class (The Child)
class Dog : public Animal { // "Dog inherits from Animal"
public:
    void bark() {
        std::cout << "Woof!\n";
    }
};

int main() {
    Dog my_dog;
    my_dog.bark(); // Its own method
    my_dog.eat();  // Inherited from Animal!
}
```

Because a `Dog` *is an* `Animal`, you can use a `Dog` anywhere an `Animal` is expected.

```cpp
void feed_animal(Animal* a) {
    a->eat();
}

int main() {
    Dog* my_dog = new Dog();
    feed_animal(my_dog); // Valid! A Dog is an Animal.
    delete my_dog;
}
```

## 10.2 The `protected` Access Modifier

You already know `public` (everyone can touch) and `private` (only the class itself can touch). 

What if the `Animal` class has a `weight` variable? If it is `private`, the `Dog` class cannot touch it, even though a `Dog` *is an* `Animal`.

This is where `protected` comes in.

*   `protected`: Private to the outside world, but fully accessible to any Derived classes.

```cpp
class Animal {
protected:
    int weight; // Accessible to Animal and Dog. Hidden from main().
};

class Dog : public Animal {
public:
    void grow() {
        weight += 5; // Dog is allowed to touch protected data from Animal
    }
};
```

## 10.3 Polymorphism and the `virtual` Keyword

"Polymorphism" comes from Greek, meaning "many forms." In programming, it means the ability to call the *same function* on different objects and have each object respond in its own specific way.

If you have an array of `Animal*` pointers (some pointing to Dogs, some to Cats, some to Birds), and you tell them all to `speak()`, you want the Dog to bark, the Cat to meow, and the Bird to chirp.

To do this, the Base class must declare the function as `virtual`. This tells the C++ compiler: *"If a child class has their own version of this function, use theirs instead of mine."*

```cpp
class Animal {
public:
    // The 'virtual' keyword enables Polymorphism
    virtual void speak() {
        std::cout << "...\n";
    }
};

class Dog : public Animal {
public:
    void speak() override { // 'override' is C++11, ensuring we typed it correctly
        std::cout << "Woof!\n";
    }
};

class Cat : public Animal {
public:
    void speak() override {
        std::cout << "Meow!\n";
    }
};

int main() {
    Animal* my_pet = new Dog();
    
    // Because it is virtual, it knows it's actually a Dog!
    my_pet->speak(); // Prints "Woof!"
    
    delete my_pet;
}
```

## 10.4 🧠 Brain Power: The vTable (How it Actually Works)

How does `my_pet->speak()` know to print "Woof!" when `my_pet` is just an `Animal*` pointer? 

When you use the `virtual` keyword, C++ secretly adds a hidden pointer to your class. This pointer points to a **Virtual Table (vTable)**. 
1.  The vTable is a secret array of function pointers.
2.  When you build a `Dog`, the `Dog`'s vTable points to `Dog::speak()`.
3.  When you call `my_pet->speak()`, the program looks at the object in memory, follows its hidden pointer to the vTable, and executes whatever function is listed there.

This is called **Dynamic Dispatch**. It is incredibly powerful, but it has a tiny performance cost (following an extra pointer). In high-performance game loops, calling thousands of virtual functions per frame can cause cache misses.

## 10.5 Abstract Classes and Interfaces (`= 0`)

Sometimes, the Base class shouldn't actually have a function implementation. What is the default `speak()` sound for a generic `Animal`? It doesn't make sense.

You can force all child classes to provide their own implementation by making the function **Pure Virtual**. You do this by putting `= 0` at the end of the declaration.

```cpp
class Weapon {
public:
    // Pure Virtual Function.
    virtual void attack() = 0; 
};

class Sword : public Weapon {
public:
    void attack() override { std::cout << "Swing!\n"; }
};

int main() {
    // Weapon w; // ERROR! Cannot build an abstract concept.
    Weapon* my_weapon = new Sword(); // OK!
    my_weapon->attack();
}
```

If a class has even one Pure Virtual Function, the entire class becomes **Abstract**. You cannot instantiate it. It exists solely to act as an **Interface** for other classes to inherit from.

## 10.6 The Virtual Destructor Trap

This is one of the most famous bugs in C++. Look closely at this code:

```cpp
class Base {
public:
    ~Base() { std::cout << "Base destroyed.\n"; }
};

class Derived : public Base {
    int* array;
public:
    Derived() { array = new int[100]; }
    ~Derived() { delete[] array; std::cout << "Derived destroyed.\n"; }
};

int main() {
    Base* b = new Derived();
    delete b; 
}
```
**Output:**
```text
Base destroyed.
```

Notice what happened? The `Derived` destructor was **never called**! The integer array is leaked into Mem-City forever!

Because the pointer `b` is of type `Base*`, and the `Base` destructor is NOT `virtual`, the compiler just statically destroys the `Base` part of the object and stops. 

> [!CAUTION]
> **⚠️ The Golden Rule of Inheritance**
> If your class is designed to be inherited from (if it has even one `virtual` function), you **MUST** give it a `virtual` destructor. 

```cpp
class Base {
public:
    virtual ~Base() { std::cout << "Base destroyed.\n"; }
};
```
Now, `delete b;` will correctly look at the vTable, call `~Derived()` first, and then automatically call `~Base()`.

---

You now possess the tools to build massive, hierarchical software architectures. But what if you have a `double` and you need an `int`? What if you have a `Base*` and you need to force it back into a `Derived*`? In the next chapter, we will master the art of Type Conversions and Casting.
