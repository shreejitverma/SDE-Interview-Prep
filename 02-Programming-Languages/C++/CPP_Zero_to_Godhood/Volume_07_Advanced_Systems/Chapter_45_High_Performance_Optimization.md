# HIGH-PERFORMANCE OPTIMIZATION

## 1. CPU Caching

Memory is slow. CPU registers are fast. Caches (L1, L2, L3) bridge the gap.

*   **Cache Miss:** CPU waits hundreds of cycles for RAM.
*   **Data-Oriented Design:** Structure of Arrays (SoA) vs Array of Structures (AoS). SoA is often friendlier to cache and SIMD.

## 2. Branch Prediction

CPUs guess which way an `if` will go. If they guess wrong, pipeline flush (expensive).

*   **Sorted Data:** Branch prediction loves patterns (TTTTFFFF).
*   **Branchless Programming:** Using bitwise ops to avoid branches.
    ```cpp
    // Branchy
    if (x > 0) y = 1; else y = 0;
    // Branchless
    y = (x > 0);
    ```
*   `[[likely]]` / `[[unlikely]]` (C++20).

## 3. SIMD (Single Instruction, Multiple Data)

Doing math on 4, 8, or 16 numbers at once.

*   **Intrinsics:** `_mm256_add_ps` (AVX). Hard to read.
*   **Auto-vectorization:** Compiler does it if code is simple enough.
*   **Libraries:** `std::experimental::simd` (C++26?), Highway, Vc.

## 4. Link Time Optimization (LTO)

Allows the compiler to inline functions across translation units (object files).

## 5. Profile-Guided Optimization (PGO)

1.  Compile with instrumentation.
2.  Run the program on representative data.
3.  Recompile using the profile data. Optimizes hot paths heavily.


# Professional Notes: Chapter 143: Optimization in C++

Section 143.1: Introduction to performance 
Section 143.2: Empty Base Class Optimization 
Section 143.3: Optimizing by executing less code 
Section 143.4: Using ecient containers 
Section 143.5: Small Object Optimization 

# Professional Notes: Chapter 143: Optimization in C++

Section 143.1: Introduction to performance
C and C++ are well known as high-performance languages - largely due to the heavy amount of code customization,
allowing a user to specify performance by choice of structure.
When optimizing it is important to benchmark relevant code and completely understand how the code will be used.
Common optimization mistakes include:
Premature optimization: Complex code may perform worse after optimization, wasting time and eﬀort.
First priority should be to write correct and maintainable code, rather than optimized code.
Optimization for the wrong use case: Adding overhead for the 1% might not be worth the slowdown for
the other 99%
Micro-optimization: Compilers do this very eﬃciently and micro-optimization can even hurt the compilers
ability to further optimize the code
Typical optimization goals are:
To do less work
To use more eﬃcient algorithms/structures
To make better use of hardware
Optimized code can have negative side eﬀects, including:
Higher memory usage
Complex code -being diﬃcult to read or maintain
Compromised API and code design
Section 143.2: Empty Base Class Optimization
An object cannot occupy less than 1 byte, as then the members of an array of this type would have the same
address. Thus sizeof(T)>=1 always holds. It's also true that a derived class cannot be smaller than any of its base
classes. However, when the base class is empty, its size is not necessarily added to the derived class:
class Base {};
class Derived : public Base
{
public:
    int i;
};
In this case, it's not required to allocate a byte for Base within Derived to have a distinct address per type per
object. If empty base class optimization is performed (and no padding is required), then sizeof(Derived) ==
sizeof(int), that is, no additional allocation is done for the empty base. This is possible with multiple base classes
as well (in C++, multiple bases cannot have the same type, so no issues arise from that).
Note that this can only be performed if the ﬁrst member of Derived diﬀers in type from any of the base classes.
This includes any direct or indirect common bases. If it's the same type as one of the bases (or there's a common
base), at least allocating a single byte is required to ensure that no two distinct objects of the same type have the
same address.
Section 143.3: Optimizing by executing less code
The most straightforward approach to optimizing is by executing less code. This approach usually gives a ﬁxed
speed-up without changing the time complexity of the code.
Even though this approach gives you a clear speedup, this will only give noticable improvements when the code is
called a lot.
Removing useless code
void func(const A *a); // Some random function
// useless memory allocation + deallocation for the instance
auto a1 = std::make_unique<A>();
func(a1.get());
// making use of a stack object prevents
auto a2 = A{};
func(&a2);
Version ≥ C++14
From C++14, compilers are allowed to optimize this code to remove the allocation and matching deallocation.
Doing code only once
std::map<std::string, std::unique_ptr<A>> lookup;
// Slow insertion/lookup
// Within this function, we will traverse twice through the map lookup an element
// and even a thirth time when it wasn't in
const A *lazyLookupSlow(const std::string &key) {
    if (lookup.find(key) != lookup.cend())
        lookup.emplace_back(key, std::make_unique<A>());
    return lookup[key].get();
}
// Within this function, we will have the same noticeable effect as the slow variant while going at
double speed as we only traverse once through the code
const A *lazyLookupSlow(const std::string &key) {
    auto &value = lookup[key];
    if (!value)
        value = std::make_unique<A>();
    return value.get();
}
A similar approach to this optimization can be used to implement a stable version of unique
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // As insert returns if the insertion was successful, we can deduce if the element was
already in or not
        // This prevents an insertion, which will traverse through the map for every unique element
        // As a result we can almost gain 50% if v would not contain any duplicates
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
Preventing useless reallocating and copying/moving
In the previous example, we already prevented lookups in the std::set, however the std::vector still contains a
growing algorithm, in which it will have to realloc its storage. This can be prevented by ﬁrst reserving for the right
size.
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    // By reserving 'result', we can ensure that no copying or moving will be done in the vector
    // as it will have capacity for the maximum number of elements we will be inserting
    // If we make the assumption that no allocation occurs for size zero
    // and allocating a large block of memory takes the same time as a small block of memory
    // this will never slow down the program
    // Side note: Compilers can even predict this and remove the checks the growing from the
generated code
    result.reserve(v.size());
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // See example above
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
Section 143.4: Using ecient containers
Optimizing by using the right data structures at the right time can change the time-complexity of the code.
// This variant of stableUnique contains a complexity of N log(N)
// N > number of elements in v
// log(N) > insert complexity of std::set
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // See Optimizing by executing less code
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
By using a container which uses a diﬀerent implementation for storing its elements (hash container instead of tree),
we can transform our implementation to complexity N. As a side eﬀect, we will call the comparison operator for
std::string less, as it only has to be called when the inserted string should end up in the same bucket.
// This variant of stableUnique contains a complexity of N
// N > number of elements in v
// 1 > insert complexity of std::unordered_set
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::unordered_set<std::string> checkUnique;
    for (const auto &s : v) {
        // See Optimizing by executing less code
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
Section 143.5: Small Object Optimization
Small object optimization is a technique which is used within low level data structures, for instance the std::string
(Sometimes referred to as Short/Small String Optimization). It's meant to use stack space as a buﬀer instead of
some allocated memory in case the content is small enough to ﬁt within the reserved space.
By adding extra memory overhead and extra calculations, it tries to prevent an expensive heap allocation. The
beneﬁts of this technique are dependent on the usage and can even hurt performance if incorrectly used.
Example
A very naive way of implementing a string with this optimization would the following:
#include <cstring>
class string final
{
    constexpr static auto SMALL_BUFFER_SIZE = 16;
    bool _isAllocated{false};                       ///< Remember if we allocated memory
    char *_buffer{nullptr};                         ///< Pointer to the buffer we are using
    char _smallBuffer[SMALL_BUFFER_SIZE]= {'\0'};   ///< Stack space used for SMALL OBJECT
OPTIMIZATION
public:
    ~string()
    {
        if (_isAllocated)
            delete [] _buffer;
    }        
    explicit string(const char *cStyleString)
    {
        auto stringSize = std::strlen(cStyleString);
        _isAllocated = (stringSize > SMALL_BUFFER_SIZE);
        if (_isAllocated)
            _buffer = new char[stringSize];
        else
            _buffer = &_smallBuffer[0];
        std::strcpy(_buffer, &cStyleString[0]);
    }
    string(string &&rhs)
       : _isAllocated(rhs._isAllocated)
       , _buffer(rhs._buffer)
       , _smallBuffer(rhs._smallBuffer) //< Not needed if allocated
    {
        if (_isAllocated)
        {
           // Prevent double deletion of the memory
           rhs._buffer = nullptr;
        }
        else
        {
            // Copy over data
            std::strcpy(_smallBuffer, rhs._smallBuffer);
            _buffer = &_smallBuffer[0];
        }
    }
    // Other methods, including other constructors, copy constructor,
    // assignment operators have been omitted for readability
};
As you can see in the code above, some extra complexity has been added in order to prevent some new and delete
operations. On top of this, the class has a larger memory footprint which might not be used except in a couple of
cases.
Often it is tried to encode the bool value _isAllocated, within the pointer _buffer with bit manipulation to reduce
the size of a single instance (intel 64 bit: Could reduce size by 8 byte). An optimization which is only possible when
its known what the alignment rules of the platform is.
When to use?
As this optimization adds a lot of complexity, it is not recommended to use this optimization on every single class. It
will often be encountered in commonly used, low-level data structures. In common C++11 standard library
implementations one can ﬁnd usages in std::basic_string<> and std::function<>.
As this optimization only prevents memory allocations when the stored data is smaller than the buﬀer, it will only
give beneﬁts if the class is often used with small data.
A ﬁnal drawback of this optimization is that extra eﬀort is required when moving the buﬀer, making the move-
operation more expensive than when the buﬀer would not be used. This is especially true when the buﬀer contains
a non-POD type.