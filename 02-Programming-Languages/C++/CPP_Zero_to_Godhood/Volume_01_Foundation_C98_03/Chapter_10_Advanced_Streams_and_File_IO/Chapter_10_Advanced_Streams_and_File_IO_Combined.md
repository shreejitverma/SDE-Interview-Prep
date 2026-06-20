# Chapter 10: Advanced Streams and File I/O

> *Talking to the outside world — files, formatted output, and the iostream hierarchy.*

C++ streams are far more powerful than `printf`/`scanf`. They compose with the type system, support custom types via `operator<<` and `operator>>`, and share a uniform model for console, file, and in-memory string I/O. This chapter covers: C-strings and `std::string`, the iostream class hierarchy, file I/O with `<fstream>`, stream states and error recovery, format manipulators from `<iomanip>`, binary I/O, string streams, stream internals, and custom manipulators.

---

## Table of Contents

- [10.1 C-Strings: The Null-Terminated Array](#101-c-strings-the-null-terminated-array)
- [10.2 `std::string`: The Safe Alternative](#102-stdstring-the-safe-alternative)
- [10.3 Basic Console I/O: `cin`, `cout`, `cerr`](#103-basic-console-io-cin-cout-cerr)
- [10.4 Stream States and Error Recovery](#104-stream-states-and-error-recovery)
- [10.5 File I/O with `<fstream>`](#105-file-io-with-fstream)
- [10.6 Open Modes](#106-open-modes)
- [10.7 Binary File I/O](#107-binary-file-io)
- [10.8 Stream Positioning: `seekg`, `seekp`, `tellg`, `tellp`](#108-stream-positioning-seekg-seekp-tellg-tellp)
- [10.9 Format Manipulators (`<iomanip>`)](#109-format-manipulators-iomanip)
- [10.10 String Streams (`<sstream>`)](#1010-string-streams-sstream)
- [10.11 Stream Iterators](#1011-stream-iterators)
- [10.12 Professional Insights: Stream Architecture](#1012-professional-insights-stream-architecture)

---

## 10.1 C-Strings: The Null-Terminated Array

Before `std::string` existed, C++ (inherited from C) used **null-terminated character arrays** for text:

```cpp
// Listing 10.1: C-string basics
#include <cstring>
#include <iostream>

int main() {
    char name[6] = {'H', 'e', 'l', 'l', 'o', '\0'}; // '\0' is the sentinel
    const char* greeting = "World"; // String literal — compiler adds '\0'

    std::cout << name << " " << greeting << "\n";
    std::cout << "Length: " << strlen(name) << "\n"; // 5, stops at '\0'
    return 0;
}
```

**The null terminator (`'\0'`)** is the only way the C runtime knows where the string ends. Without it, functions like `strlen`, `printf`, and `strcpy` will read past the end of the array into adjacent memory — undefined behaviour and a security vulnerability.

**Common C-string functions:**

```cpp
// Listing 10.2: C-string manipulation
#include <cstring>
#include <cstdio>

int main() {
    char src[] = "Hello";
    char dst[20];

    strcpy(dst, src);                  // Copy (unsafe: no bounds check)
    strncpy(dst, src, sizeof(dst)-1);  // Safer: limited copy
    dst[sizeof(dst)-1] = '\0';

    strcat(dst, " World");             // Append (unsafe)
    printf("Length: %zu\n", strlen(dst)); // 11
    printf("Compare: %d\n", strcmp("abc", "abd")); // negative (abc < abd)
    return 0;
}
```

**Warning:** `strcpy`, `strcat`, and `gets` are unsafe — they do not check buffer bounds. Buffer overflows are a leading cause of security exploits. Prefer `std::string` in C++ code and `strncpy`/`snprintf` when interoperating with C APIs.

---

## 10.2 `std::string`: The Safe Alternative

`std::string` manages its own buffer, resizes automatically, and provides safe, member-function-based operations:

```cpp
// Listing 10.3: std::string replacing C-strings
#include <string>
#include <iostream>
using namespace std;

int main() {
    string first = "John";
    string last  = "Wick";

    string full = first + " " + last;   // Concatenation
    cout << "Name: "   << full << "\n";
    cout << "Length: " << full.length() << "\n"; // Knows its own size
    cout << "Upper: "  << full[0] << "\n";       // 'J'

    // Interop with C APIs
    const char* cstr = full.c_str(); // Null-terminated const pointer
    return 0;
}
```

For full `std::string` operations (find, replace, substr, tokenization, conversions), see Chapter 7, Section 7.14–7.17.

---

## 10.3 Basic Console I/O: `cin`, `cout`, `cerr`

The standard stream objects:

| Object | Direction | Description |
| :----- | :-------- | :---------- |
| `std::cout` | Output | Standard output (screen), buffered |
| `std::cin` | Input | Standard input (keyboard) |
| `std::cerr` | Output | Standard error, **unbuffered** |
| `std::clog` | Output | Standard error, **buffered** |

```cpp
// Listing 10.4: Console I/O basics
#include <iostream>
#include <string>
using namespace std;

int main() {
    string name;
    int age;

    cout << "Enter your name and age: ";
    cin >> name >> age;  // Extracts whitespace-delimited tokens

    cout << "Welcome, " << name << ". You are " << age << ".\n";

    // cerr writes immediately — use for errors and diagnostics
    cerr << "Diagnostic: processed input\n";
    return 0;
}
```

**Reading a full line** (including spaces) requires `std::getline`:

```cpp
// Listing 10.5: Reading lines with getline
#include <iostream>
#include <string>
using namespace std;

int main() {
    string line;
    cout << "Enter a sentence: ";
    getline(cin, line); // Reads until '\n', discards the newline
    cout << "You typed: " << line << "\n";
    return 0;
}
```

---

## 10.4 Stream States and Error Recovery

Every stream maintains four state bits:

| Flag | Access | Meaning |
| :--- | :----- | :------ |
| `goodbit` | `good()` | No errors |
| `eofbit` | `eof()` | End of file reached |
| `failbit` | `fail()` | Non-fatal error (type mismatch, format error) |
| `badbit` | `bad()` | Fatal error (hardware failure, lost buffer) |

When the stream enters a failed state, all subsequent I/O operations silently do nothing until the state is cleared:

```cpp
// Listing 10.6: Stream state error recovery
#include <iostream>
#include <string>
using namespace std;

int main() {
    int age;
    cout << "Enter age: ";

    if (!(cin >> age)) {
        cout << "Invalid input — not an integer.\n";

        cin.clear();                    // Reset failbit/badbit to goodbit
        cin.ignore(10000, '\n');        // Discard the bad input in the buffer
        // Now cin is usable again
    }

    // Check state explicitly
    cout << boolalpha;
    cout << "good: " << cin.good() << "\n";
    cout << "eof:  " << cin.eof()  << "\n";
    cout << "fail: " << cin.fail() << "\n";
    cout << "bad:  " << cin.bad()  << "\n";
    return 0;
}
```

---

## 10.5 File I/O with `<fstream>`

Three classes handle file streams:

| Class | Direction | Base |
| :---- | :-------- | :--- |
| `std::ifstream` | Input (read) | `std::istream` |
| `std::ofstream` | Output (write) | `std::ostream` |
| `std::fstream` | Both | `std::iostream` |

```cpp
// Listing 10.7: Writing and reading text files
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

int main() {
    // --- Writing ---
    ofstream out("save_data.txt");
    if (out.is_open()) {
        out << "PlayerLevel: 99\n";
        out << "Gold: 5000\n";
        out.close();
    } else {
        cerr << "Cannot open file for writing.\n";
    }

    // --- Reading line by line ---
    ifstream in("save_data.txt");
    if (!in.is_open()) {
        cerr << "Cannot open file for reading.\n";
        return 1;
    }

    string line;
    while (getline(in, line)) {
        cout << "Read: " << line << "\n";
    }
    in.close();
    return 0;
}
```

`is_open()` returns true if the file was successfully opened. A stream object also converts to `bool` — you can write `if (in)` as shorthand for `if (!in.fail())`.

---

## 10.6 Open Modes

Open modes are bitmask flags combined with `|`:

| Flag | Meaning |
| :--- | :------ |
| `std::ios::in` | Open for reading |
| `std::ios::out` | Open for writing (creates or truncates) |
| `std::ios::app` | All writes go to end-of-file (append) |
| `std::ios::ate` | Open, seek to end; may write anywhere |
| `std::ios::binary` | Binary mode — disables CRLF translation |
| `std::ios::trunc` | Truncate existing file to zero length |

```cpp
// Listing 10.8: Combining open modes
#include <fstream>
using namespace std;

int main() {
    // Append to existing file (or create if absent)
    ofstream log("app.log", ios::out | ios::app);
    log << "Session started\n";
    log.close();

    // Open for both reading and writing in binary mode
    fstream db("data.bin", ios::in | ios::out | ios::binary);
    if (db) {
        // ... read/write binary data
        db.close();
    }
    return 0;
}
```

---

## 10.7 Binary File I/O

Binary mode writes raw bytes — no line-ending conversion, no formatting:

```cpp
// Listing 10.9: Binary write and read
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    // --- Write binary ---
    {
        ofstream out("data.bin", ios::binary);
        int numbers[] = {10, 20, 30, 40, 50};
        out.write(reinterpret_cast<const char*>(numbers), sizeof(numbers));
    }

    // --- Read binary ---
    {
        ifstream in("data.bin", ios::binary);
        int buffer[5];
        in.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
        for (int i = 0; i < 5; ++i)
            cout << buffer[i] << " ";
        cout << "\n";
    }
    return 0;
}
```

`write(ptr, n)` writes exactly `n` bytes starting from `ptr`. `read(ptr, n)` reads exactly `n` bytes into `ptr`. Check `gcount()` after `read()` to see how many bytes were actually read (may be less than `n` at end of file).

```cpp
// Listing 10.10: Writing a struct to a binary file
#include <fstream>
#include <iostream>
using namespace std;

struct PlayerRecord {
    int   level;
    float health;
    char  name[32];
};

int main() {
    PlayerRecord rec = {99, 100.0f, "Arthur"};
    {
        ofstream out("player.bin", ios::binary);
        out.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    }
    {
        PlayerRecord loaded;
        ifstream in("player.bin", ios::binary);
        in.read(reinterpret_cast<char*>(&loaded), sizeof(loaded));
        cout << "Loaded: " << loaded.name
             << " level=" << loaded.level << "\n";
    }
    return 0;
}
```

**Caution:** binary struct serialisation is not portable across compilers, architectures, or when struct layout changes (padding, alignment). For portable serialisation, write each field individually.

---

## 10.8 Stream Positioning: `seekg`, `seekp`, `tellg`, `tellp`

Streams support **random access** for files (not for console or network streams):

| Function | Direction | Purpose |
| :------- | :-------- | :------ |
| `tellg()` | Input | Return current read position |
| `seekg(pos)` | Input | Set read position |
| `seekg(offset, dir)` | Input | Set read position relative to `dir` |
| `tellp()` | Output | Return current write position |
| `seekp(pos)` | Output | Set write position |

`dir` is one of `ios::beg` (beginning), `ios::cur` (current), `ios::end` (end).

```cpp
// Listing 10.11: File positioning
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    // Write a known string
    {
        ofstream out("test.txt");
        out << "0123456789";
    }

    ifstream in("test.txt");

    cout << "Position: " << in.tellg() << "\n"; // 0

    in.seekg(5);         // Absolute seek to byte 5
    char c;
    in.get(c);
    cout << "At pos 5: " << c << "\n"; // '5'

    in.seekg(-3, ios::end); // 3 bytes before end
    in.get(c);
    cout << "3 from end: " << c << "\n"; // '7'

    in.seekg(2, ios::cur); // 2 bytes forward from current
    in.get(c);
    cout << "2 forward: " << c << "\n"; // '0' (past end, so implementation-defined)

    in.close();
    return 0;
}
```

---

## 10.9 Format Manipulators (`<iomanip>`)

Manipulators modify the formatting state of a stream. **Sticky manipulators** persist until changed; **non-sticky manipulators** apply only to the next output.

### 10.9.1 Integer Base Formatting

```cpp
// Listing 10.12: Integer base and prefix
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    int n = 255;
    cout << dec << n << "\n";              // 255
    cout << hex << n << "\n";              // ff
    cout << uppercase << hex << n << "\n"; // FF
    cout << showbase << hex << n << "\n";  // 0XFF
    cout << oct << n << "\n";              // 0377
    cout << dec;                           // Reset to decimal (sticky)
    return 0;
}
```

### 10.9.2 Floating-Point Precision

```cpp
// Listing 10.13: Floating-point notation and precision
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double pi = 3.14159265358979;

    cout << pi                           << "\n"; // 3.14159  (default)
    cout << fixed      << setprecision(2) << pi << "\n"; // 3.14
    cout << scientific << setprecision(4) << pi << "\n"; // 3.1416e+00
    cout << defaultfloat;                          // Reset
    return 0;
}
```

### 10.9.3 Width, Fill, and Alignment

```cpp
// Listing 10.14: Width, fill, and alignment
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    // setw resets after each use (non-sticky)
    cout << "no setw: "   << 51 << "\n";
    cout << "setw(7): "   << setw(7) << 51 << "\n";         // "     51"
    cout << "setfill(*): " << setw(7) << setfill('*') << 51 << "\n"; // "*****51"

    cout << left  << setw(10) << setfill(' ') << "left"   << "|\n"; // "left      |"
    cout << right << setw(10) << "right"  << "|\n"; // "     right|"

    return 0;
}
```

### 10.9.4 Boolean Formatting

```cpp
// Listing 10.15: boolalpha
#include <iostream>
using namespace std;

int main() {
    cout << boolalpha << true << " " << false << "\n"; // true false
    cout << noboolalpha << true << " " << false << "\n"; // 1 0
    return 0;
}
```

---

## 10.10 String Streams (`<sstream>`)

`std::stringstream`, `std::istringstream`, and `std::ostringstream` treat a `std::string` as a stream, enabling in-memory serialisation/deserialisation:

```cpp
// Listing 10.16: ostringstream for in-memory serialisation
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

class Point {
public:
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

ostream& operator<<(ostream& os, const Point& p) {
    return os << "(" << p.x << "," << p.y << ")";
}

int main() {
    ostringstream ss;
    ss << "Point: " << Point(3, 4) << " at index " << 0;
    string result = ss.str(); // "Point: (3,4) at index 0"
    cout << result << "\n";
    return 0;
}
```

```cpp
// Listing 10.17: istringstream for parsing
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

int main() {
    string data = "Alice 30 3.14";
    istringstream ss(data);

    string name;
    int age;
    double value;
    ss >> name >> age >> value;

    cout << name << " is " << age << " years old, val=" << value << "\n";
    return 0;
}
```

**Type conversion via `stringstream` (C++98 idiom):**

```cpp
// Listing 10.18: Number <-> string conversion in C++98
#include <sstream>
#include <string>
using namespace std;

template<typename T>
string to_string_98(const T& val) {
    ostringstream ss;
    ss << val;
    return ss.str();
}

template<typename T>
T from_string_98(const string& s) {
    T result;
    istringstream ss(s);
    ss >> result;
    return result;
}

int main() {
    string s  = to_string_98(42);        // "42"
    int    n  = from_string_98<int>("99"); // 99
    return 0;
}
```

---

## 10.11 Stream Iterators

`std::ostream_iterator` creates an output iterator that calls `operator<<` to write elements to a stream. Use it to connect STL algorithms directly to output:

```cpp
// Listing 10.19: ostream_iterator with copy and transform
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
using namespace std;

int square(int x) { return x * x; }

int main() {
    int arr[] = {1, 2, 3, 4, 8, 16};
    vector<int> v(arr, arr + 6);

    // Print each element separated by " "
    copy(v.begin(), v.end(),
         ostream_iterator<int>(cout, " ")); // 1 2 3 4 8 16
    cout << "\n";

    // Print squares
    transform(v.begin(), v.end(),
              ostream_iterator<int>(cout, " "),
              square); // 1 4 9 16 64 256
    cout << "\n";
    return 0;
}
```

`std::istream_iterator` reads whitespace-delimited tokens from a stream:

```cpp
// Listing 10.20: istream_iterator to read all ints from cin
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
using namespace std;

int main() {
    vector<int> nums(
        istream_iterator<int>(cin),
        istream_iterator<int>()); // Default-constructed = end

    sort(nums.begin(), nums.end());
    copy(nums.begin(), nums.end(),
         ostream_iterator<int>(cout, "\n"));
    return 0;
}
```

---

## 10.12 Professional Insights: Stream Architecture

### 10.12.1 The `ios_base` Class Hierarchy

```
ios_base
└── basic_ios<charT>
    ├── basic_istream<charT>   → std::istream, std::ifstream, std::istringstream
    ├── basic_ostream<charT>   → std::ostream, std::ofstream, std::ostringstream
    └── basic_iostream<charT>  → std::iostream, std::fstream, std::stringstream
```

All streams share the same format flags, error state, and locale stored in `ios_base`. Manipulators (except `setw`) modify these flags persistently.

### 10.12.2 Stream Buffering (`streambuf`)

The actual byte transfer is handled by a **`std::streambuf`** object (`rdbuf()`). You can redirect `cout` to a file by swapping buffers:

```cpp
// Listing 10.21: Redirect cout to a file using rdbuf
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    ofstream log("log.txt");

    streambuf* old_cout = cout.rdbuf(); // Save cout's buffer
    cout.rdbuf(log.rdbuf());            // Point cout at log.txt

    cout << "This goes to log.txt\n";

    cout.rdbuf(old_cout);               // Restore cout
    cout << "This is back to screen\n";
    return 0;
}
```

### 10.12.3 Custom Manipulators

A manipulator is a function that takes and returns a `std::ostream&` (or `istream&`):

```cpp
// Listing 10.22: Custom stream manipulator
#include <iostream>
using namespace std;

ostream& tab(ostream& os) {
    return os << "\t";
}

ostream& separator(ostream& os) {
    return os << "---\n";
}

int main() {
    cout << "Column1" << tab << "Column2" << "\n";
    cout << separator;
    cout << "Data1"   << tab << "Data2"   << "\n";
    return 0;
}
```

### 10.12.4 Performance: `sync_with_stdio` and `tie`

By default, C++ streams are synchronised with C's `<stdio.h>` — `cout` and `printf` write in the correct order even when mixed. This synchronisation has overhead:

```cpp
// Listing 10.23: Fast I/O setup
#include <iostream>
using namespace std;

int main() {
    ios::sync_with_stdio(false); // Decouple C++ and C streams — 2-10x faster
    cin.tie(NULL);               // Decouple cin from cout (no auto-flush before read)

    // After these calls, do NOT mix cout with printf — undefined order
    int n;
    cin >> n;
    cout << n << "\n";
    return 0;
}
```

**After calling `sync_with_stdio(false)`**, do not mix `printf`/`scanf` with `cout`/`cin` in the same program — the output ordering becomes undefined.
