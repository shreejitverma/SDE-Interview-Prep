# The Ultimate Python Design Patterns Guide: From Basics to Advanced Mastery

## Table of Contents

### SECTION 1: DESIGN PATTERNS FUNDAMENTALS
1. [What Are Design Patterns?](#what-are-design-patterns)
2. [Why Python Design Patterns?](#why-python-design-patterns)
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
28. [Thread Pool Pattern](#thread-pool-pattern)
29. [Producer-Consumer Pattern](#producer-consumer-pattern)
30. [Reactor Pattern](#reactor-pattern)
31. [Active Object Pattern](#active-object-pattern)

### SECTION 6: ASYNCHRONOUS PATTERNS
32. [Async/Await Pattern](#asyncawait-pattern)
33. [Future/Promise Pattern](#futurepromise-pattern)
34. [Callback Pattern](#callback-pattern)

### SECTION 7: PYTHONIC PATTERNS & IDIOMS
35. [Decorator (Python Decorator)](#python-decorator-idiom)
36. [Context Manager](#context-manager-idiom)
37. [Property Pattern](#property-pattern)
38. [Descriptor Pattern](#descriptor-pattern)
39. [Metaclass Pattern](#metaclass-pattern)
40. [Generator & Iterator Protocol](#generator--iterator-protocol)
41. [Closure Pattern](#closure-pattern)

### SECTION 8: ARCHITECTURAL PATTERNS
42. [MVC (Model-View-Controller)](#mvc-pattern)
43. [Repository Pattern](#repository-pattern)
44. [Dependency Injection](#dependency-injection)

### SECTION 9: ANTI-PATTERNS
45. [Common Anti-Patterns](#anti-patterns)

---

## SECTION 1: DESIGN PATTERNS FUNDAMENTALS

## What Are Design Patterns?

Design patterns are **reusable solutions to common problems** in software design. They provide:

- A **common vocabulary** for developers (e.g., "Let's use a Strategy here")
- **Proven solutions** to recurring design problems
- **Better code structure** and maintainability
- **Flexibility** and extensibility

Think of them as **templates or blueprints** for solving design problems, not code you copy-paste.

---

## Why Python Design Patterns?

Python is different from Java/C++:

- **Highly flexible** - multiple paradigms (OOP, functional, procedural)
- **First-class functions** - many patterns become simpler
- **Protocols not inheritance** - duck typing reduces pattern complexity
- **Built-in abstractions** - decorators, context managers, generators
- **Dynamic typing** - some patterns are implicit in Python

**Result:** Many classic GoF patterns are **simpler in Python** or **not needed** because Python's features handle them natively.

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
├─ Thread Pool
├─ Producer-Consumer
├─ Reactor
└─ Active Object

ASYNCHRONOUS (3 patterns)
├─ Async/Await
├─ Future/Promise
└─ Callback

PYTHONIC (7 patterns)
├─ Decorator
├─ Context Manager
├─ Property
├─ Descriptor
├─ Metaclass
├─ Generator Protocol
└─ Closure
```

---

## SECTION 2: CREATIONAL PATTERNS

## Singleton Pattern

### What It Is

Ensures a class has **only one instance** and provides a **global access point**.

### When to Use

- Logging, configuration, database connections
- But use sparingly—can hurt testability

### Implementation 1: Class-Based (Classic)

```python
class Singleton:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

# Usage
s1 = Singleton()
s2 = Singleton()
assert s1 is s2  # Same instance
```

### Implementation 2: Decorator-Based (Pythonic)

```python
def singleton(cls):
    instances = {}
    def get_instance(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]
    return get_instance

@singleton
class Logger:
    def __init__(self):
        self.logs = []
    
    def log(self, message):
        self.logs.append(message)

# Usage
logger1 = Logger()
logger2 = Logger()
assert logger1 is logger2
```

### Implementation 3: Module-Level (Most Pythonic)

```python
# config.py
class Config:
    def __init__(self):
        self.host = "localhost"
        self.port = 5432

_config = Config()

def get_config():
    return _config

# elsewhere
from config import get_config
config = get_config()
```

### Real-World Example: Application Logger

```python
import logging
from datetime import datetime

class AppLogger:
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._init_logger()
        return cls._instance
    
    def _init_logger(self):
        self.logger = logging.getLogger('app')
        handler = logging.FileHandler('app.log')
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
    
    def info(self, message):
        self.logger.info(message)
    
    def error(self, message):
        self.logger.error(message)

# Usage
logger = AppLogger()
logger.info("Application started")
logger.error("Something went wrong")
```

---

## Factory Method Pattern

### What It Is

Creates objects **without specifying exact classes**. Uses a method to instantiate objects based on parameters.

### When to Use

- Decouple object creation from usage
- Object type determined at runtime
- Creating related objects

### Implementation

```python
from abc import ABC, abstractmethod

class Transport(ABC):
    @abstractmethod
    def deliver(self):
        pass

class Truck(Transport):
    def deliver(self):
        return "Delivering by land"

class Ship(Transport):
    def deliver(self):
        return "Delivering by sea"

class Plane(Transport):
    def deliver(self):
        return "Delivering by air"

class Logistics(ABC):
    @abstractmethod
    def create_transport(self) -> Transport:
        pass
    
    def plan_delivery(self):
        transport = self.create_transport()
        return transport.deliver()

class RoadLogistics(Logistics):
    def create_transport(self) -> Transport:
        return Truck()

class SeaLogistics(Logistics):
    def create_transport(self) -> Transport:
        return Ship()

class AirLogistics(Logistics):
    def create_transport(self) -> Transport:
        return Plane()

# Usage
def client_code(logistics: Logistics):
    print(logistics.plan_delivery())

client_code(RoadLogistics())
client_code(SeaLogistics())
client_code(AirLogistics())
```

### Simple Function-Based Factory (Most Pythonic)

```python
def create_transport(mode: str) -> Transport:
    """Factory function - simpler than class-based"""
    if mode == "road":
        return Truck()
    elif mode == "sea":
        return Ship()
    elif mode == "air":
        return Plane()
    else:
        raise ValueError(f"Unknown mode: {mode}")

# Usage
transport = create_transport("sea")
print(transport.deliver())
```

---

## Abstract Factory Pattern

### What It Is

Provides an interface to create **families of related objects** without specifying concrete classes.

### When to Use

- Product families (Windows UI vs Mac UI)
- Supporting multiple implementations
- Platform-specific code

### Implementation

```python
from abc import ABC, abstractmethod

# Abstract Products
class Button(ABC):
    @abstractmethod
    def render(self):
        pass

class Checkbox(ABC):
    @abstractmethod
    def render(self):
        pass

# Concrete Products (Windows)
class WindowsButton(Button):
    def render(self):
        return "Render Windows Button"

class WindowsCheckbox(Checkbox):
    def render(self):
        return "Render Windows Checkbox"

# Concrete Products (Mac)
class MacButton(Button):
    def render(self):
        return "Render Mac Button"

class MacCheckbox(Checkbox):
    def render(self):
        return "Render Mac Checkbox"

# Abstract Factory
class GUIFactory(ABC):
    @abstractmethod
    def create_button(self) -> Button:
        pass
    
    @abstractmethod
    def create_checkbox(self) -> Checkbox:
        pass

# Concrete Factories
class WindowsFactory(GUIFactory):
    def create_button(self) -> Button:
        return WindowsButton()
    
    def create_checkbox(self) -> Checkbox:
        return WindowsCheckbox()

class MacFactory(GUIFactory):
    def create_button(self) -> Button:
        return MacButton()
    
    def create_checkbox(self) -> Checkbox:
        return MacCheckbox()

# Client Code
def build_ui(factory: GUIFactory):
    button = factory.create_button()
    checkbox = factory.create_checkbox()
    print(button.render())
    print(checkbox.render())

# Usage
import sys
factory = WindowsFactory() if sys.platform.startswith('win') else MacFactory()
build_ui(factory)
```

---

## Builder Pattern

### What It Is

Constructs complex objects **step by step**, separating construction from representation.

### When to Use

- Objects with many optional parameters
- Complex construction process
- Readability and maintainability

### Implementation 1: Classic Builder

```python
class House:
    def __init__(self):
        self.rooms = []
        self.has_garage = False
        self.has_pool = False
        self.has_garden = False
    
    def __repr__(self):
        return f"House: {len(self.rooms)} rooms, garage={self.has_garage}, pool={self.has_pool}"

class HouseBuilder:
    def __init__(self):
        self.house = House()
    
    def add_room(self, room_type: str):
        self.house.rooms.append(room_type)
        return self
    
    def add_garage(self):
        self.house.has_garage = True
        return self
    
    def add_pool(self):
        self.house.has_pool = True
        return self
    
    def add_garden(self):
        self.house.has_garden = True
        return self
    
    def build(self):
        return self.house

# Usage (Fluent Interface)
house = (HouseBuilder()
    .add_room("Kitchen")
    .add_room("Bedroom")
    .add_room("Living Room")
    .add_garage()
    .add_pool()
    .build())

print(house)
```

### Implementation 2: Using Dataclass (Modern Python)

```python
from dataclasses import dataclass
from typing import Optional

@dataclass
class DatabaseConfig:
    host: str
    port: int
    user: str
    password: str
    database: str
    pool_size: int = 10
    timeout: int = 30
    ssl_enabled: bool = False

class DatabaseConfigBuilder:
    def __init__(self):
        self.host = "localhost"
        self.port = 5432
        self.user = "admin"
        self.password = ""
        self.database = "mydb"
        self.pool_size = 10
        self.timeout = 30
        self.ssl_enabled = False
    
    def set_host(self, host: str):
        self.host = host
        return self
    
    def set_port(self, port: int):
        self.port = port
        return self
    
    def set_credentials(self, user: str, password: str):
        self.user = user
        self.password = password
        return self
    
    def set_database(self, database: str):
        self.database = database
        return self
    
    def set_pool_size(self, size: int):
        self.pool_size = size
        return self
    
    def set_timeout(self, timeout: int):
        self.timeout = timeout
        return self
    
    def enable_ssl(self):
        self.ssl_enabled = True
        return self
    
    def build(self) -> DatabaseConfig:
        return DatabaseConfig(
            host=self.host,
            port=self.port,
            user=self.user,
            password=self.password,
            database=self.database,
            pool_size=self.pool_size,
            timeout=self.timeout,
            ssl_enabled=self.ssl_enabled
        )

# Usage
config = (DatabaseConfigBuilder()
    .set_host("db.example.com")
    .set_port(5432)
    .set_credentials("admin", "secure_password")
    .set_database("production_db")
    .set_pool_size(50)
    .enable_ssl()
    .build())

print(config)
```

---

## Object Pool Pattern

### What It Is

Reuses objects that are **expensive to create** by storing them in a pool.

### When to Use

- Database connections
- Thread pools
- Expensive resource initialization

### Implementation

```python
import queue
import threading

class ExpensiveResource:
    def __init__(self, resource_id):
        self.resource_id = resource_id
        print(f"Creating expensive resource {resource_id}")
    
    def use(self):
        print(f"Using resource {self.resource_id}")

class ResourcePool:
    def __init__(self, size: int, factory):
        self._pool = queue.Queue(maxsize=size)
        self._lock = threading.Lock()
        self._factory = factory
        
        for i in range(size):
            self._pool.put(factory(i))
    
    def acquire(self):
        return self._pool.get()
    
    def release(self, resource):
        self._pool.put(resource)

# Usage
pool = ResourcePool(3, ExpensiveResource)

resource = pool.acquire()
resource.use()
pool.release(resource)

# Same resource reused
resource2 = pool.acquire()
resource2.use()
pool.release(resource2)
```

---

## Prototype Pattern

### What It Is

Creates objects by **copying an existing prototype** rather than creating from scratch.

### When to Use

- Cloning complex objects
- Avoiding expensive initialization
- Undo/Redo functionality

### Implementation

```python
import copy
from abc import ABC, abstractmethod

class Shape(ABC):
    @abstractmethod
    def clone(self):
        pass
    
    @abstractmethod
    def draw(self):
        pass

class Circle(Shape):
    def __init__(self, radius: int):
        self.radius = radius
    
    def clone(self):
        return copy.deepcopy(self)
    
    def draw(self):
        return f"Circle with radius {self.radius}"

class Rectangle(Shape):
    def __init__(self, width: int, height: int):
        self.width = width
        self.height = height
    
    def clone(self):
        return copy.deepcopy(self)
    
    def draw(self):
        return f"Rectangle {self.width}x{self.height}"

class ShapePrototypeRegistry:
    def __init__(self):
        self._prototypes = {}
    
    def register(self, name: str, prototype: Shape):
        self._prototypes[name] = prototype
    
    def create(self, name: str) -> Shape:
        prototype = self._prototypes.get(name)
        if prototype is None:
            raise ValueError(f"Prototype {name} not found")
        return prototype.clone()

# Usage
registry = ShapePrototypeRegistry()
registry.register("small_circle", Circle(5))
registry.register("large_rectangle", Rectangle(100, 50))

circle1 = registry.create("small_circle")
circle2 = registry.create("small_circle")
print(circle1.draw())
print(circle2.draw())
print(circle1 is circle2)  # False - different objects
```

---

## SECTION 3: STRUCTURAL PATTERNS

## Adapter Pattern

### What It Is

Converts the interface of a class into **another interface clients expect**. Allows incompatible classes to work together.

### When to Use

- Integrating legacy code with new system
- Third-party library integration
- Different interface expectations

### Implementation

```python
from abc import ABC, abstractmethod

# Old interface (Adaptee)
class OldPaymentGateway:
    def process_payment(self, amount: float):
        print(f"Old gateway processing ${amount}")

# New interface expected by application (Target)
class PaymentProcessor(ABC):
    @abstractmethod
    def pay(self, amount: float):
        pass

# Adapter
class OldGatewayAdapter(PaymentProcessor):
    def __init__(self, old_gateway: OldPaymentGateway):
        self.old_gateway = old_gateway
    
    def pay(self, amount: float):
        # Adapt old interface to new interface
        self.old_gateway.process_payment(amount)

# Client code (expects PaymentProcessor)
def checkout(processor: PaymentProcessor, amount: float):
    processor.pay(amount)

# Usage
old_gateway = OldPaymentGateway()
adapter = OldGatewayAdapter(old_gateway)
checkout(adapter, 99.99)
```

---

## Decorator Pattern

### What It Is

Attaches **additional responsibilities to objects dynamically** without modifying their class.

### When to Use

- Add features without modifying original class
- Multiple optional features
- Combination of features (logging, caching, auth)

### Implementation 1: Class-Based

```python
from abc import ABC, abstractmethod

class Coffee(ABC):
    @abstractmethod
    def get_cost(self) -> float:
        pass
    
    @abstractmethod
    def get_description(self) -> str:
        pass

class SimpleCoffee(Coffee):
    def get_cost(self) -> float:
        return 2.0
    
    def get_description(self) -> str:
        return "Simple Coffee"

class CoffeeDecorator(Coffee):
    def __init__(self, coffee: Coffee):
        self.coffee = coffee
    
    def get_cost(self) -> float:
        return self.coffee.get_cost()
    
    def get_description(self) -> str:
        return self.coffee.get_description()

class MilkDecorator(CoffeeDecorator):
    def get_cost(self) -> float:
        return self.coffee.get_cost() + 0.5
    
    def get_description(self) -> str:
        return self.coffee.get_description() + ", Milk"

class SugarDecorator(CoffeeDecorator):
    def get_cost(self) -> float:
        return self.coffee.get_cost() + 0.2
    
    def get_description(self) -> str:
        return self.coffee.get_description() + ", Sugar"

class VanillaDecorator(CoffeeDecorator):
    def get_cost(self) -> float:
        return self.coffee.get_cost() + 0.7
    
    def get_description(self) -> str:
        return self.coffee.get_description() + ", Vanilla"

# Usage
coffee = SimpleCoffee()
coffee = MilkDecorator(coffee)
coffee = SugarDecorator(coffee)
coffee = VanillaDecorator(coffee)

print(f"{coffee.get_description()}: ${coffee.get_cost():.2f}")
```

### Implementation 2: Function Decorator (Pythonic)

See [Python Decorator Idiom](#python-decorator-idiom) section.

---

## Facade Pattern

### What It Is

Provides a **simple unified interface** to a set of interfaces in a complex subsystem.

### When to Use

- Simplifying complex systems
- Decoupling client from complexity
- Creating a convenient API

### Implementation

```python
# Complex subsystem components
class CPU:
    def freeze(self):
        print("CPU: freezing processor")
    
    def jump(self, position):
        print(f"CPU: jumping to position {position}")
    
    def execute(self):
        print("CPU: executing instructions")

class Memory:
    def load(self, position, data):
        print(f"Memory: loading data at {position}")

class HardDrive:
    def read(self, lba, size):
        return f"data_from_{lba}"

# Facade - simplified interface
class ComputerFacade:
    def __init__(self):
        self.cpu = CPU()
        self.memory = Memory()
        self.hdd = HardDrive()
    
    def start(self):
        print("Computer starting...")
        self.cpu.freeze()
        data = self.hdd.read(0, 1024)
        self.memory.load(0, data)
        self.cpu.jump(0)
        self.cpu.execute()
        print("Computer started successfully!")

# Client code (simple interface)
computer = ComputerFacade()
computer.start()
```

---

## Proxy Pattern

### What It Is

Provides a **surrogate or placeholder** for another object to control access to it.

### Types

- **Virtual Proxy** - lazy initialization
- **Protection Proxy** - access control
- **Logging Proxy** - logging access

### Implementation: Virtual Proxy (Lazy Loading)

```python
from abc import ABC, abstractmethod

class Image(ABC):
    @abstractmethod
    def display(self):
        pass

class RealImage(Image):
    def __init__(self, filename: str):
        self.filename = filename
        self._load_from_disk()
    
    def _load_from_disk(self):
        print(f"Loading {self.filename} from disk... [EXPENSIVE]")
    
    def display(self):
        print(f"Displaying {self.filename}")

class ImageProxy(Image):
    def __init__(self, filename: str):
        self.filename = filename
        self._real_image = None
    
    def display(self):
        if self._real_image is None:
            self._real_image = RealImage(self.filename)
        self._real_image.display()

# Usage
images = [ImageProxy(f"photo_{i}.jpg") for i in range(3)]

# Images not loaded yet - only proxies created
print("Images created (not loaded)")

# Load only when needed
images[0].display()
images[1].display()
```

---

## Bridge Pattern

### What It Is

Decouples an **abstraction from its implementation** so they can vary independently.

### When to Use

- Multiple implementations of abstraction
- Avoid permanent binding
- Different platforms

### Implementation

```python
from abc import ABC, abstractmethod

# Implementor
class Device(ABC):
    @abstractmethod
    def turn_on(self):
        pass
    
    @abstractmethod
    def turn_off(self):
        pass
    
    @abstractmethod
    def set_volume(self, volume: int):
        pass

class TV(Device):
    def turn_on(self):
        print("TV is on")
    
    def turn_off(self):
        print("TV is off")
    
    def set_volume(self, volume: int):
        print(f"TV volume: {volume}")

class Radio(Device):
    def turn_on(self):
        print("Radio is on")
    
    def turn_off(self):
        print("Radio is off")
    
    def set_volume(self, volume: int):
        print(f"Radio volume: {volume}")

# Abstraction
class RemoteControl:
    def __init__(self, device: Device):
        self.device = device
    
    def power_on(self):
        self.device.turn_on()
    
    def power_off(self):
        self.device.turn_off()
    
    def increase_volume(self):
        self.device.set_volume(10)

# Advanced abstraction
class AdvancedRemote(RemoteControl):
    def mute(self):
        self.device.set_volume(0)

# Usage
tv = TV()
radio = Radio()

tv_remote = RemoteControl(tv)
radio_remote = RemoteControl(radio)

tv_remote.power_on()
tv_remote.increase_volume()

radio_remote.power_on()
radio_remote.increase_volume()
```

---

## Composite Pattern

### What It Is

Composes objects into **tree structures** to represent part-whole hierarchies. Treats individual objects and compositions uniformly.

### When to Use

- File systems (files and directories)
- UI component hierarchies
- Organization structures

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

class Component(ABC):
    @abstractmethod
    def operation(self) -> str:
        pass

class Leaf(Component):
    def __init__(self, name: str):
        self.name = name
    
    def operation(self) -> str:
        return self.name

class Composite(Component):
    def __init__(self, name: str):
        self.name = name
        self.children: List[Component] = []
    
    def add(self, component: Component):
        self.children.append(component)
        return self
    
    def remove(self, component: Component):
        self.children.remove(component)
    
    def operation(self) -> str:
        results = [self.name]
        for child in self.children:
            results.append(child.operation())
        return "\n".join(results)

# Usage - File System
root = Composite("root")
documents = Composite("Documents")
pictures = Composite("Pictures")

root.add(documents)
root.add(pictures)

documents.add(Leaf("report.txt"))
documents.add(Leaf("resume.pdf"))

pictures.add(Leaf("photo1.jpg"))
pictures.add(Leaf("photo2.jpg"))

print(root.operation())
```

---

## Flyweight Pattern

### What It Is

Uses **sharing to support large numbers of fine-grained objects** efficiently.

### When to Use

- Many similar objects consume memory
- Text editors (character objects)
- Game engines (particles)

### Implementation

```python
class Glyph:
    def __init__(self, char: str, font: str, size: int):
        self.char = char
        self.font = font
        self.size = size
    
    def display(self, x: int, y: int):
        print(f"Displaying '{self.char}' at ({x}, {y}) - {self.font} {self.size}pt")

class GlyphFactory:
    def __init__(self):
        self._glyphs = {}
    
    def get_glyph(self, char: str, font: str, size: int) -> Glyph:
        key = (char, font, size)
        if key not in self._glyphs:
            self._glyphs[key] = Glyph(char, font, size)
        return self._glyphs[key]

# Usage
factory = GlyphFactory()

# Many characters reuse same Glyph objects
a1 = factory.get_glyph('A', 'Arial', 12)
a2 = factory.get_glyph('A', 'Arial', 12)
a3 = factory.get_glyph('B', 'Arial', 12)

print(a1 is a2)  # True - same object
print(a1 is a3)  # False - different object

a1.display(10, 20)
a2.display(20, 30)
```

---

## SECTION 4: BEHAVIORAL PATTERNS

## Observer Pattern

### What It Is

Defines a **one-to-many dependency** where when one object changes state, all dependents are notified automatically.

### When to Use

- Event systems
- Real-time data updates
- MVC architectures
- Pub-Sub systems

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List
from weakref import WeakSet

class Observer(ABC):
    @abstractmethod
    def update(self, subject):
        pass

class Subject:
    def __init__(self):
        self._observers: WeakSet = WeakSet()
        self._state = None
    
    @property
    def state(self):
        return self._state
    
    @state.setter
    def state(self, value):
        self._state = value
        self.notify_observers()
    
    def attach(self, observer: Observer):
        self._observers.add(observer)
    
    def detach(self, observer: Observer):
        self._observers.discard(observer)
    
    def notify_observers(self):
        for observer in self._observers:
            observer.update(self)

class ConcreteObserver(Observer):
    def __init__(self, name: str):
        self.name = name
    
    def update(self, subject: Subject):
        print(f"{self.name}: Got update! New state = {subject.state}")

# Usage - Stock Price Updates
class Stock(Subject):
    def __init__(self, symbol: str):
        super().__init__()
        self.symbol = symbol

stock = Stock("AAPL")
observer1 = ConcreteObserver("TradingBot1")
observer2 = ConcreteObserver("TradingBot2")

stock.attach(observer1)
stock.attach(observer2)

stock.state = 150.50
stock.state = 151.00
```

---

## Strategy Pattern

### What It Is

Defines a **family of algorithms**, encapsulates each, and makes them **interchangeable**.

### When to Use

- Multiple algorithms for same problem
- Avoid conditional statements
- Runtime algorithm selection

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

class SortStrategy(ABC):
    @abstractmethod
    def sort(self, data: List) -> List:
        pass

class BubbleSort(SortStrategy):
    def sort(self, data: List) -> List:
        result = data.copy()
        print("Using Bubble Sort")
        for i in range(len(result)):
            for j in range(len(result) - 1):
                if result[j] > result[j + 1]:
                    result[j], result[j + 1] = result[j + 1], result[j]
        return result

class QuickSort(SortStrategy):
    def sort(self, data: List) -> List:
        print("Using Quick Sort")
        return sorted(data)

class MergeSort(SortStrategy):
    def sort(self, data: List) -> List:
        print("Using Merge Sort")
        return sorted(data)

class Sorter:
    def __init__(self, strategy: SortStrategy):
        self._strategy = strategy
    
    def set_strategy(self, strategy: SortStrategy):
        self._strategy = strategy
    
    def sort(self, data: List) -> List:
        return self._strategy.sort(data)

# Usage
data = [5, 2, 8, 1, 9, 3]

sorter = Sorter(BubbleSort())
print(sorter.sort(data))

sorter.set_strategy(QuickSort())
print(sorter.sort(data))

sorter.set_strategy(MergeSort())
print(sorter.sort(data))
```

---

## Command Pattern

### What It Is

Encapsulates a **request as an object**, allowing parameterization, queuing, logging, and undo/redo.

### When to Use

- Undo/Redo functionality
- Task queuing
- Macros
- Logging requests

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

class Command(ABC):
    @abstractmethod
    def execute(self):
        pass
    
    @abstractmethod
    def undo(self):
        pass

class BuyOrder(Command):
    def __init__(self, symbol: str, quantity: int):
        self.symbol = symbol
        self.quantity = quantity
        self.executed = False
    
    def execute(self):
        print(f"Buying {self.quantity} shares of {self.symbol}")
        self.executed = True
    
    def undo(self):
        if self.executed:
            print(f"Canceling buy order for {self.symbol}")
            self.executed = False

class SellOrder(Command):
    def __init__(self, symbol: str, quantity: int):
        self.symbol = symbol
        self.quantity = quantity
        self.executed = False
    
    def execute(self):
        print(f"Selling {self.quantity} shares of {self.symbol}")
        self.executed = True
    
    def undo(self):
        if self.executed:
            print(f"Canceling sell order for {self.symbol}")
            self.executed = False

class TradingApp:
    def __init__(self):
        self._commands: List[Command] = []
        self._history: List[Command] = []
    
    def queue_command(self, command: Command):
        self._commands.append(command)
    
    def execute_all(self):
        for command in self._commands:
            command.execute()
            self._history.append(command)
        self._commands.clear()
    
    def undo_last(self):
        if self._history:
            command = self._history.pop()
            command.undo()

# Usage
app = TradingApp()
app.queue_command(BuyOrder("AAPL", 100))
app.queue_command(BuyOrder("MSFT", 50))
app.queue_command(SellOrder("GOOG", 25))

app.execute_all()
app.undo_last()
```

---

## State Pattern

### What It Is

Allows an object to **alter its behavior** when its internal state changes.

### When to Use

- State machines (order processing, payment)
- Context-dependent behavior

### Implementation

```python
from abc import ABC, abstractmethod

class OrderState(ABC):
    @abstractmethod
    def handle(self, order: "Order"):
        pass

class PendingState(OrderState):
    def handle(self, order: "Order"):
        print("Processing pending order")
        order.set_state(ProcessingState())

class ProcessingState(OrderState):
    def handle(self, order: "Order"):
        print("Shipping order")
        order.set_state(ShippedState())

class ShippedState(OrderState):
    def handle(self, order: "Order"):
        print("Order delivered")
        order.set_state(DeliveredState())

class DeliveredState(OrderState):
    def handle(self, order: "Order"):
        print("Order already delivered")

class Order:
    def __init__(self, order_id: str):
        self.order_id = order_id
        self._state: OrderState = PendingState()
    
    def set_state(self, state: OrderState):
        self._state = state
    
    def process(self):
        self._state.handle(self)

# Usage
order = Order("ORD-001")
order.process()
order.process()
order.process()
order.process()
```

---

## Template Method Pattern

### What It Is

Defines the **skeleton of an algorithm** in a base class, letting subclasses override specific steps.

### When to Use

- Code reuse across related classes
- Framework design
- Avoiding code duplication

### Implementation

```python
from abc import ABC, abstractmethod

class DataProcessor(ABC):
    def process_data(self, data):
        """Template method - defines algorithm structure"""
        raw_data = self.load_data(data)
        processed = self.process(raw_data)
        result = self.validate(processed)
        self.save(result)
        return result
    
    @abstractmethod
    def load_data(self, data):
        pass
    
    @abstractmethod
    def process(self, data):
        pass
    
    @abstractmethod
    def validate(self, data):
        pass
    
    @abstractmethod
    def save(self, data):
        pass

class CSVProcessor(DataProcessor):
    def load_data(self, data):
        print("Loading CSV data")
        return data.split(',')
    
    def process(self, data):
        print("Processing CSV")
        return [x.strip() for x in data]
    
    def validate(self, data):
        print("Validating CSV")
        return data if data else []
    
    def save(self, data):
        print(f"Saving CSV: {data}")

class JSONProcessor(DataProcessor):
    def load_data(self, data):
        import json
        print("Loading JSON data")
        return json.loads(data)
    
    def process(self, data):
        print("Processing JSON")
        return data
    
    def validate(self, data):
        print("Validating JSON")
        return data
    
    def save(self, data):
        import json
        print(f"Saving JSON: {json.dumps(data)}")

# Usage
csv_processor = CSVProcessor()
csv_processor.process_data("name, age, city")

json_processor = JSONProcessor()
json_processor.process_data('{"name": "John", "age": 30}')
```

---

## Chain of Responsibility Pattern

### What It Is

Passes a request along a **chain of handlers** where each handler decides to process or pass it along.

### When to Use

- Event handling
- Logging with levels
- Approval workflows
- Request routing

### Implementation

```python
from abc import ABC, abstractmethod
from typing import Optional

class Handler(ABC):
    def __init__(self):
        self._next_handler: Optional[Handler] = None
    
    def set_next(self, handler: "Handler") -> "Handler":
        self._next_handler = handler
        return handler
    
    @abstractmethod
    def handle(self, request: int) -> Optional[str]:
        if self._next_handler:
            return self._next_handler.handle(request)
        return None

class SmallRequestHandler(Handler):
    def handle(self, request: int) -> Optional[str]:
        if request < 100:
            return f"SmallRequestHandler: Handling ${request}"
        return super().handle(request)

class MediumRequestHandler(Handler):
    def handle(self, request: int) -> Optional[str]:
        if 100 <= request < 1000:
            return f"MediumRequestHandler: Handling ${request}"
        return super().handle(request)

class LargeRequestHandler(Handler):
    def handle(self, request: int) -> Optional[str]:
        if request >= 1000:
            return f"LargeRequestHandler: Handling ${request}"
        return super().handle(request)

# Usage - Approval Chain
small = SmallRequestHandler()
medium = MediumRequestHandler()
large = LargeRequestHandler()

small.set_next(medium).set_next(large)

requests = [50, 250, 1500, 5000]
for req in requests:
    print(small.handle(req))
```

---

## Iterator Pattern

### What It Is

Provides a way to **access elements of a collection** sequentially without exposing its structure.

### When to Use

- Different traversal strategies
- Hiding collection implementation

### Implementation

```python
from abc import ABC, abstractmethod
from typing import Iterator, Generic, TypeVar

T = TypeVar('T')

class Collection(ABC, Generic[T]):
    @abstractmethod
    def create_iterator(self) -> Iterator[T]:
        pass

class LinkedList(Collection[int]):
    def __init__(self):
        self.items = []
    
    def add(self, item: int):
        self.items.append(item)
    
    def create_iterator(self) -> Iterator[int]:
        return iter(self.items)

class ReverseList(Collection[int]):
    def __init__(self):
        self.items = []
    
    def add(self, item: int):
        self.items.append(item)
    
    def create_iterator(self) -> Iterator[int]:
        return reversed(self.items)

# Usage
linked_list = LinkedList()
for i in [1, 2, 3, 4, 5]:
    linked_list.add(i)

print("Normal iteration:")
for item in linked_list.create_iterator():
    print(item)

reverse_list = ReverseList()
for i in [1, 2, 3, 4, 5]:
    reverse_list.add(i)

print("\nReverse iteration:")
for item in reverse_list.create_iterator():
    print(item)
```

---

## Mediator Pattern

### What It Is

Defines an object that **encapsulates how objects interact**, promoting **loose coupling**.

### When to Use

- Complex inter-object communication
- Dialog boxes with controls
- Chat rooms, air traffic control

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

class Mediator(ABC):
    @abstractmethod
    def send(self, message: str, sender):
        pass

class ChatRoom(Mediator):
    def __init__(self):
        self.users: List["User"] = []
    
    def register_user(self, user: "User"):
        self.users.append(user)
    
    def send(self, message: str, sender: "User"):
        for user in self.users:
            if user != sender:
                user.receive(f"{sender.name}: {message}")

class User:
    def __init__(self, name: str, mediator: Mediator):
        self.name = name
        self.mediator = mediator
    
    def send(self, message: str):
        self.mediator.send(message, self)
    
    def receive(self, message: str):
        print(f"{self.name} received: {message}")

# Usage
chat_room = ChatRoom()

user1 = User("Alice", chat_room)
user2 = User("Bob", chat_room)
user3 = User("Charlie", chat_room)

chat_room.register_user(user1)
chat_room.register_user(user2)
chat_room.register_user(user3)

user1.send("Hello everyone!")
user2.send("Hi Alice!")
```

---

## Memento Pattern

### What It Is

Captures and externalizes an object's **internal state** without breaking encapsulation.

### When to Use

- Undo/Redo
- Snapshots
- Transactions

### Implementation

```python
from copy import deepcopy
from typing import List

class Memento:
    def __init__(self, state: dict):
        self._state = deepcopy(state)
    
    def get_state(self) -> dict:
        return self._state

class TextEditor:
    def __init__(self):
        self.content = ""
    
    def write(self, text: str):
        self.content += text
        print(f"Wrote: {text}")
    
    def save_state(self) -> Memento:
        return Memento({"content": self.content})
    
    def restore_state(self, memento: Memento):
        state = memento.get_state()
        self.content = state["content"]
        print(f"Restored content: {self.content}")
    
    def display(self):
        print(f"Current content: {self.content}")

class TextEditorHistory:
    def __init__(self):
        self._history: List[Memento] = []
    
    def save(self, memento: Memento):
        self._history.append(memento)
    
    def undo(self) -> Memento:
        if self._history:
            return self._history.pop()
        return None

# Usage
editor = TextEditor()
history = TextEditorHistory()

editor.write("Hello")
history.save(editor.save_state())

editor.write(" World")
editor.display()

editor.restore_state(history.undo())
editor.display()
```

---

## Visitor Pattern

### What It Is

Represents an **operation on elements** of a structure without changing the element classes.

### When to Use

- Complex operations on object structures
- Avoiding type casting

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

class Visitor(ABC):
    @abstractmethod
    def visit_file(self, file: "File"):
        pass
    
    @abstractmethod
    def visit_folder(self, folder: "Folder"):
        pass

class Element(ABC):
    @abstractmethod
    def accept(self, visitor: Visitor):
        pass

class File(Element):
    def __init__(self, name: str, size: int):
        self.name = name
        self.size = size
    
    def accept(self, visitor: Visitor):
        visitor.visit_file(self)

class Folder(Element):
    def __init__(self, name: str):
        self.name = name
        self.elements: List[Element] = []
    
    def add(self, element: Element):
        self.elements.append(element)
    
    def accept(self, visitor: Visitor):
        visitor.visit_folder(self)
        for element in self.elements:
            element.accept(visitor)

class SizeCalculator(Visitor):
    def __init__(self):
        self.total_size = 0
    
    def visit_file(self, file: File):
        self.total_size += file.size
        print(f"File: {file.name} ({file.size} bytes)")
    
    def visit_folder(self, folder: Folder):
        print(f"Folder: {folder.name}")

# Usage
root = Folder("root")
documents = Folder("Documents")
root.add(documents)
documents.add(File("report.pdf", 5000))
documents.add(File("resume.doc", 3000))
root.add(File("config.txt", 100))

calculator = SizeCalculator()
root.accept(calculator)
print(f"Total size: {calculator.total_size} bytes")
```

---

## Interpreter Pattern

### What It Is

Defines a **grammar and interpreter** for a language.

### When to Use

- DSL (Domain-Specific Language)
- Expression evaluation
- Query languages

### Implementation

```python
from abc import ABC, abstractmethod

class Expression(ABC):
    @abstractmethod
    def interpret(self) -> int:
        pass

class Number(Expression):
    def __init__(self, value: int):
        self.value = value
    
    def interpret(self) -> int:
        return self.value

class Add(Expression):
    def __init__(self, left: Expression, right: Expression):
        self.left = left
        self.right = right
    
    def interpret(self) -> int:
        return self.left.interpret() + self.right.interpret()

class Subtract(Expression):
    def __init__(self, left: Expression, right: Expression):
        self.left = left
        self.right = right
    
    def interpret(self) -> int:
        return self.left.interpret() - self.right.interpret()

class Multiply(Expression):
    def __init__(self, left: Expression, right: Expression):
        self.left = left
        self.right = right
    
    def interpret(self) -> int:
        return self.left.interpret() * self.right.interpret()

# Usage: 5 + 3 * 2 - 1
expr = Subtract(
    Add(
        Number(5),
        Multiply(Number(3), Number(2))
    ),
    Number(1)
)

print(f"Result: {expr.interpret()}")
```

---

## SECTION 5: CONCURRENCY PATTERNS

## Thread Pool Pattern

### What It Is

Maintains a pool of **reusable threads** to handle tasks, avoiding expensive thread creation.

### When to Use

- High-volume task processing
- Server request handling
- Parallel computation

### Implementation

```python
import threading
import queue
import time
from typing import Callable

class ThreadPool:
    def __init__(self, num_threads: int):
        self.task_queue: queue.Queue = queue.Queue()
        self.threads = []
        
        for _ in range(num_threads):
            t = threading.Thread(target=self._worker, daemon=True)
            t.start()
            self.threads.append(t)
    
    def _worker(self):
        while True:
            task_func, args = self.task_queue.get()
            try:
                task_func(*args)
            finally:
                self.task_queue.task_done()
    
    def submit(self, task: Callable, *args):
        self.task_queue.put((task, args))
    
    def wait_completion(self):
        self.task_queue.join()

def expensive_task(n: int):
    print(f"Processing task {n}")
    time.sleep(1)
    print(f"Completed task {n}")

# Usage
pool = ThreadPool(4)

for i in range(10):
    pool.submit(expensive_task, i)

pool.wait_completion()
print("All tasks completed")
```

---

## Producer-Consumer Pattern

### What It Is

Producers generate data, consumers process it, with a **queue** between them.

### When to Use

- Data processing pipelines
- Event processing
- Decoupling producers from consumers

### Implementation

```python
import queue
import threading
import time
import random

def producer(q: queue.Queue, producer_id: int):
    for i in range(5):
        item = f"Item-{producer_id}-{i}"
        print(f"Producer {producer_id} producing {item}")
        q.put(item)
        time.sleep(random.random())

def consumer(q: queue.Queue, consumer_id: int):
    while True:
        try:
            item = q.get(timeout=2)
            print(f"Consumer {consumer_id} processing {item}")
            time.sleep(random.random())
            q.task_done()
        except queue.Empty:
            break

# Usage
q = queue.Queue(maxsize=5)

producers = [
    threading.Thread(target=producer, args=(q, i))
    for i in range(2)
]
consumers = [
    threading.Thread(target=consumer, args=(q, i))
    for i in range(2)
]

for t in producers:
    t.start()

for t in consumers:
    t.start()

for t in producers + consumers:
    t.join()

print("Done")
```

---

## Reactor Pattern

### What It Is

Uses a **single-threaded event loop** to handle multiple I/O events.

### When to Use

- High-concurrency I/O (web servers, games)
- Async servers
- Event-driven systems

### Implementation

```python
import select
import socket
from typing import Dict, Callable

class Reactor:
    def __init__(self):
        self.handlers: Dict[socket.socket, Callable] = {}
    
    def register(self, sock: socket.socket, handler: Callable):
        self.handlers[sock] = handler
    
    def unregister(self, sock: socket.socket):
        if sock in self.handlers:
            del self.handlers[sock]
            sock.close()
    
    def run(self):
        while self.handlers:
            ready_socks, _, _ = select.select(self.handlers.keys(), [], [])
            
            for sock in ready_socks:
                handler = self.handlers[sock]
                try:
                    handler(sock)
                except Exception as e:
                    print(f"Error: {e}")
                    self.unregister(sock)
```

---

## Active Object Pattern

### What It Is

**Decouples method execution from method invocation** to enhance concurrency.

### When to Use

- Concurrent method execution
- Async task execution

### Implementation

```python
import threading
from typing import Callable
from queue import Queue

class ActiveObject:
    def __init__(self):
        self._queue = Queue()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()
    
    def _run(self):
        while True:
            method, args = self._queue.get()
            if method is None:
                break
            method(*args)
    
    def async_method(self, method: Callable, *args):
        self._queue.put((method, args))
    
    def shutdown(self):
        self._queue.put((None, None))

class Service(ActiveObject):
    def long_operation(self, value: int):
        import time
        print(f"Starting operation with {value}")
        time.sleep(2)
        print(f"Completed operation with {value}")

# Usage
service = Service()
service.async_method(service.long_operation, 42)
service.async_method(service.long_operation, 100)
service.shutdown()
```

---

## SECTION 6: ASYNCHRONOUS PATTERNS

## Async/Await Pattern

### What It Is

Modern async/await syntax for **non-blocking asynchronous code** execution.

### When to Use

- Async I/O operations
- Concurrent network requests
- Real-time applications

### Implementation

```python
import asyncio
import aiohttp
from typing import List

async def fetch_price(symbol: str) -> dict:
    """Simulate async price fetch"""
    await asyncio.sleep(1)
    prices = {"AAPL": 150.0, "MSFT": 300.0, "GOOG": 140.0}
    return {symbol: prices.get(symbol, 0)}

async def fetch_multiple_prices(symbols: List[str]):
    """Fetch multiple prices concurrently"""
    tasks = [fetch_price(symbol) for symbol in symbols]
    results = await asyncio.gather(*tasks)
    return results

# Usage
async def main():
    symbols = ["AAPL", "MSFT", "GOOG"]
    results = await fetch_multiple_prices(symbols)
    print(results)

asyncio.run(main())
```

---

## Future/Promise Pattern

### What It Is

Represents a value that **may not be available yet** but will be available in the future.

### When to Use

- Async operations with callbacks
- Promise chains

### Implementation

```python
import asyncio
from concurrent.futures import Future

def callback_example():
    future = Future()
    
    def on_done():
        future.set_result("Operation complete!")
    
    # Schedule callback
    asyncio.run(asyncio.sleep(1))
    on_done()
    
    try:
        result = future.result(timeout=2)
        print(result)
    except Exception as e:
        print(f"Error: {e}")

# Modern asyncio.Future
async def async_future_example():
    future = asyncio.Future()
    
    async def set_result():
        await asyncio.sleep(1)
        future.set_result("Done!")
    
    asyncio.create_task(set_result())
    result = await future
    print(result)

asyncio.run(async_future_example())
```

---

## Callback Pattern

### What It Is

Passes a function to be **executed later** when an event occurs.

### When to Use

- Event handling
- Async completion
- Observer-like patterns

### Implementation

```python
from typing import Callable
import threading
import time

class AsyncTask:
    def __init__(self, callback: Callable):
        self.callback = callback
    
    def execute_async(self, data):
        def run():
            time.sleep(2)
            result = f"Processed: {data}"
            self.callback(result)
        
        thread = threading.Thread(target=run)
        thread.start()

# Usage
def on_completion(result: str):
    print(f"Callback received: {result}")

task = AsyncTask(on_completion)
task.execute_async("important data")

# Keep main thread alive
time.sleep(3)
```

---

## SECTION 7: PYTHONIC PATTERNS & IDIOMS

## Python Decorator Idiom

### What It Is

A **function that wraps another function** or method to extend its behavior.

### When to Use

- Logging, timing, caching
- Authorization
- Modifying function behavior

### Implementation

```python
import functools
import time

def timer_decorator(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        elapsed = time.time() - start
        print(f"{func.__name__} took {elapsed:.4f} seconds")
        return result
    return wrapper

@timer_decorator
def slow_function():
    time.sleep(1)
    print("Function executed")

slow_function()
```

### Decorator with Arguments

```python
def repeat(times: int):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            results = []
            for _ in range(times):
                results.append(func(*args, **kwargs))
            return results
        return wrapper
    return decorator

@repeat(3)
def greet(name: str):
    return f"Hello, {name}!"

print(greet("Alice"))
```

### Class-Based Decorator

```python
import functools

class LoggingDecorator:
    def __init__(self, func):
        self.func = func
        functools.update_wrapper(self, func)
    
    def __call__(self, *args, **kwargs):
        print(f"Calling {self.func.__name__}")
        result = self.func(*args, **kwargs)
        print(f"Finished {self.func.__name__}")
        return result

@LoggingDecorator
def add(a, b):
    return a + b

print(add(5, 3))
```

---

## Context Manager Idiom

### What It Is

Manages **resource acquisition and release** using `with` statement.

### When to Use

- File operations
- Database transactions
- Lock management
- Cleanup operations

### Implementation

```python
class FileManager:
    def __init__(self, filename: str, mode: str):
        self.filename = filename
        self.mode = mode
        self.file = None
    
    def __enter__(self):
        self.file = open(self.filename, self.mode)
        print(f"Opened {self.filename}")
        return self.file
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.file:
            self.file.close()
            print(f"Closed {self.filename}")
        return False  # Don't suppress exceptions

# Usage
with FileManager("test.txt", "w") as f:
    f.write("Hello, World!")

# File automatically closed
```

### Using contextlib

```python
from contextlib import contextmanager
import time

@contextmanager
def timer():
    start = time.time()
    print("Timer started")
    try:
        yield
    finally:
        elapsed = time.time() - start
        print(f"Elapsed: {elapsed:.4f} seconds")

# Usage
with timer():
    time.sleep(1)
```

---

## Property Pattern

### What It Is

Creates **managed attributes** with getter/setter validation.

### When to Use

- Encapsulation
- Validation
- Lazy loading

### Implementation

```python
class Person:
    def __init__(self, name: str, age: int):
        self._name = name
        self._age = age
    
    @property
    def name(self) -> str:
        return self._name
    
    @name.setter
    def name(self, value: str):
        if not value:
            raise ValueError("Name cannot be empty")
        self._name = value
    
    @property
    def age(self) -> int:
        return self._age
    
    @age.setter
    def age(self, value: int):
        if value < 0 or value > 150:
            raise ValueError("Age must be between 0 and 150")
        self._age = value

# Usage
person = Person("Alice", 30)
print(person.name)
person.age = 31
print(person.age)

# Validation triggered
try:
    person.age = -5
except ValueError as e:
    print(f"Error: {e}")
```

---

## Descriptor Pattern

### What It Is

Objects that implement `__get__`, `__set__`, `__delete__` to control attribute access.

### When to Use

- Computed properties
- Validation
- Lazy loading
- Type enforcement

### Implementation

```python
class PositiveInteger:
    def __init__(self, name: str):
        self.name = name
    
    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        return obj.__dict__.get(self.name, 0)
    
    def __set__(self, obj, value):
        if not isinstance(value, int) or value < 0:
            raise ValueError(f"{self.name} must be a positive integer")
        obj.__dict__[self.name] = value

class Product:
    price = PositiveInteger("price")
    quantity = PositiveInteger("quantity")
    
    def __init__(self, name: str, price: float, quantity: int):
        self.name = name
        self.price = price
        self.quantity = quantity

# Usage
product = Product("Laptop", 999, 5)
print(product.price)

# Validation enforced
try:
    product.price = -100
except ValueError as e:
    print(f"Error: {e}")
```

---

## Metaclass Pattern

### What It Is

A **class of a class** that controls class creation and behavior.

### When to Use

- ORM implementations
- Framework base classes
- API design
- Plugin systems

### Implementation

```python
class SingletonMeta(type):
    _instances = {}
    
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]

class Database(metaclass=SingletonMeta):
    def __init__(self):
        self.connection = "Connected"

# Usage
db1 = Database()
db2 = Database()
print(db1 is db2)  # True
```

---

## Generator & Iterator Protocol

### What It Is

Objects that implement `__iter__` and `__next__` for lazy evaluation.

### When to Use

- Large data streams
- Memory efficiency
- Lazy computation

### Implementation

```python
class CountUp:
    def __init__(self, max: int):
        self.max = max
        self.current = 0
    
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.current < self.max:
            self.current += 1
            return self.current
        else:
            raise StopIteration

# Usage
for num in CountUp(5):
    print(num)

# Generator (simpler)
def count_up_gen(max: int):
    current = 0
    while current < max:
        current += 1
        yield current

for num in count_up_gen(5):
    print(num)
```

---

## Closure Pattern

### What It Is

A **function that captures variables** from its enclosing scope.

### When to Use

- Data encapsulation
- Factory functions
- Decorators
- Callbacks

### Implementation

```python
def make_multiplier(factor: int):
    """Creates a function that multiplies by factor"""
    def multiplier(x: int):
        return x * factor
    return multiplier

multiply_by_3 = make_multiplier(3)
multiply_by_5 = make_multiplier(5)

print(multiply_by_3(10))  # 30
print(multiply_by_5(10))  # 50

# Counter with closure
def make_counter():
    count = 0
    
    def increment():
        nonlocal count
        count += 1
        return count
    
    def decrement():
        nonlocal count
        count -= 1
        return count
    
    return increment, decrement

inc, dec = make_counter()
print(inc())  # 1
print(inc())  # 2
print(dec())  # 1
```

---

## SECTION 8: ARCHITECTURAL PATTERNS

## MVC Pattern

### What It Is

Separates application into **Model** (data), **View** (presentation), **Controller** (logic).

### When to Use

- Web applications (Django, Flask)
- GUI applications
- Decoupling concerns

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List

# Model
class Book:
    def __init__(self, title: str, author: str):
        self.title = title
        self.author = author

class Library:
    def __init__(self):
        self.books: List[Book] = []
    
    def add_book(self, book: Book):
        self.books.append(book)
    
    def get_books(self) -> List[Book]:
        return self.books

# View
class LibraryView:
    def show_books(self, books: List[Book]):
        print("=== Library Books ===")
        for book in books:
            print(f"- {book.title} by {book.author}")

# Controller
class LibraryController:
    def __init__(self, library: Library, view: LibraryView):
        self.library = library
        self.view = view
    
    def add_book(self, title: str, author: str):
        book = Book(title, author)
        self.library.add_book(book)
    
    def display_books(self):
        books = self.library.get_books()
        self.view.show_books(books)

# Usage
library = Library()
view = LibraryView()
controller = LibraryController(library, view)

controller.add_book("Python Design Patterns", "Gang of Four")
controller.add_book("Clean Code", "Robert Martin")
controller.display_books()
```

---

## Repository Pattern

### What It Is

Abstraction layer for **data access**, isolating business logic from persistence.

### When to Use

- Testing (mock repositories)
- Multiple data sources
- Decoupling business logic

### Implementation

```python
from abc import ABC, abstractmethod
from typing import List, Optional, Dict

class User:
    def __init__(self, id: int, name: str, email: str):
        self.id = id
        self.name = name
        self.email = email

class IUserRepository(ABC):
    @abstractmethod
    def add(self, user: User): ...
    
    @abstractmethod
    def get(self, user_id: int) -> Optional[User]: ...
    
    @abstractmethod
    def get_all(self) -> List[User]: ...
    
    @abstractmethod
    def delete(self, user_id: int): ...

class InMemoryUserRepository(IUserRepository):
    def __init__(self):
        self._users: Dict[int, User] = {}
    
    def add(self, user: User):
        self._users[user.id] = user
    
    def get(self, user_id: int) -> Optional[User]:
        return self._users.get(user_id)
    
    def get_all(self) -> List[User]:
        return list(self._users.values())
    
    def delete(self, user_id: int):
        if user_id in self._users:
            del self._users[user_id]

class DatabaseUserRepository(IUserRepository):
    """Could implement actual database operations"""
    def add(self, user: User):
        print(f"INSERT INTO users VALUES ({user.id}, '{user.name}', '{user.email}')")
    
    def get(self, user_id: int) -> Optional[User]:
        print(f"SELECT * FROM users WHERE id={user_id}")
        return None
    
    def get_all(self) -> List[User]:
        print("SELECT * FROM users")
        return []
    
    def delete(self, user_id: int):
        print(f"DELETE FROM users WHERE id={user_id}")

# Business logic (doesn't care about data source)
class UserService:
    def __init__(self, repo: IUserRepository):
        self.repo = repo
    
    def register_user(self, id: int, name: str, email: str):
        user = User(id, name, email)
        self.repo.add(user)
        print(f"User {name} registered")
    
    def list_users(self):
        return self.repo.get_all()

# Usage
# In-memory repository
repo = InMemoryUserRepository()
service = UserService(repo)

service.register_user(1, "Alice", "alice@example.com")
service.register_user(2, "Bob", "bob@example.com")
print(service.list_users())

# Database repository (just swaps implementation)
# db_repo = DatabaseUserRepository()
# service = UserService(db_repo)
```

---

## Dependency Injection

### What It Is

**Injecting dependencies** into objects rather than creating them internally.

### When to Use

- Testing
- Loose coupling
- Configuration management

### Implementation

```python
from abc import ABC, abstractmethod

# Dependencies
class Logger(ABC):
    @abstractmethod
    def log(self, message: str): ...

class ConsoleLogger(Logger):
    def log(self, message: str):
        print(f"[LOG] {message}")

class EmailLogger(Logger):
    def log(self, message: str):
        print(f"[EMAIL] {message}")

# Service with dependency injection
class UserService:
    def __init__(self, logger: Logger):
        self.logger = logger
    
    def create_user(self, name: str):
        self.logger.log(f"Creating user: {name}")
        # Create user logic...
        self.logger.log(f"User created: {name}")

# Usage
console_logger = ConsoleLogger()
service = UserService(console_logger)
service.create_user("Alice")

# Easy to swap implementation
email_logger = EmailLogger()
service2 = UserService(email_logger)
service2.create_user("Bob")
```

---

## SECTION 9: ANTI-PATTERNS

## Common Anti-Patterns

### 1. GOD OBJECT

A class that knows too much and does too much.

```python
# BAD
class GODObject:
    def handle_users(self): pass
    def handle_payments(self): pass
    def handle_emails(self): pass
    def handle_database(self): pass
    # ... 1000 more methods

# GOOD - Separate responsibilities
class UserManager:
    def handle_users(self): pass

class PaymentProcessor:
    def handle_payments(self): pass

class EmailService:
    def handle_emails(self): pass
```

### 2. CIRCULAR DEPENDENCIES

Classes depending on each other.

```python
# BAD
class A:
    def __init__(self):
        self.b = B()

class B:
    def __init__(self):
        self.a = A()

# GOOD - Use dependency injection
class A:
    def __init__(self, b):
        self.b = b

class B:
    def __init__(self, a):
        self.a = a
```

### 3. SPAGHETTI CODE

No clear structure or logic flow.

```python
# BAD
def process():
    x = get_data()
    for i in range(len(x)):
        if x[i] > 10:
            y = x[i] * 2
            if y > 100:
                z = sqrt(y)
                if z > 5:
                    # ... lots of tangled logic
                    pass

# GOOD - Break into small functions
def is_valid(value):
    return value > 10

def transform(value):
    return value * 2

def process():
    data = get_data()
    for item in data:
        if is_valid(item):
            transformed = transform(item)
            handle(transformed)
```

### 4. MAGIC NUMBERS

Unexplained constants in code.

```python
# BAD
if user_age > 18 and user_balance > 5000:
    process()

# GOOD
ADULT_AGE = 18
MIN_BALANCE = 5000

if user_age > ADULT_AGE and user_balance > MIN_BALANCE:
    process()
```

### 5. TIGHT COUPLING

Classes depend on concrete implementations.

```python
# BAD
class Service:
    def __init__(self):
        self.db = PostgresDatabase()

# GOOD - Depend on abstraction
class Service:
    def __init__(self, db: DatabaseInterface):
        self.db = db
```

---

## Final Pattern Checklist

### Creational (6)
- ☐ Singleton
- ☐ Factory Method
- ☐ Abstract Factory
- ☐ Builder
- ☐ Object Pool
- ☐ Prototype

### Structural (7)
- ☐ Adapter
- ☐ Decorator
- ☐ Facade
- ☐ Proxy
- ☐ Bridge
- ☐ Composite
- ☐ Flyweight

### Behavioral (11)
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

### Concurrency (4)
- ☐ Thread Pool
- ☐ Producer-Consumer
- ☐ Reactor
- ☐ Active Object

### Asynchronous (3)
- ☐ Async/Await
- ☐ Future/Promise
- ☐ Callback

### Pythonic (7)
- ☐ Decorator
- ☐ Context Manager
- ☐ Property
- ☐ Descriptor
- ☐ Metaclass
- ☐ Generator Protocol
- ☐ Closure

### Architectural (3)
- ☐ MVC
- ☐ Repository
- ☐ Dependency Injection

---

## Key Principles

1. **SOLID Principles**
   - **S**ingle Responsibility
   - **O**pen/Closed
   - **L**iskov Substitution
   - **I**nterface Segregation
   - **D**ependency Inversion

2. **DRY** - Don't Repeat Yourself

3. **KISS** - Keep It Simple, Stupid

4. **YAGNI** - You Aren't Gonna Need It

5. **Composition over Inheritance**

---

## Best Practices

1. **Use patterns to solve real problems**, not for their own sake
2. **Start simple**, refactor to patterns when needed
3. **Know when NOT to use patterns**
4. **Combine patterns** - they often work together
5. **Document pattern usage** - make it clear why you're using them
6. **Test thoroughly** - patterns should improve testability
7. **Python first** - use Python idioms before classic GoF patterns

---

**Master these 40+ patterns and write professional, maintainable Python code!** 🚀

*Last Updated: December 2025*
*Python Version: 3.8+*
*Total Patterns Covered: 40+*
