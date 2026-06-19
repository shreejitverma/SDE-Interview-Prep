# Chapter 49: Writing a C Compiler Basics

# WRITING A C++ COMPILER (BASICS)

To understand C++, build a toy compiler.

### 18.1 Lexical Analysis (Tokenizer)

Converting source code into tokens.

```cpp
enum class TokenType { Int, Identifier, Plus, Minus, End };

struct Token {
    TokenType type;
    std::string text;
};

std::vector<Token> tokenize(std::string_view source) {
    std::vector<Token> tokens;
    // ... implementation ...
    return tokens;
}
```

### 18.2 Parsing (Recursive Descent)

Building an Abstract Syntax Tree (AST).

```cpp
struct ASTNode { virtual ~ASTNode() = default; };
struct BinaryExpr : ASTNode {
    std::unique_ptr<ASTNode> left, right;
    char op;
};

// parseExpression() calls parseTerm(), etc.
```

### 18.3 Semantic Analysis (Types & Scopes)

Before generating code, we must validate it.

**Symbol Table:**
```cpp
struct Symbol { string type; };
using Scope = map<string, Symbol>;
vector<Scope> scopes; // Stack of scopes

void enter_scope() { scopes.push_back({}); }
void exit_scope() { scopes.pop_back(); }
```

**Type Checking:**
Recursively visit the AST.
*   `BinaryExpr`: Check left.type == right.type.
*   `Variable`: Check if exists in symbol table.

