# MEMORY, TYPES, AND POINTERS

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

---
### Professional Notes: Pointers & References

#### 1. Arrays as Pointers
In C++, an array name decays to a pointer to its first element in most contexts.
*   **Accessing**: `arr[i]` is equivalent to `*(arr + i)`.
*   **Size**: `sizeof(arr)` returns the total size in bytes, whereas `sizeof(ptr)` returns the size of the pointer (usually 4 or 8 bytes).

#### 2. References vs. Pointers
*   **References**: Must be initialized upon declaration. Cannot be NULL. Cannot be reseated (re-pointed).
*   **Pointers**: Can be initialized later. Can be NULL. Can point to different objects over time.

**Godhood Tip**: Use references for function parameters to avoid copying and for operator overloading. Use pointers for dynamic memory management and optional parameters.

#### 3. Pointers to Members
Pointers to members are a specialized feature allowing you to point to a data member or function inside a class without an instance.
```cpp
struct Point { int x, y; };
int Point::*p_x = &Point::x; // Pointer to member x

Point p = {10, 20};
std::cout << p.*p_x << std::endl; // Accessing via pointer to member
```

#### 4. The `this` Pointer
Inside every non-static member function, `this` is a hidden pointer to the current instance.
*   **Type**: `T* const` (or `const T* const` in const methods).
*   **Usage**: Returning `*this` allows for method chaining (e.g., in a Fluent API).

---
### Professional Notes: Language Boundary & Storage

#### 1. C Incompatibilities: The Parent's Shadow
While C++ evolved from C, they are distinct languages.
*   **`void*` Conversion**: C allows `int* p = malloc(10);`. C++ requires `int* p = static_cast<int*>(malloc(10));`.
*   **Enumerations**: In C, enums are effectively integers. In C++, they are distinct types.
*   **Tentative Definitions**: C allows `int x; int x;` at file scope. C++ considers the second one a redefinition (ODR violation).

#### 2. Storage Class Specifiers
*   **`static`**: Internal linkage for globals; persistent lifetime for locals.
*   **`extern`**: External linkage. Tells the compiler the variable is defined elsewhere.
*   **`thread_local` (C++11)**: Unique instance per thread.
*   **`register`**: (Deprecated in C++11, removed in C++17) Hint to use a CPU register.
*   **`auto`**: (C++98) Automatic storage. (C++11) Type deduction.

#### 3. Digit Separators and Binary Literals (C++14)
Use `'` to separate digits for readability: `int x = 1'000'000;`. Use `0b` for binary: `int b = 0b1101;`.

---

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

# ADVANCED ARRAYS


## 4.1 Dynamic Arrays

```cpp
#include <iostream>
using namespace std;

int main() {
    // 1D dynamic array
    int size = 5;
    int* arr = new int[size];
    
    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    delete[] arr;
    arr = NULL;
    
    // 2D dynamic array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    
    // Fill matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }
    
    // Delete matrix
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    
    return 0;
}
```

## 4.2 Variable Length Arrays (Non-standard)

```cpp
#include <iostream>
using namespace std;

int main() {
    int size;
    cout << "Enter size: ";
    cin >> size;
    
    // VLA - not standard but supported by many compilers
    int arr[size];  // GCC extension
    
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 4.3 Array Bounds & Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    
    // No bounds checking in C++
    cout << arr[0] << endl;   // 10 (OK)
    cout << arr[10] << endl;  // Undefined behavior!
    
    // Manual bounds checking
    int index = 5;
    if (index >= 0 && index < 5) {
        cout << arr[index] << endl;
    } else {
        cout << "Index out of bounds" << endl;
    }
    
    return 0;
}
```

---


<!-- Merged content from Chapter_12_CONST__VOLATILE.md -->

# CONST & VOLATILE


## 11.1 Const Correctness

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10;
    
    // const variable
    const int constant = 5;
    // constant = 10;  // ERROR
    
    // pointer to const
    const int* ptr1 = &x;
    // *ptr1 = 20;  // ERROR
    ptr1 = &constant;  // OK
    
    // const pointer
    int* const ptr2 = &x;
    *ptr2 = 20;  // OK
    // ptr2 = &constant;  // ERROR
    
    // const reference
    const int& ref = x;
    // ref = 20;  // ERROR
    
    cout << x << endl;
    cout << *ptr1 << endl;
    cout << *ptr2 << endl;
    cout << ref << endl;
    
    return 0;
}
```

## 11.2 Volatile Keyword

```cpp
#include <iostream>
using namespace std;

int main() {
    // volatile - tells compiler value may change unexpectedly
    volatile int sensor_reading = 0;  // From hardware
    
    // Compiler won't optimize away reads
    while (sensor_reading < 100) {
        // Check actual value each time, not cached
    }
    
    // Common use: hardware registers
    volatile int* hardware_register = (volatile int*)0x1000;
    
    // Each access reads from actual location
    int val1 = *hardware_register;
    int val2 = *hardware_register;
    
    // Without volatile, compiler might optimize one read
    
    return 0;
}
```

---


<!-- Merged content from Chapter_9_TYPE_CASTING.md -->

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


<!-- Merged content from Chapter_11_ENUMERATION__UNIONS.md -->

# ENUMERATION & UNIONS


## 10.1 Enumerations

```cpp
#include <iostream>
using namespace std;

// Enum definition
enum Color { RED, GREEN, BLUE };

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

int main() {
    Color c = RED;
    cout << c << endl;  // 0
    
    Color colors[3] = {RED, GREEN, BLUE};
    
    // Switching on enum
    switch (c) {
        case RED:
            cout << "Red" << endl;
            break;
        case GREEN:
            cout << "Green" << endl;
            break;
        case BLUE:
            cout << "Blue" << endl;
            break;
    }
    
    // Iterate through enum values
    for (int dir = NORTH; dir <= WEST; dir++) {
        cout << "Direction: " << dir << endl;
    }
    
    return 0;
}
```

## 10.2 Unions

```cpp
#include <iostream>
using namespace std;

// Union - all members share same memory
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data data;
    
    cout << "Size of Data: " << sizeof(data) << endl;  // 4 (size of largest member)
    
    data.i = 10;
    cout << "data.i: " << data.i << endl;     // 10
    cout << "data.f: " << data.f << endl;     // Garbage (overwrites data.i)
    
    data.f = 3.14;
    cout << "data.i: " << data.i << endl;     // Garbage (overwrites by data.f)
    cout << "data.f: " << data.f << endl;     // 3.14
    
    // Union useful for memory-constrained systems
    union Variant {
        int int_val;
        double double_val;
        char char_val;
    };
    
    cout << "Size of Variant: " << sizeof(Variant) << endl;
    
    return 0;
}
```

---


<!-- Merged content from Chapter_7_BITWISE_OPERATIONS.md -->

# BITWISE OPERATIONS


## 6.1 Bitwise Operators

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned char a = 5;   // 0101
    unsigned char b = 3;   // 0011
    
    // AND
    cout << (a & b) << endl;  // 0001 = 1
    
    // OR
    cout << (a | b) << endl;  // 0111 = 7
    
    // XOR
    cout << (a ^ b) << endl;  // 0110 = 6
    
    // NOT (bitwise complement)
    cout << (~a) << endl;     // 1010 = 250 (for unsigned char)
    
    // Left shift
    cout << (a << 1) << endl; // 1010 = 10
    
    // Right shift
    cout << (b >> 1) << endl; // 0001 = 1
    
    return 0;
}
```

## 6.2 Bit Manipulation Techniques

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned int num = 5;  // 0101
    
    // Check if bit is set
    int bit_pos = 2;
    bool is_set = (num >> bit_pos) & 1;
    cout << "Bit " << bit_pos << " is: " << is_set << endl;
    
    // Set a bit
    num |= (1 << 1);  // Set bit 1
    cout << "After setting bit 1: " << num << endl;  // 7 (0111)
    
    // Clear a bit
    num &= ~(1 << 1);  // Clear bit 1
    cout << "After clearing bit 1: " << num << endl;  // 5 (0101)
    
    // Toggle a bit
    num ^= (1 << 0);  // Toggle bit 0
    cout << "After toggling bit 0: " << num << endl;  // 4 (0100)
    
    // Count set bits
    unsigned int count = 0;
    unsigned int temp = num;
    while (temp) {
        count += temp & 1;
        temp >>= 1;
    }
    cout << "Number of set bits: " << count << endl;
    
    return 0;
}
```

---


# Professional Notes: Chapter 8: Arrays

Arrays are elements of the same type placed in adjoining memory locations. The elements can be individually
referenced by a unique identiﬁer with an added index.
This allows you to declare multiple variable values of a speciﬁc type and access them individually without needing
to declare a variable for each value.
Section 8.1: Array initialization
An array is just a block of sequential memory locations for a speciﬁc type of variable. Arrays are allocated the same
way as normal variables, but with square brackets appended to its name [] that contain the number of elements
that ﬁt into the array memory.
The following example of an array uses the typ int, the variable name arrayOfInts, and the number of elements
[5] that the array has space for:
int arrayOfInts[5];
An array can be declared and initialized at the same time like this
int arrayOfInts[5] = {10, 20, 30, 40, 50};
When initializing an array by listing all of its members, it is not necessary to include the number of elements inside
the square brackets. It will be automatically calculated by the compiler. In the following example, it's 5:
int arrayOfInts[] = {10, 20, 30, 40, 50};
It is also possible to initialize only the ﬁrst elements while allocating more space. In this case, deﬁning the length in
brackets is mandatory. The following will allocate an array of length 5 with partial initialization, the compiler
initializes all remaining elements with the standard value of the element type, in this case zero.
int arrayOfInts[5] = {10,20}; // means 10, 20, 0, 0, 0
Arrays of other basic data types may be initialized in the same way.
char arrayOfChars[5]; // declare the array and allocate the memory, don't initialize
char arrayOfChars[5] = { 'a', 'b', 'c', 'd', 'e' } ; //declare and initialize
double arrayOfDoubles[5] = {1.14159, 2.14159, 3.14159, 4.14159, 5.14159};
string arrayOfStrings[5] = { "C++", "is", "super", "duper", "great!"};
It is also important to take note that when accessing array elements, the array's element index(or position) starts
from 0.
int array[5] = { 10/*Element no.0*/, 20/*Element no.1*/, 30, 40, 50/*Element no.4*/};
std::cout << array[4]; //outputs 50
std::cout << array[0]; //outputs 10
Section 8.2: A ﬁxed size raw array matrix (that is, a 2D raw
array)
// A fixed size raw array matrix (that is, a 2D raw array).
#include <iostream>
#include <iomanip>
using namespace std;
auto main() -> int
{
    int const   n_rows  = 3;
    int const   n_cols  = 7;
    int const   m[n_rows][n_cols] =             // A raw array matrix.
    {
        {  1,  2,  3,  4,  5,  6,  7 },
        {  8,  9, 10, 11, 12, 13, 14 },
        { 15, 16, 17, 18, 19, 20, 21 }
    };
    for( int y = 0; y < n_rows; ++y )
    {
        for( int x = 0; x < n_cols; ++x )
        {
            cout << setw( 4 ) << m[y][x];       // Note: do NOT use m[y,x]!
        }
        cout << '\n';
    }
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
C++ doesn't support special syntax for indexing a multi-dimensional array. Instead such an array is viewed as an
array of arrays (possibly of arrays, and so on), and the ordinary single index notation [i] is used for each level. In
the example above m[y] refers to row y of m, where y is a zero-based index. Then this row can be indexed in turn,
e.g. m[y][x], which refers to the xth item – or column – of row y.
I.e. the last index varies fastest, and in the declaration the range of this index, which here is the number of columns
per row, is the last and “innermost” size speciﬁed.
Since C++ doesn't provide built-in support for dynamic size arrays, other than dynamic allocation, a dynamic size
matrix is often implemented as a class. Then the raw array matrix indexing notation m[y][x] has some cost, either
by exposing the implementation (so that e.g. a view of a transposed matrix becomes practically impossible) or by
adding some overhead and slight inconvenience when it's done by returning a proxy object from operator[]. And
so the indexing notation for such an abstraction can and will usually be diﬀerent, both in look-and-feel and in the
order of indices, e.g. m(x,y) or m.at(x,y) or m.item(x,y).
Section 8.3: Dynamically sized raw array
// Example of raw dynamic size array. It's generally better to use std::vector.
#include <algorithm>            // std::sort
#include <iostream>
using namespace std;
auto int_from( istream& in ) -> int { int x; in >> x; return x; }
auto main()
    -> int
{
    cout << "Sorting n integers provided by you.\\n";
    cout << "n? ";
    int const   n   = int_from( cin );
    int*        a   = new int[n];       // ← Allocation of array of n items.
    for( int i = 1; i <= n; ++i )
    {
        cout << "The #" << i << " number, please: ";
        a[i-1] = int_from( cin );
    }
    sort( a, a + n );
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\n';
    delete[] a;
}
A program that declares an array T a[n]; where n is determined a run-time, can compile with certain compilers
that support C99 variadic length arrays (VLAs) as a language extension. But VLAs are not supported by standard C++.
This example shows how to manually allocate a dynamic size array via a new[]-expression,
int*        a   = new int[n];       // ← Allocation of array of n items.
… then use it, and ﬁnally deallocate it via a delete[]-expression:
delete[] a;
The array allocated here has indeterminate values, but it can be zero-initialized by just adding an empty
parenthesis (), like this: new int[n](). More generally, for arbitrary item type, this performs a value-initialization.
As part of a function down in a call hierarchy this code would not be exception safe, since an exception before the
delete[] expression (and after the new[]) would cause a memory leak. One way to address that issue is to
automate the cleanup via e.g. a std::unique_ptr smart pointer. But a generally better way to address it is to just
use a std::vector: that's what std::vector is there for.
Section 8.4: Array size: type safe at compile time
#include      // size_t, ptrdiff_t
//----------------------------------- Machinery:
using Size = ptrdiff_t;
template< class Item, size_t n >
constexpr auto n_items( Item (&)[n] ) noexcept
-> Size
{ return n; }
//----------------------------------- Usage:
#include
using namespace std;
auto main()
-> int
{
int const   a[]     = {3, 1, 4, 1, 5, 9, 2, 6, 5, 4};
Size const  n       = n_items( a );
int         b[n]    = {};       // An array of the same size as a.
(void) b;
cout <}
The C idiom for array size, sizeof(a)/sizeof(a[0]), will accept a pointer as argument and will then generally yield
an incorrect result.
For C++11
using C++11 you can do:
std::extent<decltype(MyArray)>::value;
Example:
char MyArray[] = { 'X','o','c','e' };
const auto n = std::extent<decltype(MyArray)>::value;
std::cout << n << "\n"; // Prints 4
Up till C++17 (forthcoming as of this writing) C++ had no built-in core language or standard library utility to obtain
the size of an array, but this can be implemented by passing the array by reference to a function template, as shown
above. Fine but important point: the template size parameter is a size_t, somewhat inconsistent with the signed
Size function result type, in order to accommodate the g++ compiler which sometimes insists on size_t for
template matching.
With C++17 and later one may instead use std::size, which is specialized for arrays.
Section 8.5: Expanding dynamic size array by using
std::vector
// Example of std::vector as an expanding dynamic size array.
#include <algorithm>            // std::sort
#include <iostream>
#include <vector>               // std::vector
using namespace std;
int int_from( std::istream& in ) { int x = 0; in >> x; return x; }
int main()
{
    cout << "Sorting integers provided by you.\n";
    cout << "You can indicate EOF via F6 in Windows or Ctrl+D in Unix-land.\n";
    vector<int> a;      // ← Zero size by default.
    while( cin )
    {
        cout << "One number, please, or indicate EOF: ";
        int const x = int_from( cin );
        if( !cin.fail() ) { a.push_back( x ); }  // Expands as necessary.
    }
    sort( a.begin(), a.end() );
    int const n = a.size();
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\n';
}
std::vector is a standard library class template that provides the notion of a variable size array. It takes care of all
the memory management, and the buﬀer is contiguous so a pointer to the buﬀer (e.g. &v[0] or v.data()) can be
passed to API functions requiring a raw array. A vector can even be expanded at run time, via e.g. the push_back
member function that appends an item.
The complexity of the sequence of n push_back operations, including the copying or moving involved in the vector
expansions, is amortized O(n). “Amortized”: on average.
Internally this is usually achieved by the vector doubling its buﬀer size, its capacity, when a larger buﬀer is needed.
E.g. for a buﬀer starting out as size 1, and being repeatedly doubled as needed for n=17 push_back calls, this
involves 1 + 2 + 4 + 8 + 16 = 31 copy operations, which is less than 2×n = 34. And more generally the sum of this
sequence can't exceed 2×n.
Compared to the dynamic size raw array example, this vector-based code does not require the user to supply (and
know) the number of items up front. Instead the vector is just expanded as necessary, for each new item value
speciﬁed by the user.
Section 8.6: A dynamic size matrix using std::vector for
storage
Unfortunately as of C++14 there's no dynamic size matrix class in the C++ standard library. Matrix classes that
support dynamic size are however available from a number of 3rd party libraries, including the Boost Matrix library
(a sub-library within the Boost library).
If you don't want a dependency on Boost or some other library, then one poor man's dynamic size matrix in C++ is
just like
vector<vector<int>> m( 3, vector<int>( 7 ) );
… where vector is std::vector. The matrix is here created by copying a row vector n times where n is the number
of rows, here 3. It has the advantage of providing the same m[y][x] indexing notation as for a ﬁxed size raw array
matrix, but it's a bit ineﬃcient because it involves a dynamic allocation for each row, and it's a bit unsafe because
it's possible to inadvertently resize a row.
A more safe and eﬃcient approach is to use a single vector as storage for the matrix, and map the client code's (x, y)
to a corresponding index in that vector:
// A dynamic size matrix using std::vector for storage.
//--------------------------------------------- Machinery:
#include         // std::copy
#include          // assert
#include  // std::initializer_list
#include            // std::vector
#include          // ptrdiff_t
namespace my {
using Size = ptrdiff_t;
using std::initializer_list;
using std::vector;
template< class Item >
class Matrix
{
private:
vector    items_;
Size            n_cols_;
auto index_for( Size const x, Size const y ) const
-> Size
{ return y*n_cols_ + x; }
public:
auto n_rows() const -> Size { return items_.size()/n_cols_; }
auto n_cols() const -> Size { return n_cols_; }
auto item( Size const x, Size const y )
-> Item&
{ return items_[index_for(x, y)]; }
auto item( Size const x, Size const y ) const
-> Item const&
{ return items_[index_for(x, y)]; }
Matrix(): n_cols_( 0 ) {}
Matrix( Size const n_cols, Size const n_rows )
: items_( n_cols*n_rows )
, n_cols_( n_cols )
{}
Matrix( initializer_list< initializer_list > const& values )
: items_()
, n_cols_( values.size() == 0? 0 : values.begin()->size() )
{
for( auto const& row : values )
{
assert( Size( row.size() ) == n_cols_ );
items_.insert( items_.end(), row.begin(), row.end() );
}
}
};
}  // namespace my
//--------------------------------------------- Usage:
using my::Matrix;
auto some_matrix()
-> Matrix
{
return
{
{  1,  2,  3,  4,  5,  6,  7 },
{  8,  9, 10, 11, 12, 13, 14 },
{ 15, 16, 17, 18, 19, 20, 21 }
};
}
#include
#include
using namespace std;
auto main() -> int
{
Matrix const m = some_matrix();
assert( m.n_cols() == 7 );
assert( m.n_rows() == 3 );
for( int y = 0, y_end = m.n_rows(); y < y_end; ++y )
{
for( int x = 0, x_end = m.n_cols(); x < x_end; ++x )
{
cout <← Note: not `m[y][x]`!
}
cout <}
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
The above code is not industrial grade: it's designed to show the basic principles, and serve the needs of students
learning C++.
For example, one may deﬁne operator() overloads to simplify the indexing notation.