# PREPROCESSOR DIRECTIVES


## 7.1 #define and #include

```cpp
// Define constants
#define PI 3.14159
#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))

// Conditional compilation
#define DEBUG

#ifdef DEBUG
    #define LOG(msg) cout << msg << endl
#else
    #define LOG(msg)  // Do nothing in release
#endif

#include <iostream>
using namespace std;

int main() {
    cout << "PI = " << PI << endl;
    
    int arr[MAX_SIZE];
    cout << "Array size: " << sizeof(arr) << endl;
    
    cout << "Square of 5: " << SQUARE(5) << endl;
    
    LOG("Debug message");
    
    return 0;
}
```

## 7.2 Conditional Compilation

```cpp
#include <iostream>
using namespace std;

// Platform-specific code
#ifdef _WIN32
    #define OS "Windows"
#elif __APPLE__
    #define OS "macOS"
#elif __linux__
    #define OS "Linux"
#else
    #define OS "Unknown"
#endif

int main() {
    cout << "Running on: " << OS << endl;
    
#if defined(DEBUG)
    cout << "Debug mode" << endl;
#else
    cout << "Release mode" << endl;
#endif
    
    return 0;
}
```

## 7.3 Pragma Directives

```cpp
#include <iostream>
using namespace std;

// Disable specific warnings
#pragma warning(disable: 4996)  // MSVC

// Pack structure
#pragma pack(1)
struct PackedData {
    char a;
    int b;
    double c;
};
#pragma pack()

int main() {
    cout << "Size of PackedData: " << sizeof(PackedData) << endl;
    // Without pragma pack: 24 (aligned)
    // With pragma pack: 13 (packed)
    
    return 0;
}
```

---
