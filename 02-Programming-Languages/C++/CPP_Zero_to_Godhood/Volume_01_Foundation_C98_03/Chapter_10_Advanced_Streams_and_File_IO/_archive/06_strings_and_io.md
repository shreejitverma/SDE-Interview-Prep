# Chapter 6: Strings and I/O

> *Talking to the outside world.*

Computers are incredibly fast calculators, but a calculator is useless if it can't show you the result. Software needs to communicate—with the user, with the hard drive, and with the network. In most languages, handling text and printing it to the screen is trivial. In C++, because you have direct access to memory, text is a fascinating (and sometimes dangerous) topic.

---

## 6.1 C-Strings (The Ancient Null-Terminator)

Before C++ was invented, there was only C. And in C, there was no such thing as a "String" type. There were only arrays of characters.

```cpp
char name[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
```

Notice the `'\0'` at the very end? That is the **Null Terminator**. Because C-arrays don't know how long they are, the only way the computer knows it has reached the end of the text is by reading memory byte-by-byte until it hits a `0`. 

If you forget the `\0`, the computer will keep reading memory past the end of the array, printing whatever garbage happens to be stored in the adjacent memory houses until it accidentally hits a `0`.

> [!WARNING]
> **⚠️ The Danger Zone: Buffer Overflows**
> C-Strings are the root cause of countless security vulnerabilities. If a hacker gives you a 100-character name, and you copy it into a 10-character C-String array, the extra 90 characters will overwrite adjacent memory. The hacker can use this to overwrite the function's return address and hijack your program!

You can write C-Strings more simply using string literals, and the compiler will add the `\0` for you:
```cpp
const char* name = "Hello"; // Still just an array of characters in memory!
```

## 6.2 `std::string` — The Modern Way

To save us from the madness of null-terminators and buffer overflows, C++ gave us `<string>`. `std::string` is an intelligent, dynamic object that automatically resizes itself to fit whatever text you give it.

```cpp
#include <iostream>
#include <string>

int main() {
    std::string first = "John";
    std::string last = "Wick";
    
    // Concatenation is as easy as addition
    std::string full = first + " " + last; 
    
    std::cout << "Name: " << full << "\n";
    std::cout << "Length: " << full.length() << "\n"; // Knows its own size!
}
```

Behind the scenes, `std::string` manages a dynamic array on the Heap. If you add more text than it has room for, it secretly rents a bigger locker, copies the data over, and deletes the old one.

## 6.3 String Views (`std::string_view`) `[C++17]`

Because `std::string` manages Heap memory, creating one involves a costly "allocation" (calling the warehouse manager). 

If you just want a function to *read* a string, passing a `std::string` by value is a performance disaster. Even passing it by `const std::string&` has overhead if you pass a raw C-string literal to it (it forces the creation of a temporary `std::string`).

C++17 introduced the ultimate solution: **`std::string_view`**.

```cpp
#include <string_view>

// Fast, non-owning view of a string.
void print_name(std::string_view name) {
    std::cout << name << "\n";
}

int main() {
    std::string s = "Alice";
    print_name(s);       // Fast! No copy.
    print_name("Bob");   // Fast! No temporary std::string created.
}
```

> [!TIP]
> **🔥 Godhood Tip: Read-Only Text**
> A `string_view` is just two things under the hood: a pointer to the start of the text, and an integer representing the length. That's it. It doesn't own the memory. If you are writing a function that only *reads* text, always use `std::string_view`. 

## 6.4 Basic I/O: `cin`, `cout`, `cerr`

C++ communicates with the terminal using **Streams**. Think of a stream as an infinite conveyor belt.

*   **`std::cout`**: The Standard Output stream. A conveyor belt leading to the screen.
*   **`std::cin`**: The Standard Input stream. A conveyor belt coming from the keyboard.
*   **`std::cerr`**: The Standard Error stream. An un-buffered belt leading to the screen, used specifically for printing errors immediately (even if the program is crashing).

```cpp
#include <iostream>
#include <string>

int main() {
    std::string name;
    int age;

    std::cout << "Enter your name and age: ";
    
    // The >> operator pulls items off the cin conveyor belt
    std::cin >> name >> age; 
    
    std::cout << "Welcome, " << name << ". You are " << age << ".\n";
}
```

## 6.5 File I/O (`<fstream>`)

Reading from and writing to files is exactly the same as reading from the keyboard and writing to the screen. You just use a different conveyor belt.

```cpp
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // 1. Writing to a file (Output File Stream)
    std::ofstream out_file("save_data.txt"); 
    if (out_file.is_open()) {
        out_file << "PlayerLevel: 99\n";
        out_file << "Gold: 5000\n";
        out_file.close(); // Important! Close the file when done.
    }

    // 2. Reading from a file (Input File Stream)
    std::ifstream in_file("save_data.txt");
    std::string line;
    
    if (in_file.is_open()) {
        // Read the file line-by-line until the end
        while (std::getline(in_file, line)) {
            std::cout << "Read: " << line << "\n";
        }
        in_file.close();
    } else {
        std::cerr << "Error: Could not find save_data.txt\n";
    }
}
```

## 6.6 Stream States and Error Handling

What happens if you ask the user for their `age` (an `int`), and they type `"Twenty"`? 

The `cin` stream will crash. It enters an error state and refuses to process any more input until you manually clear the error.

```cpp
int age;
std::cout << "Enter age: ";

if (!(std::cin >> age)) {
    // The user typed text instead of a number!
    std::cout << "Invalid input!\n";
    
    // 1. Clear the error flags so the stream works again
    std::cin.clear(); 
    
    // 2. Throw away the garbage the user typed in the buffer
    std::cin.ignore(10000, '\n'); 
}
```

Every stream has internal state flags you can check:
*   `good()`: Everything is fine.
*   `eof()`: End-Of-File reached.
*   `fail()`: Non-fatal error (like the type mismatch above).
*   `bad()`: Fatal error (the hard drive was ripped out of the computer).

> [!NOTE]
> **📋 Professional Notes: Fast I/O**
> Are you doing competitive programming or processing gigabytes of text? C++ streams are synchronized with C-style `stdio` by default, which makes them slow. 
> To make `std::cout` and `std::cin` blazing fast, put this at the very top of `main()`:
> `std::ios::sync_with_stdio(false);`
> `std::cin.tie(NULL);`

---

You can now manage memory, process text, and save data to the hard drive. In the next chapter, we will learn how to group different variables together to create custom data structures.
