# TYPE CASTING


## 8.1 C-Style Casting

```cpp
#include <iostream>
#include <cmath>
using namespace std;

int main() {
    double d = 3.14;
    
    // C-style cast (avoid in modern C++)
    int i = (int)d;  // 3
    cout << i << endl;
    
    int x = 65;
    char c = (char)x;  // 'A'
    cout << c << endl;
    
    float f = (float)d;
    cout << f << endl;
    
    return 0;
}
```

## 8.2 Implicit Conversions

```cpp
#include <iostream>
using namespace std;

int main() {
    // Implicit conversions
    int x = 5;
    double d = x;  // int to double (automatic)
    cout << d << endl;  // 5.0
    
    double d2 = 3.9;
    int y = d2;  // double to int (loses precision)
    cout << y << endl;  // 3
    
    // Char arithmetic
    char c = 'A';
    int code = c;  // char to int (gets ASCII)
    cout << code << endl;  // 65
    
    return 0;
}
```

---
