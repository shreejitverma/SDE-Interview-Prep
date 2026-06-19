# Chapter 43: Writing a Compiler and a Garbage Collector

> *To understand the machine, you must build the machine.*

We have explored the depths of C++. We have written templates that execute at compile time, and we have rebuilt the Standard Library. But there is one final system left to demystify: the compiler itself. 

In this chapter, we will walk through the architecture of a C++ compiler. As a bonus, we will implement a Garbage Collector—something C++ explicitly lacks—to understand how managed languages like Java and Python work under the hood.

---

## 43.1 Phase 1: Lexical Analysis (The Tokenizer)

The first step in compiling a C++ program is converting a giant string of text (`std::string_view`) into a stream of meaningful "Tokens." The compiler doesn't care about spaces or newlines; it only cares about syntax.

```cpp
#include <string>
#include <vector>

enum class TokenType { 
    Keyword_Int, Identifier, Operator_Plus, Operator_Minus, Semicolon, EndOfFile 
};

struct Token {
    TokenType type;
    std::string text;
};

// The Lexer loops through the source code character by character.
std::vector<Token> tokenize(std::string_view source) {
    std::vector<Token> tokens;
    // ... string parsing logic ...
    // e.g., if it sees "int", it outputs {TokenType::Keyword_Int, "int"}
    return tokens;
}
```

## 43.2 Phase 2: Parsing (The AST)

Once we have a linear list of tokens, we must understand the *grammar* of the program. Does the `*` operator mean "multiply" or "dereference"?

We build an **Abstract Syntax Tree (AST)** using a technique called *Recursive Descent Parsing*. Every node in the tree represents an operation or a value.

```cpp
#include <memory>

// Base class for all nodes in the tree
struct ASTNode { 
    virtual ~ASTNode() = default; 
    virtual void print() = 0; 
};

// A node representing a number like '42'
struct NumberNode : ASTNode {
    int value;
    NumberNode(int v) : value(v) {}
    void print() override { /* ... */ }
};

// A node representing math: (Left Node) + (Right Node)
struct BinaryOpNode : ASTNode {
    char op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(char o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) 
        : op(o), left(std::move(l)), right(std::move(r)) {}
        
    void print() override { /* ... */ }
};
```
If the parser sees `5 + 3`, it creates a `BinaryOpNode` with `+` as the operator, and two `NumberNode`s as children.

## 43.3 Phase 3: Semantic Analysis

Before we can generate assembly code, we must ensure the AST makes sense. This is where Type Checking happens.

The compiler maintains a **Symbol Table**—a dictionary mapping variable names to their types. When it encounters `x = y + 5`, it looks up `x` and `y` in the Symbol Table.
If `y` is a `std::string` and `5` is an `int`, the compiler flags a Type Error and halts.

```cpp
#include <map>
#include <string>
#include <vector>

struct Symbol { 
    std::string type_name; 
    int memory_offset;
};

// A Stack of Scopes (Global Scope -> Function Scope -> If-Block Scope)
std::vector<std::map<std::string, Symbol>> symbol_tables;

void enter_scope() { symbol_tables.push_back({}); }
void exit_scope() { symbol_tables.pop_back(); }
```

## 43.4 Phase 4: Code Generation

Once the AST is perfectly valid, the compiler traverses the tree one last time, translating each node into machine instructions (or an intermediate language like LLVM IR).

*   `NumberNode(5)` translates to `mov eax, 5`.
*   `BinaryOpNode(+)` translates to `add eax, ebx`.

---

## 43.5 Writing a Garbage Collector (Mark-and-Sweep)

C++ uses RAII (Resource Acquisition Is Initialization) to manage memory deterministically. When a `std::unique_ptr` goes out of scope, the memory is instantly freed. 

Languages like Java, Python, and Go use **Garbage Collection (GC)**. The programmer never calls `delete`. Instead, a background thread occasionally pauses the program, scans for unused memory, and frees it.

Let's build a basic **Mark-and-Sweep** Garbage Collector in C++.

### Step 1: The Virtual Machine Heap
Every object allocated in our language must inherit from a base `GCObject` that has a `marked` flag. The VM keeps a master list of all allocated objects.

```cpp
#include <vector>
#include <algorithm>

struct GCObject {
    bool marked = false;
    virtual ~GCObject() = default;
};

class VM {
    // The Heap: Every object we've ever allocated
    std::vector<GCObject*> heap;
    
    // The Roots: Objects currently referenced by local variables on the Stack
    std::vector<GCObject*> roots;

public:
    GCObject* allocate(GCObject* obj) {
        heap.push_back(obj);
        return obj;
    }
    // ...
```

### Step 2: The Mark Phase
When memory runs low, the GC pauses the world. It starts at the "Roots" (the active variables in the current function) and recursively follows every pointer, setting `marked = true`.

```cpp
    void mark() {
        for (auto* obj : roots) {
            mark_object(obj);
        }
    }

    void mark_object(GCObject* obj) {
        if (!obj || obj->marked) return;
        
        obj->marked = true;
        
        // If this object holds pointers to other objects, 
        // we must recursively mark them here!
    }
```

### Step 3: The Sweep Phase
Once all reachable objects are marked `true`, any object in the Heap that is still marked `false` is completely inaccessible to the programmer. It is garbage. We delete it.

```cpp
    void sweep() {
        // Remove-Erase idiom
        auto it = std::remove_if(heap.begin(), heap.end(), [](GCObject* obj) {
            if (!obj->marked) {
                delete obj;  // It's garbage! Free the memory.
                return true; // Remove pointer from the heap vector
            }
            
            // It survived! Unmark it for the next GC cycle.
            obj->marked = false; 
            return false;
        });
        
        heap.erase(it, heap.end());
    }
};
```

This is exactly how early versions of Java and JavaScript worked. While modern GCs are vastly more complex (using generational copying and concurrent marking), the fundamental theory remains identical.

---

With a deep understanding of Compilers, Standard Libraries, and Memory Management, you have conquered the Systems domain. We now move to our final phase: **Part XIII: Specialized Domains**, where we will tackle Networking, Interoperability, and Game Engine architecture.
