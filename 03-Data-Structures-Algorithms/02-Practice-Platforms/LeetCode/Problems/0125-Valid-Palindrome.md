---
id: leetcode-0125-valid-palindrome
title: "LeetCode 0125: Valid Palindrome"
tags:
  - dsa
  - leetcode
  - two-pointers
  - string
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/valid-palindrome/"
---

# LeetCode 0125: Valid Palindrome

## 1. Problem Formalization and Constraints

A phrase is a palindrome if, after converting all uppercase letters into lowercase letters and removing all non-alphanumeric characters, it reads the same forward and backward.
Alphanumeric characters include letters and numbers.
Given a string `s`, return `true` if it is a palindrome, or `false` otherwise.

### Constraints
- $1 \le \text{s.length} \le 2 \times 10^5$
- `s` consists only of printable ASCII characters.

### Examples
- **Example 1**:
  - Input: `s = "A man, a plan, a canal: Panama"`
  - Output: `true`
  - Explanation: `"amanaplanacanalpanama"` is a palindrome.
- **Example 2**:
  - Input: `s = "race a car"`
  - Output: `false`
  - Explanation: `"raceacar"` is not a palindrome.
- **Example 3**:
  - Input: `s = " "`
  - Output: `true`
  - Explanation: `s` becomes an empty string `""` after removing non-alphanumeric characters, which is trivially a palindrome.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | In-Place Two Pointers (Converging) | $O(N)$ | $O(1)$ | Advances left pointer and decrements right pointer skipping non-alphanumerics without heap allocations. |
| **Tier 2 (Filter & Reverse)** | Filtered String Reversal Comparison | $O(N)$ | $O(N)$ | Extracts alphanumeric characters into a new string buffer and checks equality with its reversed clone. |
| **Tier 3 (Recursive)** | Recursive Two-Pointer Decomposition | $O(N)$ | $O(N)$ | Compares boundary characters and recurses on inner sub-indices; incurs call-stack overhead proportional to string length. |
| **Tier 4 (Brute Force)** | Regular Expression Sanitization with Deque | $O(N)$ | $O(N)$ | Strips characters via regex engine and matches characters via double-ended queue pop operations. |

---

## 3. Tier 1: Most Optimal Solution (In-Place Two Pointers)

### 3.1 Algorithmic Mechanics and Invariant Proof

We initialize two indices:
- `left = 0` at the start of the string.
- `right = s.length() - 1` at the end of the string.

While `left < right`:
1. If `s[left]` is not alphanumeric, increment `left` and continue.
2. If `s[right]` is not alphanumeric, decrement `right` and continue.
3. Compare `toLowerCase(s[left])` and `toLowerCase(s[right])`.
4. If they differ, the string cannot be a palindrome; immediately return `false`.
5. If they match, advance both pointers: `left++` and `right--`.

If the pointers cross without detecting a mismatch, return `true`.

**Invariant Proof**:
Let $A$ denote the ordered sequence of alphanumeric characters extracted from $s$.
At each comparison step where both `s[left]` and `s[right]` are valid alphanumeric characters, `s[left]` corresponds to $A[k]$ and `s[right]` corresponds to $A[|A| - 1 - k]$ for some index $k$.
If for all $0 \le k < \lfloor |A| / 2 \rfloor$, $A[k] = A[|A| - 1 - k]$, then $A$ is identical to its reversal $A^R$ by definition of a palindrome.
Because non-alphanumeric characters are skipped without modifying relative ordering, in-place index convergence strictly validates the palindrome condition while allocating zero auxiliary heap memory.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$, where $N$ is the length of `s`. Each character in the string is inspected at most twice (once by `left` and once by `right`).
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. Only two integer pointers and scalar character variables are maintained in CPU registers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <cctype>

class Solution {
public:
    bool isPalindrome(std::string s) {
        int left = 0;
        int right = static_cast<int>(s.length()) - 1;

        while (left < right) {
            while (left < right && !std::isalnum(static_cast<unsigned char>(s[left]))) {
                ++left;
            }
            while (left < right && !std::isalnum(static_cast<unsigned char>(s[right]))) {
                --right;
            }

            if (std::tolower(static_cast<unsigned char>(s[left])) != 
                std::tolower(static_cast<unsigned char>(s[right]))) {
                return false;
            }

            ++left;
            --right;
        }

        return true;
    }
};
```

#### Python 3
```python
class Solution:
    def isPalindrome(self, s: str) -> bool:
        left = 0
        right = len(s) - 1

        while left < right:
            while left < right and not s[left].isalnum():
                left += 1
            while left < right and not s[right].isalnum():
                right -= 1

            if s[left].lower() != s[right].lower():
                return False

            left += 1
            right -= 1

        return True
```

#### Java 21
```java
public class Solution {
    public boolean isPalindrome(String s) {
        int left = 0;
        int right = s.length() - 1;

        while (left < right) {
            while (left < right && !Character.isLetterOrDigit(s.charAt(left))) {
                left++;
            }
            while (left < right && !Character.isLetterOrDigit(s.charAt(right))) {
                right--;
            }

            if (Character.toLowerCase(s.charAt(left)) != Character.toLowerCase(s.charAt(right))) {
                return false;
            }

            left++;
            right--;
        }

        return true;
    }
}
```

#### TypeScript 5
```typescript
function isPalindrome(s: string): boolean {
    let left = 0;
    let right = s.length - 1;

    function isAlphanumeric(char: string): boolean {
        const code = char.charCodeAt(0);
        return (
            (code >= 48 && code <= 57) || // 0-9
            (code >= 65 && code <= 90) || // A-Z
            (code >= 97 && code <= 122)   // a-z
        );
    }

    while (left < right) {
        while (left < right && !isAlphanumeric(s[left])) {
            left++;
        }
        while (left < right && !isAlphanumeric(s[right])) {
            right--;
        }

        if (s[left].toLowerCase() !== s[right].toLowerCase()) {
            return false;
        }

        left++;
        right--;
    }

    return true;
}
```

#### Go 1.22
```go
package main

import "unicode"

func isPalindrome(s string) bool {
	runes := []rune(s)
	left := 0
	right := len(runes) - 1

	for left < right {
		for left < right && !isAlphanumeric(runes[left]) {
			left++
		}
		for left < right && !isAlphanumeric(runes[right]) {
			right--
		}

		if unicode.ToLower(runes[left]) != unicode.ToLower(runes[right]) {
			return false
		}

		left++
		right--
	}

	return true
}

func isAlphanumeric(r rune) bool {
	return unicode.IsLetter(r) || unicode.IsDigit(r)
}
```

#### Rust 1.75
```rust
pub struct Solution;

impl Solution {
    pub fn is_palindrome(s: String) -> bool {
        let bytes = s.as_bytes();
        if bytes.is_empty() {
            return true;
        }

        let mut left = 0;
        let mut right = bytes.len() - 1;

        while left < right {
            while left < right && !bytes[left].is_ascii_alphanumeric() {
                left += 1;
            }
            while left < right && !bytes[right].is_ascii_alphanumeric() {
                right -= 1;
            }

            if bytes[left].to_ascii_lowercase() != bytes[right].to_ascii_lowercase() {
                return false;
            }

            if left < right {
                left += 1;
            }
            if right > 0 {
                right -= 1;
            }
        }

        true
    }
}
```

---

## 4. Tier 2: Filtered String Reversal Comparison

### 4.1 Implementation Mechanism
Construct an intermediate buffer containing only lowercase alphanumeric characters, then compare the buffer to its reversal.

```python
class SolutionFilter:
    def isPalindrome(self, s: str) -> bool:
        filtered = [c.lower() for c in s if c.isalnum()]
        return filtered == filtered[::-1]
```

### 4.2 Trade-offs
- Highly concise and expressive in high-level scripting languages.
- Requires allocating memory for the filtered list and its reversed copy ($O(N)$ auxiliary memory), causing significant garbage collection overhead on long inputs.

---

## 5. Tier 3: Recursive Two-Pointer Decomposition

### 5.1 Algorithmic Structure
A recursive function checks characters at `left` and `right`, skipping non-alphanumeric characters, and recurses on `helper(left + 1, right - 1)`.

```cpp
class SolutionRecursive {
public:
    bool isPalindrome(std::string s) {
        return helper(s, 0, static_cast<int>(s.length()) - 1);
    }

private:
    bool helper(const std::string& s, int left, int right) {
        if (left >= right) return true;
        if (!std::isalnum(static_cast<unsigned char>(s[left]))) return helper(s, left + 1, right);
        if (!std::isalnum(static_cast<unsigned char>(s[right]))) return helper(s, left, right - 1);
        if (std::tolower(static_cast<unsigned char>(s[left])) != std::tolower(static_cast<unsigned char>(s[right]))) {
            return false;
        }
        return helper(s, left + 1, right - 1);
    }
};
```

### 5.2 Trade-offs
- Straightforward inductive definition.
- For $N = 2 \times 10^5$, recursion depth can reach $10^5$, causing call stack overflow in runtimes lacking tail-call optimization.

---

## 6. Tier 4: Brute Force Baseline (Regex Preprocessing and Deque Pop)

### 6.1 Mechanical Description
Use regular expressions to strip non-alphanumeric characters, convert the remaining string to lowercase, insert all characters into a double-ended queue (deque), and repeatedly compare `pop_front()` with `pop_back()`.

### 6.2 Complexity
- **Time Complexity**: $O(N)$ with heavy constant factor due to regex DFA/NFA compilation and node-based deque pointer manipulations.
- **Space Complexity**: $O(N)$ for regex match buffers and deque heap nodes.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Contiguous Array Streaming**: The input string is stored in a contiguous linear memory buffer.
2. In-place two-pointer traversal streams data sequentially from both ends toward the middle, yielding near-optimal L1 data cache prefetching.
3. **Branch Prediction and Lookups**: Testing `isalnum` can be implemented as a fast 256-element boolean lookup table or bitmask in cache, eliminating conditional branch mispredictions.
4. **Rust UTF-8 versus ASCII**: Because constraints guarantee printable ASCII characters, indexing raw byte slices (`s.as_bytes()`) avoids repeated $O(N)$ UTF-8 grapheme boundary validations.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| All Punctuation / Whitespace | `",.!@#$% "` | Returns `true` | `left < right` loop terminates gracefully without array underflow. |
| Single Character | `"a"` or `"Z"` | Returns `true` | Outer loop condition `left < right` fails immediately. |
| Mixed Case with Numerics | `"0P"` | Returns `false` | Verifies '0' and 'p' are not equal after lowercase conversion. |
| Identical Repeated Character | `"aaaa"` | Returns `true` | Symmetric convergence succeeds. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Does the problem consider numbers as valid palindrome characters?
Yes. Alphanumeric includes both alphabetic characters ('a'-'z', 'A'-'Z') and decimal digits ('0'-'9').

### 2. Can '0' match with 'O' or 'P'?
No. Numeric digits and alphabetic characters have distinct ASCII codes and are never equal under case conversion.

### 3. Why cast to `unsigned char` before calling `std::isalnum` in C++?
In C and C++, passing a negative `char` value (such as EOF or extended ASCII) to `<cctype>` functions causes undefined behavior. Casting to `unsigned char` ensures safe domain values.

### 4. What is the impact of string copying in Python's `s.lower()`?
Calling `s.lower()` on the entire string creates a full copy in memory ($O(N)$ allocations). The Tier 1 approach lowercases characters one by one in-place, keeping auxiliary space at $O(1)$.

### 5. How should non-ASCII Unicode characters be handled if constraints change?
If extended UTF-8 characters are allowed, characters must be decoded into Unicode code points or grapheme clusters before case-folding and equivalence comparison.

### 6. Is it safe to decrement `right` when it is an unsigned integer?
In languages with unsigned index types (like `size_t` in C++ or `usize` in Rust), decrementing past zero causes underflow wraps. Using signed integers or checking `right > 0` prevents underflow.

### 7. Why is `left < right` sufficient as the while-loop termination condition?
For odd-length palindromes, `left` and `right` meet at the exact center character, which is trivially symmetric with itself. For even-length palindromes, `left` exceeds `right`.

### 8. Does early exit upon mismatch improve average-case time complexity?
Yes. On non-palindromic inputs, a mismatch is typically encountered within the first few comparisons, resulting in $O(1)$ best-case execution time.

### 9. Can bitwise operations accelerate ASCII case conversion?
For ASCII alphabetic characters, bitwise OR with `0x20` converts uppercase to lowercase, but numeric digits must not be modified by this operation.

### 10. How does this compare with LeetCode 680 (Valid Palindrome II)?
LeetCode 680 allows deleting at most one character, which requires branching the two-pointer check upon the first mismatch into two sub-checks.

---

## 10. Related Problems and Systematic Progression Links

- [[0009-Palindrome-Number]]: Palindrome verification on numeric integer representations without string conversion.
- [[0234-Palindrome-Linked-List]]: Verifying palindrome symmetry in singly linked lists using fast/slow pointers and in-place reversal.
- [[0680-Valid-Palindrome-II]]: Validating palindrome properties with at most one character deletion.
- [[0005-Longest-Palindromic-Substring]]: Expanding two-pointers outwards to find the maximum palindromic interval.
- [[0647-Palindromic-Substrings]]: Counting all palindromic substrings via center expansion.
