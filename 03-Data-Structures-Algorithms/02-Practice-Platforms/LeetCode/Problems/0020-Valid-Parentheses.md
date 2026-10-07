---
id: leetcode-0020-valid-parentheses
title: "LeetCode 0020: Valid Parentheses"
tags:
  - dsa
  - leetcode
  - string
  - stack
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/valid-parentheses/"
---

# LeetCode 0020: Valid Parentheses

## 1. Problem Formalization and Constraints

Given a string `s` containing just the characters `'('`, `')'`, `'{'`, `'}'`, `'['` and `']'`, determine if the input string is valid.
An input string is valid if:
1. Open brackets must be closed by the same type of brackets.
2. Open brackets must be closed in the correct order.
3. Every close bracket has a corresponding open bracket of the same type.

### Constraints
- $1 \le \text{s.length} \le 10^4$
- `s` consists of parentheses only `'()[]{}'`.

### Examples
- **Example 1**:
  - Input: `s = "()"`
  - Output: `true`
- **Example 2**:
  - Input: `s = "()[]{}"`
  - Output: `true`
- **Example 3**:
  - Input: `s = "(]"`
  - Output: `false`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | LIFO Stack Pushing Expected Closing Brackets | $O(N)$ | $O(N)$ | Pushes the matching closing bracket directly; simplifies comparison to a single equality check. |
| **Tier 2 (Space-Optimized Alternative)** | In-Place Index Buffer Mutation Stack | $O(N)$ | $O(1)$ extra | Reuses the input byte array/vector as an in-place stack memory without heap allocation. |
| **Tier 3 (Time-Optimized Alternative)** | Hash Map Bracket Matching Table | $O(N)$ | $O(N)$ | Explicit hash map storing open-close mappings with explicit stack validation. |
| **Tier 4 (Brute Force)** | Iterative Pair Substring Replacement | $O(N^2)$ | $O(N)$ | Repeatedly strips occurrences of `"()"`, `"{}"`, `"[]"` until no changes occur. |

---

## 3. Tier 1: Most Optimal Solution (Expected Closing Bracket Stack)

### 3.1 Algorithmic Mechanics and Invariant Proof

1. **Parity Check**: If the string length is odd ($\text{length} \pmod 2 \ne 0$), return `false` immediately because parentheses must form balanced pairs.
2. **Matching Invariant**:
   When encountering an open bracket (`'('`, `'{'`, `'['`), push its corresponding closing bracket (`')'`, `'}'`, `']'`) onto the stack.
   When encountering a closing bracket:
   - If the stack is empty, there is no matching opening bracket; return `false`.
   - If the popped character does not match the current character, bracket nesting is violated; return `false`.
3. **Termination**:
   After processing all characters, the string is valid if and only if the stack is completely empty.

**Correctness Invariant**:
Parentheses satisfy Context-Free Grammar Dyck Language rules.
By pushing the expected closing bracket, the top of the stack always contains the exact unique closing character required to preserve nesting balance.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each character is pushed and popped at most once.
- **Space Complexity**: $O(N)$. At most $N$ characters stored in the stack.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>

class Solution {
public:
    bool isValid(const std::string& s) {
        if (s.size() % 2 != 0) return false;
        std::vector<char> stack;
        stack.reserve(s.size());
        for (char c : s) {
            switch (c) {
                case '(': stack.push_back(')'); break;
                case '{': stack.push_back('}'); break;
                case '[': stack.push_back(']'); break;
                default:
                    if (stack.empty() || stack.back() != c) {
                        return false;
                    }
                    stack.pop_back();
            }
        }
        return stack.empty();
    }
};
```

#### Python 3.12
```python
class Solution:
    def isValid(self, s: str) -> bool:
        if len(s) % 2 != 0:
            return False
        stack = []
        matching = {'(': ')', '{': '}', '[': ']'}
        for c in s:
            if c in matching:
                stack.append(matching[c])
            elif not stack or stack.pop() != c:
                return False
        return len(stack) == 0
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public boolean isValid(String s) {
        if ((s.length() & 1) != 0) {
            return false;
        }
        Deque<Character> stack = new ArrayDeque<>();
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c == '(') {
                stack.push(')');
            } else if (c == '{') {
                stack.push('}');
            } else if (c == '[') {
                stack.push(']');
            } else if (stack.isEmpty() || stack.pop() != c) {
                return false;
            }
        }
        return stack.isEmpty();
    }
}
```

#### TypeScript
```typescript
function isValid(s: string): boolean {
    if ((s.length & 1) !== 0) {
        return false;
    }
    const stack: string[] = [];
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === '(') {
            stack.push(')');
        } else if (c === '{') {
            stack.push('}');
        } else if (c === '[') {
            stack.push(']');
        } else if (stack.length === 0 || stack.pop() !== c) {
            return false;
        }
    }
    return stack.length === 0;
}
```

#### Go
```go
package main

func isValid(s string) bool {
    if len(s)%2 != 0 {
        return false
    }
    stack := make([]byte, 0, len(s))
    for i := 0; i < len(s); i++ {
        c := s[i]
        switch c {
        case '(':
            stack = append(stack, ')')
        case '{':
            stack = append(stack, '}')
        case '[':
            stack = append(stack, ']')
        default:
            if len(stack) == 0 || stack[len(stack)-1] != c {
                return false
            }
            stack = stack[:len(stack)-1]
        }
    }
    return len(stack) == 0
}
```

#### Rust
```rust
impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 {
            return false;
        }
        let mut stack = Vec::with_capacity(s.len());
        for b in s.bytes() {
            match b {
                b'(' => stack.push(b')'),
                b'{' => stack.push(b'}'),
                b'[' => stack.push(b']'),
                expected => {
                    if stack.pop() != Some(expected) {
                        return false;
                    }
                }
            }
        }
        stack.is_empty()
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (In-Place Array as Stack Pointer)

### 4.1 Algorithmic Mechanics

If the input string or mutable byte array can be rewritten in-place, we maintain a `top` integer pointer representing the stack height.
Open brackets are written at `s[top++]`.
When encountering a closing bracket, we check `top > 0` and verify the element at `s[--top]`.
This eliminates heap vector allocations entirely.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear scan.
- **Space Complexity**: $O(1)$ auxiliary memory using in-place buffer mutation.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>

class Solution {
public:
    bool isValid(std::string s) {
        if (s.size() % 2 != 0) return false;
        int top = 0;
        for (char c : s) {
            if (c == '(' || c == '{' || c == '[') {
                s[top++] = c;
            } else {
                if (top == 0) return false;
                char last = s[--top];
                if ((c == ')' && last != '(') ||
                    (c == '}' && last != '{') ||
                    (c == ']' && last != '[')) {
                    return false;
                }
            }
        }
        return top == 0;
    }
};
```

#### Python 3.12
```python
class Solution:
    def isValid(self, s: str) -> bool:
        if len(s) % 2 != 0:
            return False
        buf = list(s)
        top = 0
        pairs = {')': '(', '}': '{', ']': '['}
        for c in s:
            if c not in pairs:
                buf[top] = c
                top += 1
            else:
                if top == 0 or buf[top - 1] != pairs[c]:
                    return False
                top -= 1
        return top == 0
```

#### Java 21
```java
class Solution {
    public boolean isValid(String s) {
        if ((s.length() & 1) != 0) return false;
        char[] buf = s.toCharArray();
        int top = 0;
        for (char c : buf) {
            if (c == '(' || c == '{' || c == '[') {
                buf[top++] = c;
            } else {
                if (top == 0) return false;
                char last = buf[--top];
                if ((c == ')' && last != '(') ||
                    (c == '}' && last != '{') ||
                    (c == ']' && last != '[')) {
                    return false;
                }
            }
        }
        return top == 0;
    }
}
```

#### TypeScript
```typescript
function isValid(s: string): boolean {
    if ((s.length & 1) !== 0) return false;
    const buf = new Uint8Array(s.length);
    let top = 0;
    for (let i = 0; i < s.length; i++) {
        const c = s.charCodeAt(i);
        if (c === 40 || c === 123 || c === 91) { // '(', '{', '['
            buf[top++] = c;
        } else {
            if (top === 0) return false;
            const last = buf[--top];
            if ((c === 41 && last !== 40) ||
                (c === 125 && last !== 123) ||
                (c === 93 && last !== 91)) {
                return false;
            }
        }
    }
    return top === 0;
}
```

#### Go
```go
package main

func isValid(s string) bool {
    if len(s)%2 != 0 {
        return false
    }
    buf := []byte(s)
    top := 0
    for i := 0; i < len(buf); i++ {
        c := buf[i]
        if c == '(' || c == '{' || c == '[' {
            buf[top] = c
            top++
        } else {
            if top == 0 {
                return false
            }
            top--
            last := buf[top]
            if (c == ')' && last != '(') ||
                (c == '}' && last != '{') ||
                (c == ']' && last != '[') {
                return false
            }
        }
    }
    return top == 0
}
```

#### Rust
```rust
impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 {
            return false;
        }
        let mut buf = s.into_bytes();
        let mut top = 0;
        for i in 0..buf.len() {
            let b = buf[i];
            if b == b'(' || b == b'{' || b == b'[' {
                buf[top] = b;
                top += 1;
            } else {
                if top == 0 {
                    return false;
                }
                top -= 1;
                let last = buf[top];
                if (b == b')' && last != b'(')
                    || (b == b'}' && last != b'{')
                    || (b == b']' && last != b'[')
                {
                    return false;
                }
            }
        }
        top == 0
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Hash Table Bracket Pairing)

### 5.1 Algorithmic Mechanics

We configure an explicit associative lookup table mapping each closing bracket to its required opening counterpart.
When processing a bracket, we query the map to identify whether it is open or closed, standardizing extensibility for arbitrary symbol grammars.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear pass.
- **Space Complexity**: $O(N)$ stack memory and $O(1)$ constant-size map.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <unordered_map>

class Solution {
public:
    bool isValid(const std::string& s) {
        std::unordered_map<char, char> mapping = {
            {')', '('},
            {'}', '{'},
            {']', '['}
        };
        std::vector<char> stack;
        for (char c : s) {
            auto it = mapping.find(c);
            if (it != mapping.end()) {
                if (stack.empty() || stack.back() != it->second) {
                    return false;
                }
                stack.pop_back();
            } else {
                stack.push_back(c);
            }
        }
        return stack.empty();
    }
};
```

#### Python 3.12
```python
class Solution:
    def isValid(self, s: str) -> bool:
        mapping = {')': '(', '}': '{', ']': '['}
        stack = []
        for c in s:
            if c in mapping:
                top_elem = stack.pop() if stack else '#'
                if mapping[c] != top_elem:
                    return False
            else:
                stack.append(c)
        return not stack
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Map;

class Solution {
    private static final Map<Character, Character> MAP = Map.of(
        ')', '(',
        '}', '{',
        ']', '['
    );

    public boolean isValid(String s) {
        Deque<Character> stack = new ArrayDeque<>();
        for (char c : s.toCharArray()) {
            if (MAP.containsKey(c)) {
                if (stack.isEmpty() || stack.pop() != MAP.get(c)) {
                    return false;
                }
            } else {
                stack.push(c);
            }
        }
        return stack.isEmpty();
    }
}
```

#### TypeScript
```typescript
function isValid(s: string): number | boolean {
    const map = new Map<string, string>([
        [')', '('],
        ['}', '{'],
        [']', '[']
    ]);
    const stack: string[] = [];
    for (const c of s) {
        if (map.has(c)) {
            if (stack.length === 0 || stack.pop() !== map.get(c)) {
                return false;
            }
        } else {
            stack.push(c);
        }
    }
    return stack.length === 0;
}
```

#### Go
```go
package main

func isValid(s string) bool {
    mapping := map[byte]byte{
        ')': '(',
        '}': '{',
        ']': '[',
    }
    stack := make([]byte, 0, len(s))
    for i := 0; i < len(s); i++ {
        c := s[i]
        if open, exists := mapping[c]; exists {
            if len(stack) == 0 || stack[len(stack)-1] != open {
                return false
            }
            stack = stack[:len(stack)-1]
        } else {
            stack = append(stack, c)
        }
    }
    return len(stack) == 0
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn is_valid(s: String) -> bool {
        let mapping: HashMap<char, char> = [(')', '('), ('}', '{'), (']', '[')].into_iter().collect();
        let mut stack = Vec::new();
        for c in s.chars() {
            if let Some(&open) = mapping.get(&c) {
                if stack.pop() != Some(open) {
                    return false;
                }
            } else {
                stack.push(c);
            }
        }
        stack.is_empty()
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Iterative String Substring Replacement)

### 6.1 Algorithmic Mechanics

We repeatedly scan the string and replace any occurrence of `"()"`, `"{}"`, or `"[]"` with an empty string `""`.
We repeat this loop until the string becomes empty (valid) or ceases to change (invalid).

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. At each iteration, string scanning and re-allocation take $O(N)$, with at most $N/2$ iterations.
- **Space Complexity**: $O(N)$ for newly created intermediate strings.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>

class Solution {
public:
    bool isValid(std::string s) {
        size_t prevLen;
        do {
            prevLen = s.size();
            size_t pos;
            if ((pos = s.find("()")) != std::string::npos) s.erase(pos, 2);
            else if ((pos = s.find("{}")) != std::string::npos) s.erase(pos, 2);
            else if ((pos = s.find("[]")) != std::string::npos) s.erase(pos, 2);
        } while (s.size() < prevLen);
        return s.empty();
    }
};
```

#### Python 3.12
```python
class Solution:
    def isValid(self, s: str) -> bool:
        prev_len = len(s) + 1
        while len(s) < prev_len:
            prev_len = len(s)
            s = s.replace('()', '').replace('{}', '').replace('[]', '')
        return len(s) == 0
```

#### Java 21
```java
class Solution {
    public boolean isValid(String s) {
        int prevLen;
        do {
            prevLen = s.length();
            s = s.replace("()", "").replace("{}", "").replace("[]", "");
        } while (s.length() < prevLen);
        return s.isEmpty();
    }
}
```

#### TypeScript
```typescript
function isValid(s: string): boolean {
    let prevLen = s.length + 1;
    while (s.length < prevLen) {
        prevLen = s.length;
        s = s.replaceAll("()", "").replaceAll("{}", "").replaceAll("[]", "");
    }
    return s.length === 0;
}
```

#### Go
```go
package main

import "strings"

func isValid(s string) bool {
    prevLen := len(s) + 1
    for len(s) < prevLen {
        prevLen = len(s)
        s = strings.ReplaceAll(s, "()", "")
        s = strings.ReplaceAll(s, "{}", "")
        s = strings.ReplaceAll(s, "[]", "")
    }
    return len(s) == 0
}
```

#### Rust
```rust
impl Solution {
    pub fn is_valid(mut s: String) -> bool {
        let mut prev_len = s.len() + 1;
        while s.len() < prev_len {
            prev_len = s.len();
            s = s.replace("()", "").replace("{}", "").replace("[]", "");
        }
        s.is_empty()
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is checking `s.length() % 2 != 0` an optimal first branch?</summary>
Every valid bracket structure is an exact pairing of an opening and closing symbol.
An odd string length mathematically cannot partition into disjoint pairs, allowing immediate return in $O(1)$ time without examining any characters or allocating stack memory.
</details>

<details>
<summary>2. Why is `std::vector` preferred over `std::stack` in C++?</summary>
`std::stack` is a container adaptor that defaults to wrapping `std::deque`.
`std::deque` allocates segmented memory blocks on the heap, increasing pointer chasing and allocation overhead.
`std::vector` allocates a contiguous array, allowing `reserve(s.size())` to prevent all dynamic reallocations and optimize cache line hits.
</details>

<details>
<summary>3. Why is pushing the expected closing bracket faster than pushing the opening bracket?</summary>
If you push the opening bracket, checking the closing bracket requires an extra branch or table lookup to verify matching (`c == ')' && top == '('`).
Pushing the expected closing bracket collapses the validation logic into a single direct equality comparison: `pop() == c`.
</details>

<details>
<summary>4. In Java, why is `ArrayDeque` preferred over `Stack`?</summary>
`java.util.Stack` inherits from the legacy `java.util.Vector`, where all methods (`push`, `pop`, `peek`) are synchronized with re-entrant monitors.
Synchronizing every stack operation incurs major locking overhead.
`ArrayDeque` is non-synchronized and uses a circular array buffer, running substantially faster.
</details>

<details>
<summary>5. How does this algorithm handle deeply nested expressions without stack overflow?</summary>
Because the stack is allocated on the heap (via dynamic vectors or slices), the nesting depth is limited only by available system RAM ($>10^8$ elements).
A recursive descent parser would risk thread call stack overflow on strings of length $10^4$.
</details>

<details>
<summary>6. How does branch prediction affect the switch/case matching loop?</summary>
In random bracket strings, opening vs closing transitions can mispredict.
However, because closing brackets are resolved directly against `stack.back()`, CPU instruction pipelining executes the pop and increment instructions with minimal bubbles.
</details>

<details>
<summary>7. What is the formal language classification of valid parentheses strings?</summary>
Valid parentheses strings form the Dyck language $D_k$, a classic Context-Free Language generated by the grammar $S \to \epsilon \mid (S) \mid [S] \mid \{S\} \mid SS$.
A deterministic pushdown automaton (implemented by a LIFO stack) recognizes $D_k$ in linear time.
</details>

<details>
<summary>8. How does compiler optimization vectorize bracket matching?</summary>
Because stack operations maintain state dependencies between adjacent characters, compilers cannot easily autovectorize the general multi-bracket stack into SIMD registers.
However, for single-bracket variants (`()`), running prefix sum counts can be accelerated using SIMD horizontal adds.
</details>

<details>
<summary>9. What ensures that an empty stack pop does not trigger undefined behavior?</summary>
Every implementation explicitly checks `stack.empty()` before attempting to access or pop elements.
Returning `false` when a closing bracket appears while the stack is empty prevents underflow crashes.
</details>

<details>
<summary>10. What are the key unit test edge cases for Valid Parentheses?</summary>
1. Single bracket strings: `"("`, `")"`, `"{"`.
2. Odd length strings: `"())"`, `"((("`.
3. Interleaved mismatch: `"([)]"` (tests correct nesting order).
4. Correct nested pairs: `"{[()]}"`.
5. Consecutive matching pairs: `"()[]{}"`.
6. Prefixed closing bracket: `")("`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/valid-parentheses.cpp)
- [Python Implementation](../Python/valid-parentheses.py)
- [Java Implementation](../Java/valid-parentheses.java)
- [TypeScript Implementation](../TypeScript/valid-parentheses.ts)
- [Go Implementation](../Golang/valid-parentheses.go)
- [Rust Implementation](../Rust/valid-parentheses.rs)
