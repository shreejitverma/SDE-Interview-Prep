# FUNCTION POINTERS & CALLBACKS


## 3.1 Function Pointers

```cpp
#include <iostream>
using namespace std;

// Function pointer declaration: return_type (*name)(parameters)
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int main() {
    // Declare function pointer
    int (*operation)(int, int);
    
    // Assign function to pointer
    operation = add;
    cout << operation(5, 3) << endl;  // 8
    
    operation = subtract;
    cout << operation(5, 3) << endl;  // 2
    
    operation = multiply;
    cout << operation(5, 3) << endl;  // 15
    
    return 0;
}
```

## 3.2 Function Pointer Arrays

```cpp
#include <iostream>
using namespace std;

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }

int main() {
    // Array of function pointers
    int (*operations[4])(int, int) = {
        add, subtract, multiply, divide
    };
    
    int a = 20, b = 4;
    
    for (int i = 0; i < 4; i++) {
        cout << "Result: " << operations[i](a, b) << endl;
    }
    // Output: 24, 16, 80, 5
    
    return 0;
}
```

## 3.3 Callbacks

```cpp
#include <iostream>
#include <vector>
using namespace std;

// Callback function type
typedef void (*Callback)(const string&);

class Button {
private:
    Callback on_click;
    
public:
    void set_click_handler(Callback callback) {
        on_click = callback;
    }
    
    void click() {
        if (on_click) {
            on_click("Button clicked!");
        }
    }
};

void handle_click(const string& message) {
    cout << message << endl;
}

int main() {
    Button button;
    button.set_click_handler(handle_click);
    button.click();  // Output: Button clicked!
    
    return 0;
}
```

---
