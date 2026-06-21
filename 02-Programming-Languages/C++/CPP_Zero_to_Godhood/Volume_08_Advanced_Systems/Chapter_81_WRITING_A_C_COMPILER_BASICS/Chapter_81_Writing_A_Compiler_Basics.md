# Chapter 81: Writing a Compiler — The Basics

Nothing demystifies C++ like building a compiler for a slice of it. The translation pipeline — lexing, parsing, semantic analysis, code generation — is exactly what runs every time you build, and understanding it changes how you read error messages, why undefined behaviour is exploitable, and what the optimizer can and cannot prove. This chapter builds a toy compiler front-end for a small expression/statement language, then connects each stage to the real cost model and to the language-lawyer concepts (ODR, type systems, UB) that the rest of the volume depends on.

## Chapter Roadmap

- 81.1 Why Build a Compiler
- 81.2 The Translation Pipeline
- 81.3 Lexical Analysis (Tokenizing)
- 81.4 Parsing (Recursive Descent and the AST)
- 81.5 Semantic Analysis: Symbol Tables and Scopes
- 81.6 Type Checking
- 81.7 From AST to IR and Code Generation
- 81.8 What This Teaches About C++ and Performance

---

## 81.1 Why Build a Compiler

A compiler is a program that translates source text into a semantically-equivalent lower-level form. Building even a toy one forces you to internalise what the C++ front-end does on every translation unit: it tokenizes, parses to an abstract syntax tree, resolves names and types, and lowers to an intermediate representation the optimizer transforms before emitting machine code.

> **Why this matters.** Most "mysterious" C++ behaviour dissolves once you have written the relevant compiler stage. Why is a missing semicolon reported on the *next* line? Because the parser only notices the syntax error when it fails to find an expected token. Why does ADL find an overload you did not expect? Because name resolution is a deliberate algorithm over scopes. Why can the optimizer delete your null check? Because a prior dereference let it *prove* the pointer non-null. The compiler is not magic; it is the algorithms in this chapter, scaled up.

---

## 81.2 The Translation Pipeline

A classical compiler is a sequence of stages, each consuming the previous stage's output:

```text
source text
   → [lexer]        → token stream
   → [parser]       → abstract syntax tree (AST)
   → [semantic]     → annotated AST (types resolved, names bound)
   → [IR lowering]  → intermediate representation (e.g. SSA)
   → [optimizer]    → optimized IR
   → [codegen]      → assembly / machine code
```
*Listing 81.1 — The phases of translation. The first three (the front-end) are this chapter's focus.*

> **Why this matters / cost model.** Each stage has a distinct cost and failure mode. Lexing and parsing are roughly linear in source size and rarely the bottleneck. *Semantic analysis* in C++ is where template instantiation lives — potentially super-linear and the usual cause of slow builds (Chapter 74). The optimizer is where most compile time goes at `-O2`/`-O3`. Separating the stages is what lets a compiler report a *syntax* error (parser) distinctly from a *type* error (semantic) — and what lets tools like clang-tidy operate on the AST without code generation.

---

## 81.3 Lexical Analysis (Tokenizing)

The **lexer** (scanner/tokenizer) converts the raw character stream into a sequence of **tokens** — the atomic units of the grammar (identifiers, literals, operators, punctuation) — discarding whitespace and comments.

```cpp
// Min standard: C++17 (string_view). Portable.
#include <string>
#include <string_view>
#include <vector>
#include <cctype>

enum class TokenType { Int, Identifier, Plus, Minus, Star, Slash, LParen, RParen, End };

struct Token {
    TokenType type;
    std::string text;
};

std::vector<Token> tokenize(std::string_view src) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < src.size()) {
        char c = src[i];
        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            size_t j = i;
            while (j < src.size() && std::isdigit(static_cast<unsigned char>(src[j]))) ++j;
            tokens.push_back({TokenType::Int, std::string(src.substr(i, j - i))});
            i = j;
        } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t j = i;
            while (j < src.size() && (std::isalnum(static_cast<unsigned char>(src[j])) || src[j] == '_')) ++j;
            tokens.push_back({TokenType::Identifier, std::string(src.substr(i, j - i))});
            i = j;
        } else {
            switch (c) {
                case '+': tokens.push_back({TokenType::Plus,  "+"}); break;
                case '-': tokens.push_back({TokenType::Minus, "-"}); break;
                case '*': tokens.push_back({TokenType::Star,  "*"}); break;
                case '/': tokens.push_back({TokenType::Slash, "/"}); break;
                case '(': tokens.push_back({TokenType::LParen,"("}); break;
                case ')': tokens.push_back({TokenType::RParen,")"}); break;
                default:  /* error: unexpected character */ break;
            }
            ++i;
        }
    }
    tokens.push_back({TokenType::End, ""});
    return tokens;
}
```
*Listing 81.2 — A hand-written lexer for an arithmetic language. Real lexers are often generated from regular expressions (a DFA).*

> **Why this matters.** Lexing is where the language's *lexical* rules live: maximal munch (`>>` is one token, or two in nested templates — a famous C++ ambiguity the standard had to fix in C++11), keyword vs identifier classification, and literal formats. A hand-written lexer like this is O(n) and trivial; production lexers are DFAs generated from regexes (lex/flex) for speed and maintainability. The lesson: tokens, not characters, are what the parser reasons about.

---

## 81.4 Parsing (Recursive Descent and the AST)

The **parser** consumes the token stream and builds an **Abstract Syntax Tree (AST)** that encodes the program's grammatical structure. **Recursive descent** is the most readable technique: one function per grammar rule, each calling the functions for its sub-rules — operator precedence falls out of the call nesting.

```cpp
// Min standard: C++14. Portable.
#include <memory>

struct ASTNode { virtual ~ASTNode() = default; };

struct NumberLit : ASTNode { long value; };
struct BinaryExpr : ASTNode {
    std::unique_ptr<ASTNode> left, right;
    char op;                         // '+', '-', '*', '/'
};

// Grammar (precedence encoded by the call hierarchy):
//   expression := term   (('+' | '-') term)*
//   term       := factor (('*' | '/') factor)*
//   factor     := NUMBER | '(' expression ')'
//
// parseExpression() calls parseTerm(), which calls parseFactor().
// Lower-precedence rules sit higher in the call tree, so '*' binds tighter than '+'.
```
*Listing 81.3 — AST node types and the precedence-encoding grammar for recursive descent.*

> **Why this matters / cost model.** The AST is the central data structure of every later stage — semantic analysis annotates it, the IR is lowered from it, and refactoring/linting tools walk it. Recursive descent is O(n) for unambiguous grammars and matches the grammar one-to-one, which is why most production compilers (including Clang) hand-write their parsers rather than use generators: better error messages and recovery. C++'s grammar is famously *not* context-free — `T * p;` is a multiplication or a pointer declaration depending on whether `T` is a type, which the parser cannot know without consulting the symbol table. This "lexer hack" coupling of parsing and semantics is why C++ is hard to parse.

---

## 81.5 Semantic Analysis: Symbol Tables and Scopes

Parsing proves the program is *grammatically* valid; **semantic analysis** proves it is *meaningful* — every name refers to a declared entity, types are compatible, and scoping rules hold. The core data structure is the **symbol table**, typically a stack of scopes.

```cpp
// Min standard: C++11. Portable.
#include <map>
#include <string>
#include <vector>

struct Symbol { std::string type; /* + storage, qualifiers, etc. */ };
using Scope = std::map<std::string, Symbol>;

std::vector<Scope> scopes;                       // stack of scopes (innermost = back)

void enter_scope() { scopes.emplace_back(); }
void exit_scope()  { scopes.pop_back(); }

void declare(const std::string& name, Symbol s) { scopes.back()[name] = std::move(s); }

const Symbol* lookup(const std::string& name) {  // search innermost outward — name shadowing
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        if (auto f = it->find(name); f != it->end()) return &f->second;
    return nullptr;                              // undeclared identifier
}
```
*Listing 81.4 — A scoped symbol table. `lookup` searches inner-to-outer, implementing shadowing.*

> **Why this matters.** This single structure *is* C++ name resolution, scaled up. Block scope, function scope, namespace scope, and class scope are all stacks-of-scopes with extra rules; **shadowing** is exactly the inner-to-outer search in `lookup`; **ADL** is an additional set of scopes consulted based on argument types. The **One Definition Rule** is, at bottom, the linker's global symbol table rejecting two definitions of the same name. When you understand the symbol table, ODR violations, ambiguous lookups, and "undeclared identifier" stop being mysteries — they are this code returning the wrong entry or `nullptr`.

---

## 81.6 Type Checking

With names bound, the type checker walks the AST and verifies each operation's operands have compatible types, computing the type of each expression bottom-up.

```cpp
// Min standard: C++14. Conceptual visitor over the AST.
// For a BinaryExpr:
//   1. type-check left and right (recursively) -> left.type, right.type
//   2. verify the operator is defined for those types (and apply conversions)
//   3. compute and annotate the result type
// For an identifier:
//   1. look it up in the symbol table; error if absent
//   2. its type is the symbol's declared type
```
*Listing 81.5 — Type checking as a bottom-up tree walk annotating each node with a type.*

> **Why this matters / cost model.** Type checking is where C++'s overload resolution, implicit conversions, template argument deduction, and concept satisfaction all live — by far the most complex part of a C++ front-end and the reason its semantic phase dwarfs lexing/parsing in both code and time. Catching a type error here, at compile time, is the entire value proposition of a statically-typed language: the bug is a build failure, not a runtime crash. Every `static_assert`, every concept, and every `enable_if` from Chapters 74–75 is a hook into this phase, instructing the type checker to reject programs you have defined as invalid.

---

## 81.7 From AST to IR and Code Generation

After the front-end, the type-annotated AST is lowered to an **intermediate representation (IR)** — typically in **static single assignment (SSA)** form, where every variable is assigned exactly once — on which the optimizer runs its passes (constant folding, dead-code elimination, inlining, vectorization) before the back-end emits target machine code.

> **Why this matters.** The optimizer operates on the IR, not your source, and it transforms aggressively under the **as-if rule**: it may reorder, delete, and combine operations as long as observable behaviour is preserved. This is the mechanism behind every "the compiler optimized away my benchmark" story (Chapter 103) and every "undefined behaviour deleted my safety check" story (Chapter 104): once the optimizer *proves* a fact (this branch is dead, this pointer is non-null, this loop has no observable effect), it acts on it. SSA makes those proofs tractable — assigning each value once turns data-flow analysis into a graph problem. Understanding that your code becomes IR before it becomes instructions is the key to reading disassembly (Chapter 89) and to not being surprised by the optimizer.

---

## 81.8 What This Teaches About C++ and Performance

| Compiler concept | What it explains about C++ |
|---|---|
| Token stream | Maximal munch, `>>` ambiguity, keyword classification |
| Recursive-descent AST | Operator precedence, why error locations lag, the `T * p` ambiguity |
| Symbol table / scopes | Name lookup, shadowing, ADL, the ODR |
| Type checker | Overload resolution, conversions, templates, concepts, `static_assert` |
| IR / SSA / as-if rule | Optimization, dead-code elimination, why UB is exploitable |

> **The discipline.** You do not need to write a production compiler, but knowing how one works converts the compiler from an oracle into a tool you can reason about. The optimizer is the most important collaborator in high-performance C++: Chapter 89 reads its assembly output, Chapter 102 controls its cross-TU inlining, and Chapter 104 examines exactly how it exploits undefined behaviour. All three build on the pipeline sketched here. The next chapter applies the same "build it to understand it" approach to memory management, by writing a garbage collector.
