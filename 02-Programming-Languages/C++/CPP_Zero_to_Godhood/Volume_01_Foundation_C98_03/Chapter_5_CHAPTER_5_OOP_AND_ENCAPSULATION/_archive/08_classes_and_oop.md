# Chapter 8: Classes and OOP

> *Building your own types.*

Welcome to the world of objects. Up until now, we have been writing "Procedural" code—essentially a long list of instructions for the computer to follow. We used built-in types like `int`, `float`, and `char`. 

But what if you are building a game and need a `Player` type? A player isn't just an integer. A player has a name (string), health (int), and an inventory (array). More importantly, a player has *behaviors*—they can jump, take damage, and heal.

Object-Oriented Programming (OOP) allows you to bind data and behavior together into a single, cohesive unit.

---

## 8.1 🛋️ Fireside Chat: The Blueprint vs. The House

To understand OOP, you must understand the difference between a **Class** and an **Object**.

Think of a **Class** as a **Blueprint** for a house. 
*   The blueprint isn't a house. You can't live in it, and it doesn't take up any physical space on the street (in Mem-City). 
*   It simply describes *what* a house should have (3 bedrooms, 2 bathrooms) and *what* it can do (open doors, turn on lights).

An **Object** (also called an **Instance**) is the actual **House** built from that blueprint. 
*   You can build 1,000 houses from a single blueprint. 
*   Each house has its own unique address in memory, and each house can have different colored walls (data).

```cpp
// The Blueprint (Class)
class Player {
public:
    int health;
    int ammo;
    
    void shoot() {
        ammo -= 1;
    }
};

int main() {
    // Building the Houses (Objects)
    Player player1; 
    Player player2;
    
    player1.ammo = 10;
    player2.ammo = 100;
    
    player1.shoot(); // Only player1 loses ammo!
}
```

## 8.2 Encapsulation: The Smart TV Analogy

Notice the word `public:` in the blueprint above? This relates to **Encapsulation**.

Why do we make data `private`? 

Imagine your Smart TV. It has complex wiring and high-voltage circuit boards inside. If the manufacturer left all those wires exposed on the outside, you might accidentally touch a capacitor and break the TV (or get electrocuted).

Instead, they **Encapsulate** the TV. They put all the dangerous, complex stuff inside a plastic shell and give you a **Remote Control**.

1.  **Private Data**: The circuit boards and wires. Only the TV itself (the class methods) is allowed to touch these.
2.  **Public Methods**: The Power button and Volume buttons (the Remote Control). These are the only things the user (the caller) is allowed to interact with.

```cpp
class BankAccount {
private:
    double balance; // Hidden data (The Circuit Board)

public:
    // The Remote Control
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount; // The class is allowed to touch private data
        }
    }

    double get_balance() const { 
        return balance; 
    }
};

int main() {
    BankAccount my_account;
    // my_account.balance = 1000000; // ERROR! You cannot touch the wires directly!
    my_account.deposit(500);         // You MUST use the remote control.
}
```
If you want to change the volume on a TV, you press the button. You don't care *how* the volume increases inside the TV, as long as it works. This separation of interface from implementation is called **Decoupling**.

> [!TIP]
> **What is the difference between `class` and `struct`?**
> In C++, the *only* technical difference is the default access modifier. In a `struct`, everything is `public` by default. In a `class`, everything is `private` by default. 
> By convention, C++ developers use `struct` for simple data containers without complex behavior, and `class` when Encapsulation is required.

## 8.3 Constructors & Destructors

When a house is built, certain things need to happen immediately (like turning on the water). When a house is demolished, certain things must happen (like turning off the gas).

*   **Constructor**: A special function called automatically exactly once when the object is created. It has the same name as the class and no return type.
*   **Destructor**: A special function called automatically exactly once when the object is destroyed (e.g., when it goes out of scope). It is preceded by a tilde `~`.

```cpp
class Car {
private:
    std::string brand;

public:
    // Default Constructor
    Car() {
        brand = "Unknown";
        std::cout << "A generic car was built.\n";
    }

    // Parameterized Constructor
    Car(std::string b) {
        brand = b;
        std::cout << "A " << brand << " was built.\n";
    }

    // Destructor
    ~Car() {
        std::cout << "The " << brand << " was destroyed.\n";
    }
};

int main() {
    std::cout << "--- Start ---\n";
    {
        Car c1("Toyota"); // Calls Parameterized Constructor
    } // c1 goes out of scope. Destructor is called HERE.
    std::cout << "--- End ---\n";
}
```

## 8.4 The Rule of Three (C++98 Memory Management)

If your class rents memory on the Heap (using `new`), you are entering the danger zone. You must clean up that memory in the Destructor. 

But what happens if someone copies your object?

```cpp
class Buffer {
public:
    int* data;
    
    Buffer() { data = new int[100]; } // Rent memory
    ~Buffer() { delete[] data; }      // Return memory
};

int main() {
    Buffer b1;
    Buffer b2 = b1; // COPY!
} // CRASH!
```

When `b2` is created, C++ does a "shallow copy"—it copies the memory address. Both `b1` and `b2` now point to the *exact same locker* in the warehouse.
When `main()` ends, `b2`'s destructor deletes the locker. Then `b1`'s destructor runs and tries to delete the locker *again*. This is a "Double Free" error, and your program will instantly crash.

**The Rule of Three states:** If you need to manually define *any* of the following three functions, you almost certainly need to define *all three* to safely manage memory:
1.  **Destructor**: To free the memory.
2.  **Copy Constructor**: To intercept copies and rent a *new* locker for the new object (Deep Copy).
3.  **Copy Assignment Operator (`operator=`)**: To intercept assignments between two existing objects.

## 8.5 `static` Members

Sometimes, you want a piece of data to be shared by *all* houses built from the blueprint. For example, you might want to keep track of how many `Player` objects exist in the game.

If you make a member `static`, it doesn't live inside the individual houses. It lives inside the Blueprint itself.

```cpp
class Player {
public:
    static int player_count; // Shared by all players
    
    Player() { player_count++; }
    ~Player() { player_count--; }
};

// You must initialize static members outside the class in a .cpp file!
int Player::player_count = 0;

int main() {
    Player p1;
    Player p2;
    std::cout << Player::player_count; // Prints 2
}
```

## 8.6 `friend` Functions (Breaking the Rules)

Sometimes, Encapsulation gets in the way. What if two classes are heavily intertwined, and one needs to see the other's `private` circuitry to function efficiently?

C++ allows a class to declare another function or class as a `friend`. A friend is granted full access to all `private` and `protected` members.

```cpp
class SecretVault {
private:
    int password;

public:
    SecretVault() : password(42) {}
    
    // Declare an external function as a friend
    friend void lock_picker(SecretVault& v);
};

void lock_picker(SecretVault& v) {
    // This function can touch private data!
    std::cout << "The password is: " << v.password << "\n"; 
}
```

> [!WARNING]
> **Use Friends Sparingly**
> By making something a friend, you are bypassing the Remote Control and letting someone touch the wires directly. This violates Encapsulation. Use it only when absolutely necessary (like overloading the `<<` operator for printing classes).

---

You now know how to design Blueprints, manage their lifespans, and protect their data. But what if you want to build a `SportsCar` that inherits all the features of a `Car`, but adds a turbocharger? In the next chapter, we look at the crown jewels of OOP: Inheritance and Polymorphism.
