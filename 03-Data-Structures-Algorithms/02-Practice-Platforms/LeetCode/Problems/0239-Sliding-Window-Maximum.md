---
title: LeetCode - 0239 - Sliding Window Maximum
tags:
  - leetcode
  - problem
  - sliding-window
  - monotonic-queue
  - heap
  - array
difficulty: hard
source: leetcode
problem_number: "0239"
topics:
  - Sliding Window
  - Monotonic Queue
  - Heap (Priority Queue)
  - Array
---

# LeetCode 0239: Sliding Window Maximum

## Problem Breakdown

You are given an array of integers `nums`, and there is a sliding window of size `k` moving from the very left of the array to the very right.
You can only see the `k` numbers in the window.
Each time the sliding window moves right by one position.
Return the max sliding window.

### Monotonic Invariant Analysis

- A naive search examines all $k$ elements for each of the $n - k + 1$ window positions, taking $O(n \cdot k)$ time.
- A balanced BST or max-heap achieves $O(n \log k)$ time, which incurs logarithmic overhead per insertion and deletion.
- To achieve strictly linear $O(n)$ time, we observe an elimination principle:
  - If an incoming element `nums[i]` is strictly greater than or equal to an earlier element `nums[j]` ($j < i$) already inside the window, `nums[j]` can **never** be the maximum of the current window or any future window that contains `nums[i]`.
  - Thus, `nums[j]` can be discarded from consideration immediately.
- Storing candidate indices in a double-ended queue (deque) maintains values in strictly decreasing order.
- The maximum element of the active window is always positioned at `dq.front()`.

## Optimal Approaches

### Monotonic Deque Algorithm

```
nums = [1, 3, -1, -3, 5, 3, 6, 7], k = 3

i = 0 (1):  dq = [0 (val 1)]
i = 1 (3):  3 > 1 -> pop 0. dq = [1 (val 3)]
i = 2 (-1): -1 < 3 -> push 2. dq = [1 (val 3), 2 (val -1)]. Window 1: max = nums[1] = 3
i = 3 (-3): -3 < -1 -> push 3. dq = [1 (val 3), 2 (val -1), 3 (val -3)]. Window 2: max = nums[1] = 3
i = 4 (5):  idx 1 out of window (4 - 3 = 1 -> pop 1).
            5 > -3, 5 > -1 -> pop 3, 2. push 4. dq = [4 (val 5)]. Window 3: max = nums[4] = 5
i = 5 (3):  3 < 5 -> push 5. dq = [4 (val 5), 5 (val 3)]. Window 4: max = nums[4] = 5
i = 6 (6):  6 > 3, 6 > 5 -> pop 5, 4. push 6. dq = [6 (val 6)]. Window 5: max = nums[6] = 6
i = 7 (7):  7 > 6 -> pop 6. push 7. dq = [7 (val 7)]. Window 6: max = nums[7] = 7

Result = [3, 3, 5, 5, 6, 7]
```

1. Initialize an empty deque `dq` to store array indices.
2. For each index `i` from 0 to $n - 1$:
   - If `dq.front() <= i - k`, pop from the front because that index is now outside the window boundary.
   - While `dq` is not empty and `nums[dq.back()] <= nums[i]`, pop from the back to maintain strictly decreasing value ordering.
   - Push index `i` to the back of `dq`.
   - If `i >= k - 1`, append `nums[dq.front()]` to the result list.
3. Return the result list.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | Each array index is pushed onto the deque once and popped at most once. |
| **Space Complexity** | $O(k)$ | The deque contains at most $k$ indices corresponding to the active window. |

## Common Traps & Edge Cases

- **Storing Values vs Indices**: Storing raw values in the deque prevents checking whether an element has slipped past the left edge of the sliding window; storing indices resolves expiration unambiguously.
- **Window Size $k = 1$**: Handled seamlessly without extra branch logic; every element is pushed and immediately yielded.
- **Strictly Decreasing Ordering**: When encountering an equal element (`nums[dq.back()] == nums[i]`), popping the older index is valid because the newer index will remain inside future windows longer.
