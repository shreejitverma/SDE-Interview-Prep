---
title: LeetCode - 0215 - Kth Largest Element in an Array
tags:
  - leetcode
  - problem
  - quickselect
  - heap
  - divide-and-conquer
difficulty: medium
source: leetcode
problem_number: "0215"
topics:
  - QuickSelect
  - Heap (Priority Queue)
  - Divide and Conquer
  - Array
---

# LeetCode 0215: Kth Largest Element in an Array

## Problem Breakdown

Given an integer array `nums` and an integer `k`, return the `k`-th largest element in the array.
Note that it is the `k`-th largest element in the sorted order, not the `k`-th distinct element.
Can you solve it without sorting the entire array?

### Key Observations

- Sorting the whole array takes $O(n \log n)$ time, which does more work than necessary since only one specific position is needed.
- The $k$-th largest element in an array of size $n$ corresponds to the element at index `n - k` in a 0-indexed ascending sorted array.
- Selection algorithms can identify this element without fully ordering the surrounding elements.
- Two primary paradigms exist for optimal selection:
  1. **QuickSelect (Hoare's Selection with 3-way Dutch National Flag Partitioning)**: Expected linear time $O(n)$, $O(1)$ extra space.
  2. **Min-Heap of Size $k$**: Deterministic $O(n \log k)$ time and $O(k)$ auxiliary space, highly resilient to adversarial inputs.

## Optimal Approaches

### 1. Randomized QuickSelect with 3-Way Partitioning

Standard 2-way QuickSelect degrades to $O(n^2)$ when all elements are identical or heavily duplicated.
Dutch National Flag 3-way partitioning divides elements into three regions: `< pivot`, `== pivot`, and `> pivot`.

```
Array:   [3, 2, 1, 5, 6, 4], k = 2 -> target_idx = 6 - 2 = 4
Pivot:   4
After 3-Way Partition:
         [3, 2, 1,  4,  5, 6]
         < pivot   ==   > pivot
Indices: 0  1  2   3    4  5
Target index 4 is in the '> pivot' partition -> recurse into [5, 6].
```

1. Compute `target_idx = nums.length - k`.
2. Pick a random pivot index in `[left, right]`.
3. Partition `nums[left..right]` into three sub-ranges: elements less than pivot, elements equal to pivot `[lt, gt]`, and elements greater than pivot.
4. If `lt <= target_idx <= gt`, the pivot value is precisely the $k$-th largest element.
5. If `target_idx < lt`, continue searching in `[left, lt - 1]`.
6. Otherwise, continue searching in `[gt + 1, right]`.

### 2. Min-Heap of Size $k$

1. Maintain a min-heap storing at most $k$ elements.
2. For each number in `nums`, push it into the min-heap.
3. If the size of the min-heap exceeds $k$, pop the minimum element.
4. After processing all numbers, the root of the min-heap contains the $k$-th largest element.

## Complexity Analysis

| Approach | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| **Randomized QuickSelect** | $O(n)$ average / $O(n^2)$ worst | $O(1)$ iterative | 3-way partitioning prevents degradation on duplicate values. |
| **Min-Heap (Size $k$)** | $O(n \log k)$ | $O(k)$ | Guaranteed deterministic bound, cache-friendly for small $k$. |
| **Full Array Sort** | $O(n \log n)$ | $O(1)$ or $O(n)$ | Trivial baseline implementation. |

## Common Traps & Edge Cases

- **Duplicate Elements**: Many test cases feature arrays consisting entirely of the same number (e.g., all 1s). Standard 2-way Lomuto partition degenerates to $O(n^2)$; 3-way partitioning handles this in $O(n)$.
- **Index Conversion**: Remember that $k$-th largest translates to index `n - k` in ascending order, or index `k - 1` in descending order.
- **Negative Numbers**: The array can contain negative values; min-heap comparisons must preserve signed integers.
