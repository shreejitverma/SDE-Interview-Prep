# CHAPTER 9: ERROR HANDLING AND ROBUSTNESS


# ERROR HANDLING & ROBUSTNESS

<!-- Merged content from Chapter_16_ERROR_HANDLING__DEBUGGING.md -->

# ERROR HANDLING & DEBUGGING (C++98)

## 1. ASSERTIONS

Use `assert` to catch programming logic errors during development.

```cpp
#include <iostream>
#include <cassert>

int divide(int a, int b) {
    assert(b != 0);  // Terminates program if b is 0
    return a / b;
}

int main() {
    std::cout << divide(10, 2) << "\n";
    return 0;
}
```

---

## 2. EXCEPTIONS

C++ uses exceptions to handle runtime errors gracefully.

### 2.1 Basic Try-Catch

```cpp
#include <iostream>

int safe_divide(int a, int b) {
    if (b == 0) {
        throw "Division by zero condition!";
    }
    return a / b;
}

int main() {
    try {
        int result = safe_divide(10, 0);
        std::cout << result << "\n";
    } catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
    }
    return 0;
}
```

### 2.2 Catching Multiple Types

```cpp
try {
    // code...
} catch (int e) {
    std::cerr << "Integer exception: " << e << "\n";
} catch (const char* e) {
    std::cerr << "String exception: " << e << "\n";
} catch (...) {
    std::cerr << "Unknown exception caught!\n";
}
```

### 2.3 Standard Exceptions

Inherit from `std::exception` for custom exceptions.

```cpp
#include <iostream>
#include <exception>
#include <stdexcept> // runtime_error, logic_error

class MyError : public std::exception {
public:
    virtual const char* what() const throw() {
        return "My Custom Error";
    }
};

void test() {
    throw std::runtime_error("Standard Runtime Error");
}

int main() {
    try {
        test();
    } catch (const std::exception& e) {
        std::cerr << "Caught: " << e.what() << "\n";
    }
    return 0;
}
```

---
### Professional Notes: Debugging & Quality Assurance

#### 1. Unit Testing in C++
Testing is essential for maintaining large codebases. Modern C++ typically uses third-party frameworks:
*   **Google Test (GTest)**: The industry standard. Uses macros like `EXPECT_EQ`, `ASSERT_TRUE`.
*   **Catch2**: Header-only, very easy to integrate. Uses natural BDD style: `REQUIRE(x == 42)`.

#### 2. Debugging with GDB and LLDB
Command-line debuggers allow you to inspect the program state at runtime.
*   **`break [file]:[line]`**: Set a breakpoint.
*   **`print [var]`**: Inspect variable value.
*   **`backtrace` (bt)**: Show the current call stack.
*   **`watch [var]`**: Pause whenever a variable's value changes.

#### 3. Defensive Programming Techniques
*   **Static Assertions (C++11)**: `static_assert(sizeof(int) == 4, "32-bit int required");`. Checks conditions at compile time.
*   **`noexcept`**: Mark functions that are guaranteed not to throw. Allows the compiler to generate more optimized code and skip stack unwinding logic.
*   **Core Dumps**: On Linux, enable core dumps (`ulimit -c unlimited`) to capture the memory state of a crashed program for post-mortem analysis.

---

### 2.4 Exception Specifications (C++98)

In C++98, you can specify what a function might throw. (Note: Deprecated in C++11, but valid here).

```cpp
// Can throw only int
void func() throw(int) {
    throw 42;
}

// Cannot throw anything
void no_throw() throw() {
    // logic...
}
```

### 2.5 Stack Unwinding & RAII

When an exception is thrown, the stack is unwound, identifying destructors for all local objects.

```cpp
class Resource {
public:
    Resource() { std::cout << "Acquired\n"; }
    ~Resource() { std::cout << "Released\n"; }
};

void risky() {
    Resource r; // Destructor guaranteed to run
    throw 1;
}

int main() {
    try {
        risky();
    } catch (...) {
        std::cout << "Caught\n";
    }
    return 0;
}
// Output: Acquired -> Released -> Caught
```

---

## 3. DEBUGGING STRATEGIES

### 3.1 Debug Macros (C++98 Style)

```cpp
#include <iostream>

#ifdef DEBUG
    #define LOG(x) std::cout << "DEBUG: " << x << "\n"
#else
    #define LOG(x)
#endif

int main() {
    int x = 10;
    LOG("x is " << x);
    return 0;
}
```

# Professional Notes: Chapter 72: Exceptions

Section 72.1: Catching exceptions
Section 72.2: Rethrow (propagate) exception
Section 72.3: Best practice: throw by value, catch by const reference
Section 72.4: Custom exception
Section 72.5: std::uncaught_exceptions
Section 72.6: Function Try Block for regular function
Section 72.7: Nested exception
Section 72.8: Function Try Blocks In constructor
Section 72.9: Function Try Blocks In destructor

# Professional Notes: Chapter 72: Exceptions

Section 72.1: Catching exceptions
A try/catch block is used to catch exceptions. The code in the try section is the code that may throw an exception,
and the code in the catch clause(s) handles the exception.
#include <iostream>
#include <string>
#include <stdexcept>
int main() {
  std::string str("foo");
  try {
      str.at(10); // access element, may throw std::out_of_range
  } catch (const std::out_of_range& e) {
      // what() is inherited from std::exception and contains an explanatory message
      std::cout << e.what();
  }
}
Multiple catch clauses may be used to handle multiple exception types. If multiple catch clauses are present, the
exception handling mechanism tries to match them in order of their appearance in the code:
std::string str("foo");
try {
    str.reserve(2); // reserve extra capacity, may throw std::length_error
    str.at(10); // access element, may throw std::out_of_range
} catch (const std::length_error& e) {
    std::cout << e.what();
} catch (const std::out_of_range& e) {
    std::cout << e.what();
}
Exception classes which are derived from a common base class can be caught with a single catch clause for the
common base class. The above example can replace the two catch clauses for std::length_error and
std::out_of_range with a single clause for std:exception:
std::string str("foo");
try {
    str.reserve(2); // reserve extra capacity, may throw std::length_error
    str.at(10); // access element, may throw std::out_of_range
} catch (const std::exception& e) {
    std::cout << e.what();
}
Because the catch clauses are tried in order, be sure to write more specic catch clauses rst, otherwise your
exception handling code might never get called:
try {
    /* Code throwing exceptions omitted. */
} catch (const std::exception& e) {
    /* Handle all exceptions of type std::exception. */
} catch (const std::runtime_error& e) {
    /* This block of code will never execute, because std::runtime_error inherits
       from std::exception, and all exceptions of type std::exception were already
       caught by the previous catch clause. */
}
Another possibility is the catch-all handler, which will catch any thrown object:
try {
    throw 10;
} catch (...) {
    std::cout << "caught an exception";
}
Section 72.2: Rethrow (propagate) exception
Sometimes you want to do something with the exception you catch (like write to log or print a warning) and let it
bubble up to the upper scope to be handled. To do so, you can rethrow any exception you catch:
try {
    ... // some code here
} catch (const SomeException& e) {
    std::cout << "caught an exception";
    throw;
}
Using throw; without arguments will re-throw the currently caught exception.
Version  C++11
To rethrow a managed std::exception_ptr, the C++ Standard Library has the rethrow_exception function that
can be used by including the <exception> header in your program.
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
void handle_eptr(std::exception_ptr eptr) // passing by value is ok
{
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch(const std::exception& e) {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
    }
}
int main()
{
    std::exception_ptr eptr;
    try {
        std::string().at(1); // this generates an std::out_of_range
    } catch(...) {
        eptr = std::current_exception(); // capture
    }
    handle_eptr(eptr);
} // destructor for std::out_of_range called here, when the eptr is destructed
Section 72.3: Best practice: throw by value, catch by const
reference
In general, it is considered good practice to throw by value (rather than by pointer), but catch by (const) reference.
try {
    // throw new std::runtime_error("Error!");   // Don't do this!
    // This creates an exception object
    // on the heap and would require you to catch the
    // pointer and manage the memory yourself. This can
    // cause memory leaks!
    throw std::runtime_error("Error!");
} catch (const std::runtime_error& e) {
    std::cout << e.what() << std::endl;
}
One reason why catching by reference is a good practice is that it eliminates the need to reconstruct the object
when being passed to the catch block (or when propagating through to other catch blocks). Catching by reference
also allows the exceptions to be handled polymorphically and avoids object slicing. However, if you are rethrowing
an exception (like throw e;, see example below), you can still get object slicing because the throw e; statement
makes a copy of the exception as whatever type is declared:
#include <iostream>
struct BaseException {
    virtual const char* what() const { return "BaseException"; }
};
struct DerivedException : BaseException {
    // "virtual" keyword is optional here
    virtual const char* what() const { return "DerivedException"; }
};
int main(int argc, char** argv) {
    try {
        try {
            throw DerivedException();
        } catch (const BaseException& e) {
            std::cout << "First catch block: " << e.what() << std::endl;
            // Output ==> First catch block: DerivedException
            throw e; // This changes the exception to BaseException
                     // instead of the original DerivedException!
        }
    } catch (const BaseException& e) {
        std::cout << "Second catch block: " << e.what() << std::endl;
        // Output ==> Second catch block: BaseException
    }
    return 0;
}
If you are sure that you are not going to do anything to change the exception (like add information or modify the
message), catching by const reference allows the compiler to make optimizations and can improve performance.
But this can still cause object splicing (as seen in the example above).
Warning: Beware of throwing unintended exceptions in catch blocks, especially related to allocating extra memory
or resources. For example, constructing logic_error, runtime_error or their subclasses might throw bad_alloc
due to memory running out when copying the exception string, I/O streams might throw during logging with
respective exception masks set, etc.
Section 72.4: Custom exception
You shouldn't throw raw values as exceptions, instead use one of the standard exception classes or make your
own.
Having your own exception class inherited from std::exception is a good way to go about it. Here's a custom
exception class which directly inherits from std::exception:
#include <exception>
class Except: virtual public std::exception {
protected:
    int error_number;               ///< Error number
    int error_offset;               ///< Error offset
    std::string error_message;      ///< Error message
public:
    /** Constructor (C++ STL string, int, int).
     *  @param msg The error message
     *  @param err_num Error number
     *  @param err_off Error offset
     */
    explicit
    Except(const std::string& msg, int err_num, int err_off):
        error_number(err_num),
        error_offset(err_off),
        error_message(msg)
        {}
    /** Destructor.
     *  Virtual to allow for subclassing.
     */
    virtual ~Except() throw () {}
    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *  is in possession of the Except object. Callers must
     *  not attempt to free the memory.
     */
    virtual const char* what() const throw () {
       return error_message.c_str();
    }
    /** Returns error number.
     *  @return #error_number
     */
    virtual int getErrorNumber() const throw() {
        return error_number;
    }
    /**Returns error offset.
     * @return #error_offset
     */
    virtual int getErrorOffset() const throw() {
        return error_offset;
    }
};
An example throw catch:
try {
    throw(Except("Couldn't do what you were expecting", -12, -34));
} catch (const Except& e) {
    std::cout<<e.what()
             <<"\nError number: "<<e.getErrorNumber()
             <<"\nError offset: "<<e.getErrorOffset();
}
As you are not only just throwing a dumb error message, also some other values representing what the error
exactly was, your error handling becomes much more ecient and meaningful.
There's an exception class that let's you handle error messages nicely :std::runtime_error
You can inherit from this class too:
#include <stdexcept>
class Except: virtual public std::runtime_error {
protected:
    int error_number;               ///< Error number
    int error_offset;               ///< Error offset
public:
    /** Constructor (C++ STL string, int, int).
     *  @param msg The error message
     *  @param err_num Error number
     *  @param err_off Error offset
     */
    explicit
    Except(const std::string& msg, int err_num, int err_off):
        std::runtime_error(msg)
        {
            error_number = err_num;
            error_offset = err_off;
        }
    /** Destructor.
     *  Virtual to allow for subclassing.
     */
    virtual ~Except() throw () {}
    /** Returns error number.
     *  @return #error_number
     */
    virtual int getErrorNumber() const throw() {
        return error_number;
    }
    /**Returns error offset.
     * @return #error_offset
     */
    virtual int getErrorOffset() const throw() {
        return error_offset;
    }
};
Note that I haven't overridden the what() function from the base class (std::runtime_error) i.e we will be using
the base class's version of what(). You can override it if you have further agenda.
Section 72.5: std::uncaught_exceptions
Version  c++17
C++17 introduces int std::uncaught_exceptions() (to replace the limited bool std::uncaught_exception()) to
know how many exceptions are currently uncaught. That allows for a class to determine if it is destroyed during a
stack unwinding or not.
#include <exception>
#include <string>
#include <iostream>
// Apply change on destruction:
// Rollback in case of exception (failure)
// Else Commit (success)
class Transaction
{
public:
    Transaction(const std::string& s) : message(s) {}
    Transaction(const Transaction&) = delete;
    Transaction& operator =(const Transaction&) = delete;
    void Commit() { std::cout << message << ": Commit\n"; }
    void RollBack() noexcept(true) { std::cout << message << ": Rollback\n"; }
    // ...
    ~Transaction() {
        if (uncaughtExceptionCount == std::uncaught_exceptions()) {
            Commit(); // May throw.
        } else { // current stack unwinding
            RollBack();
        }
    }
private:
    std::string message;
    int uncaughtExceptionCount = std::uncaught_exceptions();
};
class Foo
{
public:
    ~Foo() {
        try {
            Transaction transaction("In ~Foo"); // Commit,
                                            // even if there is an uncaught exception
            //...
        } catch (const std::exception& e) {
            std::cerr << "exception/~Foo:" << e.what() << std::endl;
        }
    }
};
int main()
{
    try {
        Transaction transaction("In main"); // RollBack
        Foo foo; // ~Foo commit its transaction.
        //...
        throw std::runtime_error("Error");
    } catch (const std::exception& e) {
        std::cerr << "exception/main:" << e.what() << std::endl;
    }
}
Output:
In ~Foo: Commit
In main: Rollback
exception/main:Error
Section 72.6: Function Try Block for regular function
void function_with_try_block()
try
{
    // try block body
}
catch (...)
{
    // catch block body
}
Which is equivalent to
void function_with_try_block()
{
    try
    {
        // try block body
    }
    catch (...)
    {
        // catch block body
    }
}
Note that for constructors and destructors, the behavior is dierent as the catch block re-throws an exception
anyway (the caught one if there is no other throw in the catch block body).
The function main is allowed to have a function try block like any other function, but main's function try block will
not catch exceptions that occur during the construction of a non-local static variable or the destruction of any static
variable. Instead, std::terminate is called.
Section 72.7: Nested exception
Version  C++11
During exception handling there is a common use case when you catch a generic exception from a low-level
function (such as a lesystem error or data transfer error) and throw a more specic high-level exception which
indicates that some high-level operation could not be performed (such as being unable to publish a photo on Web).
This allows exception handling to react to specic problems with high level operations and also allows, having only
error an message, the programmer to nd a place in the application where an exception occurred. Downside of this
solution is that exception callstack is truncated and original exception is lost. This forces developers to manually
include text of original exception into a newly created one.
Nested exceptions aim to solve the problem by attaching low-level exception, which describes the cause, to a high
level exception, which describes what it means in this particular case.
std::nested_exception allows to nest exceptions thanks to std::throw_with_nested:
#include <stdexcept>
#include <exception>
#include <string>
#include <fstream>
#include <iostream>
struct MyException
{
    MyException(const std::string& message) : message(message) {}
    std::string message;
};
void print_current_exception(int level)
{
    try {
        throw;
    } catch (const std::exception& e) {
        std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    } catch (const MyException& e) {
        std::cerr << std::string(level, ' ') << "MyException: " << e.message << '\n';
    } catch (...) {
        std::cerr << "Unkown exception\n";
    }
}
void print_current_exception_with_nested(int level =  0)
{
    try {
        throw;
    } catch (...) {
        print_current_exception(level);
    }
    try {
        throw;
    } catch (const std::nested_exception& nested) {
        try {
            nested.rethrow_nested();
        } catch (...) {
            print_current_exception_with_nested(level + 1); // recursion
        }
    } catch (...) {
        //Empty // End recursion
    }
}
// sample function that catches an exception and wraps it in a nested exception
void open_file(const std::string& s)
{
    try {
        std::ifstream file(s);
        file.exceptions(std::ios_base::failbit);
    } catch(...) {
        std::throw_with_nested(MyException{"Couldn't open " + s});
    }
}
// sample function that catches an exception and wraps it in a nested exception
void run()
{
    try {
        open_file("nonexistent.file");
    } catch(...) {
        std::throw_with_nested( std::runtime_error("run() failed") );
    }
}
// runs the sample function above and prints the caught exception
int main()
{
    try {
        run();
    } catch(...) {
        print_current_exception_with_nested();
    }
}
Possible output:
exception: run() failed
 MyException: Couldn't open nonexistent.file
  exception: basic_ios::clear
If you work only with exceptions inherited from std::exception, code can even be simplied.
Section 72.8: Function Try Blocks In constructor
The only way to catch exception in initializer list:
struct A : public B
{
    A() try : B(), foo(1), bar(2)
    {
        // constructor body
    }
    catch (...)
    {
        // exceptions from the initializer list and constructor are caught here
        // if no exception is thrown here
        // then the caught exception is re-thrown.
    }
private:
    Foo foo;
    Bar bar;
};
Section 72.9: Function Try Blocks In destructor
struct A
{
    ~A() noexcept(false) try
    {
        // destructor body
    }
    catch (...)
    {
        // exceptions of destructor body are caught here
        // if no exception is thrown here
        // then the caught exception is re-thrown.
    }
};
Note that, although this is possible, one needs to be very careful with throwing from destructor, as if a destructor
called during stack unwinding throws an exception, std::terminate is called.

---

# Chapter 10: Advanced Streams & File I/O

This chapter explores the full power of the C++ `<iostream>`, `<fstream>`, and `<sstream>` libraries, deconstructing how they interact with the filesystem and formatted data.

## 10.1 File I/O Foundations

Working with files involves the `std::fstream`, `std::ifstream`, and `std::ofstream` classes.

### 1. Opening Modes
*   `std::ios::in`: Open for reading.
*   `std::ios::out`: Open for writing (overwrites existing).
*   `std::ios::app`: Append to end of file.
*   `std::ios::ate`: Open and seek to end.
*   `std::ios::binary`: Binary mode (no CRLF translation).

### 2. Binary vs Text Mode
In text mode, special characters like `\n` might be translated (e.g., to `\r\n` on Windows). Binary mode ensures that exactly what you write is what appears on disk.
```cpp
std::ofstream file("data.bin", std::ios::binary);
double d = 3.14;
file.write(reinterpret_cast<const char*>(&d), sizeof(d));
```

---

## 10.2 Stream Manipulators

Manipulators allow you to change how data is formatted on the fly.

### 1. Numeric Formatting
*   `std::hex`, `std::oct`, `std::dec`: Change base.
*   `std::setprecision(n)`: Set floating point precision (requires `<iomanip>`).
*   `std::fixed`, `std::scientific`: Change notation.

### 2. Padding and Alignment
*   `std::setw(n)`: Set width of next field.
*   `std::setfill(c)`: Set fill character.
*   `std::left`, `std::right`, `std::internal`: Change alignment.

---

## 10.3 String Streams (`sstream`)

`std::stringstream` allows you to treat a string like a stream, enabling easy conversion between types and strings.
```cpp
#include <sstream>
std::stringstream ss;
ss << "The answer is " << 42;
std::string s = ss.str();
```

---
### Professional Notes: Stream Architecture

#### 1. The `ios_base` State Machine
Every stream inherits from `ios_base`, which maintains internal flags for formatting and errors.
*   **Performance Trap**: Streams are significantly slower than C's `printf`/`scanf` due to the overhead of object construction and virtual function calls.
*   **Optimization**: `std::ios::sync_with_stdio(false);` disables the synchronization between C++ and C streams, making `std::cin` as fast as `scanf`.

#### 2. Stream Buffering (`streambuf`)
The actual data transfer is handled by a buffer object (`rdbuf`).
*   **Redirection**: You can redirect `cout` to a file by swapping its buffer:
```cpp
std::ofstream out("log.txt");
std::streambuf *coutbuf = std::cout.rdbuf();
std::cout.rdbuf(out.rdbuf()); // cout now writes to log.txt
```

#### 3. Custom Manipulators
You can create your own manipulators by defining functions that take and return a reference to a stream:
```cpp
std::ostream& tab(std::ostream& os) {
    return os << "\t";
}
std::cout << "Data1" << tab << "Data2" << std::endl;
```

# VOLUME 01: GODHOOD SUMMARY

Volume 01 established the "Archaic" foundations of C++. By mastering C++98/03, you have learned the manual labor of the language:
1. **Memory Management**: The raw power and danger of pointers and `new/delete`.
2. **OOP Mechanics**: How virtualization and the object model work under the hood.
3. **The Classic STL**: The original containers and algorithms that still form the backbone of modern systems.

**The Golden Rule of C++98**: Everything is explicit. There are no shortcuts. To achieve Godhood, you must respect these roots while preparing to transcend them with the features of the Modern Revolution.

# VOLUME 02 MODERN REVOLUTION C11
