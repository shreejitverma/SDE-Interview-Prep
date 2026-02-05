# ADVANCED CONTROL FLOW


## 9.1 Ternary Operator

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10, y = 5;
    
    // condition ? true_value : false_value
    int max = (x > y) ? x : y;
    cout << "Max: " << max << endl;  // 10
    
    // Nested ternary (use with caution)
    int age = 20;
    string status = (age < 18) ? "Minor" : (age < 65) ? "Adult" : "Senior";
    cout << status << endl;
    
    // String ternary
    cout << (x % 2 == 0 ? "Even" : "Odd") << endl;
    
    return 0;
}
```

## 9.2 goto Statement (Avoid)

```cpp
#include <iostream>
using namespace std;

int main() {
    // goto is generally discouraged
    int x = 0;
    
loop:
    cout << x << " ";
    x++;
    
    if (x < 5) {
        goto loop;
    }
    cout << endl;
    
    // Better alternative: use loops
    for (int i = 0; i < 5; i++) {
        cout << i << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 9.3 Label & Goto for Error Handling

```cpp
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
    FILE* file = NULL;
    char* buffer = NULL;
    
    // Using goto for cleanup (rare acceptable use)
    file = fopen("test.txt", "r");
    if (!file) {
        cout << "Failed to open file" << endl;
        goto cleanup;
    }
    
    buffer = new char[100];
    if (!buffer) {
        cout << "Memory allocation failed" << endl;
        goto cleanup;
    }
    
    // Do work...
    
cleanup:
    if (buffer) delete[] buffer;
    if (file) fclose(file);
    
    return 0;
}
```

---
