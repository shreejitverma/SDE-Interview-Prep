# C++14 STANDARD LIBRARY ENHANCEMENTS

## 1. `std::make_unique`

The missing counterpart to `std::make_shared`.

```cpp
auto ptr = std::make_unique<int>(42);
// Exception safe, no `new` keyword.
```

## 2. Shared Locks (`std::shared_timed_mutex`)

Reader-writer locking. Multiple readers can hold a shared lock; writers need an exclusive lock.

```cpp
#include <shared_mutex>

std::shared_timed_mutex mtx;

void reader() {
    std::shared_lock<std::shared_timed_mutex> lock(mtx);
    // read
}

void writer() {
    std::unique_lock<std::shared_timed_mutex> lock(mtx);
    // write
}
```

## 3. `std::exchange`

Assigns a new value to an object and returns the old value.

```cpp
int x = 10;
int old = std::exchange(x, 20); // x=20, old=10
```

## 4. `std::quoted`

Quoted string I/O for streams.

```cpp
#include <iomanip>
std::cout << std::quoted("Hello World"); // "Hello World"
```

## 5. Tuple Addressing by Type

Access tuple elements by type (if unique).

```cpp
std::tuple<int, double> t(1, 3.14);
int i = std::get<int>(t);
```
