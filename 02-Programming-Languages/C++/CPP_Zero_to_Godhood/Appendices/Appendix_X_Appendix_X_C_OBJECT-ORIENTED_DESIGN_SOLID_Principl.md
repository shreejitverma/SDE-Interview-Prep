# Appendix X: C++ OBJECT-ORIENTED DESIGN (SOLID Principles)


When you write a 1,000-line program, you can keep the whole thing in your head. When you write a 1,000,000-line program, you need rules. The SOLID principles are the golden rules of Object-Oriented Architecture.

### 1. Single Responsibility Principle (SRP)
**"A class should have one, and only one, reason to change."**

**The Analogy**: The Swiss Army Knife vs The Chef's Knife.
A Swiss Army Knife is great for camping, but you wouldn't use it to prep a 5-star meal. If the scissors break, the whole tool is compromised. 

**Bad C++**:
```cpp
class UserProfile {
public:
    void update_email(std::string email) { ... }
    void save_to_database() { ... } // BAD! Database logic mixed with User logic!
    void print_to_html() { ... }    // BAD! UI logic mixed with User logic!
};
```
If the database changes from MySQL to MongoDB, the `UserProfile` class has to be rewritten. 

**Good C++**:
```cpp
class UserProfile { ... }; // Only holds user data
class UserRepository { void save(UserProfile& u); }; // Handles database
class UserView { void render(UserProfile& u); }; // Handles UI
```

### 2. Open-Closed Principle (OCP)
**"Software entities should be open for extension, but closed for modification."**

**The Analogy**: The USB Port.
When Apple wants to support a new type of printer, they don't open up the Mac and solder new wires. They just ask the printer manufacturer to build a USB plug. The Mac is *closed* to internal modification, but *open* to extension via the USB interface.

**Bad C++**:
```cpp
class PaymentProcessor {
public:
    void process(Order o, string type) {
        if (type == "CreditCard") { /* ... */ }
        else if (type == "PayPal") { /* ... */ }
        // If we add Bitcoin, we have to modify this core class!
    }
};
```

**Good C++ (Using Interfaces/Virtual Functions)**:
```cpp
class IPaymentMethod {
    virtual void pay(Order o) = 0;
};
class CreditCard : public IPaymentMethod { ... };
class PayPal : public IPaymentMethod { ... };

class PaymentProcessor {
public:
    void process(Order o, IPaymentMethod& method) {
        method.pay(o); // Never changes, even if we add Bitcoin!
    }
};
```

### 3. Liskov Substitution Principle (LSP)
**"Derived classes must be substitutable for their base classes without breaking the program."**

**The Analogy**: The Toy Duck.
If it looks like a duck and quacks like a duck, but needs batteries, you probably have the wrong abstraction. If I write a function that takes a `Duck&`, and you pass me a `ToyDuck`, my code will break when I try to feed it bread.

**Bad C++**:
```cpp
class Rectangle {
public:
    virtual void set_width(int w) { width = w; }
    virtual void set_height(int h) { height = h; }
};

class Square : public Rectangle {
public:
    // A square must have equal sides, so we hack the base class!
    void set_width(int w) override { width = w; height = w; }
    void set_height(int h) override { width = h; height = h; }
};

void resize_box(Rectangle& r) {
    r.set_width(5);
    r.set_height(4);
    assert(r.area() == 20); // CRASHES IF YOU PASS A SQUARE!
}
```
**The Fix**: A Square is mathematically a Rectangle, but in software behavior, it is NOT. Do not use inheritance here.

### 4. Interface Segregation Principle (ISP)
**"Many client-specific interfaces are better than one general-purpose interface."**

**The Analogy**: The All-In-One Remote.
Imagine a remote with 500 buttons that controls the TV, the microwave, and the car. You give it to your grandma just to change the channel, and she accidentally opens the garage door.

**Bad C++**:
```cpp
class IMachine {
    virtual void print() = 0;
    virtual void fax() = 0;
    virtual void scan() = 0;
};

class SimplePrinter : public IMachine {
    void print() override { ... }
    void fax() override { throw NotSupported(); } // FORCED to implement this
    void scan() override { throw NotSupported(); }
};
```

**Good C++**:
```cpp
class IPrinter { virtual void print() = 0; };
class IFax { virtual void fax() = 0; };

class SimplePrinter : public IPrinter { ... };
class SuperCopier : public IPrinter, public IFax { ... };
```

### 5. Dependency Inversion Principle (DIP)
**"High-level modules should not depend on low-level modules. Both should depend on abstractions."**

**The Analogy**: The Wall Outlet.
Your lamp (high-level) doesn't have the wires soldered directly into the city power grid (low-level). Both the lamp and the power grid agree on an abstraction: The 120V Wall Outlet.

**Good C++**:
We already saw this in Chapter 94 (Clean Architecture). By using Abstract Base Classes (or C++20 Concepts), we invert the dependency. The database relies on the Interface defined by the core logic, rather than the core logic relying on the database.

---
