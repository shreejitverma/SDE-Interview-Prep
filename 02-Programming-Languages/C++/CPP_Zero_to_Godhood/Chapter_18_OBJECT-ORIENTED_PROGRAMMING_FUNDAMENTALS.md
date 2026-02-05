# OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS


## Classes & Objects

### Basic Class (C++98)

```cpp
#include <iostream>
#include <string>

class Person {
public:      // Publicly accessible
    std::string name;
    int age;
    
    void introduce() {
        std::cout << "I am " << name << ", age " << age << "\n";
    }
    
private:     // Private to class
    std::string secret;
    
    void hidden_method() {
        // Can't be called from outside
    }
    
protected:   // Protected (for inheritance)
    std::string protected_data;
};

int main() {
    Person p;
    p.name = "Alice";
    p.age = 30;
    p.introduce();
    
    // p.secret = "xyz";  // Error: private
    // p.hidden_method();  // Error: private
    
    return 0;
}
```

### Class Member Variables & Methods (C++98)

```cpp
#include <iostream>

class BankAccount {
private:
    double balance;
    std::string owner;
public:
    // Constructor
    BankAccount(const std::string& name, double initial) 
        : owner(name), balance(initial) {}
    
    // Methods
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        }
    }
    
    double get_balance() const {  // const = doesn't modify
        return balance;
    }
    
    std::string get_owner() const {
        return owner;
    }
};

int main() {
    BankAccount account("Alice", 1000);
    account.deposit(500);
    account.withdraw(200);
    
    std::cout << account.get_owner() << ": $" 
              << account.get_balance() << "\n";
    
    return 0;
}
```
## Classes & Objects - Complete Mastery

### What is a Class?

A **class** is a blueprint for creating objects. It defines:
- **Attributes** (member variables) - what the object has
- **Methods** (member functions) - what the object does

### What is an Object?

An **object** is an instance of a class - a concrete entity with specific values.

## The Four Pillars of OOP

Object-Oriented Programming is built on four fundamental concepts that distinguish it from procedural programming:

### 1. **Encapsulation** - Data Hiding
### 2. **Inheritance** - Code Reuse
### 3. **Polymorphism** - Flexible Behavior
### 4. **Abstraction** - Simplified Interface

```cpp
#include <iostream>
#include <string>
using namespace std;

// Class definition
class Car {
public:  // Public members accessible from outside
    // Member variables
    string brand;
    string color;
    int year;
    int speed;
    
    // Member functions
    void accelerate() {
        speed += 10;
        cout << brand << " accelerates to " << speed << " mph\n";
    }
    
    void brake() {
        if (speed >= 10) {
            speed -= 10;
        } else {
            speed = 0;
        }
        cout << brand << " brakes to " << speed << " mph\n";
    }
    
    void displayInfo() {
        cout << year << " " << color << " " << brand 
             << " at " << speed << " mph\n";
    }
};

int main() {
    // Creating objects (instances)
    Car car1;
    car1.brand = "Toyota";
    car1.color = "Red";
    car1.year = 2020;
    car1.speed = 0;
    
    Car car2;
    car2.brand = "BMW";
    car2.color = "Blue";
    car2.year = 2022;
    car2.speed = 0;
    
    // Using objects
    car1.displayInfo();
    car1.accelerate();
    car1.accelerate();
    car1.brake();
    
    car2.displayInfo();
    car2.accelerate();
    car2.accelerate();
    car2.accelerate();
    
    return 0;
}
```

**Output:**
```
2020 Red Toyota at 0 mph
Toyota accelerates to 10 mph
Toyota accelerates to 20 mph
Toyota brakes to 10 mph
2022 Blue BMW at 0 mph
BMW accelerates to 10 mph
BMW accelerates to 20 mph
BMW accelerates to 30 mph
```

---

## Encapsulation

**Encapsulation** is hiding internal details and exposing only what's necessary.

### Access Levels (Access Modifiers)

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
private:  // Only accessible within this class
    double balance;
    string accountNumber;
    
    // Private helper function
    bool validateAmount(double amount) {
        return amount > 0;
    }
    
public:  // Accessible from anywhere
    // Constructor
    BankAccount(string accNum, double initialBalance)
        : accountNumber(accNum), balance(initialBalance) {
        cout << "Account created: " << accountNumber << "\n";
    }
    
    // Public methods to access private data
    void deposit(double amount) {
        if (validateAmount(amount)) {
            balance += amount;
            cout << "Deposited: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Invalid deposit amount\n";
        }
    }
    
    void withdraw(double amount) {
        if (validateAmount(amount) && amount <= balance) {
            balance -= amount;
            cout << "Withdrew: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Cannot withdraw that amount\n";
        }
    }
    
    double getBalance() const {  // const = doesn't modify
        return balance;
    }
    
    string getAccountNumber() const {
        return accountNumber;
    }

protected:  // Accessible in this class and derived classes
    void logTransaction(string type) {
        cout << "Transaction (" << type << ") logged\n";
    }
};

int main() {
    BankAccount account("ACC123456", 1000);
    
    account.deposit(500);
    account.withdraw(200);
    account.withdraw(2000);  // Won't work - insufficient funds
    
    cout << "Final balance: $" << account.getBalance() << "\n";
    
    // These would cause compile errors:
    // account.balance = 10000;        // Error: private
    // account.validateAmount(100);    // Error: private
    // account.accountNumber = "123";  // Error: private
    
    return 0;
}
```

**Output:**
```
Account created: ACC123456
Deposited: $500. New balance: $1500
Withdrew: $200. New balance: $1300
Cannot withdraw that amount
Final balance: $1300
```

**Why Encapsulation?**
- **Data Protection**: Prevents invalid states
- **Control**: You control how data is modified
- **Flexibility**: Can change internal implementation without affecting users
- **Security**: Sensitive data is hidden
- **Maintainability**: Easy to modify implementation later

---

## Constructors & Destructors - Complete Guide

### Types of Constructors

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    string name;
    int rollNumber;
    double gpa;
    
public:
    // 1. Default Constructor (no parameters)
    Student() : name("Unknown"), rollNumber(0), gpa(0.0) {
        cout << "Default constructor called\n";
    }
    
    // 2. Parameterized Constructor
    Student(string n, int roll, double g) 
        : name(n), rollNumber(roll), gpa(g) {
        cout << "Parameterized constructor called\n";
    }
    
    // 3. Copy Constructor (copies another object)
    Student(const Student& other) 
        : name(other.name), rollNumber(other.rollNumber), gpa(other.gpa) {
        cout << "Copy constructor called\n";
    }
    
    // 4. Move Constructor (C++11) - transfers ownership
    Student(Student&& other) noexcept
        : name(move(other.name)), rollNumber(other.rollNumber), gpa(other.gpa) {
        other.rollNumber = 0;
        other.gpa = 0.0;
        cout << "Move constructor called\n";
    }
    
    // Destructor - called when object is destroyed
    ~Student() {
        cout << "Destructor called for " << name << "\n";
    }
    
    // Getter methods
    void display() const {
        cout << "Name: " << name << ", Roll: " << rollNumber 
             << ", GPA: " << gpa << "\n";
    }
};

int main() {
    cout << "--- Creating objects ---\n";
    
    // Default constructor
    Student s1;
    s1.display();
    cout << "\n";
    
    // Parameterized constructor
    Student s2("Alice", 101, 3.8);
    s2.display();
    cout << "\n";
    
    // Copy constructor (explicit)
    Student s3 = s2;  // Calls copy constructor
    s3.display();
    cout << "\n";
    
    // Move constructor
    Student s4 = move(s2);  // Calls move constructor
    s4.display();
    cout << "\n";
    
    cout << "--- Objects going out of scope ---\n";
    
    return 0;  // Destructors called here for all objects
}
```

**Output:**
```
--- Creating objects ---
Default constructor called
Name: Unknown, Roll: 0, GPA: 0

Parameterized constructor called
Name: Alice, Roll: 101, GPA: 3.8

Copy constructor called
Name: Alice, Roll: 101, GPA: 3.8

Move constructor called
Name: Alice, Roll: 101, GPA: 3.8

--- Objects going out of scope ---
Destructor called for Alice
Destructor called for Alice
Destructor called for Unknown
Destructor called for Unknown
```

### Initialization Lists (Member Initializer List)

```cpp
#include <iostream>
using namespace std;

class Rectangle {
private:
    int width;
    int height;

public:
    // Using initialization list
    // This is ALWAYS better than assignment in constructor body
    Rectangle(int w, int h) : width(w), height(h) {
        cout << "Rectangle created\n";
    }
    
    // Alternative (NOT recommended) - less efficient
    // Rectangle(int w, int h) {
    //     width = w;    // Assignment, not initialization
    //     height = h;
    // }
    
    int getArea() const {
        return width * height;
    }
};

int main() {
    Rectangle rect(5, 10);
    cout << "Area: " << rect.getArea() << "\n";
    
    return 0;
}
```

**Why initialization lists?**
- **Efficiency**: Initializes variables once (not create then assign)
- **Const members**: Can only be initialized with initializer list
- **Reference members**: Must use initializer list
- **Base class initialization**: Only way to initialize base class

---

## Inheritance - All Types

**Inheritance** allows a class to inherit properties and methods from another class.

### Single Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

// Base class (Parent class)
class Animal {
protected:  // Accessible in derived classes
    string name;
    int age;

public:
    Animal(string n, int a) : name(n), age(a) {
        cout << "Animal constructor called\n";
    }
    
    virtual void eat() {
        cout << name << " is eating\n";
    }
    
    virtual void sleep() {
        cout << name << " is sleeping\n";
    }
    
    virtual void makeSound() {
        cout << name << " makes a sound\n";
    }
    
    virtual ~Animal() {
        cout << "Animal destructor called\n";
    }
};

// Derived class (Child class)
class Dog : public Animal {
private:
    string breed;

public:
    Dog(string n, int a, string b) 
        : Animal(n, a), breed(b) {
        cout << "Dog constructor called\n";
    }
    
    // Override methods from base class
    void makeSound() override {
        cout << name << " barks: Woof! Woof!\n";
    }
    
    void fetch() {
        cout << name << " is fetching the ball\n";
    }
    
    ~Dog() {
        cout << "Dog destructor called\n";
    }
};

class Cat : public Animal {
private:
    bool isIndoor;

public:
    Cat(string n, int a, bool indoor) 
        : Animal(n, a), isIndoor(indoor) {
        cout << "Cat constructor called\n";
    }
    
    void makeSound() override {
        cout << name << " meows: Meow! Meow!\n";
    }
    
    void scratch() {
        cout << name << " is scratching the furniture\n";
    }
    
    ~Cat() {
        cout << "Cat destructor called\n";
    }
};

int main() {
    cout << "=== Creating Dog ===\n";
    Dog dog("Rex", 3, "Golden Retriever");
    dog.eat();
    dog.sleep();
    dog.makeSound();
    dog.fetch();
    
    cout << "\n=== Creating Cat ===\n";
    Cat cat("Whiskers", 2, true);
    cat.eat();
    cat.makeSound();
    cat.scratch();
    
    cout << "\n=== Using Polymorphism ===\n";
    Animal* animals[2] = {&dog, &cat};
    for (int i = 0; i < 2; i++) {
        animals[i]->makeSound();
    }
    
    cout << "\n=== Destructors ===\n";
    return 0;
}
```

**Output:**
```
=== Creating Dog ===
Animal constructor called
Dog constructor called
Rex is eating
Rex is sleeping
Rex barks: Woof! Woof!
Rex is fetching the ball

=== Creating Cat ===
Animal constructor called
Cat constructor called
Whiskers is eating
Whiskers meows: Meow! Meow!
Whiskers is scratching the furniture

=== Using Polymorphism ===
Rex barks: Woof! Woof!
Whiskers meows: Meow! Meow!

=== Destructors ===
Cat destructor called
Animal destructor called
Dog destructor called
Animal destructor called
```

### Multiple Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

class Flyer {
public:
    virtual void fly() {
        cout << "Flying...\n";
    }
    virtual ~Flyer() {}
};

class Swimmer {
public:
    virtual void swim() {
        cout << "Swimming...\n";
    }
    virtual ~Swimmer() {}
};

// Duck inherits from both Flyer and Swimmer
class Duck : public Flyer, public Swimmer {
private:
    string name;

public:
    Duck(string n) : name(n) {}
    
    void fly() override {
        cout << name << " is flying\n";
    }
    
    void swim() override {
        cout << name << " is swimming\n";
    }
    
    void quack() {
        cout << name << " quacks: Quack! Quack!\n";
    }
};

int main() {
    Duck duck("Donald");
    duck.fly();
    duck.swim();
    duck.quack();
    
    return 0;
}
```

### Multilevel Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

// Level 1: Base class
class Vehicle {
protected:
    string brand;
    
public:
    Vehicle(string b) : brand(b) {}
    virtual void start() {
        cout << brand << " is starting\n";
    }
    virtual ~Vehicle() {}
};

// Level 2: Derived from Vehicle
class Car : public Vehicle {
protected:
    int numDoors;
    
public:
    Car(string b, int doors) : Vehicle(b), numDoors(doors) {}
    void start() override {
        cout << brand << " car with " << numDoors 
             << " doors is starting\n";
    }
    virtual ~Car() {}
};

// Level 3: Derived from Car
class ElectricCar : public Car {
private:
    int batteryPercentage;
    
public:
    ElectricCar(string b, int doors, int battery)
        : Car(b, doors), batteryPercentage(battery) {}
    
    void start() override {
        cout << "Electric " << brand << " (Battery: " 
             << batteryPercentage << "%) starting\n";
    }
};

int main() {
    ElectricCar tesla("Tesla", 4, 95);
    tesla.start();
    
    return 0;
}
```

### Hierarchical Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
protected:
    string name;
    double salary;
    
public:
    Employee(string n, double s) : name(n), salary(s) {}
    virtual void work() = 0;
    virtual ~Employee() {}
};

class Manager : public Employee {
public:
    Manager(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is managing the team\n";
    }
};

class Developer : public Employee {
private:
    string language;
    
public:
    Developer(string n, double s, string lang)
        : Employee(n, s), language(lang) {}
    void work() override {
        cout << name << " is coding in " << language << "\n";
    }
};

class Designer : public Employee {
public:
    Designer(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is designing UI/UX\n";
    }
};

int main() {
    Manager manager("Alice", 80000);
    Developer dev("Bob", 70000, "C++");
    Designer designer("Carol", 65000);
    
    manager.work();
    dev.work();
    designer.work();
    
    return 0;
}
```

---

## Polymorphism

**Polymorphism** means "many forms" - the ability of objects to take multiple forms.

### Compile-Time Polymorphism (Static Polymorphism)

#### 1. Function Overloading

```cpp
#include <iostream>
#include <string>
using namespace std;

class Calculator {
public:
    // Overload for integers
    int add(int a, int b) {
        cout << "Adding integers\n";
        return a + b;
    }
    
    // Overload for doubles
    double add(double a, double b) {
        cout << "Adding doubles\n";
        return a + b;
    }
    
    // Overload for strings (concatenation)
    string add(string a, string b) {
        cout << "Concatenating strings\n";
        return a + b;
    }
    
    // Overload with different number of parameters
    int add(int a, int b, int c) {
        cout << "Adding three integers\n";
        return a + b + c;
    }
};

int main() {
    Calculator calc;
    
    cout << "Result: " << calc.add(5, 3) << "\n";
    cout << "Result: " << calc.add(2.5, 3.7) << "\n";
    cout << "Result: " << calc.add("Hello", " World") << "\n";
    cout << "Result: " << calc.add(5, 3, 2) << "\n";
    
    return 0;
}
```

#### 2. Operator Overloading

```cpp
#include <iostream>
using namespace std;

class Complex {
private:
    double real, imag;
    
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // Overload + operator
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    // Overload - operator
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    // Overload * operator
    Complex operator*(const Complex& other) const {
        double r = real * other.real - imag * other.imag;
        double i = real * other.imag + imag * other.real;
        return Complex(r, i);
    }
    
    // Overload == operator
    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }
    
    // Overload << operator for output
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << c.real << " + " << c.imag << "i";
        return os;
    }
    
    // Overload = operator (assignment)
    Complex& operator=(const Complex& other) {
        if (this != &other) {
            real = other.real;
            imag = other.imag;
        }
        return *this;
    }
};

int main() {
    Complex c1(3, 4);
    Complex c2(2, 5);
    
    Complex c3 = c1 + c2;
    cout << c1 << " + " << c2 << " = " << c3 << "\n";
    
    Complex c4 = c1 * c2;
    cout << c1 << " * " << c2 << " = " << c4 << "\n";
    
    if (c1 == c2) {
        cout << "Complex numbers are equal\n";
    } else {
        cout << "Complex numbers are not equal\n";
    }
    
    return 0;
}
```

#### 3. Template Specialization

```cpp
#include <iostream>
#include <string>
using namespace std;

// Generic template
template <typename T>
class Printer {
public:
    void print(T value) {
        cout << "Generic: " << value << "\n";
    }
};

// Template specialization for bool
template <>
class Printer<bool> {
public:
    void print(bool value) {
        cout << "Boolean: " << (value ? "true" : "false") << "\n";
    }
};

// Template specialization for string
template <>
class Printer<string> {
public:
    void print(string value) {
        cout << "String: \"" << value << "\"\n";
    }
};

int main() {
    Printer<int> intPrinter;
    intPrinter.print(42);
    
    Printer<double> doublePrinter;
    doublePrinter.print(3.14);
    
    Printer<bool> boolPrinter;
    boolPrinter.print(true);
    
    Printer<string> stringPrinter;
    stringPrinter.print("Hello, World!");
    
    return 0;
}
```

### Run-Time Polymorphism (Dynamic Polymorphism)

#### Virtual Functions & Override

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Shape {
public:
    virtual void draw() = 0;           // Pure virtual
    virtual double area() = 0;
    virtual string getName() = 0;
    virtual ~Shape() {}                // Virtual destructor (important!)
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        cout << "Drawing Circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
    
    string getName() override {
        return "Circle";
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
    
    string getName() override {
        return "Rectangle";
    }
};

class Triangle : public Shape {
private:
    double base, height;
    
public:
    Triangle(double b, double h) : base(b), height(h) {}
    
    void draw() override {
        cout << "Drawing Triangle\n";
    }
    
    double area() override {
        return 0.5 * base * height;
    }
    
    string getName() override {
        return "Triangle";
    }
};

int main() {
    // Using smart pointers (C++11)
    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(5));
    shapes.push_back(make_unique<Rectangle>(4, 6));
    shapes.push_back(make_unique<Triangle>(3, 4));
    
    cout << "=== Drawing all shapes ===\n";
    for (auto& shape : shapes) {
        shape->draw();
        cout << "Area of " << shape->getName() << ": " 
             << shape->area() << "\n\n";
    }
    
    return 0;  // Smart pointers automatically deleted
}
```

**Output:**
```
=== Drawing all shapes ===
Drawing Circle
Area of Circle: 78.5385

Drawing Rectangle
Area of Rectangle: 24

Drawing Triangle
Area of Triangle: 6
```

#### Abstract Classes & Interfaces

```cpp
#include <iostream>
#include <string>
using namespace std;

// Abstract class (interface-like)
class DatabaseConnection {
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual ~DatabaseConnection() {}
};

class MySQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    MySQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to MySQL...\n";
        connected = true;
        cout << "Connected to MySQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from MySQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

class PostgreSQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    PostgreSQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to PostgreSQL...\n";
        connected = true;
        cout << "Connected to PostgreSQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from PostgreSQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

int main() {
    // Cannot create DatabaseConnection directly
    // DatabaseConnection db;  // Error: abstract class
    
    // Create through derived classes
    MySQLConnection mysql;
    PostgreSQLConnection postgres;
    
    // Use polymorphically
    DatabaseConnection* db1 = &mysql;
    DatabaseConnection* db2 = &postgres;
    
    db1->connect();
    cout << "MySQL connected: " << (db1->isConnected() ? "Yes" : "No") << "\n\n";
    
    db2->connect();
    cout << "PostgreSQL connected: " << (db2->isConnected() ? "Yes" : "No") << "\n\n";
    
    db1->disconnect();
    db2->disconnect();
    
    return 0;
}
```

---

## Abstraction

**Abstraction** is showing only essential features and hiding unnecessary details.

```cpp
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// Abstract class - hides implementation details
class Shape {
public:
    virtual void draw() = 0;
    virtual double area() = 0;
    virtual double perimeter() = 0;
    virtual ~Shape() {}
};

// Concrete implementation
class Circle : public Shape {
private:
    double radius;
    const double PI = 3.14159;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        cout << "Displaying Circle\n";
    }
    
    double area() override {
        return PI * radius * radius;
    }
    
    double perimeter() override {
        return 2 * PI * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Displaying Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
    
    double perimeter() override {
        return 2 * (width + height);
    }
};

// Client code doesn't know HOW things are calculated
void printShapeInfo(Shape* shape) {
    shape->draw();
    cout << "Area: " << shape->area() << "\n";
    cout << "Perimeter: " << shape->perimeter() << "\n";
}

int main() {
    Circle circle(5);
    Rectangle rectangle(4, 6);
    
    cout << "=== Circle ===\n";
    printShapeInfo(&circle);
    
    cout << "\n=== Rectangle ===\n";
    printShapeInfo(&rectangle);
    
    return 0;
}
```

---

## Advanced Class Features

### Nested Classes

```cpp
#include <iostream>
#include <string>
using namespace std;

class Car {
public:
    // Nested class
    class Engine {
    private:
        double horsepower;
        
    public:
        Engine(double hp) : horsepower(hp) {}
        
        void startEngine() {
            cout << "Engine with " << horsepower 
                 << " HP is starting\n";
        }
    };
    
private:
    string brand;
    Engine engine;
    
public:
    Car(string b, double hp) : brand(b), engine(hp) {}
    
    void display() {
        cout << brand << " car\n";
        engine.startEngine();
    }
};

int main() {
    Car car("Ferrari", 800);
    car.display();
    
    // Can also create nested class independently
    Car::Engine standaloneEngine(500);
    standaloneEngine.startEngine();
    
    return 0;
}
```

### Inner Classes with Access Control

```cpp
#include <iostream>
using namespace std;

class Outer {
private:
    int outerPrivate = 10;
    
public:
    class Inner {
    public:
        void accessOuterPrivate(Outer& outer) {
            // Inner class can access Outer's private members
            cout << "Accessing outer private: " 
                 << outer.outerPrivate << "\n";
        }
    };
    
    Inner createInner() {
        return Inner();
    }
};

int main() {
    Outer outer;
    Outer::Inner inner = outer.createInner();
    inner.accessOuterPrivate(outer);
    
    return 0;
}
```

---

## Access Modifiers & Friend Classes

### Public, Private, Protected

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    int publicData = 1;
    void publicMethod() {
        cout << "Public method\n";
    }
    
protected:
    int protectedData = 2;
    void protectedMethod() {
        cout << "Protected method\n";
    }
    
private:
    int privateData = 3;
    void privateMethod() {
        cout << "Private method\n";
    }
};

class Derived : public Base {
public:
    void testAccess() {
        cout << "Public data: " << publicData << "\n";
        cout << "Protected data: " << protectedData << "\n";
        // cout << "Private data: " << privateData << "\n";  // Error!
    }
};

int main() {
    Base b;
    cout << "Public: " << b.publicData << "\n";
    // cout << "Protected: " << b.protectedData << "\n";  // Error!
    // cout << "Private: " << b.privateData << "\n";      // Error!
    
    Derived d;
    d.testAccess();
    
    return 0;
}
```

### Friend Functions and Classes

```cpp
#include <iostream>
using namespace std;

class MyClass {
private:
    int secretValue;
    
public:
    MyClass(int value) : secretValue(value) {}
    
    // Friend function - can access private members
    friend void revealSecret(MyClass& obj);
    
    // Friend class - can access private members
    friend class FriendClass;
};

// Friend function
void revealSecret(MyClass& obj) {
    cout << "Secret value: " << obj.secretValue << "\n";
}

// Friend class
class FriendClass {
public:
    void accessPrivate(MyClass& obj) {
        cout << "Friend class accessing: " << obj.secretValue << "\n";
    }
};

int main() {
    MyClass obj(42);
    revealSecret(obj);
    
    FriendClass friend;
    friend.accessPrivate(obj);
    
    return 0;
}
```

---

## Static Members

### Static Variables & Methods

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
private:
    string name;
    int id;
    static int nextId;      // Shared by all instances
    static int totalCount;  // Count of employees
    
public:
    Employee(string n) : name(n), id(nextId++) {
        totalCount++;
    }
    
    ~Employee() {
        totalCount--;
    }
    
    // Static method - can only access static members
    static void displayStats() {
        cout << "Total employees: " << totalCount << "\n";
        cout << "Next ID will be: " << nextId << "\n";
    }
    
    void display() {
        cout << "ID: " << id << ", Name: " << name << "\n";
    }
};

// Initialize static members
int Employee::nextId = 1;
int Employee::totalCount = 0;

int main() {
    cout << "Creating employees...\n";
    Employee emp1("Alice");
    emp1.display();
    
    Employee emp2("Bob");
    emp2.display();
    
    Employee emp3("Carol");
    emp3.display();
    
    // Call static method
    Employee::displayStats();
    
    {
        Employee emp4("David");
        emp4.display();
        Employee::displayStats();
    }
    
    cout << "\nAfter scope ends:\n";
    Employee::displayStats();
    
    return 0;
}
```

**Output:**
```
Creating employees...
ID: 1, Name: Alice
ID: 2, Name: Bob
ID: 3, Name: Carol
Total employees: 3
Next ID will be: 4
ID: 4, Name: David
Total employees: 4
Next ID will be: 5

After scope ends:
Total employees: 3
Next ID will be: 5
```

---

## Const Correctness in OOP

```cpp
#include <iostream>
#include <string>
using namespace std;

class Person {
private:
    mutable int accessCount;  // Can be modified even in const functions
    string name;
    int age;
    
public:
    Person(string n, int a) : name(n), age(a), accessCount(0) {}
    
    // Const method - cannot modify member variables
    string getName() const {
        accessCount++;  // OK because accessCount is mutable
        return name;
    }
    
    // Const method returning const reference
    const string& getNameRef() const {
        accessCount++;
        return name;
    }
    
    // Non-const method - can modify members
    void setAge(int newAge) {
        age = newAge;  // OK in non-const method
    }
    
    // Const method
    int getAge() const {
        return age;
    }
    
    int getAccessCount() const {
        return accessCount;
    }
    
    void display() const {
        cout << "Name: " << name << ", Age: " << age << "\n";
    }
};

int main() {
    Person p("Alice", 30);
    
    // Can call both const and non-const methods
    cout << p.getName() << "\n";
    cout << p.getAge() << "\n";
    p.setAge(31);
    
    // Const object
    const Person cp("Bob", 25);
    
    // Can only call const methods
    cout << cp.getName() << "\n";
    cout << cp.getAge() << "\n";
    // cp.setAge(26);  // Error: cannot call non-const method on const object
    
    cp.display();
    cout << "Access count: " << cp.getAccessCount() << "\n";
    
    return 0;
}
```

---

## Operator Overloading

### Overloadable Operators

```cpp
#include <iostream>
using namespace std;

class Vector {
private:
    int x, y;
    
public:
    Vector(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Arithmetic operators
    Vector operator+(const Vector& v) const {
        return Vector(x + v.x, y + v.y);
    }
    
    Vector operator-(const Vector& v) const {
        return Vector(x - v.x, y - v.y);
    }
    
    Vector operator*(int scalar) const {
        return Vector(x * scalar, y * scalar);
    }
    
    // Comparison operators
    bool operator==(const Vector& v) const {
        return x == v.x && y == v.y;
    }
    
    bool operator!=(const Vector& v) const {
        return !(*this == v);
    }
    
    // Assignment operator
    Vector& operator=(const Vector& v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }
    
    // Unary operators
    Vector operator-() const {
        return Vector(-x, -y);
    }
    
    // Increment/Decrement
    Vector& operator++() {  // Pre-increment
        x++; y++;
        return *this;
    }
    
    Vector operator++(int) {  // Post-increment
        Vector temp = *this;
        x++; y++;
        return temp;
    }
    
    // Subscript operator
    int operator[](int index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        throw out_of_range("Invalid index");
    }
    
    // Stream operators (must be friend or free functions)
    friend ostream& operator<<(ostream& os, const Vector& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
    
    friend istream& operator>>(istream& is, Vector& v) {
        is >> v.x >> v.y;
        return is;
    }
};

int main() {
    Vector v1(3, 4);
    Vector v2(1, 2);
    
    cout << "v1 = " << v1 << "\n";
    cout << "v2 = " << v2 << "\n";
    
    Vector v3 = v1 + v2;
    cout << "v1 + v2 = " << v3 << "\n";
    
    Vector v4 = v1 - v2;
    cout << "v1 - v2 = " << v4 << "\n";
    
    Vector v5 = v1 * 2;
    cout << "v1 * 2 = " << v5 << "\n";
    
    cout << "v1 == v2: " << (v1 == v2 ? "true" : "false") << "\n";
    
    cout << "-v1 = " << (-v1) << "\n";
    
    cout << "v1[0] = " << v1[0] << ", v1[1] = " << v1[1] << "\n";
    
    Vector v6 = v1;
    cout << "After v6 = v1: v6 = " << v6 << "\n";
    
    return 0;
}
```

---

## SOLID Principles

### S - Single Responsibility Principle

```cpp
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// BAD: Class doing multiple things
// class User {
//     void save() { }      // Saving logic
//     void sendEmail() { } // Sending email
//     void validate() { }  // Validation
// };

// GOOD: Each class has one responsibility
class User {
private:
    string name, email;
    
public:
    User(string n, string e) : name(n), email(e) {}
    string getName() { return name; }
    string getEmail() { return email; }
};

class UserValidator {
public:
    bool isValid(const User& user) {
        return !user.getName().empty() && 
               !user.getEmail().empty();
    }
};

class UserRepository {
public:
    void save(const User& user) {
        // Save to database or file
        cout << "Saving user: " << user.getName() << "\n";
    }
};

class EmailService {
public:
    void sendWelcomeEmail(const User& user) {
        cout << "Sending welcome email to: " << user.getEmail() << "\n";
    }
};

int main() {
    User user("Alice", "alice@example.com");
    UserValidator validator;
    UserRepository repo;
    EmailService emailService;
    
    if (validator.isValid(user)) {
        repo.save(user);
        emailService.sendWelcomeEmail(user);
    }
    
    return 0;
}
```

### O - Open/Closed Principle

```cpp
#include <iostream>
#include <vector>
using namespace std;

// GOOD: Open for extension, closed for modification
class Shape {
public:
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() override {
        return 3.14 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() override {
        return width * height;
    }
};

// New shape can be added without modifying existing code
class Triangle : public Shape {
private:
    double base, height;
public:
    Triangle(double b, double h) : base(b), height(h) {}
    double area() override {
        return 0.5 * base * height;
    }
};

class AreaCalculator {
public:
    double totalArea(vector<Shape*> shapes) {
        double total = 0;
        for (Shape* shape : shapes) {
            total += shape->area();
        }
        return total;
    }
};

int main() {
    Circle c(5);
    Rectangle r(4, 6);
    Triangle t(3, 4);
    
    vector<Shape*> shapes = {&c, &r, &t};
    AreaCalculator calc;
    
    cout << "Total area: " << calc.totalArea(shapes) << "\n";
    
    return 0;
}
```

### L - Liskov Substitution Principle

```cpp
#include <iostream>
using namespace std;

class Bird {
public:
    virtual void fly() = 0;
    virtual ~Bird() {}
};

// GOOD: Penguin shouldn't be Bird if it can't fly
// Instead, create separate hierarchy
class FlyingBird : public Bird {
public:
    void fly() override {
        cout << "Flying...\n";
    }
};

class Sparrow : public FlyingBird {
public:
    void fly() override {
        cout << "Sparrow flying\n";
    }
};

// Penguin is a Bird, but not a FlyingBird
class NonFlyingBird {
public:
    virtual void move() = 0;
    virtual ~NonFlyingBird() {}
};

class Penguin : public NonFlyingBird {
public:
    void move() override {
        cout << "Penguin swimming/waddling\n";
    }
};

int main() {
    Sparrow sparrow;
    sparrow.fly();
    
    Penguin penguin;
    penguin.move();
    
    return 0;
}
```

### I - Interface Segregation Principle

```cpp
#include <iostream>
using namespace std;

// BAD: Worker interface too bloated
// class Worker {
//     virtual void work() = 0;
//     virtual void eat() = 0;
//     virtual void sleep() = 0;
// };

// GOOD: Segregated interfaces
class Workable {
public:
    virtual void work() = 0;
    virtual ~Workable() {}
};

class Eatable {
public:
    virtual void eat() = 0;
    virtual ~Eatable() {}
};

class Sleepable {
public:
    virtual void sleep() = 0;
    virtual ~Sleepable() {}
};

class Human : public Workable, public Eatable, public Sleepable {
public:
    void work() override {
        cout << "Human working\n";
    }
    void eat() override {
        cout << "Human eating\n";
    }
    void sleep() override {
        cout << "Human sleeping\n";
    }
};

class Robot : public Workable {
public:
    void work() override {
        cout << "Robot working\n";
    }
};

int main() {
    Human human;
    human.work();
    human.eat();
    human.sleep();
    
    Robot robot;
    robot.work();
    // robot.eat();  // Robot doesn't have eat, which is correct!
    
    return 0;
}
```

### D - Dependency Inversion Principle

```cpp
#include <iostream>
#include <memory>
using namespace std;

// Abstraction
class DataStore {
public:
    virtual void save(const string& data) = 0;
    virtual ~DataStore() {}
};

// Concrete implementations
class DatabaseStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to database: " << data << "\n";
    }
};

class FileStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to file: " << data << "\n";
    }
};

// High-level module depends on abstraction, not concrete class
class UserService {
private:
    shared_ptr<DataStore> dataStore;
    
public:
    UserService(shared_ptr<DataStore> store) : dataStore(store) {}
    
    void registerUser(const string& username) {
        cout << "Registering user: " << username << "\n";
        dataStore->save(username);
    }
};

int main() {
    // Can easily switch implementations
    auto dbStore = make_shared<DatabaseStore>();
    UserService service1(dbStore);
    service1.registerUser("Alice");
    
    cout << "\n";
    
    auto fileStore = make_shared<FileStore>();
    UserService service2(fileStore);
    service2.registerUser("Bob");
    
    return 0;
}
```
---

## Constructors & Destructors

### Constructors (C++98)

```cpp
#include <iostream>
#include <string>

class Car {
private:
    std::string brand;
    int year;
public:
    // Default constructor (no parameters)
    Car() : brand("Unknown"), year(2000) {
        std::cout << "Default constructor called\n";
    }
    
    // Parameterized constructor
    Car(const std::string& b, int y) : brand(b), year(y) {
        std::cout << "Constructor called\n";
    }
    
    // Copy constructor
    Car(const Car& other) : brand(other.brand), year(other.year) {
        std::cout << "Copy constructor called\n";
    }
    
    void show() const {
        std::cout << brand << " (" << year << ")\n";
    }
};

int main() {
    Car c1;                        // Calls default
    Car c2("Toyota", 2020);        // Calls parameterized
    Car c3 = c2;                   // Calls copy
    
    c1.show();
    c2.show();
    c3.show();
    
    return 0;
}
```

### Destructors (C++98)

```cpp
#include <iostream>
#include <fstream>

class File {
private:
    std::ofstream file;
public:
    File(const std::string& filename) {
        file.open(filename);
        if (file) {
            std::cout << "File opened\n";
        }
    }
    
    // Destructor (called when object destroyed)
    ~File() {
        if (file.is_open()) {
            file.close();
            std::cout << "File closed\n";
        }
    }
    
    void write(const std::string& data) {
        file << data << "\n";
    }
};

int main() {
    {
        File f("output.txt");
        f.write("Hello, World!");
    }  // Destructor called here, file closed
    
    return 0;
}
```

### 2.2 Advanced Constructor Features (C++11)

#### Delegating Constructors
One constructor can call another constructor of the same class to reduce code duplication.

```cpp
class Data {
    int x, y;
    std::string s;
public:
    // Target constructor
    Data(int x, int y, std::string s) : x(x), y(y), s(s) {}
    
    // Delegating constructor
    Data() : Data(0, 0, "default") {} 
    
    // Another delegating constructor
    Data(int x) : Data(x, 0, "default") {}
};
```

#### Inheriting Constructors
Using `using` to expose base class constructors.

```cpp
class Base {
public:
    Base(int x) { std::cout << "Base(int)\n"; }
};

class Derived : public Base {
public:
    using Base::Base; // Inherits Base(int)
    // Implicitly generates Derived(int x) : Base(x) {}
};
```

### 2.3 Construction & Destruction Order
The order is strict and deterministic (Stack logic: LIFO).

**Construction Order:**
1.  Base Classes (in order of inheritance)
2.  Member Objects (in order of declaration in class)
3.  Constructor Body

**Destruction Order:**
1.  Destructor Body
2.  Member Objects (reverse order of declaration)
3.  Base Classes (reverse order of inheritance)

```cpp
class Base { public: Base() { cout << "Base "; } ~Base() { cout << "~Base "; } };
class Member { public: Member() { cout << "Member "; } ~Member() { cout << "~Member "; } };

class Derived : public Base {
    Member m;
public:
    Derived() { cout << "Derived "; }
    ~Derived() { cout << "~Derived "; }
};

int main() {
    Derived d; // Output: Base Member Derived
    // End of scope: ~Derived ~Member ~Base
}
```

### 2.4 The Rule of Three, Five, and Zero

This is the cornerstone of resource management.

1.  **Rule of Three (C++98)**: If you implement one of: Destructor, Copy Constructor, Copy Assignment Operator; you likely need to implement all three.
2.  **Rule of Five (C++11)**: For Move semantics, add Move Constructor and Move Assignment Operator.
3.  **Rule of Zero**: If your class uses RAII types (`std::string`, `std::vector`, `std::unique_ptr`), do **NOT** declare any of the special member functions. Let the compiler generate them.

```cpp
// Rule of Zero Example (Best Practice)
class User {
    std::string name; // Manages its own memory
    std::vector<int> scores; // Manages its own memory
    // No destructor needed!
};
```

---

## Inheritance

### Basic Inheritance (C++98)

```cpp
#include <iostream>
#include <string>

// Base class
class Animal {
protected:
    std::string name;
public:
    Animal(const std::string& n) : name(n) {}
    
    virtual void speak() {  // virtual allows override
        std::cout << name << " makes a sound\n";
    }
    
    virtual ~Animal() {}
};

// Derived class
class Dog : public Animal {
public:
    Dog(const std::string& n) : Animal(n) {}
    
    void speak() override {  // override keyword (C++11)
        std::cout << name << " barks\n";
    }
};

class Cat : public Animal {
public:
    Cat(const std::string& n) : Animal(n) {}
    
    void speak() override {
        std::cout << name << " meows\n";
    }
};

int main() {
    Dog dog("Rex");
    Cat cat("Whiskers");
    
    dog.speak();
    cat.speak();
    
    // Polymorphism
    Animal* animals[2] = {&dog, &cat};
    for (int i = 0; i < 2; i++) {
        animals[i]->speak();
    }
    
    return 0;
}
```

### Multiple Inheritance (C++98)

```cpp
#include <iostream>

class A {
public:
    void func_a() { std::cout << "A::func\n"; }
};

class B {
public:
    void func_b() { std::cout << "B::func\n"; }
};

class C : public A, public B {
public:
    void func_c() { std::cout << "C::func\n"; }
};

int main() {
    C c;
    c.func_a();
    c.func_b();
    c.func_c();
    
    return 0;
}
```

---

## Virtual Functions & Polymorphism

### Virtual Functions (C++98)

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Shape {
public:
    virtual void draw() = 0;  // Pure virtual (abstract)
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        std::cout << "Drawing circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        std::cout << "Drawing rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
};

int main() {
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(5));
    shapes.push_back(new Rectangle(4, 6));
    
    for (Shape* s : shapes) {
        s->draw();
        std::cout << "Area: " << s->area() << "\n";
    }
    
    // Cleanup
    for (Shape* s : shapes) {
        delete s;
    }
    
    return 0;
}
```

### 2.6 Advanced Polymorphism

#### 1. Virtual Destructors (CRITICAL)
If you delete a derived object through a base pointer, the base destructor must be virtual. Otherwise, the derived destructor is **never called**, causing memory leaks.

```cpp
class Base {
public:
    // virtual ~Base() {} // Correct
    ~Base() {} // Dangerous!
};

class Derived : public Base {
    int* ptr;
public:
    Derived() { ptr = new int[100]; }
    ~Derived() { delete[] ptr; }
};

Base* b = new Derived();
delete b; // If ~Base is not virtual, ~Derived is NOT called! Leak!
```

#### 2. Covariant Return Types
An override can return a pointer/reference to a *derived* class, not just the base.

```cpp
class Shape {
public:
    virtual Shape* clone() = 0;
};

class Circle : public Shape {
public:
    // Returns Circle* instead of Shape* - Valid!
    Circle* clone() override { return new Circle(*this); }
};
```

#### 3. RTTI & dynamic_cast
Run-Time Type Information allows safe downcasting. It uses the `vptr` to check the actual type.

```cpp
Shape* s = new Circle(5);

// Safe cast: returns nullptr if s is not a Circle
if (Circle* c = dynamic_cast<Circle*>(s)) {
    c->special_circle_method();
} else {
    std::cout << "Not a circle\n";
}
```

#### 4. Static Polymorphism (CRTP)
Curiously Recurring Template Pattern. Faster than virtual functions (compile-time resolution).

```cpp
template<typename Derived>
class Shape {
public:
    void draw() {
        // Compile-time dispatch
        static_cast<Derived*>(this)->draw_impl();
    }
};

class Circle : public Shape<Circle> {
public:
    void draw_impl() { cout << "Circle\n"; }
};
```

---
