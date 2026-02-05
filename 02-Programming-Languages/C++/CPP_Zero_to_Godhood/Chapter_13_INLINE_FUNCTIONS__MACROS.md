# INLINE FUNCTIONS & MACROS


## 12.1 Macro Functions vs Inline Functions

```cpp
#include <iostream>
using namespace std;

// Macro function - preprocessor substitution
#define ADD_MACRO(a, b) ((a) + (b))

// Inline function - type-safe
inline int add_inline(int a, int b) {
    return a + b;
}

int main() {
    cout << ADD_MACRO(5, 3) << endl;         // 8
    cout << add_inline(5, 3) << endl;       // 8
    
    // Macro danger: side effects
    int x = 5, y = 3;
    cout << ADD_MACRO(x++, y++) << endl;    // Evaluates as: ((x++) + (y++))
    cout << "x = " << x << ", y = " << y << endl;  // x = 6, y = 4
    
    // Inline function is safer
    x = 5, y = 3;
    cout << add_inline(x++, y++) << endl;   // 8
    cout << "x = " << x << ", y = " << y << endl;  // x = 6, y = 4 (correct)
    
    return 0;
}
```

---
