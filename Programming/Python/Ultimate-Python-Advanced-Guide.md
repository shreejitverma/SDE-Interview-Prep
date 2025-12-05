# The Ultimate Advanced Python Programmer's Guide: From Mastery to Godhood

## Table of Contents

### Part 1: Core Advanced Concepts
1. [Generators & Iterators](#generators--iterators)
2. [Decorators](#decorators)
3. [Context Managers](#context-managers)
4. [Closures & Higher-Order Functions](#closures--higher-order-functions)
5. [Metaclasses & Descriptors](#metaclasses--descriptors)
6. [Object-Oriented Programming Mastery](#object-oriented-programming-mastery)
7. [Functional Programming](#functional-programming)

### Part 2: Performance & Memory
8. [Memory Management](#memory-management)
9. [Performance Optimization](#performance-optimization)
10. [Concurrency & Parallelism](#concurrency--parallelism)
11. [Async/Await Mastery](#asyncawait-mastery)

### Part 3: Advanced Data Structures
12. [Collections & Data Structures](#collections--data-structures)
13. [Design Patterns](#design-patterns)
14. [Testing & Debugging](#testing--debugging)

### Part 4: Professional Development
15. [Type Hints & Mypy](#type-hints--mypy)
16. [C Extensions & ctypes](#c-extensions--ctypes)
17. [Package Development](#package-development)
18. [Production Best Practices](#production-best-practices)

### Part 5: System Programming
19. [File I/O & Binary Data](#file-io--binary-data)
20. [Networking & Protocols](#networking--protocols)
21. [Database Programming](#database-programming)

---

## PART 1: CORE ADVANCED CONCEPTS

## Generators & Iterators

### Understanding Iterators

An iterator is an object that implements two methods:
- `__iter__()`: returns the iterator object itself
- `__next__()`: returns the next value from the sequence

```python
class CountUp:
    """Custom iterator"""
    def __init__(self, n):
        self.n = n
        self.current = 0
    
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.current < self.n:
            self.current += 1
            return self.current
        else:
            raise StopIteration

# Use it
for num in CountUp(3):
    print(num)  # 1, 2, 3

# Equivalent to:
it = iter(CountUp(3))
while True:
    try:
        print(next(it))
    except StopIteration:
        break
```

### Generator Functions (Advanced)

```python
def generator_advanced():
    """Generator with bidirectional communication"""
    result = None
    while True:
        # yield returns value AND receives value via send()
        received = yield result
        print(f"Received: {received}")
        result = f"Processing: {received}"

gen = generator_advanced()
print(next(gen))  # Prime the generator (reach first yield)
print(gen.send("Hello"))  # Send value, get result
print(gen.send("World"))  # Send value, get result
```

### Generator Delegation (yield from)

```python
def sub_generator():
    yield 1
    yield 2
    yield 3

def delegating_generator():
    print("Starting sub_generator")
    yield from sub_generator()
    print("Finishing sub_generator")
    yield 4

for value in delegating_generator():
    print(value)
# Output: Starting, 1, 2, 3, Finishing, 4

# yield from transparently passes values and exceptions
def chain_generators(*iterables):
    """Simplified chain using yield from"""
    for iterable in iterables:
        yield from iterable

list(chain_generators([1,2], [3,4], [5,6]))  # [1,2,3,4,5,6]
```

### Generator Performance Patterns

```python
# Pattern 1: Lazy Pipeline
def read_file(path):
    with open(path) as f:
        for line in f:
            yield line.rstrip()

def filter_lines(lines, pattern):
    import re
    for line in lines:
        if re.search(pattern, line):
            yield line

def extract_field(lines, field_index):
    for line in lines:
        yield line.split(',')[field_index]

# Memory-efficient pipeline
for field in extract_field(filter_lines(read_file('data.csv'), 'ERROR'), 2):
    process(field)  # Never loads entire file in memory

# Pattern 2: Generator with Cleanup
def resource_generator():
    resource = acquire_resource()
    try:
        while True:
            data = resource.fetch()
            yield data
    finally:
        resource.release()  # Always cleanup

# Pattern 3: Infinite Generators
import itertools

def infinite_sequence(start=0, step=1):
    while True:
        yield start
        start += step

# Use with itertools to limit
list(itertools.islice(infinite_sequence(), 5))  # [0, 1, 2, 3, 4]
```

### Memory Efficiency Comparison

```python
import sys
import itertools

# Approach 1: List (stores all in memory)
squares_list = [x**2 for x in range(1000000)]
print(sys.getsizeof(squares_list))  # ~8 MB

# Approach 2: Generator Expression
squares_gen = (x**2 for x in range(1000000))
print(sys.getsizeof(squares_gen))  # ~128 bytes

# Approach 3: itertools
squares_iter = itertools.islice(map(lambda x: x**2, range(1000000)), 1000000)
print(sys.getsizeof(squares_iter))  # ~64 bytes

# All produce same results; generator uses 60,000x less memory!
```

---

## Decorators

### Advanced Decorator Patterns

#### Decorator Factories with Configuration

```python
import functools
import time

def retry(max_attempts=3, delay=1, backoff=1, exceptions=(Exception,)):
    """Production-grade retry decorator"""
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            attempt = 1
            current_delay = delay
            
            while attempt <= max_attempts:
                try:
                    return func(*args, **kwargs)
                except exceptions as e:
                    if attempt == max_attempts:
                        raise
                    
                    print(f"Attempt {attempt} failed: {e}. "
                          f"Retrying in {current_delay}s...")
                    time.sleep(current_delay)
                    current_delay *= backoff
                    attempt += 1
        
        return wrapper
    return decorator

@retry(max_attempts=3, delay=1, backoff=2, 
       exceptions=(ConnectionError, TimeoutError))
def flaky_api_call():
    import random
    if random.random() < 0.7:
        raise ConnectionError("Network error")
    return "Success"
```

#### Parametric Decorators with State

```python
class Throttle:
    """Throttle function calls (max N calls per period)"""
    def __init__(self, calls=10, period=60):
        self.calls = calls
        self.period = period
        self.clock = time.time
        self.last_reset = self.clock()
        self.num_calls = 0
    
    def __call__(self, func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            now = self.clock()
            
            if now - self.last_reset > self.period:
                self.num_calls = 0
                self.last_reset = now
            
            if self.num_calls >= self.calls:
                raise ThrottleError(f"Rate limit: {self.calls} calls per {self.period}s")
            
            self.num_calls += 1
            return func(*args, **kwargs)
        
        return wrapper

@Throttle(calls=10, period=60)
def rate_limited_api():
    return "API response"
```

#### Decorator Stacking & Composition

```python
def timing(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        print(f"{func.__name__} took {time.time()-start:.4f}s")
        return result
    return wrapper

def logging_decorator(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        print(f"Calling {func.__name__} with {args}, {kwargs}")
        result = func(*args, **kwargs)
        print(f"Result: {result}")
        return result
    return wrapper

# Stack decorators
@timing
@logging_decorator
def process_data(data):
    time.sleep(0.1)
    return f"Processed: {data}"

process_data("test")
# Output:
# Calling process_data with ('test',), {}
# Result: Processed: test
# process_data took 0.1005s
```

#### Class Decorators

```python
def dataclass_like(cls):
    """Simplified dataclass decorator"""
    def __init__(self, **kwargs):
        for key, value in kwargs.items():
            setattr(self, key, value)
    
    def __repr__(self):
        attrs = ", ".join(f"{k}={v!r}" for k, v in self.__dict__.items())
        return f"{cls.__name__}({attrs})"
    
    cls.__init__ = __init__
    cls.__repr__ = __repr__
    return cls

@dataclass_like
class Person:
    pass

p = Person(name="Alice", age=30)
print(p)  # Person(name='Alice', age=30)
```

#### Decorator Composition Function

```python
def compose(*decorators):
    """Compose multiple decorators"""
    def composed(func):
        for decorator in reversed(decorators):
            func = decorator(func)
        return func
    return composed

@compose(timing, logging_decorator)
def process():
    pass

# Equivalent to:
# @timing
# @logging_decorator
# def process():
#     pass
```

---

## Context Managers

### Advanced Context Manager Patterns

#### Context Manager for Resource Pooling

```python
import contextlib
from collections import deque

class ResourcePool:
    """Pool context manager for connection/resource pooling"""
    def __init__(self, factory, pool_size=10):
        self.factory = factory
        self.pool = deque()
        for _ in range(pool_size):
            self.pool.append(factory())
    
    @contextlib.contextmanager
    def acquire(self):
        if not self.pool:
            raise RuntimeError("Pool exhausted")
        
        resource = self.pool.popleft()
        try:
            yield resource
        finally:
            self.pool.append(resource)

# Usage
def create_connection():
    return {"id": id({}), "data": []}

pool = ResourcePool(create_connection, pool_size=3)

with pool.acquire() as conn:
    conn["data"].append("transaction1")
    print(f"Using connection {conn['id']}")

# Connection automatically returned to pool
```

#### Nested Context Managers (Multiple Resource Management)

```python
import contextlib

@contextlib.contextmanager
def open_multiple_files(*filenames):
    """Open multiple files safely"""
    files = []
    try:
        for filename in filenames:
            files.append(open(filename, 'r'))
        yield files
    finally:
        for f in files:
            f.close()

# Usage
with open_multiple_files('file1.txt', 'file2.txt', 'file3.txt') as files:
    for f in files:
        print(f.read())

# Or use ExitStack for dynamic number of resources
from contextlib import ExitStack

with ExitStack() as stack:
    files = [stack.enter_context(open(f)) for f in filenames]
    # All files managed automatically
```

#### Context Manager for Transaction-Like Behavior

```python
class Transaction:
    """Context manager for transactional operations"""
    def __init__(self):
        self.committed = False
        self.changes = []
    
    def __enter__(self):
        print("Transaction started")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is None and not self.committed:
            self.rollback()
        elif exc_type is not None:
            print(f"Exception occurred: {exc_val}. Rolling back.")
            self.rollback()
        
        return False  # Propagate exception
    
    def add_change(self, change):
        self.changes.append(change)
    
    def commit(self):
        print(f"Committing {len(self.changes)} changes")
        self.committed = True
    
    def rollback(self):
        print(f"Rolling back {len(self.changes)} changes")
        self.changes = []

# Usage
with Transaction() as tx:
    tx.add_change("UPDATE users SET active=1")
    tx.add_change("INSERT INTO logs VALUES (...)")
    tx.commit()
```

---

## Closures & Higher-Order Functions

### Advanced Closure Patterns

```python
def make_multiplier(factor):
    """Closure capturing factor"""
    def multiplier(x):
        return x * factor
    
    # Store metadata
    multiplier.factor = factor
    return multiplier

double = make_multiplier(2)
triple = make_multiplier(3)

print(double(5))   # 10
print(triple(5))   # 15
print(double.factor)  # 2
```

### Partial Application

```python
from functools import partial

def power(base, exponent):
    return base ** exponent

# Create specialized functions via partial application
square = partial(power, exponent=2)
cube = partial(power, exponent=3)

print(square(5))  # 25
print(cube(5))    # 125

# Partial with positional arguments
multiply_by_three = partial(lambda x, y: x * y, 3)
print(multiply_by_three(4))  # 12
```

### Currying

```python
def curry(func):
    """Convert function to curried form"""
    def curried(*args):
        if len(args) >= func.__code__.co_argcount:
            return func(*args)
        return partial(curried, *args)
    return curried

@curry
def add(a, b, c):
    return a + b + c

add_1 = add(1)
add_1_2 = add_1(2)
result = add_1_2(3)  # 6

# Or in one line
print(add(1)(2)(3))  # 6
```

---

## Metaclasses & Descriptors

### Understanding Descriptors

```python
class Descriptor:
    """Base descriptor demonstrating descriptor protocol"""
    def __get__(self, obj, objtype=None):
        print(f"__get__ called: obj={obj}, objtype={objtype}")
        return self.value
    
    def __set__(self, obj, value):
        print(f"__set__ called: obj={obj}, value={value}")
        self.value = value
    
    def __delete__(self, obj):
        print(f"__delete__ called")
        del self.value

class MyClass:
    attr = Descriptor()

obj = MyClass()
obj.attr = 5    # Calls __set__
print(obj.attr) # Calls __get__
del obj.attr    # Calls __delete__
```

### Property Descriptor Implementation

```python
class Validated:
    """Descriptor for validated attributes"""
    def __init__(self, validator):
        self.validator = validator
    
    def __set_name__(self, owner, name):
        self.name = f'_{name}'
    
    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        return getattr(obj, self.name, None)
    
    def __set__(self, obj, value):
        if not self.validator(value):
            raise ValueError(f"Invalid value: {value}")
        setattr(obj, self.name, value)

class Person:
    age = Validated(lambda x: isinstance(x, int) and 0 < x < 150)
    email = Validated(lambda x: '@' in x)

p = Person()
p.age = 25      # OK
p.email = "test@example.com"  # OK
p.age = 200     # Raises ValueError
```

### Metaclasses

```python
class Meta(type):
    """Metaclass that logs class creation"""
    def __new__(mcs, name, bases, namespace):
        print(f"Creating class {name}")
        
        # Add methods dynamically
        def __repr__(self):
            return f"<{name} instance>"
        
        namespace['__repr__'] = __repr__
        return super().__new__(mcs, name, bases, namespace)

class MyClass(metaclass=Meta):
    pass

obj = MyClass()  # Output: Creating class MyClass
print(repr(obj))  # <MyClass instance>
```

### Singleton Metaclass

```python
class SingletonMeta(type):
    """Metaclass for singleton pattern"""
    _instances = {}
    
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        return cls._instances[cls]

class Database(metaclass=SingletonMeta):
    def __init__(self):
        self.connection = None

db1 = Database()
db2 = Database()
print(db1 is db2)  # True (same instance!)
```

### Metaclass with Validation

```python
class ValidatedMeta(type):
    """Metaclass that validates attributes"""
    def __new__(mcs, name, bases, namespace):
        # Extract validation rules
        annotations = namespace.get('__annotations__', {})
        
        for field_name, field_type in annotations.items():
            if field_name.startswith('_'):
                continue
            
            # Create validator
            def make_property(fname, ftype):
                private_name = f'_{fname}'
                
                def getter(self):
                    return getattr(self, private_name, None)
                
                def setter(self, value):
                    if not isinstance(value, ftype):
                        raise TypeError(f"{fname} must be {ftype}")
                    setattr(self, private_name, value)
                
                return property(getter, setter)
            
            namespace[field_name] = make_property(field_name, field_type)
        
        return super().__new__(mcs, name, bases, namespace)

class Person(metaclass=ValidatedMeta):
    name: str
    age: int
    
    def __init__(self, name, age):
        self.name = name
        self.age = age

p = Person("Alice", 30)
p.age = "thirty"  # Raises TypeError
```

---

## Object-Oriented Programming Mastery

### Advanced Class Design

```python
class Base:
    """Base class with method resolution order (MRO)"""
    def method(self):
        print("Base.method")

class Mixin1:
    def method(self):
        print("Mixin1.method")
        super().method()

class Mixin2:
    def method(self):
        print("Mixin2.method")
        super().method()

class Derived(Mixin1, Mixin2, Base):
    pass

obj = Derived()
obj.method()
# Output:
# Mixin1.method
# Mixin2.method
# Base.method

print(Derived.__mro__)
# Shows: Derived → Mixin1 → Mixin2 → Base → object
```

### Abstract Base Classes (ABC)

```python
from abc import ABC, abstractmethod

class DataProcessor(ABC):
    """Abstract base class for data processors"""
    
    @abstractmethod
    def process(self, data):
        """Process data (must be implemented)"""
        pass
    
    @abstractmethod
    def validate(self, data):
        """Validate data (must be implemented)"""
        pass
    
    def execute(self, data):
        """Template method (uses abstract methods)"""
        if self.validate(data):
            return self.process(data)
        raise ValueError("Invalid data")

class CSVProcessor(DataProcessor):
    def process(self, data):
        return [line.split(',') for line in data.split('\n')]
    
    def validate(self, data):
        return isinstance(data, str) and len(data) > 0

# Can't instantiate abstract class
# processor = DataProcessor()  # TypeError

processor = CSVProcessor()
result = processor.execute("a,b,c\nd,e,f")
```

### Property Getters/Setters with Caching

```python
class Cached:
    """Property with automatic caching"""
    def __init__(self, func):
        self.func = func
        self.cache = {}
    
    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        
        if obj not in self.cache:
            self.cache[obj] = self.func(obj)
        return self.cache[obj]

class Person:
    def __init__(self, name):
        self.name = name
    
    @Cached
    def expensive_computation(self):
        print("Computing...")
        import time
        time.sleep(1)
        return f"Result for {self.name}"

p = Person("Alice")
print(p.expensive_computation)  # Computing... (1s delay)
print(p.expensive_computation)  # (instant, cached)
```

---

## Functional Programming

### Map, Filter, Reduce

```python
from functools import reduce

data = [1, 2, 3, 4, 5, 6]

# Map: transform
squared = list(map(lambda x: x**2, data))  # [1, 4, 9, 16, 25, 36]

# Filter: select
evens = list(filter(lambda x: x % 2 == 0, data))  # [2, 4, 6]

# Reduce: accumulate
product = reduce(lambda x, y: x * y, data)  # 1*2*3*4*5*6 = 720
sum_val = reduce(lambda x, y: x + y, data, 0)  # 21

# Composition
result = list(map(lambda x: x**2, filter(lambda x: x % 2 == 0, data)))
# [4, 16, 36]
```

### Function Composition

```python
def compose(*functions):
    """Compose functions (right-to-left)"""
    def composed(x):
        for f in reversed(functions):
            x = f(x)
        return x
    return composed

def pipe(*functions):
    """Pipe functions (left-to-right)"""
    def piped(x):
        for f in functions:
            x = f(x)
        return x
    return piped

double = lambda x: x * 2
add_one = lambda x: x + 1
square = lambda x: x ** 2

# Compose: square(add_one(double(x)))
comp = compose(square, add_one, double)
print(comp(3))  # ((3*2)+1)^2 = 49

# Pipe: double(add_one(square(x)))
pip = pipe(square, add_one, double)
print(pip(3))  # (3^2+1)*2 = 20
```

### Immutable Data Structures

```python
from collections import namedtuple
from typing import NamedTuple, FrozenSet

# Option 1: namedtuple
Point = namedtuple('Point', ['x', 'y'])
p1 = Point(1, 2)
# p1.x = 5  # TypeError (immutable)

# Option 2: NamedTuple with type hints
class Person(NamedTuple):
    name: str
    age: int
    email: str

person = Person("Alice", 30, "alice@example.com")

# Option 3: dataclass with frozen=True
from dataclasses import dataclass

@dataclass(frozen=True)
class Config:
    host: str
    port: int

# Use with frozenset for immutable collections
frozen_data = frozenset([1, 2, 3])
# frozen_data.add(4)  # AttributeError
```

---

## PART 2: PERFORMANCE & MEMORY

## Memory Management

### Reference Counting & Garbage Collection

```python
import sys
import gc

# Reference count
a = []
print(sys.getrefcount(a))  # 2 (reference from a, parameter to getrefcount)

b = a  # Another reference
print(sys.getrefcount(a))  # 3

del a
print(sys.getrefcount(b))  # 2 (a reference deleted)

# Garbage collection
import weakref

class Node:
    def __init__(self, value):
        self.value = value
        self.next = None

node1 = Node(1)
node2 = Node(2)

# Create cycle (memory leak without GC)
node1.next = node2
node2.next = node1

del node1
del node2

# Without gc.collect(), cycle would leak
gc.collect()  # Force collection

# Weak references prevent cycles
class WeakNode:
    def __init__(self, value):
        self.value = value
        self.next = None
    
    def set_next(self, node):
        self.next = weakref.ref(node) if node else None

wn1 = WeakNode(1)
wn2 = WeakNode(2)
wn1.set_next(wn2)
wn2.set_next(wn1)

# Cycle broken: memory properly freed
```

### Memory Profiling

```python
import tracemalloc
import linecache
import os

def display_top(snapshot, key_type='lineno', limit=3):
    """Display top memory allocations"""
    snapshot = snapshot.filter_traces((
        tracemalloc.Filter(False, "<frozen importlib._bootstrap>"),
        tracemalloc.Filter(False, "<unknown>"),
    ))
    top_stats = snapshot.statistics(key_type)
    
    print(f"[ Top {limit} ]")
    for index, stat in enumerate(top_stats[:limit], 1):
        frame = stat.traceback[0]
        print(f"#{index}: {frame.filename}:{frame.lineno}: {stat.size / 1024:.1f} KiB")
        line = linecache.getline(frame.filename, frame.lineno).strip()
        if line:
            print(f'    {line}')

tracemalloc.start()

# Your code here
data = [x**2 for x in range(1000000)]

snapshot = tracemalloc.take_snapshot()
display_top(snapshot)
```

### Object Size Analysis

```python
import sys

# Check object sizes
print(sys.getsizeof([]))           # 56 bytes
print(sys.getsizeof([1, 2, 3]))    # 72 bytes
print(sys.getsizeof({}))           # 240 bytes
print(sys.getsizeof(set()))        # 216 bytes
print(sys.getsizeof("hello"))      # 54 bytes
print(sys.getsizeof(b"hello"))     # 54 bytes

# Optimization: __slots__ reduces memory
class WithSlots:
    __slots__ = ['x', 'y']
    def __init__(self, x, y):
        self.x = x
        self.y = y

class WithoutSlots:
    def __init__(self, x, y):
        self.x = x
        self.y = y

ws = WithSlots(1, 2)
wos = WithoutSlots(1, 2)

print(sys.getsizeof(ws))   # ~56 bytes
print(sys.getsizeof(wos))  # ~96 bytes

# __slots__ saves ~40%
```

---

## Performance Optimization

### Profiling with cProfile

```python
import cProfile
import pstats
from io import StringIO

def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

# Profile the function
profiler = cProfile.Profile()
profiler.enable()

result = fibonacci(30)

profiler.disable()
stream = StringIO()
stats = pstats.Stats(profiler, stream=stream)
stats.strip_dirs()
stats.sort_stats('cumulative')
stats.print_stats(10)

print(stream.getvalue())
# Shows: calls, time, cumulative time per function
```

### Optimization Techniques

```python
# Technique 1: Memoization
from functools import lru_cache

@lru_cache(maxsize=128)
def fibonacci_memo(n):
    if n < 2:
        return n
    return fibonacci_memo(n-1) + fibonacci_memo(n-2)

# Technique 2: List comprehension vs loop
# Fast
squares = [x**2 for x in range(1000000)]

# Slower
squares = []
for x in range(1000000):
    squares.append(x**2)

# Technique 3: Local variable access (faster than global)
def slow_loop():
    total = 0
    for i in range(1000000):
        total += i
    return total

def fast_loop():
    total = 0
    for i in range(1000000):
        total += i
    return total

# Technique 4: Use built-ins (written in C)
# Fast
total = sum(range(1000000))

# Slower
total = 0
for i in range(1000000):
    total += i

# Technique 5: NumPy for numerical operations
import numpy as np

# Python: slow
result = [x**2 for x in range(1000000)]

# NumPy: 100x faster
arr = np.arange(1000000)
result = arr**2
```

---

## Concurrency & Parallelism

### Threading (GIL-aware)

```python
import threading
import time
from queue import Queue

# Global Interpreter Lock (GIL) limits true parallelism
# Use for I/O-bound tasks

def worker(queue, worker_id):
    while True:
        item = queue.get()
        if item is None:
            break
        print(f"Worker {worker_id} processing {item}")
        time.sleep(0.1)  # Simulate work
        queue.task_done()

# Create threads
queue = Queue()
num_workers = 4
threads = []

for i in range(num_workers):
    t = threading.Thread(target=worker, args=(queue, i))
    t.start()
    threads.append(t)

# Queue work
for item in range(20):
    queue.put(item)

# Wait for completion
queue.join()

# Stop workers
for _ in range(num_workers):
    queue.put(None)

for t in threads:
    t.join()
```

### Multiprocessing (True Parallelism)

```python
import multiprocessing
import time

def cpu_bound_task(n):
    """CPU-bound task (benefits from multiprocessing)"""
    total = 0
    for i in range(n):
        total += i**2
    return total

if __name__ == '__main__':
    # Single process
    start = time.time()
    result = cpu_bound_task(10000000)
    serial_time = time.time() - start
    
    # Multiple processes
    start = time.time()
    with multiprocessing.Pool(processes=4) as pool:
        results = pool.map(cpu_bound_task, [2500000]*4)
    parallel_time = time.time() - start
    
    print(f"Serial: {serial_time:.2f}s")
    print(f"Parallel: {parallel_time:.2f}s")
    print(f"Speedup: {serial_time/parallel_time:.1f}x")

# Process Pool Example
def work_task(x):
    return x ** 2

with multiprocessing.Pool(processes=4) as pool:
    results = pool.map(work_task, range(100))

# Map-reduce pattern
def mapper(x):
    return x % 10, x

def reducer(key, values):
    return key, sum(values)

with multiprocessing.Pool() as pool:
    mapped = pool.map(mapper, range(100))
    
    # Group by key
    from collections import defaultdict
    groups = defaultdict(list)
    for key, val in mapped:
        groups[key].append(val)
    
    reduced = pool.starmap(reducer, groups.items())
```

---

## Async/Await Mastery

### Event Loop Fundamentals

```python
import asyncio

async def async_function():
    """Coroutine: returns when awaited"""
    print("Starting")
    await asyncio.sleep(1)
    print("Done")
    return "Result"

# Run async function
result = asyncio.run(async_function())

# Tasks allow concurrent execution
async def task_example():
    # Create tasks (don't wait yet)
    task1 = asyncio.create_task(async_function())
    task2 = asyncio.create_task(async_function())
    
    # Wait for all
    results = await asyncio.gather(task1, task2)
    return results

# Run
results = asyncio.run(task_example())
```

### Async Context Managers & Iterators

```python
class AsyncResource:
    async def __aenter__(self):
        print("Acquiring resource")
        await asyncio.sleep(0.1)
        return self
    
    async def __aexit__(self, exc_type, exc, tb):
        print("Releasing resource")
        await asyncio.sleep(0.1)
    
    async def fetch(self):
        await asyncio.sleep(0.1)
        return "data"

async def async_context_example():
    async with AsyncResource() as resource:
        data = await resource.fetch()
        print(data)

# Async iterator
class AsyncCounter:
    def __init__(self, n):
        self.n = n
        self.i = 0
    
    def __aiter__(self):
        return self
    
    async def __anext__(self):
        if self.i >= self.n:
            raise StopAsyncIteration
        
        await asyncio.sleep(0.1)
        self.i += 1
        return self.i

async def async_iterator_example():
    async for value in AsyncCounter(3):
        print(value)

# Async generator
async def async_generator():
    for i in range(3):
        await asyncio.sleep(0.1)
        yield i

async def async_gen_example():
    async for value in async_generator():
        print(value)
```

### Producer-Consumer with asyncio

```python
import asyncio

async def producer(queue, n):
    """Produce items"""
    for i in range(n):
        await asyncio.sleep(0.1)
        item = f"item-{i}"
        await queue.put(item)
        print(f"Produced {item}")
    
    # Signal completion
    await queue.put(None)

async def consumer(queue, consumer_id):
    """Consume items"""
    while True:
        item = await queue.get()
        if item is None:
            queue.task_done()
            break
        
        await asyncio.sleep(0.2)
        print(f"Consumer {consumer_id} processed {item}")
        queue.task_done()

async def main():
    queue = asyncio.Queue()
    
    # Start producers and consumers
    producer_task = asyncio.create_task(producer(queue, 10))
    consumers = [
        asyncio.create_task(consumer(queue, i))
        for i in range(3)
    ]
    
    # Wait for all work to complete
    await queue.join()
    
    # Cancel remaining tasks
    for c in consumers:
        c.cancel()
    
    await asyncio.gather(*consumers, return_exceptions=True)

asyncio.run(main())
```

### Rate Limiting with asyncio

```python
class RateLimiter:
    def __init__(self, rate, period):
        self.rate = rate          # Number of calls
        self.period = period      # Seconds
        self.allowance = rate
        self.last_check = asyncio.get_event_loop().time()
    
    async def acquire(self):
        """Acquire permission, wait if necessary"""
        while self.allowance < 1:
            now = asyncio.get_event_loop().time()
            time_passed = now - self.last_check
            self.last_check = now
            self.allowance += time_passed * (self.rate / self.period)
            
            if self.allowance >= 1:
                break
            
            # Wait before retrying
            await asyncio.sleep((1 - self.allowance) * (self.period / self.rate))
        
        self.allowance -= 1

# Usage
limiter = RateLimiter(rate=10, period=60)  # 10 requests per 60 seconds

async def api_call():
    await limiter.acquire()
    print("Making API call")

async def main():
    tasks = [api_call() for _ in range(20)]
    await asyncio.gather(*tasks)

asyncio.run(main())
```

---

## PART 3: ADVANCED DATA STRUCTURES

## Collections & Data Structures

### Advanced Dictionary Operations

```python
from collections import defaultdict, OrderedDict, Counter, ChainMap

# defaultdict: default value for missing keys
dd = defaultdict(list)
dd['a'].append(1)
dd['a'].append(2)
print(dd)  # defaultdict(<class 'list'>, {'a': [1, 2]})

# Counter: count occurrences
words = ['apple', 'banana', 'apple', 'cherry', 'banana', 'apple']
counter = Counter(words)
print(counter)  # Counter({'apple': 3, 'banana': 2, 'cherry': 1})

# Most common
print(counter.most_common(2))  # [('apple', 3), ('banana', 2)]

# ChainMap: merge multiple dicts
dict1 = {'a': 1, 'b': 2}
dict2 = {'c': 3, 'd': 4}
merged = ChainMap(dict1, dict2)
print(merged['a'])  # 1
print(merged['c'])  # 3

# OrderedDict: preserve insertion order (Python 3.7+ dict does this)
od = OrderedDict()
od['z'] = 1
od['a'] = 2
od['m'] = 3
print(list(od.keys()))  # ['z', 'a', 'm']
```

### Deque (Double-Ended Queue)

```python
from collections import deque

# Efficient append/pop from both ends
dq = deque([1, 2, 3])
dq.appendleft(0)      # [0, 1, 2, 3]
dq.append(4)          # [0, 1, 2, 3, 4]
dq.popleft()          # Returns 0, deque: [1, 2, 3, 4]
dq.pop()              # Returns 4, deque: [1, 2, 3]

# Rotate
dq = deque([1, 2, 3, 4, 5])
dq.rotate(2)  # [4, 5, 1, 2, 3]

# Extend from both ends
dq.extendleft([0, -1])  # [-1, 0, 4, 5, 1, 2, 3]

# Maxlen (act as circular buffer)
circular = deque([1, 2, 3], maxlen=3)
circular.append(4)  # [2, 3, 4] (oldest removed)
```

### Heap (Priority Queue)

```python
import heapq

# Min heap
heap = [3, 1, 4, 1, 5, 9, 2, 6]
heapq.heapify(heap)  # Convert to heap in-place
print(heap)  # [1, 1, 2, 3, 5, 9, 4, 6]

# Pop min
print(heapq.heappop(heap))  # 1
print(heap)  # [1, 3, 2, 6, 5, 9, 4]

# Push new element
heapq.heappush(heap, 0)
print(heap)  # [0, 1, 2, 6, 3, 9, 4, 5]

# Get n smallest
print(heapq.nsmallest(3, heap))  # [0, 1, 2]
print(heapq.nlargest(3, heap))   # [9, 6, 5]

# Max heap (use negative values)
max_heap = [-x for x in [3, 1, 4, 1, 5]]
heapq.heapify(max_heap)
print(-heapq.heappop(max_heap))  # 5

# Priority queue with tuples
pq = []
heapq.heappush(pq, (3, 'medium'))
heapq.heappush(pq, (1, 'high'))
heapq.heappush(pq, (5, 'low'))

while pq:
    priority, task = heapq.heappop(pq)
    print(f"Priority {priority}: {task}")
```

### Custom Data Structures

```python
class LinkedListNode:
    def __init__(self, value):
        self.value = value
        self.next = None

class LinkedList:
    def __init__(self):
        self.head = None
    
    def append(self, value):
        if not self.head:
            self.head = LinkedListNode(value)
        else:
            current = self.head
            while current.next:
                current = current.next
            current.next = LinkedListNode(value)
    
    def traverse(self):
        current = self.head
        while current:
            yield current.value
            current = current.next
    
    def __repr__(self):
        return " -> ".join(str(v) for v in self.traverse())

# Binary Search Tree
class TreeNode:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

class BinarySearchTree:
    def __init__(self):
        self.root = None
    
    def insert(self, value):
        if not self.root:
            self.root = TreeNode(value)
        else:
            self._insert_recursive(self.root, value)
    
    def _insert_recursive(self, node, value):
        if value < node.value:
            if node.left is None:
                node.left = TreeNode(value)
            else:
                self._insert_recursive(node.left, value)
        else:
            if node.right is None:
                node.right = TreeNode(value)
            else:
                self._insert_recursive(node.right, value)
    
    def search(self, value):
        return self._search_recursive(self.root, value)
    
    def _search_recursive(self, node, value):
        if node is None:
            return False
        
        if value == node.value:
            return True
        elif value < node.value:
            return self._search_recursive(node.left, value)
        else:
            return self._search_recursive(node.right, value)
    
    def in_order_traversal(self):
        return list(self._in_order(self.root))
    
    def _in_order(self, node):
        if node:
            yield from self._in_order(node.left)
            yield node.value
            yield from self._in_order(node.right)

# Usage
bst = BinarySearchTree()
for val in [5, 3, 7, 1, 9]:
    bst.insert(val)

print(bst.in_order_traversal())  # [1, 3, 5, 7, 9]
print(bst.search(7))              # True
```

---

## Design Patterns

### Singleton Pattern

```python
# Pattern 1: Metaclass-based
class SingletonMeta(type):
    _instances = {}
    
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        return cls._instances[cls]

class Logger(metaclass=SingletonMeta):
    def __init__(self):
        self.logs = []
    
    def log(self, message):
        self.logs.append(message)

# Pattern 2: Decorator-based
def singleton(cls):
    instances = {}
    
    def get_instance(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]
    
    return get_instance

@singleton
class Database:
    def __init__(self):
        self.connection = None
```

### Observer Pattern

```python
class Subject:
    def __init__(self):
        self._observers = []
    
    def attach(self, observer):
        self._observers.append(observer)
    
    def detach(self, observer):
        self._observers.remove(observer)
    
    def notify(self, event):
        for observer in self._observers:
            observer.update(event)

class Observer:
    def update(self, event):
        raise NotImplementedError

class ConcreteObserver(Observer):
    def __init__(self, name):
        self.name = name
    
    def update(self, event):
        print(f"{self.name} received event: {event}")

# Usage
subject = Subject()
obs1 = ConcreteObserver("Observer 1")
obs2 = ConcreteObserver("Observer 2")

subject.attach(obs1)
subject.attach(obs2)

subject.notify("Event occurred!")
# Output:
# Observer 1 received event: Event occurred!
# Observer 2 received event: Event occurred!
```

### Strategy Pattern

```python
from abc import ABC, abstractmethod

class Strategy(ABC):
    @abstractmethod
    def execute(self, data):
        pass

class SortingStrategy(Strategy):
    def execute(self, data):
        return sorted(data)

class ReverseStrategy(Strategy):
    def execute(self, data):
        return sorted(data, reverse=True)

class Context:
    def __init__(self, strategy):
        self.strategy = strategy
    
    def execute(self, data):
        return self.strategy.execute(data)

# Usage
data = [3, 1, 4, 1, 5, 9, 2, 6]

context = Context(SortingStrategy())
print(context.execute(data))  # [1, 1, 2, 3, 4, 5, 6, 9]

context = Context(ReverseStrategy())
print(context.execute(data))  # [9, 6, 5, 4, 3, 2, 1, 1]
```

### Factory Pattern

```python
class DatabaseFactory:
    @staticmethod
    def create_database(db_type):
        if db_type == 'mysql':
            return MySQLDatabase()
        elif db_type == 'postgresql':
            return PostgreSQLDatabase()
        elif db_type == 'mongodb':
            return MongoDBDatabase()
        else:
            raise ValueError(f"Unknown database type: {db_type}")

class Database(ABC):
    @abstractmethod
    def connect(self):
        pass

class MySQLDatabase(Database):
    def connect(self):
        return "Connected to MySQL"

class PostgreSQLDatabase(Database):
    def connect(self):
        return "Connected to PostgreSQL"

class MongoDBDatabase(Database):
    def connect(self):
        return "Connected to MongoDB"

# Usage
db = DatabaseFactory.create_database('postgresql')
print(db.connect())
```

---

## Testing & Debugging

### Unit Testing with unittest

```python
import unittest
from unittest.mock import Mock, patch, MagicMock

class Calculator:
    def add(self, a, b):
        return a + b
    
    def subtract(self, a, b):
        return a - b
    
    def multiply(self, a, b):
        return a * b

class TestCalculator(unittest.TestCase):
    def setUp(self):
        """Setup before each test"""
        self.calc = Calculator()
    
    def tearDown(self):
        """Cleanup after each test"""
        self.calc = None
    
    def test_add(self):
        result = self.calc.add(2, 3)
        self.assertEqual(result, 5)
        self.assertIsInstance(result, int)
    
    def test_subtract(self):
        result = self.calc.subtract(5, 3)
        self.assertEqual(result, 2)
    
    def test_multiply(self):
        result = self.calc.multiply(4, 5)
        self.assertEqual(result, 20)
    
    def test_add_floats(self):
        result = self.calc.add(2.5, 3.5)
        self.assertAlmostEqual(result, 6.0)

# Run tests
if __name__ == '__main__':
    unittest.main()
```

### Testing with pytest

```python
import pytest

def divide(a, b):
    if b == 0:
        raise ValueError("Cannot divide by zero")
    return a / b

def test_divide():
    assert divide(10, 2) == 5
    assert divide(9, 3) == 3

def test_divide_by_zero():
    with pytest.raises(ValueError, match="Cannot divide by zero"):
        divide(10, 0)

@pytest.mark.parametrize("a,b,expected", [
    (10, 2, 5),
    (9, 3, 3),
    (8, 4, 2),
])
def test_divide_parametrized(a, b, expected):
    assert divide(a, b) == expected
```

### Mocking and Patching

```python
from unittest.mock import Mock, patch, MagicMock

class APIClient:
    def fetch_data(self, url):
        # Real implementation would make HTTP request
        pass

def process_data(api_client, url):
    data = api_client.fetch_data(url)
    return data.upper()

def test_with_mock():
    # Create mock
    mock_api = Mock()
    mock_api.fetch_data.return_value = "hello world"
    
    result = process_data(mock_api, "http://api.example.com")
    assert result == "HELLO WORLD"
    
    # Verify call
    mock_api.fetch_data.assert_called_once_with("http://api.example.com")

def test_with_patch():
    with patch('requests.get') as mock_get:
        mock_get.return_value.status_code = 200
        mock_get.return_value.json.return_value = {"key": "value"}
        
        # Your code that uses requests.get
        response = mock_get("http://api.example.com")
        assert response.status_code == 200
```

### Debugging

```python
import pdb
import logging
import traceback

# Setup logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

def complex_function(x):
    logger.debug(f"Input: {x}")
    
    # Drop into debugger
    # pdb.set_trace()
    
    result = x ** 2
    logger.debug(f"Calculated: {result}")
    
    return result

# Exception handling
try:
    1 / 0
except ZeroDivisionError:
    logger.exception("Division by zero occurred:")
    traceback.print_exc()

# Stack inspection
import inspect

def get_caller_info():
    frame = inspect.currentframe().f_back
    info = inspect.getframeinfo(frame)
    return {
        'filename': info.filename,
        'lineno': info.lineno,
        'function': info.function,
    }
```

---

## PART 4: PROFESSIONAL DEVELOPMENT

## Type Hints & Mypy

### Advanced Type Hints

```python
from typing import (
    List, Dict, Tuple, Optional, Union, Callable, 
    TypeVar, Generic, Protocol, Literal, Final, ClassVar
)

# Basic types
def process_list(items: List[int]) -> Dict[str, int]:
    return {"count": len(items), "sum": sum(items)}

# Union types
def parse_input(value: Union[int, str]) -> str:
    if isinstance(value, int):
        return str(value)
    return value

# Optional (Union[X, None])
def get_first(items: List[int]) -> Optional[int]:
    return items[0] if items else None

# Callable
def apply_operation(x: int, y: int, op: Callable[[int, int], int]) -> int:
    return op(x, y)

# TypeVar for generics
T = TypeVar('T')

def get_first_item(items: List[T]) -> T:
    return items[0]

# Generic class
class Container(Generic[T]):
    def __init__(self, item: T):
        self.item = item
    
    def get(self) -> T:
        return self.item

# Protocol (structural typing)
from typing import Protocol

class Drawable(Protocol):
    def draw(self) -> str: ...

class Circle:
    def draw(self) -> str:
        return "Circle"

class Square:
    def draw(self) -> str:
        return "Square"

def render(obj: Drawable) -> None:
    print(obj.draw())

# Literal (specific values)
def set_status(status: Literal["pending", "complete", "failed"]) -> None:
    pass

# Final (can't override)
class Parent:
    MAX_SIZE: Final[int] = 100

# Class variable
class Counter:
    count: ClassVar[int] = 0
    
    def __init__(self):
        Counter.count += 1
```

### Type Checking with MyPy

```python
# mypy configuration (setup.cfg or pyproject.toml)
# [mypy]
# python_version = 3.9
# warn_return_any = True
# warn_unused_configs = True
# ignore_missing_imports = True

# mypy command line
# mypy your_module.py
# mypy --strict your_module.py

# Ignore errors
def problematic_function() -> None:
    x: int = "not an int"  # type: ignore
```

---

## C Extensions & ctypes

### Using ctypes

```python
import ctypes
import ctypes.util

# Load C library
libc = ctypes.CDLL(ctypes.util.find_library('c'))

# Define function signature
# int strlen(const char *s);
strlen = libc.strlen
strlen.argtypes = [ctypes.c_char_p]
strlen.restype = ctypes.c_int

# Call C function
length = strlen(b"Hello")
print(length)  # 5

# Structures
class Point(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_double),
        ("y", ctypes.c_double),
    ]

p = Point(3.5, 4.2)
print(f"Point: ({p.x}, {p.y})")
```

---

## Package Development

### Package Structure

```
my_package/
├── my_package/
│   ├── __init__.py
│   ├── module1.py
│   ├── module2.py
│   └── subpackage/
│       ├── __init__.py
│       └── module3.py
├── tests/
│   ├── __init__.py
│   ├── test_module1.py
│   └── test_module2.py
├── setup.py
├── pyproject.toml
├── README.md
└── requirements.txt
```

### setup.py

```python
from setuptools import setup, find_packages

setup(
    name="my-package",
    version="1.0.0",
    description="My awesome package",
    author="Your Name",
    author_email="email@example.com",
    url="https://github.com/username/repo",
    packages=find_packages(),
    install_requires=[
        "requests>=2.28.0",
        "numpy>=1.20.0",
    ],
    extras_require={
        "dev": ["pytest>=7.0", "mypy>=0.950"],
        "docs": ["sphinx>=4.0"],
    },
    python_requires=">=3.9",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
    ],
)
```

### pyproject.toml (Modern)

```toml
[build-system]
requires = ["setuptools>=45", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "my-package"
version = "1.0.0"
description = "My awesome package"
requires-python = ">=3.9"
dependencies = [
    "requests>=2.28.0",
    "numpy>=1.20.0",
]

[project.optional-dependencies]
dev = ["pytest>=7.0", "mypy>=0.950"]
docs = ["sphinx>=4.0"]

[tool.mypy]
python_version = "3.9"
warn_return_any = true
strict = true
```

---

## Production Best Practices

### Configuration Management

```python
import os
from dataclasses import dataclass
from pathlib import Path

@dataclass
class Config:
    """Configuration with environment variable support"""
    debug: bool = os.getenv("DEBUG", "False") == "True"
    host: str = os.getenv("HOST", "localhost")
    port: int = int(os.getenv("PORT", 8000))
    database_url: str = os.getenv("DATABASE_URL", "sqlite:///app.db")
    log_level: str = os.getenv("LOG_LEVEL", "INFO")

config = Config()

# Or use python-dotenv
from dotenv import load_dotenv

load_dotenv()
api_key = os.getenv("API_KEY")
```

### Logging Best Practices

```python
import logging
import logging.handlers
from datetime import datetime

# Setup logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# File handler with rotation
handler = logging.handlers.RotatingFileHandler(
    'app.log',
    maxBytes=10*1024*1024,  # 10 MB
    backupCount=5
)

# Format
formatter = logging.Formatter(
    '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
handler.setFormatter(formatter)

logger.addHandler(handler)

# Usage
logger.debug("Debug message")
logger.info("Info message")
logger.warning("Warning message")
logger.error("Error message")
logger.critical("Critical message")
```

### Error Handling

```python
# Custom exceptions
class ApplicationError(Exception):
    """Base application exception"""
    pass

class ConfigurationError(ApplicationError):
    """Configuration error"""
    pass

class DatabaseError(ApplicationError):
    """Database error"""
    pass

# Usage
try:
    # Your code
    pass
except ConfigurationError as e:
    logger.error(f"Configuration error: {e}")
except DatabaseError as e:
    logger.error(f"Database error: {e}")
except Exception as e:
    logger.exception(f"Unexpected error: {e}")
    raise
```

---

## PART 5: SYSTEM PROGRAMMING

## File I/O & Binary Data

### Binary Data Handling

```python
import struct
import io

# Pack binary data
data = struct.pack('idf', 42, 3.14, 1.0)  # int, double, float
print(data)  # Binary representation

# Unpack binary data
unpacked = struct.unpack('idf', data)
print(unpacked)  # (42, 3.14, 1.0)

# Working with bytes
b = bytearray()
b.extend([0x48, 0x65, 0x6c, 0x6c, 0x6f])
print(bytes(b))  # b'Hello'

# StringIO and BytesIO
text_buffer = io.StringIO()
text_buffer.write("Hello, ")
text_buffer.write("World!")
print(text_buffer.getvalue())  # "Hello, World!"

binary_buffer = io.BytesIO()
binary_buffer.write(b"Hello")
binary_buffer.seek(0)
print(binary_buffer.read())  # b'Hello'
```

---

## Networking & Protocols

### Socket Programming

```python
import socket
import select
import threading

# TCP Server
def tcp_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('localhost', 5000))
    server_socket.listen(5)
    
    print("Server listening on port 5000")
    
    while True:
        client_socket, address = server_socket.accept()
        print(f"Connection from {address}")
        
        data = client_socket.recv(1024)
        print(f"Received: {data.decode()}")
        
        client_socket.send(b"Hello, client!")
        client_socket.close()

# TCP Client
def tcp_client():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('localhost', 5000))
    
    client_socket.send(b"Hello, server!")
    data = client_socket.recv(1024)
    print(f"Received: {data.decode()}")
    
    client_socket.close()

# Non-blocking I/O with select
def non_blocking_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('localhost', 5000))
    server_socket.listen(5)
    server_socket.setblocking(False)
    
    sockets = [server_socket]
    
    while True:
        readable, _, _ = select.select(sockets, [], [])
        
        for sock in readable:
            if sock is server_socket:
                client_socket, address = server_socket.accept()
                sockets.append(client_socket)
            else:
                data = sock.recv(1024)
                if data:
                    sock.send(data.upper())
                else:
                    sockets.remove(sock)
                    sock.close()
```

---

## Database Programming

### SQLAlchemy ORM

```python
from sqlalchemy import create_engine, Column, Integer, String, DateTime, ForeignKey
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship
from datetime import datetime

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'
    
    id = Column(Integer, primary_key=True)
    username = Column(String, unique=True, nullable=False)
    email = Column(String, unique=True, nullable=False)
    created_at = Column(DateTime, default=datetime.utcnow)
    
    posts = relationship("Post", back_populates="author")

class Post(Base):
    __tablename__ = 'posts'
    
    id = Column(Integer, primary_key=True)
    title = Column(String, nullable=False)
    content = Column(String)
    user_id = Column(Integer, ForeignKey('users.id'))
    created_at = Column(DateTime, default=datetime.utcnow)
    
    author = relationship("User", back_populates="posts")

# Create engine and tables
engine = create_engine('sqlite:///app.db')
Base.metadata.create_all(engine)

# Create session
Session = sessionmaker(bind=engine)
session = Session()

# Create user
user = User(username='alice', email='alice@example.com')
session.add(user)
session.commit()

# Query
users = session.query(User).filter_by(username='alice').all()
print(users[0].email)

# Relationships
post = Post(title='My First Post', content='Hello!', author=user)
session.add(post)
session.commit()

# Query with join
user_with_posts = session.query(User).filter_by(username='alice').first()
print(user_with_posts.posts)
```

---

## FINAL CHECKLIST FOR PYTHON GODHOOD

### Core Concepts Mastery
- ☐ Generators: yield, yield from, generator expressions
- ☐ Decorators: function, class, stacking, parameterized
- ☐ Context managers: with statement, custom implementations
- ☐ Closures: capturing variables, nonlocal
- ☐ Metaclasses: custom metaclasses, singleton pattern
- ☐ Descriptors: __get__, __set__, __delete__
- ☐ OOP: ABC, inheritance, MRO, multiple inheritance
- ☐ Functional programming: map, filter, reduce, composition

### Performance & Memory
- ☐ Profiling: cProfile, memory profiling
- ☐ Optimization: list comprehensions, local variables
- ☐ Memory management: gc, weakref, __slots__
- ☐ Concurrency: threading, multiprocessing, asyncio
- ☐ Async: coroutines, async context managers, async iterators

### Advanced Data Structures
- ☐ Collections: deque, defaultdict, Counter, OrderedDict
- ☐ Heaps: heapq, priority queues
- ☐ Custom data structures: linked lists, trees, graphs
- ☐ Algorithms: sorting, searching, graph traversal

### Design Patterns
- ☐ Singleton: metaclass, decorator
- ☐ Observer: subject-observer pattern
- ☐ Strategy: strategy pattern, polymorphism
- ☐ Factory: factory method, abstract factory
- ☐ Decorator: decorator pattern
- ☐ Builder: builder pattern for complex objects

### Testing & Debugging
- ☐ Unit testing: unittest, pytest
- ☐ Mocking: Mock, patch, side_effect
- ☐ Debugging: pdb, logging, traceback
- ☐ Type checking: type hints, mypy

### Professional Development
- ☐ Type hints: advanced type annotations
- ☐ C extensions: ctypes, CFFI
- ☐ Package development: setup.py, pyproject.toml
- ☐ Configuration: environment variables, config files
- ☐ Logging: logging module, handlers, formatters
- ☐ Error handling: custom exceptions

### System Programming
- ☐ File I/O: binary, text, streaming
- ☐ Networking: sockets, select, threading
- ☐ Databases: SQLAlchemy, transactions
- ☐ Protocols: HTTP, REST, JSON

### Best Practices
- ☐ Code organization: modules, packages, namespaces
- ☐ Documentation: docstrings, type hints, examples
- ☐ Performance: benchmarking, optimization
- ☐ Security: input validation, SQL injection prevention
- ☐ Maintainability: SOLID principles, design patterns
- ☐ Testing: test coverage, edge cases
- ☐ Production: monitoring, logging, error handling

---

## Key Insights for Python Mastery

1. **Generators** = Memory efficiency (1000x for large datasets)
2. **Decorators** = Code reuse & separation of concerns
3. **Async/Await** = Concurrency without threads (better scalability)
4. **Type hints** = Better IDE support, catch bugs early
5. **Context managers** = Resource safety guaranteed
6. **Metaclasses** = Metaprogramming at the deepest level
7. **Functional patterns** = Composable, testable code
8. **Performance profiling** = Data-driven optimization
9. **Design patterns** = Proven solutions to common problems
10. **Testing** = Confidence in production code

---

**You are now ready to become the best Python programmer in the universe!** 🚀

Go forth and build amazing things!

---

## Additional Resources

### Must-Read Books
- "Fluent Python" by Luciano Ramalho
- "Effective Python" by Brett Slatkin
- "Python Cookbook" by David Beazley
- "Design Patterns in Python" by Gang of Four

### Online Resources
- Python Official Documentation: https://docs.python.org/3/
- Real Python: https://realpython.com/
- Stack Overflow: https://stackoverflow.com/questions/tagged/python
- GitHub: https://github.com (study open-source code)

### Practice
- LeetCode: https://leetcode.com/ (algorithms)
- HackerRank: https://www.hackerrank.com/ (coding challenges)
- Project Euler: https://projecteuler.net/ (math problems)
- Build projects: personal tools, web apps, data analysis

---

*Last Updated: December 2025*
*Python Version: 3.9+*
