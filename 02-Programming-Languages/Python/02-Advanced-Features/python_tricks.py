# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Advanced Python Features
# Description: Decorators, Generators, and Context Managers are favorites in Python interviews.

import time
from contextlib import contextmanager

# 1. DECORATORS (Modify function behavior)
def timeit(func):
    """Decorator to measure execution time."""
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f"[{func.__name__}] took {end - start:.6f} sec")
        return result
    return wrapper

# 2. GENERATORS (Memory Efficient Iterators)
def fibonacci_generator(n):
    """Yields first n fibonacci numbers lazily."""
    a, b = 0, 1
    for _ in range(n):
        yield a
        a, b = b, a + b

# 3. CONTEXT MANAGERS (Resource Management)
class FileManager:
    """Custom Context Manager for file handling."""
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.file = None

    def __enter__(self):
        self.file = open(self.filename, self.mode)
        print(f"Opened {self.filename}")
        return self.file

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.file.close()
        print(f"Closed {self.filename}")

@timeit
def heavy_computation():
    # Using generator to sum huge range without storing in memory
    return sum(x * x for x in range(10**6))

if __name__ == "__main__":
    print("--- Decorator Test ---")
    res = heavy_computation()
    print(f"Result: {res}")

    print("\n--- Generator Test ---")
    for num in fibonacci_generator(5):
        print(num, end=" ")
    print()

    print("\n--- Context Manager Test ---")
    # with FileManager('test.txt', 'w') as f:
    #     f.write('Hello World')
    print("Skipped file write to keep repo clean.")
