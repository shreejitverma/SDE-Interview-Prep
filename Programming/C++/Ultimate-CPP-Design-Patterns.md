# The Ultimate C++ Design Patterns Guide: From Basics to Advanced Mastery

## Table of Contents

### SECTION 1: DESIGN PATTERNS FUNDAMENTALS
1. [What Are Design Patterns?](#what-are-design-patterns)
2. [Benefits & Why Use Them](#benefits--why-use-them)
3. [Pattern Categories](#pattern-categories)

### SECTION 2: CREATIONAL PATTERNS (Object Creation)
4. [Singleton Pattern](#singleton-pattern)
5. [Factory Method Pattern](#factory-method-pattern)
6. [Abstract Factory Pattern](#abstract-factory-pattern)
7. [Builder Pattern](#builder-pattern)
8. [Object Pool Pattern](#object-pool-pattern)
9. [Prototype Pattern](#prototype-pattern)

### SECTION 3: STRUCTURAL PATTERNS (Object Composition)
10. [Adapter Pattern](#adapter-pattern)
11. [Decorator Pattern](#decorator-pattern)
12. [Facade Pattern](#facade-pattern)
13. [Proxy Pattern](#proxy-pattern)
14. [Bridge Pattern](#bridge-pattern)
15. [Composite Pattern](#composite-pattern)
16. [Flyweight Pattern](#flyweight-pattern)

### SECTION 4: BEHAVIORAL PATTERNS (Object Interaction)
17. [Observer Pattern](#observer-pattern)
18. [Strategy Pattern](#strategy-pattern)
19. [Command Pattern](#command-pattern)
20. [State Pattern](#state-pattern)
21. [Template Method Pattern](#template-method-pattern)
22. [Chain of Responsibility Pattern](#chain-of-responsibility-pattern)
23. [Iterator Pattern](#iterator-pattern)
24. [Mediator Pattern](#mediator-pattern)
25. [Memento Pattern](#memento-pattern)
26. [Visitor Pattern](#visitor-pattern)
27. [Interpreter Pattern](#interpreter-pattern)

### SECTION 5: CONCURRENCY PATTERNS
28. [Active Object Pattern](#active-object-pattern)
29. [Monitor Object Pattern](#monitor-object-pattern)
30. [Thread Pool Pattern](#thread-pool-pattern)
31. [Lock-Free Pattern](#lock-free-pattern)

### SECTION 6: ARCHITECTURAL PATTERNS
32. [MVC (Model-View-Controller)](#mvc-pattern)
33. [MVVM (Model-View-ViewModel)](#mvvm-pattern)
34. [Repository Pattern](#repository-pattern)
35. [Service Locator Pattern](#service-locator-pattern)
36. [Dependency Injection](#dependency-injection)

### SECTION 7: ADVANCED & MODERN PATTERNS
37. [CRTP (Curiously Recurring Template Pattern)](#crtp-pattern)
38. [Type Erasure](#type-erasure)
39. [Expression Templates](#expression-templates)
40. [Policy-Based Class Design](#policy-based-class-design)

### SECTION 8: IDIOMS & BEST PRACTICES
41. [RAII (Resource Acquisition Is Initialization)](#raii-idiom)
42. [PIMPL (Pointer To Implementation)](#pimpl-idiom)
43. [Copy-and-Swap](#copy-and-swap-idiom)
44. [SFINAE (Substitution Failure Is Not An Error)](#sfinae-idiom)

### SECTION 9: ANTI-PATTERNS (What NOT to Do)
45. [Common Anti-Patterns](#anti-patterns)

---

## SECTION 1: DESIGN PATTERNS FUNDAMENTALS

## What Are Design Patterns?

Design patterns are **reusable solutions to common problems** in software design. They are templates for solving design problems that can be applied to many different situations. Patterns originated from architecture (Christopher Alexander) and were popularized in software by the "Gang of Four" (Gamma, Helm, Johnson, Vlissides) in 1994.

### Three Main Categories

**Creational Patterns** - Focus on object creation mechanisms
**Structural Patterns** - Focus on object composition and relationships
**Behavioral Patterns** - Focus on communication between objects

---

## Benefits & Why Use Them

```
✓ Proven solutions to recurring problems
✓ Improved code maintainability
✓ Better code reusability
✓ Reduced development time
✓ Improved communication (common vocabulary)
✓ Flexibility and scalability
✓ Reduced dependencies
✓ Better abstraction
```

---

## Pattern Categories

```
CREATIONAL (6 patterns)
├─ Singleton
├─ Factory Method
├─ Abstract Factory
├─ Builder
├─ Object Pool
└─ Prototype

STRUCTURAL (7 patterns)
├─ Adapter
├─ Decorator
├─ Facade
├─ Proxy
├─ Bridge
├─ Composite
└─ Flyweight

BEHAVIORAL (11 patterns)
├─ Observer
├─ Strategy
├─ Command
├─ State
├─ Template Method
├─ Chain of Responsibility
├─ Iterator
├─ Mediator
├─ Memento
├─ Visitor
└─ Interpreter

CONCURRENCY (4 patterns)
├─ Active Object
├─ Monitor Object
├─ Thread Pool
└─ Lock-Free

ARCHITECTURAL (5 patterns)
├─ MVC
├─ MVVM
├─ Repository
├─ Service Locator
└─ Dependency Injection
```

---

## SECTION 2: CREATIONAL PATTERNS

## Singleton Pattern

### What It Is

Ensures a class has **only one instance** and provides a **global point of access** to it.

### When to Use

- Logging systems
- Configuration managers
- Database connection pools
- Thread pools
- Caches

### Basic Implementation (C++98)

```cpp
#include <iostream>

class Singleton {
private:
    static Singleton* instance;
    
    // Private constructor (can't create outside)
    Singleton() {
        std::cout << "Singleton created\n";
    }
    
public:
    // Delete copy operations
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    // Get the single instance
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
    
    void doSomething() {
        std::cout << "Singleton doing something\n";
    }
};

Singleton* Singleton::instance = nullptr;

int main() {
    Singleton* s1 = Singleton::getInstance();
    Singleton* s2 = Singleton::getInstance();
    
    std::cout << (s1 == s2 ? "Same instance" : "Different") << "\n";  // Same instance
    
    return 0;
}
```

### Thread-Safe Implementation (C++11)

```cpp
#include <iostream>
#include <memory>
#include <mutex>

class ThreadSafeSingleton {
private:
    static std::unique_ptr<ThreadSafeSingleton> instance;
    static std::mutex mutex;
    
    ThreadSafeSingleton() {
        std::cout << "ThreadSafeSingleton created\n";
    }
    
public:
    ThreadSafeSingleton(const ThreadSafeSingleton&) = delete;
    ThreadSafeSingleton& operator=(const ThreadSafeSingleton&) = delete;
    
    // Double-checked locking
    static ThreadSafeSingleton* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = std::make_unique<ThreadSafeSingleton>();
            }
        }
        return instance.get();
    }
};

std::unique_ptr<ThreadSafeSingleton> ThreadSafeSingleton::instance = nullptr;
std::mutex ThreadSafeSingleton::mutex;
```

### Modern C++11 Implementation (Best)

```cpp
#include <iostream>

class ModernSingleton {
private:
    ModernSingleton() {
        std::cout << "ModernSingleton created\n";
    }
    
public:
    ModernSingleton(const ModernSingleton&) = delete;
    ModernSingleton& operator=(const ModernSingleton&) = delete;
    
    // Magic statics: guaranteed thread-safe in C++11+
    static ModernSingleton& getInstance() {
        static ModernSingleton instance;
        return instance;
    }
};

int main() {
    ModernSingleton& s1 = ModernSingleton::getInstance();
    ModernSingleton& s2 = ModernSingleton::getInstance();
    
    std::cout << (&s1 == &s2 ? "Same instance" : "Different") << "\n";  // Same instance
    
    return 0;
}
```

### Real-World Example: Logger

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

class Logger {
private:
    std::ofstream logFile;
    
    Logger() {
        logFile.open("application.log", std::ios::app);
    }
    
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void log(const std::string& level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream oss;
        oss << "[" << std::ctime(&time) << "] [" << level << "] " << message;
        
        logFile << oss.str();
        logFile.flush();
        
        std::cout << oss.str();
    }
    
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

int main() {
    Logger& logger = Logger::getInstance();
    logger.log("INFO", "Application started\n");
    logger.log("ERROR", "Something went wrong\n");
    
    return 0;
}
```

---

## Factory Method Pattern

### What It Is

Creates objects **without specifying the exact classes** to create. Uses a method to instantiate objects.

### When to Use

- When a class can't anticipate the type of objects it needs
- When subclasses should specify objects to create
- When object creation logic is complex

### Basic Implementation

```cpp
#include <iostream>
#include <memory>

// Abstract product
class Animal {
public:
    virtual ~Animal() = default;
    virtual void sound() = 0;
};

// Concrete products
class Dog : public Animal {
public:
    void sound() override {
        std::cout << "Woof!\n";
    }
};

class Cat : public Animal {
public:
    void sound() override {
        std::cout << "Meow!\n";
    }
};

class Bird : public Animal {
public:
    void sound() override {
        std::cout << "Chirp!\n";
    }
};

// Factory
class AnimalFactory {
public:
    enum AnimalType { DOG, CAT, BIRD };
    
    static std::unique_ptr<Animal> createAnimal(AnimalType type) {
        switch (type) {
            case DOG:
                return std::make_unique<Dog>();
            case CAT:
                return std::make_unique<Cat>();
            case BIRD:
                return std::make_unique<Bird>();
            default:
                return nullptr;
        }
    }
};

int main() {
    auto dog = AnimalFactory::createAnimal(AnimalFactory::DOG);
    auto cat = AnimalFactory::createAnimal(AnimalFactory::CAT);
    auto bird = AnimalFactory::createAnimal(AnimalFactory::BIRD);
    
    dog->sound();
    cat->sound();
    bird->sound();
    
    return 0;
}
```

### Advanced: Factory with Registration (Self-Registering)

```cpp
#include <iostream>
#include <memory>
#include <map>
#include <functional>

class Animal {
public:
    virtual ~Animal() = default;
    virtual void sound() = 0;
};

class Dog : public Animal {
public:
    void sound() override { std::cout << "Woof!\n"; }
};

class Cat : public Animal {
public:
    void sound() override { std::cout << "Meow!\n"; }
};

// Self-registering factory
class AnimalFactory {
private:
    using Creator = std::function<std::unique_ptr<Animal>()>;
    static std::map<std::string, Creator> registry;
    
public:
    static void registerCreator(const std::string& type, Creator creator) {
        registry[type] = creator;
    }
    
    static std::unique_ptr<Animal> create(const std::string& type) {
        auto it = registry.find(type);
        if (it != registry.end()) {
            return it->second();
        }
        return nullptr;
    }
};

std::map<std::string, AnimalFactory::Creator> AnimalFactory::registry;

// Auto-registration
struct DogRegistrar {
    DogRegistrar() {
        AnimalFactory::registerCreator("dog", []() {
            return std::make_unique<Dog>();
        });
    }
};

struct CatRegistrar {
    CatRegistrar() {
        AnimalFactory::registerCreator("cat", []() {
            return std::make_unique<Cat>();
        });
    }
};

// Force registration
static DogRegistrar dogReg;
static CatRegistrar catReg;

int main() {
    auto dog = AnimalFactory::create("dog");
    auto cat = AnimalFactory::create("cat");
    
    dog->sound();
    cat->sound();
    
    return 0;
}
```

---

## Abstract Factory Pattern

### What It Is

Provides an interface to create **families of related objects** without specifying their concrete classes.

### When to Use

- Creating families of related objects
- Multi-platform UI toolkits (Windows UI, Mac UI, Linux UI)
- Switching between different implementations

### Implementation

```cpp
#include <iostream>
#include <memory>

// Abstract products
class Button {
public:
    virtual ~Button() = default;
    virtual void render() = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() = 0;
};

// Windows products
class WindowsButton : public Button {
public:
    void render() override { std::cout << "Rendering Windows Button\n"; }
};

class WindowsCheckbox : public Checkbox {
public:
    void render() override { std::cout << "Rendering Windows Checkbox\n"; }
};

// Mac products
class MacButton : public Button {
public:
    void render() override { std::cout << "Rendering Mac Button\n"; }
};

class MacCheckbox : public Checkbox {
public:
    void render() override { std::cout << "Rendering Mac Checkbox\n"; }
};

// Abstract factory
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};

// Concrete factories
class WindowsFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }
};

class MacFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<MacCheckbox>();
    }
};

// Application
void renderUI(UIFactory* factory) {
    auto button = factory->createButton();
    auto checkbox = factory->createCheckbox();
    
    button->render();
    checkbox->render();
}

int main() {
    std::unique_ptr<UIFactory> factory;
    
    #ifdef _WIN32
        factory = std::make_unique<WindowsFactory>();
    #else
        factory = std::make_unique<MacFactory>();
    #endif
    
    renderUI(factory.get());
    
    return 0;
}
```

---

## Builder Pattern

### What It Is

Separates the construction of a **complex object from its representation**, allowing step-by-step building.

### When to Use

- Creating objects with many optional parameters
- Creating immutable objects
- When construction is complex with multiple steps

### Basic Implementation

```cpp
#include <iostream>
#include <string>

class House {
public:
    void setFoundation(const std::string& foundation) {
        std::cout << "Setting foundation: " << foundation << "\n";
    }
    
    void setWalls(const std::string& walls) {
        std::cout << "Building walls: " << walls << "\n";
    }
    
    void setRoof(const std::string& roof) {
        std::cout << "Building roof: " << roof << "\n";
    }
    
    void show() {
        std::cout << "House built!\n";
    }
};

// Builder
class HouseBuilder {
private:
    House house;
    
public:
    HouseBuilder& buildFoundation(const std::string& foundation) {
        house.setFoundation(foundation);
        return *this;
    }
    
    HouseBuilder& buildWalls(const std::string& walls) {
        house.setWalls(walls);
        return *this;
    }
    
    HouseBuilder& buildRoof(const std::string& roof) {
        house.setRoof(roof);
        return *this;
    }
    
    House build() {
        return house;
    }
};

int main() {
    HouseBuilder builder;
    House house = builder
        .buildFoundation("concrete")
        .buildWalls("brick")
        .buildRoof("wood")
        .build();
    
    house.show();
    
    return 0;
}
```

### Advanced: Fluent Builder with Configuration

```cpp
#include <iostream>
#include <string>
#include <vector>

class DatabaseConfig {
public:
    std::string host;
    int port;
    std::string database;
    std::string user;
    std::string password;
    int maxConnections;
    bool enableSSL;
    std::vector<std::string> options;
    
    void show() const {
        std::cout << "Database Config:\n";
        std::cout << "  Host: " << host << "\n";
        std::cout << "  Port: " << port << "\n";
        std::cout << "  Database: " << database << "\n";
        std::cout << "  User: " << user << "\n";
        std::cout << "  Max Connections: " << maxConnections << "\n";
        std::cout << "  SSL: " << (enableSSL ? "enabled" : "disabled") << "\n";
    }
};

class DatabaseConfigBuilder {
private:
    DatabaseConfig config;
    
public:
    DatabaseConfigBuilder& host(const std::string& h) {
        config.host = h;
        return *this;
    }
    
    DatabaseConfigBuilder& port(int p) {
        config.port = p;
        return *this;
    }
    
    DatabaseConfigBuilder& database(const std::string& db) {
        config.database = db;
        return *this;
    }
    
    DatabaseConfigBuilder& user(const std::string& u) {
        config.user = u;
        return *this;
    }
    
    DatabaseConfigBuilder& password(const std::string& pwd) {
        config.password = pwd;
        return *this;
    }
    
    DatabaseConfigBuilder& maxConnections(int max) {
        config.maxConnections = max;
        return *this;
    }
    
    DatabaseConfigBuilder& enableSSL(bool enable) {
        config.enableSSL = enable;
        return *this;
    }
    
    DatabaseConfigBuilder& addOption(const std::string& option) {
        config.options.push_back(option);
        return *this;
    }
    
    DatabaseConfig build() {
        return config;
    }
};

int main() {
    DatabaseConfig config = DatabaseConfigBuilder()
        .host("localhost")
        .port(5432)
        .database("mydb")
        .user("admin")
        .password("secure_password")
        .maxConnections(100)
        .enableSSL(true)
        .addOption("timeout=30")
        .addOption("pool_size=10")
        .build();
    
    config.show();
    
    return 0;
}
```

---

## Object Pool Pattern

### What It Is

**Reuses objects** that are expensive to create by storing them in a pool.

### When to Use

- Database connections
- Thread pools
- Memory buffers
- When object creation is costly

### Implementation

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <queue>

class ExpensiveObject {
private:
    int id;
    
public:
    ExpensiveObject(int id) : id(id) {
        std::cout << "Creating expensive object " << id << "\n";
    }
    
    void use() {
        std::cout << "Using object " << id << "\n";
    }
    
    int getId() const { return id; }
};

class ObjectPool {
private:
    std::vector<std::unique_ptr<ExpensiveObject>> allObjects;
    std::queue<ExpensiveObject*> availableObjects;
    int nextId = 0;
    
public:
    ObjectPool(int initialSize) {
        for (int i = 0; i < initialSize; ++i) {
            auto obj = std::make_unique<ExpensiveObject>(nextId++);
            availableObjects.push(obj.get());
            allObjects.push_back(std::move(obj));
        }
    }
    
    ExpensiveObject* acquire() {
        if (availableObjects.empty()) {
            auto obj = std::make_unique<ExpensiveObject>(nextId++);
            availableObjects.push(obj.get());
            allObjects.push_back(std::move(obj));
        }
        
        auto obj = availableObjects.front();
        availableObjects.pop();
        return obj;
    }
    
    void release(ExpensiveObject* obj) {
        availableObjects.push(obj);
    }
};

int main() {
    ObjectPool pool(3);
    
    auto obj1 = pool.acquire();
    obj1->use();
    
    auto obj2 = pool.acquire();
    obj2->use();
    
    pool.release(obj1);
    
    auto obj3 = pool.acquire();  // Reuses obj1
    obj3->use();
    
    return 0;
}
```

---

## Prototype Pattern

### What It Is

Creates objects by **copying an existing object (prototype)** rather than creating from scratch.

### When to Use

- Cloning complex objects
- When object creation is expensive
- Undo/Redo functionality

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <map>

class Shape {
public:
    virtual ~Shape() = default;
    virtual std::unique_ptr<Shape> clone() = 0;
    virtual void draw() = 0;
};

class Circle : public Shape {
private:
    int radius;
    
public:
    Circle(int r) : radius(r) {}
    
    std::unique_ptr<Shape> clone() override {
        return std::make_unique<Circle>(*this);
    }
    
    void draw() override {
        std::cout << "Drawing circle with radius " << radius << "\n";
    }
    
    int getRadius() const { return radius; }
};

class Rectangle : public Shape {
private:
    int width, height;
    
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    
    std::unique_ptr<Shape> clone() override {
        return std::make_unique<Rectangle>(*this);
    }
    
    void draw() override {
        std::cout << "Drawing rectangle " << width << "x" << height << "\n";
    }
};

class ShapePrototypeRegistry {
private:
    std::map<std::string, std::unique_ptr<Shape>> prototypes;
    
public:
    void registerPrototype(const std::string& name, std::unique_ptr<Shape> prototype) {
        prototypes[name] = std::move(prototype);
    }
    
    std::unique_ptr<Shape> createShape(const std::string& name) {
        auto it = prototypes.find(name);
        if (it != prototypes.end()) {
            return it->second->clone();
        }
        return nullptr;
    }
};

int main() {
    ShapePrototypeRegistry registry;
    
    registry.registerPrototype("circle", std::make_unique<Circle>(5));
    registry.registerPrototype("rectangle", std::make_unique<Rectangle>(10, 20));
    
    auto circle = registry.createShape("circle");
    auto rectangle = registry.createShape("rectangle");
    
    circle->draw();
    rectangle->draw();
    
    auto circleCopy = circle->clone();
    circleCopy->draw();
    
    return 0;
}
```

---

## SECTION 3: STRUCTURAL PATTERNS

## Adapter Pattern

### What It Is

Converts the interface of a class into **another interface clients expect**. Allows incompatible classes to work together.

### When to Use

- Integrating legacy code
- Working with third-party libraries
- Incompatible interfaces

### Implementation

```cpp
#include <iostream>

// Existing interface (Target)
class ModernPrinter {
public:
    virtual ~ModernPrinter() = default;
    virtual void printDocument(const std::string& document) = 0;
};

// Old interface (Adaptee)
class LegacyPrinter {
public:
    void printLegacy(const std::string& text) {
        std::cout << "Legacy Printer: " << text << "\n";
    }
};

// Adapter
class PrinterAdapter : public ModernPrinter {
private:
    LegacyPrinter legacyPrinter;
    
public:
    void printDocument(const std::string& document) override {
        legacyPrinter.printLegacy(document);
    }
};

// Client
void usePrinter(ModernPrinter* printer) {
    printer->printDocument("Hello, World!");
}

int main() {
    PrinterAdapter adapter;
    usePrinter(&adapter);
    
    return 0;
}
```

---

## Decorator Pattern

### What It Is

Attaches additional responsibilities to an object **dynamically**. Provides a flexible alternative to subclassing.

### When to Use

- Adding features without modifying original class
- Multiple optional features
- Feature combinations

### Implementation

```cpp
#include <iostream>
#include <memory>

class Coffee {
public:
    virtual ~Coffee() = default;
    virtual std::string getDescription() = 0;
    virtual double getCost() = 0;
};

class BasicCoffee : public Coffee {
public:
    std::string getDescription() override {
        return "Basic Coffee";
    }
    
    double getCost() override {
        return 2.0;
    }
};

// Decorator base
class CoffeeDecorator : public Coffee {
protected:
    std::unique_ptr<Coffee> coffee;
    
public:
    CoffeeDecorator(std::unique_ptr<Coffee> c) : coffee(std::move(c)) {}
};

// Concrete decorators
class MilkDecorator : public CoffeeDecorator {
public:
    MilkDecorator(std::unique_ptr<Coffee> c) : CoffeeDecorator(std::move(c)) {}
    
    std::string getDescription() override {
        return coffee->getDescription() + ", Milk";
    }
    
    double getCost() override {
        return coffee->getCost() + 0.5;
    }
};

class SugarDecorator : public CoffeeDecorator {
public:
    SugarDecorator(std::unique_ptr<Coffee> c) : CoffeeDecorator(std::move(c)) {}
    
    std::string getDescription() override {
        return coffee->getDescription() + ", Sugar";
    }
    
    double getCost() override {
        return coffee->getCost() + 0.2;
    }
};

class VanillaDecorator : public CoffeeDecorator {
public:
    VanillaDecorator(std::unique_ptr<Coffee> c) : CoffeeDecorator(std::move(c)) {}
    
    std::string getDescription() override {
        return coffee->getDescription() + ", Vanilla";
    }
    
    double getCost() override {
        return coffee->getCost() + 0.7;
    }
};

int main() {
    auto coffee = std::make_unique<BasicCoffee>();
    std::cout << coffee->getDescription() << " - $" << coffee->getCost() << "\n";
    
    coffee = std::make_unique<MilkDecorator>(std::move(coffee));
    std::cout << coffee->getDescription() << " - $" << coffee->getCost() << "\n";
    
    coffee = std::make_unique<SugarDecorator>(std::move(coffee));
    std::cout << coffee->getDescription() << " - $" << coffee->getCost() << "\n";
    
    coffee = std::make_unique<VanillaDecorator>(std::move(coffee));
    std::cout << coffee->getDescription() << " - $" << coffee->getCost() << "\n";
    
    return 0;
}
```

---

## Facade Pattern

### What It Is

Provides a **unified, simplified interface** to a set of interfaces in a subsystem.

### When to Use

- Simplifying complex subsystems
- Reducing dependencies
- Decoupling client from complex components

### Implementation

```cpp
#include <iostream>

// Complex subsystem components
class CPU {
public:
    void start() { std::cout << "CPU started\n"; }
    void stop() { std::cout << "CPU stopped\n"; }
};

class Memory {
public:
    void initialize() { std::cout << "Memory initialized\n"; }
    void shutdown() { std::cout << "Memory shutdown\n"; }
};

class HardDrive {
public:
    void boot() { std::cout << "Hard drive boot\n"; }
    void shutdown() { std::cout << "Hard drive shutdown\n"; }
};

// Facade (simplified interface)
class Computer {
private:
    CPU cpu;
    Memory memory;
    HardDrive hardDrive;
    
public:
    void startup() {
        std::cout << "Computer startup...\n";
        memory.initialize();
        hardDrive.boot();
        cpu.start();
        std::cout << "Computer ready!\n";
    }
    
    void shutdown() {
        std::cout << "Computer shutdown...\n";
        cpu.stop();
        hardDrive.shutdown();
        memory.shutdown();
        std::cout << "Computer off!\n";
    }
};

int main() {
    Computer computer;
    
    computer.startup();
    std::cout << "\n";
    computer.shutdown();
    
    return 0;
}
```

---

## Proxy Pattern

### What It Is

Provides a **surrogate or placeholder** for another object to control access to it.

### When to Use

- Lazy initialization
- Access control
- Logging/caching
- Remote object access

### Implementation

```cpp
#include <iostream>
#include <memory>

class Subject {
public:
    virtual ~Subject() = default;
    virtual void request() = 0;
};

class RealSubject : public Subject {
public:
    void request() override {
        std::cout << "RealSubject handling request\n";
    }
};

class ProxySubject : public Subject {
private:
    std::unique_ptr<RealSubject> realSubject;
    
public:
    void request() override {
        std::cout << "ProxySubject: Logging access\n";
        
        // Lazy initialization
        if (!realSubject) {
            realSubject = std::make_unique<RealSubject>();
        }
        
        realSubject->request();
    }
};

int main() {
    auto proxy = std::make_unique<ProxySubject>();
    proxy->request();  // Creates RealSubject
    proxy->request();  // Reuses existing RealSubject
    
    return 0;
}
```

---

## Bridge Pattern

### What It Is

Decouples an abstraction from its implementation so they can vary **independently**.

### When to Use

- Platform-specific implementations
- Multiple ways to implement an interface
- Avoiding permanent binding between abstraction and implementation

### Implementation

```cpp
#include <iostream>
#include <memory>

// Implementation
class Color {
public:
    virtual ~Color() = default;
    virtual void fill() = 0;
};

class RedColor : public Color {
public:
    void fill() override { std::cout << "Filling with red\n"; }
};

class BlueColor : public Color {
public:
    void fill() override { std::cout << "Filling with blue\n"; }
};

// Abstraction
class Shape {
protected:
    std::shared_ptr<Color> color;
    
public:
    Shape(std::shared_ptr<Color> c) : color(c) {}
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

class Circle : public Shape {
public:
    Circle(std::shared_ptr<Color> c) : Shape(c) {}
    
    void draw() override {
        std::cout << "Drawing circle: ";
        color->fill();
    }
};

class Rectangle : public Shape {
public:
    Rectangle(std::shared_ptr<Color> c) : Shape(c) {}
    
    void draw() override {
        std::cout << "Drawing rectangle: ";
        color->fill();
    }
};

int main() {
    auto redColor = std::make_shared<RedColor>();
    auto blueColor = std::make_shared<BlueColor>();
    
    Circle circle(redColor);
    circle.draw();
    
    Rectangle rectangle(blueColor);
    rectangle.draw();
    
    return 0;
}
```

---

## Composite Pattern

### What It Is

Composes objects into **tree structures** to represent part-whole hierarchies. Allows clients to treat individual objects and compositions uniformly.

### When to Use

- File systems (files and directories)
- UI component hierarchies
- Organization structures

### Implementation

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Component {
public:
    virtual ~Component() = default;
    virtual void operation() = 0;
};

class Leaf : public Component {
private:
    std::string name;
    
public:
    Leaf(const std::string& n) : name(n) {}
    
    void operation() override {
        std::cout << "Leaf: " << name << "\n";
    }
};

class Composite : public Component {
private:
    std::string name;
    std::vector<std::shared_ptr<Component>> children;
    
public:
    Composite(const std::string& n) : name(n) {}
    
    void add(std::shared_ptr<Component> component) {
        children.push_back(component);
    }
    
    void operation() override {
        std::cout << "Composite: " << name << "\n";
        for (auto& child : children) {
            child->operation();
        }
    }
};

int main() {
    auto root = std::make_shared<Composite>("root");
    
    root->add(std::make_shared<Leaf>("leaf1"));
    root->add(std::make_shared<Leaf>("leaf2"));
    
    auto branch = std::make_shared<Composite>("branch");
    branch->add(std::make_shared<Leaf>("leaf3"));
    branch->add(std::make_shared<Leaf>("leaf4"));
    
    root->add(branch);
    
    root->operation();
    
    return 0;
}
```

---

## Flyweight Pattern

### What It Is

Uses **sharing to support large numbers of fine-grained objects** efficiently.

### When to Use

- Text editors (character objects)
- Game engines (particles)
- When many similar objects exist

### Implementation

```cpp
#include <iostream>
#include <map>
#include <memory>

class Glyph {
private:
    char character;
    
public:
    Glyph(char c) : character(c) {}
    
    void display(int x, int y) {
        std::cout << "Displaying '" << character << "' at (" << x << ", " << y << ")\n";
    }
};

class GlyphFactory {
private:
    std::map<char, std::shared_ptr<Glyph>> glyphs;
    
public:
    std::shared_ptr<Glyph> getGlyph(char c) {
        auto it = glyphs.find(c);
        if (it == glyphs.end()) {
            glyphs[c] = std::make_shared<Glyph>(c);
        }
        return glyphs[c];
    }
};

int main() {
    GlyphFactory factory;
    
    auto a = factory.getGlyph('A');
    auto a2 = factory.getGlyph('A');
    auto b = factory.getGlyph('B');
    
    std::cout << (a == a2 ? "Same object\n" : "Different objects\n");  // Same
    std::cout << (a == b ? "Same object\n" : "Different objects\n");   // Different
    
    a->display(0, 0);
    a2->display(1, 0);
    b->display(2, 0);
    
    return 0;
}
```

---

## SECTION 4: BEHAVIORAL PATTERNS

## Observer Pattern

### What It Is

Defines a **one-to-many dependency** between objects so that when one object changes state, all its dependents are notified automatically.

### When to Use

- Event handling systems
- MVC architectures
- Real-time data updates
- Publish-subscribe systems

### Implementation

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

class Subject {
private:
    std::vector<std::weak_ptr<Observer>> observers;
    
public:
    void attach(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
    }
    
    void notify(const std::string& message) {
        for (auto& weak_obs : observers) {
            if (auto obs = weak_obs.lock()) {
                obs->update(message);
            }
        }
    }
};

class ConcreteObserver : public Observer {
private:
    std::string name;
    
public:
    ConcreteObserver(const std::string& n) : name(n) {}
    
    void update(const std::string& message) override {
        std::cout << name << " received: " << message << "\n";
    }
};

int main() {
    auto subject = std::make_shared<Subject>();
    
    auto obs1 = std::make_shared<ConcreteObserver>("Observer 1");
    auto obs2 = std::make_shared<ConcreteObserver>("Observer 2");
    auto obs3 = std::make_shared<ConcreteObserver>("Observer 3");
    
    subject->attach(obs1);
    subject->attach(obs2);
    subject->attach(obs3);
    
    subject->notify("Event occurred!");
    
    return 0;
}
```

---

## Strategy Pattern

### What It Is

Defines a **family of algorithms**, encapsulates each one, and makes them interchangeable.

### When to Use

- Multiple algorithms for a task
- Avoiding conditional statements
- Runtime algorithm selection

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>

class SortingStrategy {
public:
    virtual ~SortingStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
};

class BubbleSort : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Sorting with Bubble Sort\n";
        // Implementation
    }
};

class QuickSort : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Sorting with Quick Sort\n";
        // Implementation
    }
};

class MergeSort : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Sorting with Merge Sort\n";
        // Implementation
    }
};

class Sorter {
private:
    std::unique_ptr<SortingStrategy> strategy;
    
public:
    void setStrategy(std::unique_ptr<SortingStrategy> s) {
        strategy = std::move(s);
    }
    
    void performSort(std::vector<int>& data) {
        if (strategy) {
            strategy->sort(data);
        }
    }
};

int main() {
    Sorter sorter;
    std::vector<int> data = {5, 2, 8, 1, 9};
    
    sorter.setStrategy(std::make_unique<BubbleSort>());
    sorter.performSort(data);
    
    sorter.setStrategy(std::make_unique<QuickSort>());
    sorter.performSort(data);
    
    sorter.setStrategy(std::make_unique<MergeSort>());
    sorter.performSort(data);
    
    return 0;
}
```

---

## Command Pattern

### What It Is

Encapsulates a **request as an object**, thereby allowing parameterization of clients with different requests, queuing of requests, and logging of requests.

### When to Use

- Undo/Redo functionality
- Queuing operations
- Macro recording
- Transaction systems

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class Light {
public:
    void turnOn() { std::cout << "Light is ON\n"; }
    void turnOff() { std::cout << "Light is OFF\n"; }
};

class TurnOnCommand : public Command {
private:
    Light& light;
    
public:
    TurnOnCommand(Light& l) : light(l) {}
    
    void execute() override {
        light.turnOn();
    }
    
    void undo() override {
        light.turnOff();
    }
};

class TurnOffCommand : public Command {
private:
    Light& light;
    
public:
    TurnOffCommand(Light& l) : light(l) {}
    
    void execute() override {
        light.turnOff();
    }
    
    void undo() override {
        light.turnOn();
    }
};

class RemoteControl {
private:
    std::vector<std::unique_ptr<Command>> history;
    
public:
    void press(std::unique_ptr<Command> command) {
        command->execute();
        history.push_back(std::move(command));
    }
    
    void undo() {
        if (!history.empty()) {
            history.back()->undo();
            history.pop_back();
        }
    }
};

int main() {
    Light light;
    RemoteControl remote;
    
    remote.press(std::make_unique<TurnOnCommand>(light));
    remote.press(std::make_unique<TurnOffCommand>(light));
    
    remote.undo();
    remote.undo();
    
    return 0;
}
```

---

## State Pattern

### What It Is

Allows an object to **alter its behavior** when its internal state changes. The object will appear to change its class.

### When to Use

- State machines
- Context-dependent behavior
- TCP connection states

### Implementation

```cpp
#include <iostream>
#include <memory>

class TrafficLightState {
public:
    virtual ~TrafficLightState() = default;
    virtual void next(class TrafficLight& light) = 0;
    virtual void display() = 0;
};

class RedState : public TrafficLightState {
public:
    void next(TrafficLight& light) override;
    void display() override { std::cout << "RED\n"; }
};

class GreenState : public TrafficLightState {
public:
    void next(TrafficLight& light) override;
    void display() override { std::cout << "GREEN\n"; }
};

class YellowState : public TrafficLightState {
public:
    void next(TrafficLight& light) override;
    void display() override { std::cout << "YELLOW\n"; }
};

class TrafficLight {
private:
    std::unique_ptr<TrafficLightState> state;
    
public:
    TrafficLight() : state(std::make_unique<RedState>()) {}
    
    void setState(std::unique_ptr<TrafficLightState> s) {
        state = std::move(s);
    }
    
    void change() {
        state->next(*this);
    }
    
    void display() {
        state->display();
    }
};

void RedState::next(TrafficLight& light) {
    light.setState(std::make_unique<GreenState>());
}

void GreenState::next(TrafficLight& light) {
    light.setState(std::make_unique<YellowState>());
}

void YellowState::next(TrafficLight& light) {
    light.setState(std::make_unique<RedState>());
}

int main() {
    TrafficLight light;
    
    for (int i = 0; i < 6; ++i) {
        light.display();
        light.change();
    }
    
    return 0;
}
```

---

## Template Method Pattern

### What It Is

Defines the **skeleton of an algorithm** in a method, deferring some steps to subclasses.

### When to Use

- Code reuse across related classes
- Framework design
- Avoiding code duplication

### Implementation

```cpp
#include <iostream>

class DataProcessor {
public:
    virtual ~DataProcessor() = default;
    
    // Template method
    void process() {
        readData();
        validate();
        transform();
        save();
    }
    
private:
    virtual void readData() = 0;
    virtual void validate() = 0;
    virtual void transform() = 0;
    virtual void save() = 0;
};

class CSVProcessor : public DataProcessor {
private:
    void readData() override { std::cout << "Reading CSV\n"; }
    void validate() override { std::cout << "Validating CSV\n"; }
    void transform() override { std::cout << "Transforming CSV\n"; }
    void save() override { std::cout << "Saving CSV\n"; }
};

class JSONProcessor : public DataProcessor {
private:
    void readData() override { std::cout << "Reading JSON\n"; }
    void validate() override { std::cout << "Validating JSON\n"; }
    void transform() override { std::cout << "Transforming JSON\n"; }
    void save() override { std::cout << "Saving JSON\n"; }
};

int main() {
    CSVProcessor csv;
    csv.process();
    
    std::cout << "\n";
    
    JSONProcessor json;
    json.process();
    
    return 0;
}
```

---

## Chain of Responsibility Pattern

### What It Is

Passes a request along a **chain of handlers**, where each handler decides to process it or pass it along.

### When to Use

- Event handling systems
- Logging levels
- Request approval workflows

### Implementation

```cpp
#include <iostream>
#include <memory>

class Handler {
protected:
    std::unique_ptr<Handler> nextHandler;
    
public:
    virtual ~Handler() = default;
    
    void setNext(std::unique_ptr<Handler> h) {
        nextHandler = std::move(h);
    }
    
    virtual void handle(int request) {
        if (nextHandler) {
            nextHandler->handle(request);
        }
    }
};

class ConcreteHandlerA : public Handler {
public:
    void handle(int request) override {
        if (request < 10) {
            std::cout << "Handler A handling " << request << "\n";
        } else {
            Handler::handle(request);
        }
    }
};

class ConcreteHandlerB : public Handler {
public:
    void handle(int request) override {
        if (request < 20) {
            std::cout << "Handler B handling " << request << "\n";
        } else {
            Handler::handle(request);
        }
    }
};

class ConcreteHandlerC : public Handler {
public:
    void handle(int request) override {
        std::cout << "Handler C handling " << request << "\n";
    }
};

int main() {
    auto handlerA = std::make_unique<ConcreteHandlerA>();
    auto handlerB = std::make_unique<ConcreteHandlerB>();
    auto handlerC = std::make_unique<ConcreteHandlerC>();
    
    handlerA->setNext(std::move(handlerB));
    handlerA->setNext(std::make_unique<ConcreteHandlerC>());
    
    handlerA->handle(5);   // Handler A
    handlerA->handle(15);  // Handler B
    handlerA->handle(25);  // Handler C
    
    return 0;
}
```

---

## Iterator Pattern

### What It Is

Provides a **way to access elements** of an aggregate object **sequentially** without exposing its underlying representation.

### When to Use

- Traversing different collections uniformly
- Multiple simultaneous traversals
- Hiding collection structure

### Implementation

```cpp
#include <iostream>
#include <vector>
#include <memory>

template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() = 0;
    virtual T next() = 0;
};

template<typename T>
class Collection {
public:
    virtual ~Collection() = default;
    virtual std::unique_ptr<Iterator<T>> createIterator() = 0;
};

template<typename T>
class VectorIterator : public Iterator<T> {
private:
    std::vector<T>& container;
    size_t index = 0;
    
public:
    VectorIterator(std::vector<T>& c) : container(c) {}
    
    bool hasNext() override {
        return index < container.size();
    }
    
    T next() override {
        return container[index++];
    }
};

template<typename T>
class VectorCollection : public Collection<T> {
private:
    std::vector<T> data;
    
public:
    void add(const T& item) {
        data.push_back(item);
    }
    
    std::unique_ptr<Iterator<T>> createIterator() override {
        return std::make_unique<VectorIterator<T>>(data);
    }
};

int main() {
    VectorCollection<int> collection;
    collection.add(1);
    collection.add(2);
    collection.add(3);
    
    auto it = collection.createIterator();
    while (it->hasNext()) {
        std::cout << it->next() << " ";
    }
    
    return 0;
}
```

---

## Mediator Pattern

### What It Is

Defines an object that **encapsulates how objects interact**, promoting loose coupling.

### When to Use

- Complex inter-object communication
- Dialog boxes with many controls
- Chat rooms

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>

class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void sendMessage(const std::string& message, class Colleague* sender) = 0;
};

class Colleague {
protected:
    Mediator* mediator;
    
public:
    Colleague(Mediator* m) : mediator(m) {}
    virtual ~Colleague() = default;
    virtual void receiveMessage(const std::string& message) = 0;
    virtual void sendMessage(const std::string& message) = 0;
};

class ConcreteColleague : public Colleague {
private:
    std::string name;
    
public:
    ConcreteColleague(Mediator* m, const std::string& n) : Colleague(m), name(n) {}
    
    void receiveMessage(const std::string& message) override {
        std::cout << name << " received: " << message << "\n";
    }
    
    void sendMessage(const std::string& message) override {
        mediator->sendMessage(message, this);
    }
};

class ConcreteMediator : public Mediator {
private:
    std::vector<Colleague*> colleagues;
    
public:
    void registerColleague(Colleague* c) {
        colleagues.push_back(c);
    }
    
    void sendMessage(const std::string& message, Colleague* sender) override {
        for (auto colleague : colleagues) {
            if (colleague != sender) {
                colleague->receiveMessage(message);
            }
        }
    }
};

int main() {
    ConcreteMediator mediator;
    
    ConcreteColleague user1(&mediator, "User1");
    ConcreteColleague user2(&mediator, "User2");
    ConcreteColleague user3(&mediator, "User3");
    
    mediator.registerColleague(&user1);
    mediator.registerColleague(&user2);
    mediator.registerColleague(&user3);
    
    user1.sendMessage("Hello everyone!");
    
    return 0;
}
```

---

## Memento Pattern

### What It Is

Captures and externalizes an object's **internal state** without violating encapsulation, allowing the object to be restored to this state later.

### When to Use

- Undo/Redo functionality
- Snapshots
- Transactional rollback

### Implementation

```cpp
#include <iostream>
#include <string>
#include <vector>

class Memento {
private:
    std::string state;
    
public:
    Memento(const std::string& s) : state(s) {}
    
    std::string getState() const { return state; }
};

class Originator {
private:
    std::string state;
    
public:
    void setState(const std::string& s) {
        std::cout << "Setting state to " << s << "\n";
        state = s;
    }
    
    Memento createMemento() const {
        return Memento(state);
    }
    
    void restoreFromMemento(const Memento& m) {
        state = m.getState();
        std::cout << "Restored state to " << state << "\n";
    }
    
    void show() const {
        std::cout << "Current state: " << state << "\n";
    }
};

class Caretaker {
private:
    std::vector<Memento> history;
    
public:
    void saveState(const Memento& m) {
        history.push_back(m);
    }
    
    Memento getState(size_t index) {
        return history[index];
    }
};

int main() {
    Originator originator;
    Caretaker caretaker;
    
    originator.setState("State1");
    caretaker.saveState(originator.createMemento());
    
    originator.setState("State2");
    caretaker.saveState(originator.createMemento());
    
    originator.setState("State3");
    originator.show();
    
    originator.restoreFromMemento(caretaker.getState(0));
    
    return 0;
}
```

---

## Visitor Pattern

### What It Is

Represents an **operation to be performed** on elements of an object structure. Lets you define a new operation **without changing** the classes of the elements.

### When to Use

- Complex operations on object structures
- Multiple unrelated operations
- Avoiding type casting

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>

class Visitor;

class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor* visitor) = 0;
};

class ConcreteElementA : public Element {
public:
    void accept(Visitor* visitor) override;
    
    void operationA() {
        std::cout << "Operation A\n";
    }
};

class ConcreteElementB : public Element {
public:
    void accept(Visitor* visitor) override;
    
    void operationB() {
        std::cout << "Operation B\n";
    }
};

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitElementA(ConcreteElementA* element) = 0;
    virtual void visitElementB(ConcreteElementB* element) = 0;
};

class ConcreteVisitor : public Visitor {
public:
    void visitElementA(ConcreteElementA* element) override {
        std::cout << "Visiting Element A\n";
        element->operationA();
    }
    
    void visitElementB(ConcreteElementB* element) override {
        std::cout << "Visiting Element B\n";
        element->operationB();
    }
};

void ConcreteElementA::accept(Visitor* visitor) {
    visitor->visitElementA(this);
}

void ConcreteElementB::accept(Visitor* visitor) {
    visitor->visitElementB(this);
}

int main() {
    std::vector<std::unique_ptr<Element>> elements;
    elements.push_back(std::make_unique<ConcreteElementA>());
    elements.push_back(std::make_unique<ConcreteElementB>());
    
    ConcreteVisitor visitor;
    
    for (auto& element : elements) {
        element->accept(&visitor);
    }
    
    return 0;
}
```

---

## Interpreter Pattern

### What It Is

Defines a **representation of a grammar** and an interpreter to interpret sentences in the language.

### When to Use

- SQL/expression interpreters
- Domain-specific languages
- Rule engines

### Implementation

```cpp
#include <iostream>
#include <string>
#include <memory>
#include <map>

class Context {
private:
    std::map<std::string, int> variables;
    
public:
    void setVariable(const std::string& name, int value) {
        variables[name] = value;
    }
    
    int getVariable(const std::string& name) {
        return variables[name];
    }
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(Context& context) = 0;
};

class Number : public Expression {
private:
    int value;
    
public:
    Number(int v) : value(v) {}
    
    int interpret(Context& context) override {
        return value;
    }
};

class Variable : public Expression {
private:
    std::string name;
    
public:
    Variable(const std::string& n) : name(n) {}
    
    int interpret(Context& context) override {
        return context.getVariable(name);
    }
};

class Add : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
public:
    Add(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    
    int interpret(Context& context) override {
        return left->interpret(context) + right->interpret(context);
    }
};

int main() {
    Context context;
    context.setVariable("x", 5);
    context.setVariable("y", 3);
    
    // x + y = 5 + 3 = 8
    auto expr = std::make_unique<Add>(
        std::make_unique<Variable>("x"),
        std::make_unique<Variable>("y")
    );
    
    std::cout << "Result: " << expr->interpret(context) << "\n";
    
    return 0;
}
```

---

## SECTION 5: CONCURRENCY PATTERNS

## Active Object Pattern

### What It Is

Decouples method execution from method invocation to enhance concurrency.

### Implementation

```cpp
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

class ActiveObject {
private:
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;
    std::thread worker;
    
    void workerThread() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !tasks.empty() || !running; });
            
            if (!running && tasks.empty()) break;
            
            if (!tasks.empty()) {
                auto task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
            }
        }
    }
    
public:
    ActiveObject() : worker(&ActiveObject::workerThread, this) {}
    
    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            running = false;
        }
        cv.notify_one();
        worker.join();
    }
    
    template<typename F>
    void enqueue(F&& func) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(std::forward<F>(func));
        }
        cv.notify_one();
    }
};

int main() {
    ActiveObject obj;
    
    obj.enqueue([] { std::cout << "Task 1\n"; });
    obj.enqueue([] { std::cout << "Task 2\n"; });
    obj.enqueue([] { std::cout << "Task 3\n"; });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return 0;
}
```

---

## Monitor Object Pattern

### What It Is

Synchronizes concurrent method execution to ensure only one method runs at a time within an object.

### Implementation

```cpp
#include <iostream>
#include <mutex>
#include <thread>

class MonitorObject {
private:
    int value = 0;
    mutable std::mutex mtx;
    
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        value++;
    }
    
    int getValue() const {
        std::lock_guard<std::mutex> lock(mtx);
        return value;
    }
};

int main() {
    MonitorObject obj;
    
    std::thread t1([&obj] {
        for (int i = 0; i < 1000; ++i) {
            obj.increment();
        }
    });
    
    std::thread t2([&obj] {
        for (int i = 0; i < 1000; ++i) {
            obj.increment();
        }
    });
    
    t1.join();
    t2.join();
    
    std::cout << "Final value: " << obj.getValue() << "\n";  // 2000
    
    return 0;
}
```

---

## Thread Pool Pattern

### What It Is

Maintains multiple threads waiting for tasks to be allocated for concurrent execution.

### Implementation

```cpp
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <memory>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;
    
    void workerThread() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !tasks.empty() || !running; });
            
            if (!running && tasks.empty()) break;
            
            if (!tasks.empty()) {
                auto task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
            }
        }
    }
    
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back(&ThreadPool::workerThread, this);
        }
    }
    
    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            running = false;
        }
        cv.notify_all();
        for (auto& w : workers) {
            w.join();
        }
    }
    
    template<typename F>
    void enqueue(F&& func) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(std::forward<F>(func));
        }
        cv.notify_one();
    }
};

int main() {
    ThreadPool pool(4);
    
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " running\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    return 0;
}
```

---

## Lock-Free Pattern

### What It Is

Uses **atomic operations** instead of locks for thread-safe access.

### Implementation

```cpp
#include <iostream>
#include <atomic>
#include <thread>

class LockFreeCounter {
private:
    std::atomic<int> value{0};
    
public:
    void increment() {
        value.fetch_add(1, std::memory_order_acq_rel);
    }
    
    int get() const {
        return value.load(std::memory_order_acquire);
    }
};

int main() {
    LockFreeCounter counter;
    
    std::thread t1([&counter] {
        for (int i = 0; i < 1000000; ++i) {
            counter.increment();
        }
    });
    
    std::thread t2([&counter] {
        for (int i = 0; i < 1000000; ++i) {
            counter.increment();
        }
    });
    
    t1.join();
    t2.join();
    
    std::cout << "Final value: " << counter.get() << "\n";  // 2000000
    
    return 0;
}
```

---

## SECTION 6: ARCHITECTURAL PATTERNS

## MVC Pattern

### What It Is

Separates application into **Model** (data), **View** (presentation), and **Controller** (logic).

### Implementation

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Model
class Model {
private:
    std::string data;
    std::vector<class Observer*> observers;
    
public:
    void setData(const std::string& d) {
        data = d;
        notifyObservers();
    }
    
    std::string getData() const { return data; }
    
    void attach(Observer* obs) { observers.push_back(obs); }
    
    void notifyObservers();
};

// View
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& data) = 0;
};

class View : public Observer {
public:
    void update(const std::string& data) override {
        std::cout << "View displaying: " << data << "\n";
    }
};

// Controller
class Controller {
private:
    Model& model;
    View& view;
    
public:
    Controller(Model& m, View& v) : model(m), view(v) {
        model.attach(&view);
    }
    
    void updateModel(const std::string& data) {
        model.setData(data);
    }
};

void Model::notifyObservers() {
    for (auto obs : observers) {
        obs->update(data);
    }
}

int main() {
    Model model;
    View view;
    Controller controller(model, view);
    
    controller.updateModel("Hello MVC");
    
    return 0;
}
```

---

## Repository Pattern

### What It Is

Provides an abstraction for data access, isolating business logic from data mapping layers.

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <map>

struct User {
    int id;
    std::string name;
    std::string email;
};

class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void add(const User& user) = 0;
    virtual User* find(int id) = 0;
    virtual std::vector<User> findAll() = 0;
};

class InMemoryRepository : public IRepository {
private:
    std::map<int, User> storage;
    
public:
    void add(const User& user) override {
        storage[user.id] = user;
    }
    
    User* find(int id) override {
        auto it = storage.find(id);
        return it != storage.end() ? &it->second : nullptr;
    }
    
    std::vector<User> findAll() override {
        std::vector<User> users;
        for (auto& pair : storage) {
            users.push_back(pair.second);
        }
        return users;
    }
};

int main() {
    InMemoryRepository repo;
    
    repo.add({1, "Alice", "alice@example.com"});
    repo.add({2, "Bob", "bob@example.com"});
    
    auto user = repo.find(1);
    if (user) {
        std::cout << "Found: " << user->name << "\n";
    }
    
    return 0;
}
```

---

## Dependency Injection

### What It Is

Injects **dependencies** into objects rather than having them create their own.

### Implementation

```cpp
#include <iostream>
#include <memory>

class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& message) = 0;
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "LOG: " << message << "\n";
    }
};

class Service {
private:
    std::shared_ptr<Logger> logger;
    
public:
    // Constructor injection
    Service(std::shared_ptr<Logger> log) : logger(log) {}
    
    void doWork() {
        logger->log("Doing work...");
    }
};

int main() {
    auto logger = std::make_shared<ConsoleLogger>();
    Service service(logger);
    
    service.doWork();
    
    return 0;
}
```

---

## SECTION 7: ADVANCED & MODERN PATTERNS

## CRTP Pattern

### What It Is

**Curiously Recurring Template Pattern** enables static polymorphism without virtual functions.

### Implementation

```cpp
#include <iostream>

template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};

int main() {
    Derived d;
    d.interface();
    
    return 0;
}
```

---

## Type Erasure

### What It Is

Hides type information at compile time while maintaining functionality.

### Implementation

```cpp
#include <iostream>
#include <memory>
#include <vector>

class TypeErasure {
private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void call() = 0;
    };
    
    template<typename T>
    struct Model : Concept {
        T value;
        Model(T v) : value(v) {}
        void call() override {
            value();
        }
    };
    
    std::unique_ptr<Concept> object;
    
public:
    template<typename T>
    TypeErasure(T&& t) : object(std::make_unique<Model<T>>(std::forward<T>(t))) {}
    
    void call() {
        object->call();
    }
};

int main() {
    std::vector<TypeErasure> functions;
    
    functions.emplace_back([] { std::cout << "Lambda 1\n"; });
    functions.emplace_back([] { std::cout << "Lambda 2\n"; });
    
    for (auto& f : functions) {
        f.call();
    }
    
    return 0;
}
```

---

## SECTION 8: IDIOMS & BEST PRACTICES

## RAII Idiom

### What It Is

**Resource Acquisition Is Initialization** - Resources are tied to object lifetime.

### Implementation

```cpp
#include <iostream>
#include <fstream>

class FileHandler {
private:
    std::ofstream file;
    
public:
    FileHandler(const std::string& filename) {
        file.open(filename);
        if (!file) throw std::runtime_error("Cannot open file");
    }
    
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void write(const std::string& data) {
        file << data << "\n";
    }
};

int main() {
    {
        FileHandler handler("output.txt");
        handler.write("Hello, World!");
    }  // File automatically closed
    
    return 0;
}
```

---

## PIMPL Idiom

### What It Is

**Pointer To Implementation** - Hides implementation details and reduces compilation dependencies.

### Implementation

```cpp
// header.h
#include <memory>
#include <string>

class MyClass {
private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
    
public:
    MyClass();
    ~MyClass();
    void doSomething();
    void setValue(const std::string& value);
};

// source.cpp
#include "header.h"

struct MyClass::Impl {
    std::string value;
    void doSomethingImpl() {
        // Implementation
    }
};

MyClass::MyClass() : pimpl(std::make_unique<Impl>()) {}

MyClass::~MyClass() = default;

void MyClass::doSomething() {
    pimpl->doSomethingImpl();
}

void MyClass::setValue(const std::string& value) {
    pimpl->value = value;
}
```

---

## Copy-and-Swap Idiom

### What It Is

Provides **strong exception guarantee** for assignment operations.

### Implementation

```cpp
#include <iostream>
#include <utility>

class Vector {
private:
    int* data;
    size_t size;
    
public:
    Vector(size_t s) : size(s) {
        data = new int[size];
    }
    
    ~Vector() {
        delete[] data;
    }
    
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
    }
    
    Vector& operator=(Vector temp) {
        swap(*this, temp);
        return *this;
    }
    
    friend void swap(Vector& a, Vector& b) {
        using std::swap;
        swap(a.data, b.data);
        swap(a.size, b.size);
    }
};
```

---

## SECTION 9: ANTI-PATTERNS

## Common Anti-Patterns to Avoid

```cpp
// 1. GOD OBJECT - Class doing too much
class GODObject {
    // Thousands of methods
};

// 2. CIRCULAR DEPENDENCIES
class A { B b; };
class B { A a; };

// 3. SPAGHETTI CODE - No clear structure
void process() {
    // 1000 lines of tangled logic
}

// 4. MEMORY LEAKS - Manual memory management
void leak() {
    int* ptr = new int(5);
    // Forgot to delete
}

// 5. TIGHT COUPLING - Classes depend on concrete implementations
class BadService {
    ConcreteDatabase database;  // Tightly coupled
};

// 6. MAGIC NUMBERS - Unexplained constants
if (x > 42) { }  // What does 42 mean?

// 7. FEATURE ENVY - Class using another's methods excessively
void process() {
    other.getA();
    other.getB();
    other.getC();
    // Should be a method in 'other'
}
```

---

## FINAL COMPREHENSIVE PATTERN CHECKLIST

### Creational Patterns (6)
- ☐ Singleton
- ☐ Factory Method
- ☐ Abstract Factory
- ☐ Builder
- ☐ Object Pool
- ☐ Prototype

### Structural Patterns (7)
- ☐ Adapter
- ☐ Decorator
- ☐ Facade
- ☐ Proxy
- ☐ Bridge
- ☐ Composite
- ☐ Flyweight

### Behavioral Patterns (11)
- ☐ Observer
- ☐ Strategy
- ☐ Command
- ☐ State
- ☐ Template Method
- ☐ Chain of Responsibility
- ☐ Iterator
- ☐ Mediator
- ☐ Memento
- ☐ Visitor
- ☐ Interpreter

### Concurrency Patterns (4)
- ☐ Active Object
- ☐ Monitor Object
- ☐ Thread Pool
- ☐ Lock-Free

### Architectural Patterns (5)
- ☐ MVC
- ☐ MVVM
- ☐ Repository
- ☐ Service Locator
- ☐ Dependency Injection

### Advanced Patterns (4)
- ☐ CRTP
- ☐ Type Erasure
- ☐ Expression Templates
- ☐ Policy-Based Design

### C++ Idioms (4)
- ☐ RAII
- ☐ PIMPL
- ☐ Copy-and-Swap
- ☐ SFINAE

---

## When to Use Each Pattern

| Problem | Solution |
|---------|----------|
| Object creation too complex | Builder, Factory Method, Abstract Factory |
| Need to add features dynamically | Decorator |
| Multiple algorithms for same task | Strategy |
| Object state changes behavior | State |
| Need to undo operations | Memento, Command |
| Complex inter-object communication | Observer, Mediator |
| Access to complex subsystem | Facade |
| Need for many similar objects | Flyweight |
| Multiple implementations needed | Bridge, Strategy |
| Platform-specific code | Abstract Factory |

---

## Best Practices

1. **Don't use patterns for the sake of it** - Only use when needed
2. **Keep it simple** - Use simplest pattern that solves the problem
3. **Combine patterns** - Patterns often work well together
4. **Know the costs** - Some patterns add complexity
5. **Refactor to patterns** - Don't force patterns into existing code
6. **Document patterns** - Make it clear which pattern is used
7. **Use modern C++** - Leverage smart pointers, lambdas, templates
8. **Avoid overengineering** - YAGNI (You Aren't Gonna Need It)

---

**Master these 40+ patterns and you'll write professional, maintainable C++ code!** 🚀

*Last Updated: December 2025*
*C++ Version: C++11 and later*
