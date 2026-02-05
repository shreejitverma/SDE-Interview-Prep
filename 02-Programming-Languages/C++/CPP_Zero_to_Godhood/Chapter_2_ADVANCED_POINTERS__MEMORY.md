# ADVANCED POINTERS & MEMORY


## 1.1 Pointer to Const vs Const Pointer

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5, y = 10;
    
    // Pointer to const - can't modify data
    const int* ptr1 = &x;
    // *ptr1 = 10;  // ERROR
    ptr1 = &y;      // OK - can change pointer
    
    // Const pointer - can't modify pointer
    int* const ptr2 = &x;
    *ptr2 = 10;     // OK - can change data
    // ptr2 = &y;   // ERROR
    
    // Const pointer to const - can't modify either
    const int* const ptr3 = &x;
    // *ptr3 = 10;  // ERROR
    // ptr3 = &y;   // ERROR
    
    return 0;
}
```

## 1.2 Void Pointers

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 42;
    double y = 3.14;
    
    // Void pointer can point to any type
    void* ptr = &x;
    cout << *(int*)ptr << endl;  // 42
    
    ptr = &y;
    cout << *(double*)ptr << endl;  // 3.14
    
    // Generic function using void*
    void print_value(void* ptr, char type) {
        if (type == 'i') {
            cout << *(int*)ptr << endl;
        } else if (type == 'd') {
            cout << *(double*)ptr << endl;
        }
    }
    
    print_value(&x, 'i');  // 42
    print_value(&y, 'd');  // 3.14
    
    return 0;
}
```

## 1.3 Null Pointer Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = NULL;  // Set to null
    
    // Always check before dereferencing
    if (ptr != NULL) {
        cout << *ptr << endl;
    } else {
        cout << "Pointer is NULL" << endl;
    }
    
    // Safer approach
    ptr = new int(42);
    if (ptr) {
        cout << *ptr << endl;
        delete ptr;
        ptr = NULL;
    }
    
    return 0;
}
```

## 1.4 Memory Layout & Alignment

```cpp
#include <iostream>
using namespace std;

int main() {
    struct Data {
        char a;     // 1 byte
        int b;      // 4 bytes
        double c;   // 8 bytes
    };
    
    cout << "Size of Data: " << sizeof(Data) << endl;
    // Likely 16 or 24 (due to alignment padding)
    
    cout << "Size of char: " << sizeof(char) << endl;      // 1
    cout << "Size of int: " << sizeof(int) << endl;        // 4
    cout << "Size of double: " << sizeof(double) << endl;  // 8
    
    Data data;
    cout << "Address of a: " << (void*)&data.a << endl;
    cout << "Address of b: " << (void*)&data.b << endl;
    cout << "Address of c: " << (void*)&data.c << endl;
    
    return 0;
}
```

---
