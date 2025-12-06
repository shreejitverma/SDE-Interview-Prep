# Advanced Python: Generators, Decorators, and Functional Programming

## Table of Contents
1. [Generators](#generators)
2. [Decorators](#decorators)
3. [Context Managers](#context-managers)
4. [Itertools and Functional Programming](#itertools-and-functional-programming)
5. [Closures](#closures)
6. [Lambda Functions](#lambda-functions)
7. [Advanced Patterns](#advanced-patterns)
8. [Trading System Applications](#trading-system-applications)

---

## SECTION 1: GENERATORS

### 1.1 What is a Generator?

A generator is a **function that yields values one at a time**, rather than returning all values at once. It creates an **iterator** that maintains state between calls.

#### Key Characteristics

```
Normal Function:
├─ Computes everything
├─ Returns once
└─ All data in memory

Generator Function:
├─ Yields one value at a time
├─ Pauses (suspends) after each yield
├─ Resumes from where it paused
├─ Memory-efficient (lazy evaluation)
└─ Can be infinite
```

#### Simple Example

```python
# Normal function: returns list
def get_numbers_list():
    result = []
    for i in range(1000000):
        result.append(i)
    return result  # All 1M numbers in memory!

numbers = get_numbers_list()  # Uses ~40 MB RAM

# Generator: yields one at a time
def get_numbers_generator():
    for i in range(1000000):
        yield i  # Pause here; resume when called again

numbers = get_numbers_generator()  # Uses ~50 bytes (iterator object only!)

# Use generator
for num in numbers:
    print(num)  # Each number generated on-demand
```

#### Mental Model

```python
def simple_generator():
    print("1: Start")
    yield 1
    print("2: Between 1 and 2")
    yield 2
    print("3: Between 2 and 3")
    yield 3
    print("4: Done")

gen = simple_generator()

# Nothing printed yet! Generator not started.

print(next(gen))  # Runs until first yield
# Output:
# 1: Start
# 1

print(next(gen))  # Resumes from after yield 1
# Output:
# 2: Between 1 and 2
# 2

print(next(gen))  # Resumes from after yield 2
# Output:
# 3: Between 2 and 3
# 3

print(next(gen))  # Resumes from after yield 3
# Output:
# 4: Done
# StopIteration exception (generator exhausted)
```

### 1.2 Generator Mechanics

#### The Iterator Protocol

```python
# Iterator protocol requires:
class Iterator:
    def __iter__(self):
        return self  # Returns iterator object
    
    def __next__(self):
        # Return next value
        # Raise StopIteration when done

# Generator automatically implements this!
def my_generator():
    yield 1
    yield 2
    yield 3

gen = my_generator()
print(iter(gen) is gen)  # True! Generator is its own iterator
```

#### Generator State

```python
def stateful_generator():
    x = 0
    while True:
        x += 1
        value = yield x
        print(f"Received: {value}")

gen = stateful_generator()

print(next(gen))  # 1
print(next(gen))  # 2
print(gen.send(100))  # Send value back; received: 100; yields 3

# State is preserved!
```

### 1.3 Creating Generators

#### Method 1: Generator Function (with yield)

```python
def count_up_to(n):
    """Simple generator"""
    i = 0
    while i < n:
        yield i
        i += 1

for num in count_up_to(5):
    print(num)  # 0, 1, 2, 3, 4
```

#### Method 2: Generator Expression (like list comprehension)

```python
# List comprehension (stores all in memory)
squares_list = [x**2 for x in range(1000000)]  # ~40 MB

# Generator expression (lazy)
squares_gen = (x**2 for x in range(1000000))  # ~50 bytes

# Use it
for square in squares_gen:
    print(square)

# Can pass to functions
sum(squares_gen)  # Sum without storing list
```

#### Method 3: Using itertools

```python
import itertools

# Infinite counter
counter = itertools.count(1)
for num in counter:
    print(num)  # 1, 2, 3, ... (infinite)
    if num > 5:
        break

# Repeat value
repeater = itertools.repeat("hello", 3)
for val in repeater:
    print(val)  # hello, hello, hello

# Cycle through values
cycler = itertools.cycle([1, 2, 3])
for val in cycler:
    print(val)  # 1, 2, 3, 1, 2, 3, ... (infinite)
```

### 1.4 Generator Functions in Detail

#### Sending Values to Generators

```python
def echo_generator():
    while True:
        value = yield "Ready"
        if value:
            print(f"Echoing: {value}")

gen = echo_generator()

print(next(gen))  # "Ready" (starts generator, yield returns "Ready")
print(gen.send("Hello"))  # Sends "Hello" to yield, prints "Echoing: Hello"
# "Ready"
print(gen.send("World"))  # "Echoing: World"
# "Ready"
```

#### Throwing Exceptions

```python
def error_handler():
    try:
        while True:
            try:
                value = yield "Ready for input"
            except ValueError as e:
                print(f"Caught error: {e}")
    except GeneratorExit:
        print("Generator closed")

gen = error_handler()
print(next(gen))  # "Ready for input"
print(gen.send("data"))  # "Ready for input"
gen.throw(ValueError("Bad value"))  # Raises ValueError in generator
# Caught error: Bad value
# "Ready for input"
```

#### Returning Values

```python
def generator_with_return():
    yield 1
    yield 2
    return "Done!"  # Return value accessible via StopIteration

gen = generator_with_return()
try:
    while True:
        print(next(gen))
except StopIteration as e:
    print(f"Return value: {e.value}")

# Output:
# 1
# 2
# Return value: Done!
```

### 1.5 Practical Generator Examples

#### Example 1: Reading Large Files Efficiently

```python
def read_large_file(file_path, chunk_size=1024):
    """Read file in chunks (memory-efficient)"""
    with open(file_path, 'r') as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            yield chunk

# Use it
for chunk in read_large_file('huge_file.txt', chunk_size=8192):
    process(chunk)  # Process one chunk at a time
    # Memory usage stays constant!
```

#### Example 2: Infinite Fibonacci

```python
def fibonacci():
    """Infinite Fibonacci sequence"""
    a, b = 0, 1
    while True:
        yield a
        a, b = b, a + b

fib = fibonacci()

# Get first 10 Fibonacci numbers
for _ in range(10):
    print(next(fib), end=' ')
# 0 1 1 2 3 5 8 13 21 34
```

#### Example 3: Parsing Data Stream

```python
def parse_log_stream(log_lines):
    """Parse log lines and yield errors"""
    for line in log_lines:
        if "ERROR" in line:
            timestamp, message = line.split(' | ')
            yield {"timestamp": timestamp, "message": message}

# Use with large log file
for error in parse_log_stream(read_large_file('app.log')):
    print(f"Error at {error['timestamp']}: {error['message']}")
```

#### Example 4: Pipelining (Composing Generators)

```python
def read_file(file_path):
    """Generator: read lines"""
    with open(file_path) as f:
        for line in f:
            yield line.strip()

def filter_comments(lines):
    """Generator: filter out comments"""
    for line in lines:
        if not line.startswith('#'):
            yield line

def parse_csv(lines):
    """Generator: parse CSV lines"""
    for line in lines:
        fields = line.split(',')
        yield fields

# Pipeline: read → filter → parse
for fields in parse_csv(filter_comments(read_file('data.csv'))):
    print(fields)  # Each field parsed on-demand
```

### 1.6 Generator Performance

```python
import time
import tracemalloc

# List approach
tracemalloc.start()
start = time.time()

squares_list = [x**2 for x in range(10000000)]
list_time = time.time() - start
list_memory = tracemalloc.get_traced_memory()[0] / 1024 / 1024  # MB

print(f"List: {list_time:.2f}s, {list_memory:.2f}MB")

# Generator approach
tracemalloc.reset_peak()
start = time.time()

squares_gen = (x**2 for x in range(10000000))
result = sum(squares_gen)  # Only now does computation happen
gen_time = time.time() - start
gen_memory = tracemalloc.get_traced_memory()[0] / 1024 / 1024

print(f"Generator: {gen_time:.2f}s, {gen_memory:.2f}MB")

# Output (typical):
# List: 0.45s, 400.00MB
# Generator: 0.42s, 0.05MB
# Generator is 8000x more memory-efficient!
```

---

## SECTION 2: DECORATORS

### 2.1 What is a Decorator?

A decorator is a **function that takes another function and extends its behavior without permanently modifying it**.

Decorators are **higher-order functions**: they take functions as input and return functions as output.

#### Simple Example

```python
def my_decorator(func):
    """Decorator: adds greeting before and after function"""
    def wrapper():
        print("Hello!")
        func()
        print("Goodbye!")
    return wrapper

def say_hello():
    print("I'm saying hello")

# Apply decorator
say_hello = my_decorator(say_hello)

say_hello()
# Output:
# Hello!
# I'm saying hello
# Goodbye!
```

#### Using @ Syntax

```python
@my_decorator
def say_hello():
    print("I'm saying hello")

say_hello()
# Same output as above!

# @my_decorator is shorthand for:
# say_hello = my_decorator(say_hello)
```

### 2.2 Decorator Mechanics

#### How Decorators Work (Step by Step)

```python
def my_decorator(func):
    print(f"Decorating {func.__name__}")  # Runs at decoration time
    
    def wrapper():
        print("Before")
        func()
        print("After")
    
    return wrapper

print("1. Before decoration")

@my_decorator
def my_function():
    print("Inside")

print("2. After decoration")

my_function()
print("3. After calling")

# Output:
# 1. Before decoration
# Decorating my_function       <- Runs when decorator applied
# 2. After decoration
# Before                        <- Runs when function called
# Inside
# After
# 3. After calling
```

### 2.3 Decorators with Arguments

#### Decorator with Function Arguments

```python
def repeat(times):
    """Decorator: repeat function call"""
    def decorator(func):
        def wrapper(*args, **kwargs):
            for _ in range(times):
                result = func(*args, **kwargs)
            return result
        return wrapper
    return decorator

@repeat(times=3)
def greet(name):
    print(f"Hello, {name}!")

greet("Alice")
# Output:
# Hello, Alice!
# Hello, Alice!
# Hello, Alice!

# Equivalent to:
# greet = repeat(times=3)(greet)
```

#### Breakdown

```
@repeat(times=3)           <- Call repeat with times=3
                            return decorator function

def greet(name):
    ...

greet = decorator(greet)   <- Apply decorator to greet
```

### 2.4 Preserving Function Metadata

#### Problem: Decorators Hide Original Function Info

```python
def my_decorator(func):
    def wrapper():
        """Wrapper documentation"""
        func()
    return wrapper

@my_decorator
def original():
    """Original documentation"""
    pass

print(original.__name__)  # "wrapper" (wrong!)
print(original.__doc__)   # "Wrapper documentation" (wrong!)
```

#### Solution: Use functools.wraps

```python
import functools

def my_decorator(func):
    @functools.wraps(func)  # Copies metadata from func to wrapper
    def wrapper(*args, **kwargs):
        """Wrapper"""
        return func(*args, **kwargs)
    return wrapper

@my_decorator
def original():
    """Original documentation"""
    pass

print(original.__name__)  # "original" (correct!)
print(original.__doc__)   # "Original documentation" (correct!)
```

### 2.5 Common Decorators

#### Decorator 1: Timing Function Execution

```python
import functools
import time

def timer(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        elapsed = time.time() - start
        print(f"{func.__name__} took {elapsed:.4f}s")
        return result
    return wrapper

@timer
def slow_function():
    time.sleep(1)
    return "Done"

slow_function()
# Output:
# slow_function took 1.0042s
# 'Done'
```

#### Decorator 2: Caching/Memoization

```python
import functools

def memoize(func):
    cache = {}
    
    @functools.wraps(func)
    def wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]
    
    return wrapper

@memoize
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

print(fibonacci(30))  # Instant! (cached results)
```

#### Decorator 3: Retry Logic

```python
import functools
import time

def retry(max_attempts=3, delay=1):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except Exception as e:
                    if attempt == max_attempts:
                        raise
                    print(f"Attempt {attempt} failed: {e}. Retrying in {delay}s...")
                    time.sleep(delay)
        return wrapper
    return decorator

@retry(max_attempts=3, delay=2)
def unreliable_api_call():
    import random
    if random.random() < 0.7:
        raise ConnectionError("API unavailable")
    return "Success"

result = unreliable_api_call()
```

#### Decorator 4: Type Checking

```python
import functools
from typing import get_type_hints

def type_check(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        hints = get_type_hints(func)
        
        # Check arguments
        arg_names = func.__code__.co_varnames[:func.__code__.co_argcount]
        for name, arg in zip(arg_names, args):
            if name in hints:
                expected = hints[name]
                if not isinstance(arg, expected):
                    raise TypeError(f"{name} must be {expected}, got {type(arg)}")
        
        # Check keyword arguments
        for name, value in kwargs.items():
            if name in hints:
                expected = hints[name]
                if not isinstance(value, expected):
                    raise TypeError(f"{name} must be {expected}, got {type(value)}")
        
        result = func(*args, **kwargs)
        
        # Check return type
        if 'return' in hints:
            expected = hints['return']
            if not isinstance(result, expected):
                raise TypeError(f"Return must be {expected}, got {type(result)}")
        
        return result
    
    return wrapper

@type_check
def add(a: int, b: int) -> int:
    return a + b

print(add(2, 3))  # 5 (OK)
print(add("2", 3))  # TypeError: a must be <class 'int'>, got <class 'str'>
```

#### Decorator 5: Authentication/Authorization

```python
import functools

def require_auth(func):
    @functools.wraps(func)
    def wrapper(*args, user=None, **kwargs):
        if user is None:
            raise PermissionError("Authentication required")
        return func(*args, user=user, **kwargs)
    return wrapper

@require_auth
def delete_user(user_id: int, user=None):
    print(f"User {user} deleting user {user_id}")

delete_user(123)  # PermissionError
delete_user(123, user="admin")  # OK
```

### 2.6 Class Decorators

#### Decorating Classes

```python
def add_methods(cls):
    """Decorator that adds methods to a class"""
    def __repr__(self):
        return f"{cls.__name__} instance"
    
    cls.__repr__ = __repr__
    return cls

@add_methods
class MyClass:
    pass

obj = MyClass()
print(repr(obj))  # MyClass instance
```

#### Using Classes as Decorators

```python
class CountCalls:
    def __init__(self, func):
        self.func = func
        self.count = 0
    
    def __call__(self, *args, **kwargs):
        self.count += 1
        print(f"Call #{self.count}")
        return self.func(*args, **kwargs)

@CountCalls
def greet(name):
    print(f"Hello, {name}!")

greet("Alice")  # Call #1
greet("Bob")    # Call #2
greet("Charlie")  # Call #3

print(f"Total calls: {greet.count}")  # 3
```

### 2.7 Stacking Decorators

#### Multiple Decorators (Applied Bottom-Up)

```python
def decorator_a(func):
    def wrapper():
        print("A start")
        func()
        print("A end")
    return wrapper

def decorator_b(func):
    def wrapper():
        print("B start")
        func()
        print("B end")
    return wrapper

@decorator_a
@decorator_b
def my_function():
    print("Function")

my_function()

# Output:
# A start
# B start
# Function
# B end
# A end

# Equivalent to: decorator_a(decorator_b(my_function))
```

---

## SECTION 3: CONTEXT MANAGERS

Context managers provide a way to set up and tear down resources safely.

### 3.1 What is a Context Manager?

A context manager is an object that defines what happens when entering and exiting a `with` block.

#### Simple Example

```python
with open('file.txt') as f:
    data = f.read()  # File automatically closed after block

# File is closed (even if exception occurs)
```

### 3.2 Creating Context Managers

#### Method 1: Using @contextmanager Decorator

```python
import contextlib

@contextlib.contextmanager
def open_database():
    """Context manager for database connection"""
    print("Opening database...")
    db = {"status": "connected"}
    
    try:
        yield db  # Give control to with block
    finally:
        print("Closing database...")
        db["status"] = "disconnected"

with open_database() as db:
    print(f"Database: {db}")
    # do work
# Output:
# Opening database...
# Database: {'status': 'connected'}
# Closing database...
```

#### Method 2: Using __enter__ and __exit__

```python
class DatabaseConnection:
    def __enter__(self):
        print("Opening database...")
        self.db = {"status": "connected"}
        return self.db
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing database...")
        self.db["status"] = "disconnected"
        
        if exc_type:
            print(f"Exception occurred: {exc_val}")
            return False  # Propagate exception
        
        return True  # Suppress exception

with DatabaseConnection() as db:
    print(f"Database: {db}")
```

---

## SECTION 4: ITERTOOLS AND FUNCTIONAL PROGRAMMING

### 4.1 Common itertools Functions

```python
import itertools

# chain: combine iterables
combined = itertools.chain([1,2], [3,4], [5,6])
list(combined)  # [1, 2, 3, 4, 5, 6]

# combinations: all combinations of specified length
combos = itertools.combinations(['A', 'B', 'C'], 2)
list(combos)  # [('A', 'B'), ('A', 'C'), ('B', 'C')]

# permutations: all orderings
perms = itertools.permutations(['A', 'B', 'C'], 2)
list(perms)  # [('A', 'B'), ('A', 'C'), ('B', 'A'), ...]

# product: cartesian product
prod = itertools.product([1,2], ['a','b'])
list(prod)  # [(1, 'a'), (1, 'b'), (2, 'a'), (2, 'b')]

# groupby: group consecutive equal elements
data = [1, 1, 1, 2, 2, 3, 3, 3, 3]
for key, group in itertools.groupby(data):
    print(key, list(group))
# Output:
# 1 [1, 1, 1]
# 2 [2, 2]
# 3 [3, 3, 3, 3]

# islice: slice iterator
sliced = itertools.islice(range(100), 5, 10)
list(sliced)  # [5, 6, 7, 8, 9]

# takewhile/dropwhile: take/drop while condition true
taken = itertools.takewhile(lambda x: x < 5, range(10))
list(taken)  # [0, 1, 2, 3, 4]

dropped = itertools.dropwhile(lambda x: x < 5, range(10))
list(dropped)  # [5, 6, 7, 8, 9]

# zip_longest: zip with padding
pairs = itertools.zip_longest([1,2], ['a','b','c'], fillvalue=None)
list(pairs)  # [(1, 'a'), (2, 'b'), (None, 'c')]
```

### 4.2 Functional Programming Patterns

```python
# map: apply function to each element
numbers = [1, 2, 3, 4, 5]
squared = list(map(lambda x: x**2, numbers))  # [1, 4, 9, 16, 25]

# filter: keep elements matching condition
evens = list(filter(lambda x: x % 2 == 0, numbers))  # [2, 4]

# reduce: accumulate values
from functools import reduce
product = reduce(lambda x, y: x * y, numbers)  # 1*2*3*4*5 = 120

# sorted with key
people = [
    {"name": "Alice", "age": 30},
    {"name": "Bob", "age": 25},
    {"name": "Charlie", "age": 35}
]
by_age = sorted(people, key=lambda p: p['age'])
# [Bob (25), Alice (30), Charlie (35)]
```

---

## SECTION 5: CLOSURES

Closures are functions that capture variables from their enclosing scope.

### 5.1 Understanding Closures

```python
def outer(x):
    def inner(y):
        return x + y  # inner "closes over" x
    return inner

add_5 = outer(5)
print(add_5(3))  # 8 (5 + 3)

add_10 = outer(10)
print(add_10(3))  # 13 (10 + 3)

# Each closure has its own x value!
```

#### Closure with Mutable State

```python
def counter():
    count = 0  # State captured by closure
    
    def increment():
        nonlocal count  # Modify outer scope variable
        count += 1
        return count
    
    return increment

c = counter()
print(c())  # 1
print(c())  # 2
print(c())  # 3
```

---

## SECTION 6: LAMBDA FUNCTIONS

Lambda functions are small anonymous functions.

```python
# Regular function
def add(a, b):
    return a + b

# Lambda equivalent
add_lambda = lambda a, b: a + b

# Use cases:
# 1. As argument to map/filter
numbers = [1, 2, 3, 4, 5]
squared = list(map(lambda x: x**2, numbers))

# 2. Sorting
people = [{"name": "Alice", "age": 30}, {"name": "Bob", "age": 25}]
sorted_by_age = sorted(people, key=lambda p: p['age'])

# 3. Event handlers (GUI)
button.click(lambda: print("Clicked!"))
```

---

## SECTION 7: ADVANCED PATTERNS

### 7.1 Decorator with Class State

```python
class RateLimiter:
    def __init__(self, calls=10, period=60):
        self.calls = calls
        self.period = period
        self.calls_made = 0
        self.start_time = time.time()
    
    def __call__(self, func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            elapsed = time.time() - self.start_time
            
            if elapsed > self.period:
                self.calls_made = 0
                self.start_time = time.time()
            
            if self.calls_made >= self.calls:
                raise Exception("Rate limit exceeded")
            
            self.calls_made += 1
            return func(*args, **kwargs)
        
        return wrapper

@RateLimiter(calls=3, period=10)
def api_call():
    print("API called")

# Can call 3 times in 10 seconds
for i in range(4):
    try:
        api_call()
    except Exception as e:
        print(f"Error: {e}")
```

### 7.2 Generator Pipelines (Advanced Data Processing)

```python
def read_lines(file_path):
    with open(file_path) as f:
        for line in f:
            yield line.strip()

def filter_empty(lines):
    for line in lines:
        if line.strip():
            yield line

def filter_comments(lines):
    for line in lines:
        if not line.startswith('#'):
            yield line

def parse_csv(lines):
    for line in lines:
        yield line.split(',')

def select_field(lines, field_index):
    for fields in lines:
        if len(fields) > field_index:
            yield fields[field_index]

# Pipeline
file_path = 'data.csv'
pipeline = select_field(
    parse_csv(
        filter_comments(
            filter_empty(
                read_lines(file_path)
            )
        )
    ),
    field_index=2
)

for value in pipeline:
    print(value)  # Processes one line at a time!
```

### 7.3 Asynchronous Generators (Async/Await)

```python
import asyncio

async def async_generator():
    for i in range(5):
        await asyncio.sleep(1)  # Simulate async work
        yield i

async def main():
    async for value in async_generator():
        print(f"Got: {value}")

asyncio.run(main())
```

---

## SECTION 8: TRADING SYSTEM APPLICATIONS

### 8.1 Market Data Generator with Backpressure

```python
def quote_stream(exchange, backpressure_fn):
    """Generator that yields quotes with backpressure handling"""
    while True:
        quote = exchange.get_next_quote()
        
        # Check if consumer can handle more data
        if backpressure_fn():
            yield quote
        else:
            # Consumer is slow; wait
            import time
            time.sleep(0.001)  # Back off

# Use it
exchange = MarketDataExchange()

for quote in quote_stream(exchange, lambda: queue.size() < 1000):
    process_quote(quote)
```

### 8.2 Order Decorator for Validation

```python
def validate_order(func):
    @functools.wraps(func)
    def wrapper(order: Order) -> bool:
        # Check quantity
        if order.quantity <= 0:
            raise ValueError("Quantity must be > 0")
        
        # Check price
        if order.price <= 0:
            raise ValueError("Price must be > 0")
        
        # Check risk limits
        notional = order.quantity * order.price
        if notional > MAX_NOTIONAL:
            raise ValueError("Exceeds notional limit")
        
        # Check daily limits
        if current_pnl() + notional > DAILY_LOSS_LIMIT:
            raise ValueError("Exceeds daily loss limit")
        
        return func(order)
    
    return wrapper

@validate_order
def submit_order(order: Order) -> bool:
    """Submit validated order to exchange"""
    return exchange.submit(order)
```

### 8.3 Position Tracker Context Manager

```python
import contextlib

@contextlib.contextmanager
def position_lock(symbol):
    """Context manager for atomic position updates"""
    position_lock = lock_manager.acquire(symbol)
    
    try:
        yield position_lock  # Give control to with block
    finally:
        # Always update risk, release lock
        update_risk_limits(symbol)
        lock_manager.release(symbol)

# Use it
with position_lock('AAPL') as lock:
    current_pos = get_position('AAPL')
    new_qty = current_pos + 100
    update_position('AAPL', new_qty)
# Position updates atomically; lock released
```

---

## FINAL COMPREHENSIVE CHECKLIST

### Generators

- ☐ Use yield (not return) for lazy evaluation
- ☐ Implement iterator protocol (__iter__, __next__)
- ☐ Use generator expressions for memory efficiency
- ☐ Use itertools for common patterns
- ☐ Combine with for loops for simplicity

### Decorators

- ☐ Use @functools.wraps to preserve metadata
- ☐ Understand decorator stacking (bottom-up)
- ☐ Use for cross-cutting concerns (timing, logging, caching)
- ☐ Parameterized decorators for configuration
- ☐ Both function and class decorators

### Context Managers

- ☐ Use with for resource cleanup
- ☐ Use @contextlib.contextmanager for simple cases
- ☐ Implement __enter__ and __exit__ for complex cases
- ☐ Ensure cleanup even on exceptions

### Closures

- ☐ Capture outer scope variables
- ☐ Use nonlocal to modify outer variables
- ☐ Useful for factory functions, partial application

### Lambda

- ☐ Use for short anonymous functions
- ☐ Avoid complex logic (use def instead)
- ☐ Common with map/filter/sorted

### Functional Patterns

- ☐ map: transform elements
- ☐ filter: select elements
- ☐ reduce: accumulate values
- ☐ Combine with generators for efficiency

### Trading-Specific

- ☐ Generators for market data streams (backpressure)
- ☐ Decorators for order validation
- ☐ Context managers for position locking
- ☐ Closures for order state factories

---

## Key Insights

1. **Generators** = memory-efficient streaming (key for large datasets)
2. **Decorators** = extend behavior without modifying original code
3. **Context managers** = safe resource management (cleanup guaranteed)
4. **Closures** = capture state; useful for factories
5. **Functional patterns** = expressive, composable data processing

---

**Good luck with your BNPP interview! 🚀**
