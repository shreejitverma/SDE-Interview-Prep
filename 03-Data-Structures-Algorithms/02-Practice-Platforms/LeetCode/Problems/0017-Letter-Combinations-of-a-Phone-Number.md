---
id: leetcode-0017-letter-combinations-of-a-phone-number
title: "LeetCode 0017: Letter Combinations of a Phone Number"
tags:
  - dsa
  - leetcode
  - backtracking
  - string
  - recursion
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/letter-combinations-of-a-phone-number/"
---

# LeetCode 0017: Letter Combinations of a Phone Number

## 1. Problem Formalization and Constraints

Given a string containing digits from `2-9` inclusive, return all possible letter combinations that the number could represent.
Return the answer in any order.
A mapping of digits to letters (just like on the telephone buttons) is given below:
- `2` -> `"abc"`
- `3` -> `"def"`
- `4` -> `"ghi"`
- `5` -> `"jkl"`
- `6` -> `"mno"`
- `7` -> `"pqrs"`
- `8` -> `"tuv"`
- `9` -> `"wxyz"`
Note that 1 does not map to any letters.

### Constraints
- $0 \le \text{digits.length} \le 4$
- `digits[i]` is a digit in the range `['2', '9']`.

### Examples
- **Example 1**:
  - Input: `digits = "23"`
  - Output: `["ad","ae","af","bd","be","bf","cd","ce","cf"]`
- **Example 2**:
  - Input: `digits = ""`
  - Output: `[]`
- **Example 3**:
  - Input: `digits = "2"`
  - Output: `["a","b","c"]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Backtracking DFS with Reusable Path Buffer | $O(N \cdot 4^N)$ | $O(N)$ auxiliary | Mutates single contiguous buffer; emits materialized string at leaf depth $N$. |
| **Tier 2 (Radix)** | Combinatorial Cartesian Radix Arithmetic | $O(N \cdot 4^N)$ | $O(1)$ auxiliary | Maps an integer index $i \in [0, \prod |C_k| - 1]$ to string combination via mixed-radix division. |
| **Tier 3 (BFS)** | Layered Breadth-First Expansion via Queue | $O(N \cdot 4^N)$ | $O(4^N)$ | Extends partial prefix strings level by level; allocates intermediate string copies across BFS stages. |
| **Tier 4 (Brute Force)** | Recursive Immutable String Concatenation | $O(N^2 \cdot 4^N)$ | $O(N \cdot 4^N)$ | Creates intermediate substrings at every branch point, inducing allocator thrashing. |

---

## 3. Tier 1: Most Optimal Solution (Backtracking DFS with Reusable Path Buffer)

### 3.1 Algorithmic Mechanics and Invariant Proof

The search space forms a decision tree where each level $k \in [0, N-1]$ corresponds to digit `digits[k]`.
The branching factor at level $k$ is either 3 or 4 (e.g. 7 and 9 have 4 letters).
1. If `digits` is empty, return an empty array immediately.
2. Maintain a single character buffer `path` of length $N$.
3. Define recursive function `backtrack(index)`:
   - Base Case: When `index == digits.length`, append a copy of `path` to the result collector.
   - Recursive Step: Retrieve letters mapped to `digits[index]`. For each character:
     - Assign `path[index] = char`.
     - Recurse on `backtrack(index + 1)`.
     - Pop character upon return (or overwrite in-place).

**Invariant Proof**:
Let $S_k$ denote the set of all prefixes of length $k$ formed from `digits[0...k-1]`.
Base Case: At $k=0$, $S_0 = \{\epsilon\}$ (the empty prefix).
Inductive Step: Assume $S_k$ contains all valid combinations of the first $k$ digits.
For each prefix $p \in S_k$ and each character $c \in \text{MAPPING}[\text{digits}[k]]$, the concatenation $p + c$ forms a valid, distinct prefix of length $k+1$.
Because each branch explores mutually exclusive choices without cycles, the traversal touches each leaf in the decision tree exactly once.
When `index == N`, every generated string has length $N$ and represents a unique Cartesian product tuple, guaranteeing complete and duplicate-free coverage.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \cdot 4^N)$, where $N$ is the length of `digits`. The total number of combinations is bounded by $4^N$ (in the worst case where all digits are 7 or 9). Constructing each combination takes $O(N)$ string copying time.
- **Auxiliary Space Complexity**: $O(N)$ auxiliary space. The recursion stack depth and the reused buffer length are strictly $N \le 4$.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <string>

class Solution {
private:
    const std::vector<std::string> mapping = {
        "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    };

public:
    std::vector<std::string> letterCombinations(std::string digits) {
        std::vector<std::string> result;
        if (digits.empty()) {
            return result;
        }

        std::string path;
        backtrack(digits, 0, path, result);
        return result;
    }

private:
    void backtrack(const std::string& digits, int index, std::string& path, std::vector<std::string>& result) {
        if (index == digits.length()) {
            result.push_back(path);
            return;
        }

        const std::string& letters = mapping[digits[index] - '0'];
        for (char ch : letters) {
            path.push_back(ch);
            backtrack(digits, index + 1, path, result);
            path.pop_back();
        }
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def letterCombinations(self, digits: str) -> List[str]:
        if not digits:
            return []

        mapping = ["", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"]
        result: List[str] = []
        path: List[str] = []

        def backtrack(index: int) -> None:
            if index == len(digits):
                result.append("".join(path))
                return

            letters = mapping[ord(digits[index]) - ord('0')]
            for ch in letters:
                path.append(ch)
                backtrack(index + 1)
                path.pop()

        backtrack(0)
        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

public class Solution {
    private static final String[] MAPPING = {
        "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    };

    public List<String> letterCombinations(String digits) {
        List<String> result = new ArrayList<>();
        if (digits == null || digits.isEmpty()) {
            return result;
        }

        StringBuilder path = new StringBuilder();
        backtrack(digits, 0, path, result);
        return result;
    }

    private void backtrack(String digits, int index, StringBuilder path, List<String> result) {
        if (index == digits.length()) {
            result.add(path.toString());
            return;
        }

        String letters = MAPPING[digits.charAt(index) - '0'];
        for (int i = 0; i < letters.length(); i++) {
            path.append(letters.charAt(i));
            backtrack(digits, index + 1, path, result);
            path.deleteCharAt(path.length() - 1);
        }
    }
}
```

#### TypeScript 5
```typescript
function letterCombinations(digits: string): string[] {
    const result: string[] = [];
    if (!digits) {
        return result;
    }

    const mapping: string[] = [
        "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    ];

    const path: string[] = [];

    function backtrack(index: number): void {
        if (index === digits.length) {
            result.push(path.join(""));
            return;
        }

        const digitChar = digits.charCodeAt(index) - 48;
        const letters = mapping[digitChar];
        for (let i = 0; i < letters.length; i++) {
            path.push(letters[i]);
            backtrack(index + 1);
            path.pop();
        }
    }

    backtrack(0);
    return result;
}
```

#### Go 1.22
```go
package main

var mapping = []string{
	"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz",
}

func letterCombinations(digits string) []string {
	var result []string
	if len(digits) == 0 {
		return result
	}

	path := make([]byte, len(digits))

	var backtrack func(index int)
	backtrack = func(index int) {
		if index == len(digits) {
			result = append(result, string(path))
			return
		}

		letters := mapping[digits[index]-'0']
		for i := 0; i < len(letters); i++ {
			path[index] = letters[i]
			backtrack(index + 1)
		}
	}

	backtrack(0)
	return result
}
```

#### Rust 1.75
```rust
pub struct Solution;

const MAPPING: [&str; 10] = [
    "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz",
];

impl Solution {
    pub fn letter_combinations(digits: String) -> Vec<String> {
        let mut result = Vec::new();
        if digits.is_empty() {
            return result;
        }

        let digits_bytes = digits.as_bytes();
        let mut path = String::with_capacity(digits_bytes.len());
        Self::backtrack(digits_bytes, 0, &mut path, &mut result);
        result
    }

    fn backtrack(digits: &[u8], index: usize, path: &mut String, result: &mut Vec<String>) {
        if index == digits.len() {
            result.push(path.clone());
            return;
        }

        let digit_idx = (digits[index] - b'0') as usize;
        let letters = MAPPING[digit_idx];
        for ch in letters.chars() {
            path.push(ch);
            Self::backtrack(digits, index + 1, path, result);
            path.pop();
        }
    }
}
```

---

## 4. Tier 2: Combinatorial Cartesian Radix Arithmetic

### 4.1 Implementation Mechanism
Compute the total number of combinations $T = \prod_{k=0}^{N-1} |\text{choice}_k|$.
For each integer $i \in [0, T-1]$, determine the character at position $k$ using mixed-radix positional decomposition:
$$c_k = \left(\left\lfloor \frac{i}{\prod_{m=k+1}^{N-1} |\text{choice}_m|} \right\rfloor \pmod{|\text{choice}_k|}\right)$$

```cpp
class SolutionRadix {
public:
    std::vector<std::string> letterCombinations(std::string digits) {
        static const std::vector<std::string> lookup = {
            "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
        };
        if (digits.empty()) return {};

        int total = 1;
        for (char d : digits) total *= lookup[d - '0'].size();

        std::vector<std::string> result;
        result.reserve(total);

        for (int i = 0; i < total; ++i) {
            int base = total;
            std::string curr;
            for (char d : digits) {
                const auto& choices = lookup[d - '0'];
                base /= choices.size();
                curr.push_back(choices[(i / base) % choices.size()]);
            }
            result.push_back(curr);
        }
        return result;
    }
};
```

### 4.2 Trade-offs
- Completely non-recursive with zero call-stack allocations.
- Direct mathematical indexing enables trivial multi-threaded parallel generation across thread pools.

---

## 5. Tier 3: Layered Breadth-First Expansion via Queue

### 5.1 Algorithmic Structure
Start with a queue containing a single empty string `[""]`.
For each digit in `digits`, pop every existing prefix string from the queue, append each corresponding letter, and push the newly extended strings back into the queue.

```python
class SolutionBFS:
    def letterCombinations(self, digits: str) -> List[str]:
        if not digits:
            return []
        mapping = ["", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"]
        queue = [""]
        for digit in digits:
            letters = mapping[ord(digit) - ord('0')]
            queue = [prefix + ch for prefix in queue for ch in letters]
        return queue
```

### 5.2 Trade-offs
- Straightforward iterative construction.
- Re-allocates collections at every intermediate digit level, generating excess garbage collection pressure.

---

## 6. Tier 4: Brute Force Baseline (Recursive Immutable String Concatenation)

### 6.1 Mechanical Description
A recursive function passes intermediate string copies `helper(index, current_str + letter)` by value.
Each node in the call tree clones previous characters.

### 6.2 Complexity
- **Time Complexity**: $O(N^2 \cdot 4^N)$ due to repetitive string buffer copying at each internal tree node.
- **Space Complexity**: $O(N \cdot 4^N)$ cumulative memory allocations across internal frames.
- **Verdict**: Suboptimal memory behavior compared to the in-place mutated buffer of Tier 1.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Reused Vector Buffers**: Reusing a single `std::string` or `StringBuilder` ensures the buffer stays in L1 CPU cache lines throughout tree traversal.
2. **Small String Optimization (SSO)**: Because $N \le 4$, the `path` string never exceeds 15 bytes, meaning it resides entirely within SSO inline storage without dynamic heap allocation in modern C++ standard libraries.
3. **Pre-Allocation**: Reserving capacity `result.reserve(total)` eliminates vector reallocation and reallocation copies.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Empty String | `digits = ""` | Returns empty list `[]` | Explicit guard `if (digits.empty()) return {};` |
| Single Digit | `digits = "7"` | Returns `["p", "q", "r", "s"]` | Base case activates at depth 1. |
| Maximum Length | `digits = "7979"` | Generates $4^4 = 256$ strings | Bounds within fast CPU register limits. |
| Digits 0 and 1 | N/A | Strictly prevented by problem constraints | Lookup indices mapped safely. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does the problem specify that digits 0 and 1 do not map to letters?
On standard telephone keypads, 0 is typically mapped to space or operator, and 1 has no alphabetic characters. The problem constrains inputs to digits 2 through 9.

### 2. Can combinations be returned in any order?
Yes. The problem statement explicitly permits results in any permutation order.

### 3. What is the maximum number of combinations generated?
The maximum length is 4. When all digits are 7 or 9 (which each map to 4 characters), the maximum output size is $4^4 = 256$ strings.

### 4. Why is backtracking faster than breadth-first queue expansion?
Backtracking mutates a single character buffer in-place, whereas BFS allocates and clones new string objects for every partial prefix at each step.

### 5. How does Small String Optimization (SSO) help in C++?
In C++, strings under 15-22 characters are stored inline within the string structure itself without dynamic heap allocation. Since combinations are at most 4 bytes long, zero heap allocations occur for `path`.

### 6. Why is StringBuilder preferred in Java?
Java strings are immutable. Using `StringBuilder` avoids generating intermediate garbage-collected string objects during depth-first traversal.

### 7. How does Go handle byte mutation in strings?
In Go, strings are immutable byte slices. By using a pre-allocated slice `[]byte` of length $N$, characters are written by index and converted to `string` only at leaf nodes.

### 8. What is mixed-radix notation in Tier 2?
Because digits map to varying numbers of letters (3 or 4), the combinatorial space forms a mixed-radix numeral system where each place value has a varying base.

### 9. Can this problem be solved using bitmasking?
Yes, using radix bit shifts or modular arithmetic as shown in Tier 2.

### 10. How does this problem relate to LeetCode 39 (Combination Sum)?
LeetCode 17 is a fixed-depth Cartesian product where each digit contributes exactly one letter. LeetCode 39 involves dynamic-depth subset generation bounded by a target numerical sum.

---

## 10. Related Problems and Systematic Progression Links

- [[0022-Generate-Parentheses]]: Backtracking with dynamic validity pruning constraints.
- [[0078-Subsets]]: Generating power sets via binary selection choices.
- [[0079-Word-Search]]: 2D grid backtracking and path traversal.
- LeetCode 39 (Combination Sum): Backtracking with unbounded candidate reuse.
- LeetCode 46 (Permutations): Backtracking across element permutations.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/letter-combinations-of-a-phone-number.cpp)
- [Python Implementation](../Python/letter-combinations-of-a-phone-number.py)
- [Java Implementation](../Java/letter-combinations-of-a-phone-number.java)
- [TypeScript Implementation](../TypeScript/letter-combinations-of-a-phone-number.ts)
- [Go Implementation](../Golang/letter-combinations-of-a-phone-number.go)
- [Rust Implementation](../Rust/letter-combinations-of-a-phone-number.rs)
