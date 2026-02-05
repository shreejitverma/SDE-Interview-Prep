# ERROR HANDLING & DEBUGGING


## 15.1 Assert Macro

```cpp
#include <iostream>
#include <cassert>
using namespace std;

int divide(int a, int b) {
    assert(b != 0);  // b must not be zero
    return a / b;
}

int main() {
    cout << divide(10, 2) << endl;  // 5
    
    // cout << divide(10, 0) << endl;  // Assertion fails!
    
    return 0;
}
```

## 15.2 Debug Output

```cpp
#include <iostream>
#include <cstdio>
using namespace std;

#ifdef DEBUG
    #define DPRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
    #define DPRINTF(fmt, ...) (void)0
#endif

int main() {
    int x = 42;
    
    DPRINTF("Debug: x = %d\n", x);
    
    cout << "Regular output" << endl;
    
    
    return 0;
}
```

---
