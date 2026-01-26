# The Blind 75 LeetCode Problems - Complete Python Solutions Guide

A curated list of 75 essential LeetCode problems that cover all major algorithmic concepts. Each problem includes optimal time/space complexity and production-grade Python solutions.

---

## Table of Contents

### Arrays (9 Problems)
1. Two Sum
2. Best Time to Buy and Sell Stock
3. Contains Duplicate
4. Product of Array Except Self
5. Maximum Subarray
6. Maximum Product Subarray
7. Search in Rotated Sorted Array
8. 3Sum
9. Container With Most Water

### Binary (5 Problems)
10. Sum of Two Integers
11. Number of 1 Bits
12. Counting Bits
13. Missing Number
14. Reverse Bits

### Dynamic Programming (15 Problems)
15. Climbing Stairs
16. Coin Change
17. Longest Increasing Subsequence
18. Longest Common Subsequence
19. Word Break
20. Combination Sum IV
21. House Robber
22. House Robber II
23. Decode Ways
24. Coin Change 2
25. Partition Equal Subset Sum
26. Longest Palindromic Substring
27. Palindromic Substrings
28. Number of Longest Increasing Subsequence
29. Maximal Square

### Graph (11 Problems)
30. Number of Islands
31. Clone Graph
32. Course Schedule
33. Course Schedule II
34. Alien Dictionary
35. Graph Valid Tree
36. Number of Connected Components in an Undirected Graph
37. Longest Consecutive
38. Pacific Atlantic Water Flow
39. Walls and Gates
40. Rotting Oranges

### Interval (5 Problems)
41. Insert Interval
42. Merge Intervals
43. Non-overlapping Intervals
44. Meeting Rooms
45. Meeting Rooms II

### Linked List (7 Problems)
46. Reverse Linked List
47. Detect Cycle in Linked List
48. Merge Two Sorted Lists
49. Merge K Sorted Lists
50. Remove Nth Node From End of List
51. Reorder List
52. Set Matrix Zeroes

### Matrix (3 Problems)
53. Spiral Matrix
54. Rotate Matrix
55. Word Search

### String (9 Problems)
56. Longest Substring Without Repeating Characters
57. Longest Repeating Character Replacement
58. Minimum Window Substring
59. Valid Anagram
60. Group Anagrams
61. Valid Parentheses
62. Valid Palindrome
63. Longest Palindromic Substring (Duplicate)
64. Encode and Decode Strings

### Tree (11 Problems)
65. Binary Tree Maximum Path Sum
66. Binary Tree Level Order Traversal
67. Serialize and Deserialize Binary Tree
68. Subtree of Another Tree
69. Construct Binary Tree from Preorder and Inorder Traversal
70. Validate Binary Search Tree
71. Kth Smallest Element in a BST
72. Lowest Common Ancestor of a Binary Search Tree
73. Invert Binary Tree
74. Same Tree
75. Binary Tree Right Side View

---

## 1. TWO SUM

**Link:** https://leetcode.com/problems/two-sum/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(n)

**Problem:** Given an array of integers `nums` and an integer `target`, return the indices of the two numbers that add up to the target.

**Most Optimized Solution:**

```python
class Solution:
    def twoSum(self, nums: list[int], target: int) -> list[int]:
        seen = {}
        for i, num in enumerate(nums):
            complement = target - num
            if complement in seen:
                return [seen[complement], i]
            seen[num] = i
        return []
```

**Explanation:** Use a hash map to store values we've seen with their indices. For each number, calculate complement needed (target - current). If complement exists in map, we found the pair. Time: O(n) single pass, Space: O(n) for hash map.

---

## 2. BEST TIME TO BUY AND SELL STOCK

**Link:** https://leetcode.com/problems/best-time-to-buy-and-sell-stock/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Find maximum profit from buying and selling a stock once.

**Most Optimized Solution:**

```python
class Solution:
    def maxProfit(self, prices: list[int]) -> int:
        min_price = float('inf')
        max_profit = 0
        
        for price in prices:
            min_price = min(min_price, price)
            max_profit = max(max_profit, price - min_price)
        
        return max_profit
```

**Explanation:** Track minimum price seen so far and maximum profit. For each price, calculate profit if sold at that price (price - minPrice). Time: O(n) single pass, Space: O(1) constant.

---

## 3. CONTAINS DUPLICATE

**Link:** https://leetcode.com/problems/contains-duplicate/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(n)

**Problem:** Determine if array contains any duplicate.

**Most Optimized Solution:**

```python
class Solution:
    def containsDuplicate(self, nums: list[int]) -> bool:
        seen = set()
        for num in nums:
            if num in seen:
                return True
            seen.add(num)
        return False
```

**Alternative (One-liner):**

```python
def containsDuplicate(self, nums: list[int]) -> bool:
    return len(nums) != len(set(nums))
```

**Explanation:** Use hash set to track seen numbers. If we encounter a number already in set, it's a duplicate. Time: O(n), Space: O(n) for hash set.

---

## 4. PRODUCT OF ARRAY EXCEPT SELF

**Link:** https://leetcode.com/problems/product-of-array-except-self/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Return array where result[i] is product of all elements except nums[i].

**Most Optimized Solution:**

```python
class Solution:
    def productExceptSelf(self, nums: list[int]) -> list[int]:
        n = len(nums)
        result = [1] * n
        
        # Calculate prefix products
        prefix = 1
        for i in range(n):
            result[i] = prefix
            prefix *= nums[i]
        
        # Calculate suffix products and multiply
        suffix = 1
        for i in range(n - 1, -1, -1):
            result[i] *= suffix
            suffix *= nums[i]
        
        return result
```

**Explanation:** First pass: calculate prefix product (all elements to left). Second pass: calculate suffix product (all elements to right) and multiply. Time: O(n) two passes, Space: O(1) excluding output array.

---

## 5. MAXIMUM SUBARRAY

**Link:** https://leetcode.com/problems/maximum-subarray/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find contiguous subarray with largest sum (Kadane's Algorithm).

**Most Optimized Solution:**

```python
class Solution:
    def maxSubArray(self, nums: list[int]) -> int:
        max_current = max_global = nums[0]
        
        for i in range(1, len(nums)):
            max_current = max(nums[i], max_current + nums[i])
            max_global = max(max_global, max_current)
        
        return max_global
```

**Explanation:** Kadane's Algorithm: track max sum ending at current position. If adding current element to previous sum is worse than current element alone, reset. Time: O(n), Space: O(1).

---

## 6. MAXIMUM PRODUCT SUBARRAY

**Link:** https://leetcode.com/problems/maximum-product-subarray/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find contiguous subarray with largest product.

**Most Optimized Solution:**

```python
class Solution:
    def maxProduct(self, nums: list[int]) -> int:
        max_prod = min_prod = result = nums[0]
        
        for i in range(1, len(nums)):
            # Negative number flips max/min
            if nums[i] < 0:
                max_prod, min_prod = min_prod, max_prod
            
            max_prod = max(nums[i], max_prod * nums[i])
            min_prod = min(nums[i], min_prod * nums[i])
            
            result = max(result, max_prod)
        
        return result
```

**Explanation:** Track both max and min product (negative * negative = positive). Swap when encountering negative (to handle flipping). Time: O(n), Space: O(1).

---

## 7. SEARCH IN ROTATED SORTED ARRAY

**Link:** https://leetcode.com/problems/search-in-rotated-sorted-array/

**Difficulty:** Medium | **Time:** O(log n) | **Space:** O(1)

**Problem:** Search for target in rotated sorted array.

**Most Optimized Solution:**

```python
class Solution:
    def search(self, nums: list[int], target: int) -> int:
        left, right = 0, len(nums) - 1
        
        while left <= right:
            mid = (left + right) // 2
            
            if nums[mid] == target:
                return mid
            
            # Determine which half is sorted
            if nums[left] <= nums[mid]:
                # Left half is sorted
                if nums[left] <= target < nums[mid]:
                    right = mid - 1
                else:
                    left = mid + 1
            else:
                # Right half is sorted
                if nums[mid] < target <= nums[right]:
                    left = mid + 1
                else:
                    right = mid - 1
        
        return -1
```

**Explanation:** Binary search but determine which half is sorted. Check if target is in sorted half. Narrow search space based on target location. Time: O(log n) binary search, Space: O(1).

---

## 8. 3SUM

**Link:** https://leetcode.com/problems/3sum/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Find all unique triplets that sum to zero.

**Most Optimized Solution:**

```python
class Solution:
    def threeSum(self, nums: list[int]) -> list[list[int]]:
        result = []
        if len(nums) < 3:
            return result
        
        nums.sort()
        
        for i in range(len(nums) - 2):
            # Skip duplicates
            if i > 0 and nums[i] == nums[i - 1]:
                continue
            
            # If positive, no solution possible
            if nums[i] > 0:
                break
            
            left = i + 1
            right = len(nums) - 1
            
            while left < right:
                total = nums[i] + nums[left] + nums[right]
                
                if total == 0:
                    result.append([nums[i], nums[left], nums[right]])
                    
                    # Skip duplicates
                    while left < right and nums[left] == nums[left + 1]:
                        left += 1
                    while left < right and nums[right] == nums[right - 1]:
                        right -= 1
                    
                    left += 1
                    right -= 1
                elif total < 0:
                    left += 1
                else:
                    right -= 1
        
        return result
```

**Explanation:** Sort array first. Fix one element and use two-pointer for remaining two. Skip duplicates to avoid duplicate triplets. Time: O(n²) due to two-pointer, Space: O(1) excluding output.

---

## 9. CONTAINER WITH MOST WATER

**Link:** https://leetcode.com/problems/container-with-most-water/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find two lines that form container with most water.

**Most Optimized Solution:**

```python
class Solution:
    def maxArea(self, height: list[int]) -> int:
        left = 0
        right = len(height) - 1
        max_area = 0
        
        while left < right:
            area = min(height[left], height[right]) * (right - left)
            max_area = max(max_area, area)
            
            # Move pointer pointing to smaller height
            if height[left] < height[right]:
                left += 1
            else:
                right -= 1
        
        return max_area
```

**Explanation:** Two-pointer approach from both ends. Area = min(left, right) * distance. Move pointer pointing to smaller height (only way to improve). Time: O(n), Space: O(1).

---

## 10. SUM OF TWO INTEGERS

**Link:** https://leetcode.com/problems/sum-of-two-integers/

**Difficulty:** Medium | **Time:** O(1) | **Space:** O(1)

**Problem:** Add two integers without using + or - operators.

**Most Optimized Solution:**

```python
class Solution:
    def getSum(self, a: int, b: int) -> int:
        # Python handles arbitrary precision, so we need to limit to 32-bit
        MAX = 0x7FFFFFFF
        mask = 0xFFFFFFFF
        
        while b != 0:
            carry = ((a & b) << 1) & mask
            a = (a ^ b) & mask
            b = carry
        
        # Convert back from 32-bit two's complement
        return a if a <= MAX else ~(a ^ mask)
```

**Explanation:** XOR gives sum without carry: a ^ b. AND and left shift give carry: (a & b) << 1. Repeat until carry is 0. Handle Python's arbitrary precision with masking. Time: O(1) constant iterations, Space: O(1).

---

## 11. NUMBER OF 1 BITS

**Link:** https://leetcode.com/problems/number-of-1-bits/

**Difficulty:** Easy | **Time:** O(1) | **Space:** O(1)

**Problem:** Count number of 1 bits in binary representation.

**Most Optimized Solution:**

```python
class Solution:
    def hammingWeight(self, n: int) -> int:
        count = 0
        while n:
            count += n & 1
            n >>= 1
        return count
```

**Pythonic Alternative:**

```python
def hammingWeight(self, n: int) -> int:
    return bin(n).count('1')
```

**Bit Trick Alternative:**

```python
def hammingWeight(self, n: int) -> int:
    count = 0
    while n:
        n &= n - 1  # Removes rightmost 1 bit
        count += 1
    return count
```

**Explanation:** Check rightmost bit with AND 1. Right shift to check next bit. Count until n becomes 0. Alternative: n & (n-1) removes rightmost 1 bit each iteration. Time: O(1) max 32 iterations, Space: O(1).

---

## 12. COUNTING BITS

**Link:** https://leetcode.com/problems/counting-bits/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Return array where i-th element is count of 1s in binary of i.

**Most Optimized Solution:**

```python
class Solution:
    def countBits(self, n: int) -> list[int]:
        result = [0] * (n + 1)
        
        for i in range(1, n + 1):
            # i >> 1 removes last bit, & 1 checks if odd
            result[i] = result[i >> 1] + (i & 1)
        
        return result
```

**Explanation:** DP approach: result[i] = result[i >> 1] + (i & 1). i >> 1 is i divided by 2. i & 1 is 1 if i is odd, 0 if even. Reuse previous results. Time: O(n), Space: O(1) excluding output.

---

## 13. MISSING NUMBER

**Link:** https://leetcode.com/problems/missing-number/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Given array containing n distinct numbers from 0 to n, find missing one.

**Most Optimized Solution:**

```python
class Solution:
    def missingNumber(self, nums: list[int]) -> int:
        n = len(nums)
        expected_sum = n * (n + 1) // 2
        actual_sum = sum(nums)
        return expected_sum - actual_sum
```

**XOR Alternative:**

```python
def missingNumber(self, nums: list[int]) -> int:
    result = 0
    for i in range(len(nums)):
        result ^= i ^ nums[i]
    return result ^ len(nums)
```

**Explanation:** Sum of 0 to n is n*(n+1)/2. Subtract actual sum from expected sum. Time: O(n), Space: O(1).

---

## 14. REVERSE BITS

**Link:** https://leetcode.com/problems/reverse-bits/

**Difficulty:** Easy | **Time:** O(1) | **Space:** O(1)

**Problem:** Reverse bits of 32-bit unsigned integer.

**Most Optimized Solution:**

```python
class Solution:
    def reverseBits(self, n: int) -> int:
        result = 0
        for i in range(32):
            result <<= 1  # Shift left to make space
            result |= n & 1  # Add rightmost bit
            n >>= 1  # Shift n right
        return result
```

**Explanation:** Build result by shifting left and adding bits from n. Extract rightmost bit of n with AND 1. Shift n right to process next bit. Time: O(1) exactly 32 iterations, Space: O(1).

---

## 15. CLIMBING STAIRS

**Link:** https://leetcode.com/problems/climbing-stairs/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Climb n stairs taking 1 or 2 steps at a time. How many ways?

**Most Optimized Solution:**

```python
class Solution:
    def climbStairs(self, n: int) -> int:
        if n <= 2:
            return n
        
        prev, curr = 1, 2
        
        for i in range(3, n + 1):
            next_val = prev + curr
            prev = curr
            curr = next_val
        
        return curr
```

**Explanation:** DP: ways[i] = ways[i-1] + ways[i-2]. Each step can reach from 1 or 2 steps before. Optimize space: only track last two values. Time: O(n), Space: O(1).

---

## 16. COIN CHANGE

**Link:** https://leetcode.com/problems/coin-change/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Find minimum number of coins to make amount.

**Most Optimized Solution:**

```python
class Solution:
    def coinChange(self, coins: list[int], amount: int) -> int:
        dp = [float('inf')] * (amount + 1)
        dp[0] = 0
        
        for i in range(1, amount + 1):
            for coin in coins:
                if coin <= i:
                    dp[i] = min(dp[i], dp[i - coin] + 1)
        
        return dp[amount] if dp[amount] != float('inf') else -1
```

**Explanation:** DP: dp[i] = minimum coins to make amount i. For each amount, try all coins. dp[i] = min(dp[i], dp[i - coin] + 1). Time: O(n*m) where n=amount, m=coins, Space: O(n).

---

## 17. LONGEST INCREASING SUBSEQUENCE

**Link:** https://leetcode.com/problems/longest-increasing-subsequence/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(n)

**Problem:** Find length of longest increasing subsequence.

**Most Optimized Solution (Binary Search):**

```python
class Solution:
    def lengthOfLIS(self, nums: list[int]) -> int:
        import bisect
        
        tails = []
        
        for num in nums:
            pos = bisect.bisect_left(tails, num)
            
            if pos == len(tails):
                tails.append(num)
            else:
                tails[pos] = num
        
        return len(tails)
```

**Explanation:** Maintain sorted array of smallest tails of increasing subsequences. For each number, use binary search to find position. Either add to end (longer subsequence) or replace (potentially better). Time: O(n log n), Space: O(n).

---

## 18. LONGEST COMMON SUBSEQUENCE

**Link:** https://leetcode.com/problems/longest-common-subsequence/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find length of longest common subsequence of two strings.

**Most Optimized Solution:**

```python
class Solution:
    def longestCommonSubsequence(self, text1: str, text2: str) -> int:
        m, n = len(text1), len(text2)
        dp = [[0] * (n + 1) for _ in range(m + 1)]
        
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                if text1[i - 1] == text2[j - 1]:
                    dp[i][j] = dp[i - 1][j - 1] + 1
                else:
                    dp[i][j] = max(dp[i - 1][j], dp[i][j - 1])
        
        return dp[m][n]
```

**Explanation:** DP: dp[i][j] = LCS length of first i chars of text1 and first j of text2. If chars match: dp[i][j] = dp[i-1][j-1] + 1. Else: dp[i][j] = max(dp[i-1][j], dp[i][j-1]). Time: O(m*n), Space: O(m*n).

---

## 19. WORD BREAK

**Link:** https://leetcode.com/problems/word-break/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(n)

**Problem:** Determine if string can be segmented using dictionary.

**Most Optimized Solution:**

```python
class Solution:
    def wordBreak(self, s: str, wordDict: list[str]) -> bool:
        word_set = set(wordDict)
        dp = [False] * (len(s) + 1)
        dp[0] = True
        
        for i in range(1, len(s) + 1):
            for j in range(i):
                if dp[j] and s[j:i] in word_set:
                    dp[i] = True
                    break
        
        return dp[len(s)]
```

**Explanation:** DP: dp[i] = can first i chars be segmented. For each position, check all substrings ending at position. If substring is in dict and dp[start] is true, dp[i] = true. Time: O(n²), Space: O(n).

---

## 20. COMBINATION SUM IV

**Link:** https://leetcode.com/problems/combination-sum-iv/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Find number of combinations that sum to target.

**Most Optimized Solution:**

```python
class Solution:
    def combinationSum4(self, nums: list[int], target: int) -> int:
        dp = [0] * (target + 1)
        dp[0] = 1
        
        for i in range(1, target + 1):
            for num in nums:
                if num <= i:
                    dp[i] += dp[i - num]
        
        return dp[target]
```

**Explanation:** DP: dp[i] = number of combinations to sum to i. For each amount, sum up combinations from all nums. dp[i] += dp[i - num]. Time: O(n*m), Space: O(n).

---

## 21. HOUSE ROBBER

**Link:** https://leetcode.com/problems/house-robber/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Rob houses to maximize money (can't rob adjacent houses).

**Most Optimized Solution:**

```python
class Solution:
    def rob(self, nums: list[int]) -> int:
        prev1, prev2 = 0, 0
        
        for num in nums:
            curr = max(prev1 + num, prev2)
            prev2 = prev1
            prev1 = curr
        
        return prev1
```

**Explanation:** DP: dp[i] = max money up to house i. Either rob current + max up to i-2, or skip current. dp[i] = max(dp[i-1], dp[i-2] + nums[i]). Optimize space: only track last two values. Time: O(n), Space: O(1).

---

## 22. HOUSE ROBBER II

**Link:** https://leetcode.com/problems/house-robber-ii/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Rob houses in circle (first and last adjacent).

**Most Optimized Solution:**

```python
class Solution:
    def rob(self, nums: list[int]) -> int:
        if len(nums) == 1:
            return nums[0]
        
        def rob_range(start, end):
            prev1, prev2 = 0, 0
            for i in range(start, end + 1):
                curr = max(prev1 + nums[i], prev2)
                prev2 = prev1
                prev1 = curr
            return prev1
        
        # Either rob houses [0, n-2] or [1, n-1]
        return max(rob_range(0, len(nums) - 2),
                   rob_range(1, len(nums) - 1))
```

**Explanation:** Houses arranged in circle (first and last are adjacent). Can't rob both first and last. Solve two scenarios: exclude first or exclude last. Take maximum of both. Time: O(n), Space: O(1).

---

## 23. DECODE WAYS

**Link:** https://leetcode.com/problems/decode-ways/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Count number of ways to decode string of digits.

**Most Optimized Solution:**

```python
class Solution:
    def numDecodings(self, s: str) -> int:
        if not s or s[0] == '0':
            return 0
        
        prev2, prev1 = 1, 1
        
        for i in range(1, len(s)):
            curr = 0
            
            # Single digit
            if s[i] != '0':
                curr += prev1
            
            # Two digits
            if s[i - 1] == '1' or (s[i - 1] == '2' and s[i] < '7'):
                curr += prev2
            
            prev2 = prev1
            prev1 = curr
        
        return prev1
```

**Explanation:** DP: ways[i] = ways to decode first i characters. Can decode single digit (1-9). Can decode two digits (10-26). dp[i] = dp[i-1] (if single valid) + dp[i-2] (if pair valid). Optimize space: track last two values. Time: O(n), Space: O(1).

---

## 24. COIN CHANGE 2

**Link:** https://leetcode.com/problems/coin-change-2/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Number of combinations to make target amount.

**Most Optimized Solution:**

```python
class Solution:
    def change(self, amount: int, coins: list[int]) -> int:
        dp = [0] * (amount + 1)
        dp[0] = 1
        
        for coin in coins:
            for i in range(coin, amount + 1):
                dp[i] += dp[i - coin]
        
        return dp[amount]
```

**Explanation:** DP: dp[i] = combinations to make amount i. Iterate through coins (not amounts) to avoid counting duplicates. For each coin, update all amounts it can contribute to. dp[i] += dp[i - coin]. Time: O(n*m), Space: O(n).

---

## 25. PARTITION EQUAL SUBSET SUM

**Link:** https://leetcode.com/problems/partition-equal-subset-sum/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(m)

**Problem:** Partition array into two equal sum subsets.

**Most Optimized Solution:**

```python
class Solution:
    def canPartition(self, nums: list[int]) -> bool:
        total = sum(nums)
        
        # Odd sum can't be partitioned
        if total % 2 != 0:
            return False
        
        target = total // 2
        dp = [False] * (target + 1)
        dp[0] = True
        
        for num in nums:
            for i in range(target, num - 1, -1):
                dp[i] = dp[i] or dp[i - num]
        
        return dp[target]
```

**Explanation:** Problem reduces to: find subset with sum = total/2. Use 0/1 knapsack DP. dp[i] = can we achieve sum i. Iterate backwards to avoid using same item twice. Time: O(n*m), Space: O(m).

---

## 26. LONGEST PALINDROMIC SUBSTRING

**Link:** https://leetcode.com/problems/longest-palindromic-substring/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Find longest palindromic substring.

**Most Optimized Solution (Expand Around Center):**

```python
class Solution:
    def longestPalindrome(self, s: str) -> str:
        if not s:
            return ""
        
        def expand_around_center(left, right):
            while left >= 0 and right < len(s) and s[left] == s[right]:
                left -= 1
                right += 1
            return left + 1, right - 1
        
        max_start, max_len = 0, 0
        
        for i in range(len(s)):
            # Odd length palindrome
            start1, end1 = expand_around_center(i, i)
            if end1 - start1 + 1 > max_len:
                max_start = start1
                max_len = end1 - start1 + 1
            
            # Even length palindrome
            start2, end2 = expand_around_center(i, i + 1)
            if end2 - start2 + 1 > max_len:
                max_start = start2
                max_len = end2 - start2 + 1
        
        return s[max_start:max_start + max_len]
```

**Explanation:** Expand around each center (single and double). Track longest palindrome found. Better than O(n³) DP approach. Time: O(n²), Space: O(1).

---

## 27. PALINDROMIC SUBSTRINGS

**Link:** https://leetcode.com/problems/palindromic-substrings/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Count number of palindromic substrings.

**Most Optimized Solution:**

```python
class Solution:
    def countSubstrings(self, s: str) -> int:
        def expand_around_center(left, right):
            count = 0
            while left >= 0 and right < len(s) and s[left] == s[right]:
                count += 1
                left -= 1
                right += 1
            return count
        
        count = 0
        for i in range(len(s)):
            count += expand_around_center(i, i)      # Odd length
            count += expand_around_center(i, i + 1)  # Even length
        
        return count
```

**Explanation:** Expand around each center. Count palindromes found at each expansion. Time: O(n²), Space: O(1).

---

## 28. NUMBER OF LONGEST INCREASING SUBSEQUENCE

**Link:** https://leetcode.com/problems/number-of-longest-increasing-subsequence/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(n)

**Problem:** Count number of longest increasing subsequences.

**Most Optimized Solution:**

```python
class Solution:
    def findNumberOfLIS(self, nums: list[int]) -> int:
        n = len(nums)
        length = [1] * n  # LIS length ending at i
        count = [1] * n   # Number of LIS ending at i
        
        for i in range(1, n):
            for j in range(i):
                if nums[j] < nums[i]:
                    if length[j] + 1 > length[i]:
                        length[i] = length[j] + 1
                        count[i] = count[j]
                    elif length[j] + 1 == length[i]:
                        count[i] += count[j]
        
        max_len = max(length)
        result = 0
        
        for i in range(n):
            if length[i] == max_len:
                result += count[i]
        
        return result
```

**Explanation:** Track LIS length ending at each position. Track count of LIS with that length. When extending, update count based on whether we found longer or equal. Sum counts where length equals maximum LIS length. Time: O(n²), Space: O(n).

---

## 29. MAXIMAL SQUARE

**Link:** https://leetcode.com/problems/maximal-square/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find largest square of 1s in matrix.

**Most Optimized Solution:**

```python
class Solution:
    def maximalSquare(self, matrix: list[list[str]]) -> int:
        if not matrix:
            return 0
        
        m, n = len(matrix), len(matrix[0])
        dp = [[0] * n for _ in range(m)]
        max_side = 0
        
        for i in range(m):
            for j in range(n):
                if matrix[i][j] == '1':
                    if i == 0 or j == 0:
                        dp[i][j] = 1
                    else:
                        dp[i][j] = min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]) + 1
                    max_side = max(max_side, dp[i][j])
        
        return max_side * max_side
```

**Explanation:** DP: dp[i][j] = side length of square with bottom-right at (i,j). If current is '1': dp[i][j] = min(top, left, diagonal) + 1. This ensures we can form a square. Time: O(m*n), Space: O(m*n).

---

## 30. NUMBER OF ISLANDS

**Link:** https://leetcode.com/problems/number-of-islands/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Count number of islands (connected 1s).

**Most Optimized Solution (DFS):**

```python
class Solution:
    def numIslands(self, grid: list[list[str]]) -> int:
        def dfs(i, j):
            if i < 0 or i >= len(grid) or j < 0 or j >= len(grid[0]) or \
               grid[i][j] != '1':
                return
            
            grid[i][j] = '0'  # Mark as visited
            
            dfs(i + 1, j)
            dfs(i - 1, j)
            dfs(i, j + 1)
            dfs(i, j - 1)
        
        count = 0
        for i in range(len(grid)):
            for j in range(len(grid[0])):
                if grid[i][j] == '1':
                    dfs(i, j)
                    count += 1
        
        return count
```

**BFS Alternative:**

```python
from collections import deque

def numIslands(self, grid: list[list[str]]) -> int:
    if not grid:
        return 0
    
    count = 0
    for i in range(len(grid)):
        for j in range(len(grid[0])):
            if grid[i][j] == '1':
                self.bfs(grid, i, j)
                count += 1
    
    return count

def bfs(self, grid, i, j):
    queue = deque([(i, j)])
    grid[i][j] = '0'
    
    while queue:
        i, j = queue.popleft()
        for di, dj in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
            ni, nj = i + di, j + dj
            if 0 <= ni < len(grid) and 0 <= nj < len(grid[0]) and grid[ni][nj] == '1':
                grid[ni][nj] = '0'
                queue.append((ni, nj))
```

**Explanation:** Use DFS to mark connected 1s. Each DFS call finds one complete island. Count number of DFS calls. Time: O(m*n), Space: O(m*n) for recursion or queue.

---

## 31. CLONE GRAPH

**Link:** https://leetcode.com/problems/clone-graph/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Deep copy an undirected graph.

**Most Optimized Solution (BFS):**

```python
from collections import deque

class Solution:
    def cloneGraph(self, node: 'Node') -> 'Node':
        if not node:
            return None
        
        cloned = {node: Node(node.val)}
        queue = deque([node])
        
        while queue:
            curr = queue.popleft()
            
            for neighbor in curr.neighbors:
                if neighbor not in cloned:
                    cloned[neighbor] = Node(neighbor.val)
                    queue.append(neighbor)
                
                cloned[curr].neighbors.append(cloned[neighbor])
        
        return cloned[node]
```

**Explanation:** Use BFS with hash map to track cloned nodes. Create new node when first encountering. Connect cloned nodes based on original edges. Time: O(n+e) where n=nodes, e=edges. Space: O(n) for hash map.

---

## 32. COURSE SCHEDULE

**Link:** https://leetcode.com/problems/course-schedule/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Detect cycle in directed graph (course prerequisites).

**Most Optimized Solution (Topological Sort):**

```python
from collections import deque

class Solution:
    def canFinish(self, numCourses: int, prerequisites: list[list[int]]) -> bool:
        # Build graph and calculate indegrees
        indegree = [0] * numCourses
        graph = [[] for _ in range(numCourses)]
        
        for course, prereq in prerequisites:
            graph[prereq].append(course)
            indegree[course] += 1
        
        # Start with courses that have no prerequisites
        queue = deque([i for i in range(numCourses) if indegree[i] == 0])
        count = 0
        
        while queue:
            course = queue.popleft()
            count += 1
            
            for next_course in graph[course]:
                indegree[next_course] -= 1
                if indegree[next_course] == 0:
                    queue.append(next_course)
        
        return count == numCourses
```

**Explanation:** Build directed graph of course dependencies. Use topological sort with indegree. If all courses can be sorted, no cycle exists. Time: O(n+e), Space: O(n).

---

## 33. COURSE SCHEDULE II

**Link:** https://leetcode.com/problems/course-schedule-ii/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Return course order if possible, else empty array.

**Most Optimized Solution:**

```python
from collections import deque

class Solution:
    def findOrder(self, numCourses: int, prerequisites: list[list[int]]) -> list[int]:
        indegree = [0] * numCourses
        graph = [[] for _ in range(numCourses)]
        
        for course, prereq in prerequisites:
            graph[prereq].append(course)
            indegree[course] += 1
        
        queue = deque([i for i in range(numCourses) if indegree[i] == 0])
        result = []
        
        while queue:
            course = queue.popleft()
            result.append(course)
            
            for next_course in graph[course]:
                indegree[next_course] -= 1
                if indegree[next_course] == 0:
                    queue.append(next_course)
        
        return result if len(result) == numCourses else []
```

**Explanation:** Topological sort returns valid course order. If all courses included, return order; else empty. Time: O(n+e), Space: O(n).

---

## 34. ALIEN DICTIONARY

**Link:** https://leetcode.com/problems/alien-dictionary/

**Difficulty:** Hard | **Time:** O(n*l+k) | **Space:** O(k)

**Problem:** Order of alien alphabet from sorted words.

**Most Optimized Solution:**

```python
from collections import deque, defaultdict

class Solution:
    def alienOrder(self, words: list[str]) -> str:
        graph = defaultdict(set)
        indegree = {char: 0 for word in words for char in word}
        
        # Build graph
        for i in range(len(words) - 1):
            w1, w2 = words[i], words[i + 1]
            min_len = min(len(w1), len(w2))
            
            for j in range(min_len):
                if w1[j] != w2[j]:
                    if w2[j] not in graph[w1[j]]:
                        graph[w1[j]].add(w2[j])
                        indegree[w2[j]] += 1
                    break
        
        # Topological sort
        queue = deque([char for char in indegree if indegree[char] == 0])
        result = []
        
        while queue:
            char = queue.popleft()
            result.append(char)
            
            for neighbor in graph[char]:
                indegree[neighbor] -= 1
                if indegree[neighbor] == 0:
                    queue.append(neighbor)
        
        return "".join(result) if len(result) == len(indegree) else ""
```

**Explanation:** Compare adjacent words to find order constraints. Build directed graph and use topological sort. Time: O(n*l+k), Space: O(k) where k=alphabet size.

---

## 35. GRAPH VALID TREE

**Link:** https://leetcode.com/problems/graph-valid-tree/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Check if graph is valid tree (no cycle, connected).

**Most Optimized Solution (Union-Find):**

```python
class Solution:
    def validTree(self, n: int, edges: list[list[int]]) -> bool:
        if len(edges) != n - 1:
            return False  # Tree has n-1 edges
        
        parent = list(range(n))
        
        def find(x):
            if parent[x] != x:
                parent[x] = find(parent[x])
            return parent[x]
        
        for u, v in edges:
            root_u = find(u)
            root_v = find(v)
            
            if root_u == root_v:
                return False  # Cycle detected
            
            parent[root_u] = root_v
        
        return True
```

**Explanation:** Tree has exactly n-1 edges. Use Union-Find to detect cycles. If two vertices already have same root, edge creates cycle. Time: O(n+e), Space: O(n).

---

## 36. NUMBER OF CONNECTED COMPONENTS IN AN UNDIRECTED GRAPH

**Link:** https://leetcode.com/problems/number-of-connected-components-in-an-undirected-graph/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Count connected components.

**Most Optimized Solution (Union-Find):**

```python
class Solution:
    def countComponents(self, n: int, edges: list[list[int]]) -> int:
        parent = list(range(n))
        
        def find(x):
            if parent[x] != x:
                parent[x] = find(parent[x])
            return parent[x]
        
        components = n
        for u, v in edges:
            root_u = find(u)
            root_v = find(v)
            
            if root_u != root_v:
                parent[root_u] = root_v
                components -= 1
        
        return components
```

**Explanation:** Start with n components (each node separate). For each edge, if nodes are in different components, merge them. Decrease component count when merging. Time: O(n+e), Space: O(n).

---

## 37. LONGEST CONSECUTIVE

**Link:** https://leetcode.com/problems/longest-consecutive/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Find length of longest consecutive elements sequence.

**Most Optimized Solution:**

```python
class Solution:
    def longestConsecutive(self, nums: list[int]) -> int:
        if not nums:
            return 0
        
        num_set = set(nums)
        max_len = 0
        
        for num in num_set:
            # Only start counting from sequence start
            if num - 1 not in num_set:
                length = 1
                while num + length in num_set:
                    length += 1
                max_len = max(max_len, length)
        
        return max_len
```

**Explanation:** Use set for O(1) lookup. Only start counting from sequence beginning (num-1 doesn't exist). For each sequence start, count length. Time: O(n), Space: O(n).

---

## 38. PACIFIC ATLANTIC WATER FLOW

**Link:** https://leetcode.com/problems/pacific-atlantic-water-flow/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find cells where water flows to both oceans.

**Most Optimized Solution (Reverse DFS):**

```python
class Solution:
    def pacificAtlantic(self, heights: list[list[int]]) -> list[list[int]]:
        if not heights:
            return []
        
        m, n = len(heights), len(heights[0])
        pacific = set()
        atlantic = set()
        
        def dfs(i, j, visited, prev_height):
            if i < 0 or i >= m or j < 0 or j >= n or \
               (i, j) in visited or heights[i][j] < prev_height:
                return
            
            visited.add((i, j))
            
            dfs(i + 1, j, visited, heights[i][j])
            dfs(i - 1, j, visited, heights[i][j])
            dfs(i, j + 1, visited, heights[i][j])
            dfs(i, j - 1, visited, heights[i][j])
        
        # Start from borders
        for i in range(m):
            dfs(i, 0, pacific, 0)
            dfs(i, n - 1, atlantic, 0)
        
        for j in range(n):
            dfs(0, j, pacific, 0)
            dfs(m - 1, j, atlantic, 0)
        
        return list(pacific & atlantic)
```

**Explanation:** Reverse approach: start from oceans, find cells reachable. Water flows to ocean if path has non-increasing heights. Find cells reachable from both oceans. Time: O(m*n), Space: O(m*n).

---

## 39. WALLS AND GATES

**Link:** https://leetcode.com/problems/walls-and-gates/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Fill distances from gates to empty rooms.

**Most Optimized Solution (Multi-source BFS):**

```python
from collections import deque

class Solution:
    def wallsAndGates(self, rooms: list[list[int]]) -> None:
        """Modify rooms in-place."""
        if not rooms:
            return
        
        m, n = len(rooms), len(rooms[0])
        queue = deque()
        
        # Add all gates to queue
        for i in range(m):
            for j in range(n):
                if rooms[i][j] == 0:
                    queue.append((i, j))
        
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]
        
        while queue:
            x, y = queue.popleft()
            
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                
                if 0 <= nx < m and 0 <= ny < n and \
                   rooms[nx][ny] > rooms[x][y] + 1:
                    rooms[nx][ny] = rooms[x][y] + 1
                    queue.append((nx, ny))
```

**Explanation:** Multi-source BFS starting from all gates. Breadth-first ensures shortest distance. Update room distance if shorter path found. Time: O(m*n), Space: O(m*n).

---

## 40. ROTTING ORANGES

**Link:** https://leetcode.com/problems/rotting-oranges/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find time until all fresh oranges rot.

**Most Optimized Solution (Multi-source BFS):**

```python
from collections import deque

class Solution:
    def orangesRotting(self, grid: list[list[int]]) -> int:
        m, n = len(grid), len(grid[0])
        queue = deque()
        fresh = 0
        
        # Add all rotten oranges to queue
        for i in range(m):
            for j in range(n):
                if grid[i][j] == 2:
                    queue.append((i, j))
                elif grid[i][j] == 1:
                    fresh += 1
        
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]
        minutes = 0
        
        while queue and fresh > 0:
            minutes += 1
            size = len(queue)
            
            for _ in range(size):
                x, y = queue.popleft()
                
                for dx, dy in directions:
                    nx, ny = x + dx, y + dy
                    
                    if 0 <= nx < m and 0 <= ny < n and grid[nx][ny] == 1:
                        grid[nx][ny] = 2
                        fresh -= 1
                        queue.append((nx, ny))
        
        return minutes if fresh == 0 else -1
```

**Explanation:** Multi-source BFS from all rotten oranges. Each level represents one minute. Count fresh oranges, decrease when rotting. Return minutes when all fresh are rotten. Time: O(m*n), Space: O(m*n).

---

## 41. INSERT INTERVAL

**Link:** https://leetcode.com/problems/insert-interval/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Insert interval into list of non-overlapping intervals.

**Most Optimized Solution:**

```python
class Solution:
    def insert(self, intervals: list[list[int]], newInterval: list[int]) -> list[list[int]]:
        result = []
        i = 0
        n = len(intervals)
        
        # Add intervals that end before new interval starts
        while i < n and intervals[i][1] < newInterval[0]:
            result.append(intervals[i])
            i += 1
        
        # Merge overlapping intervals
        while i < n and intervals[i][0] <= newInterval[1]:
            newInterval[0] = min(newInterval[0], intervals[i][0])
            newInterval[1] = max(newInterval[1], intervals[i][1])
            i += 1
        
        result.append(newInterval)
        
        # Add remaining intervals
        while i < n:
            result.append(intervals[i])
            i += 1
        
        return result
```

**Explanation:** Add non-overlapping intervals before new interval. Merge all overlapping intervals with new interval. Add remaining non-overlapping intervals. Time: O(n), Space: O(n).

---

## 42. MERGE INTERVALS

**Link:** https://leetcode.com/problems/merge-intervals/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Merge overlapping intervals.

**Most Optimized Solution:**

```python
class Solution:
    def merge(self, intervals: list[list[int]]) -> list[list[int]]:
        intervals.sort()
        
        result = [intervals[0]]
        
        for i in range(1, len(intervals)):
            if intervals[i][0] <= result[-1][1]:
                result[-1][1] = max(result[-1][1], intervals[i][1])
            else:
                result.append(intervals[i])
        
        return result
```

**Explanation:** Sort intervals by start time. Merge if current start <= previous end. Update end to max of two ends. Time: O(n log n), Space: O(1).

---

## 43. NON-OVERLAPPING INTERVALS

**Link:** https://leetcode.com/problems/non-overlapping-intervals/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Remove minimum intervals to make non-overlapping.

**Most Optimized Solution (Greedy):**

```python
class Solution:
    def eraseOverlapIntervals(self, intervals: list[list[int]]) -> int:
        intervals.sort(key=lambda x: x[1])  # Sort by end time
        
        removed = 0
        prev_end = float('-inf')
        
        for start, end in intervals:
            if start < prev_end:
                removed += 1  # Overlaps, remove this one
            else:
                prev_end = end  # Update end time
        
        return removed
```

**Explanation:** Greedy approach: keep intervals with earliest end times. Sort by end time, not start. If current start < previous end, remove current. Time: O(n log n), Space: O(1).

---

## 44. MEETING ROOMS

**Link:** https://leetcode.com/problems/meeting-rooms/

**Difficulty:** Easy | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Check if person can attend all meetings (non-overlapping).

**Most Optimized Solution:**

```python
class Solution:
    def canAttendMeetings(self, intervals: list[list[int]]) -> bool:
        intervals.sort()
        
        for i in range(1, len(intervals)):
            if intervals[i][0] < intervals[i - 1][1]:
                return False
        
        return True
```

**Explanation:** Sort by start time. Check if any meetings overlap. If sorted meeting starts before previous ends, overlap exists. Time: O(n log n), Space: O(1).

---

## 45. MEETING ROOMS II

**Link:** https://leetcode.com/problems/meeting-rooms-ii/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(n)

**Problem:** Minimum conference rooms needed.

**Most Optimized Solution (Sweep Line):**

```python
class Solution:
    def minMeetingRooms(self, intervals: list[list[int]]) -> int:
        events = []
        
        for start, end in intervals:
            events.append((start, 1))    # Start: +1 room
            events.append((end, -1))     # End: -1 room
        
        events.sort()
        
        rooms = 0
        max_rooms = 0
        
        for time, event_type in events:
            rooms += event_type
            max_rooms = max(max_rooms, rooms)
        
        return max_rooms
```

**Explanation:** Create events: +1 for start, -1 for end. Sort events by time. Track current rooms needed. Max rooms needed at any time is answer. Time: O(n log n), Space: O(n).

---

## 46. REVERSE LINKED LIST

**Link:** https://leetcode.com/problems/reverse-linked-list/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Reverse a singly linked list.

**Most Optimized Solution (Iterative):**

```python
class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        prev = None
        curr = head
        
        while curr:
            next_node = curr.next  # Save next
            curr.next = prev       # Reverse link
            prev = curr            # Move prev
            curr = next_node        # Move curr
        
        return prev
```

**Recursive Alternative:**

```python
def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
    if not head or not head.next:
        return head
    
    new_head = self.reverseList(head.next)
    head.next.next = head
    head.next = None
    
    return new_head
```

**Explanation:** Three pointers: prev, curr, next. Iterate through list, reversing links. Time: O(n), Space: O(1).

---

## 47. DETECT CYCLE IN LINKED LIST

**Link:** https://leetcode.com/problems/linked-list-cycle/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Detect if linked list has cycle.

**Most Optimized Solution (Floyd's Algorithm):**

```python
class Solution:
    def hasCycle(self, head: Optional[ListNode]) -> bool:
        if not head or not head.next:
            return False
        
        slow = fast = head
        
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next
            
            if slow == fast:
                return True
        
        return False
```

**Explanation:** Floyd's cycle detection: slow moves 1 step, fast moves 2. If cycle exists, they eventually meet. Time: O(n), Space: O(1).

---

## 48. MERGE TWO SORTED LISTS

**Link:** https://leetcode.com/problems/merge-two-sorted-lists/

**Difficulty:** Easy | **Time:** O(n+m) | **Space:** O(1)

**Problem:** Merge two sorted lists into one.

**Most Optimized Solution:**

```python
class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        dummy = ListNode(0)
        curr = dummy
        
        while list1 and list2:
            if list1.val <= list2.val:
                curr.next = list1
                list1 = list1.next
            else:
                curr.next = list2
                list2 = list2.next
            curr = curr.next
        
        curr.next = list1 if list1 else list2
        
        return dummy.next
```

**Explanation:** Use dummy node to simplify logic. Compare heads of both lists. Attach smaller one to result. Attach remaining list when one is exhausted. Time: O(n+m), Space: O(1).

---

## 49. MERGE K SORTED LISTS

**Link:** https://leetcode.com/problems/merge-k-sorted-lists/

**Difficulty:** Hard | **Time:** O(n log k) | **Space:** O(k)

**Problem:** Merge k sorted lists.

**Most Optimized Solution (Min Heap):**

```python
import heapq

class Solution:
    def mergeKLists(self, lists: list[Optional[ListNode]]) -> Optional[ListNode]:
        min_heap = []
        
        # Add first node of each list
        for i, lst in enumerate(lists):
            if lst:
                heapq.heappush(min_heap, (lst.val, i, lst))
        
        dummy = ListNode(0)
        curr = dummy
        
        while min_heap:
            val, idx, node = heapq.heappop(min_heap)
            curr.next = node
            curr = curr.next
            
            if node.next:
                heapq.heappush(min_heap, (node.next.val, idx, node.next))
        
        return dummy.next
```

**Explanation:** Use min heap to track smallest nodes. Always pop smallest, add its next. Continue until heap empty. Time: O(n log k) where n=total nodes, k=lists. Space: O(k) for heap.

---

## 50. REMOVE NTH NODE FROM END OF LIST

**Link:** https://leetcode.com/problems/remove-nth-node-from-end-of-list/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Remove n-th node from end of list.

**Most Optimized Solution (Two Pointers):**

```python
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        dummy = ListNode(0)
        dummy.next = head
        
        first = dummy
        second = dummy
        
        # Move first n+1 steps ahead
        for i in range(n + 1):
            first = first.next
        
        # Move both until first reaches end
        while first:
            first = first.next
            second = second.next
        
        second.next = second.next.next
        
        return dummy.next
```

**Explanation:** Use dummy node to handle head removal. Create two pointers n+1 steps apart. Move both until first reaches end. Remove node by skipping it. Time: O(n), Space: O(1).

---

## 51. REORDER LIST

**Link:** https://leetcode.com/problems/reorder-list/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Reorder list: L0->L1->...->Ln to L0->Ln->L1->Ln-1...

**Most Optimized Solution:**

```python
class Solution:
    def reorderList(self, head: Optional[ListNode]) -> None:
        if not head or not head.next:
            return
        
        # Find middle
        slow = fast = head
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next
        
        # Reverse second half
        prev = None
        while slow:
            next_node = slow.next
            slow.next = prev
            prev = slow
            slow = next_node
        
        # Merge two halves
        l1, l2 = head, prev
        while l2.next:
            next1 = l1.next
            next2 = l2.next
            
            l1.next = l2
            l2.next = next1
            
            l1 = next1
            l2 = next2
```

**Explanation:** Find middle of list. Reverse second half. Merge two halves alternately. Time: O(n), Space: O(1).

---

## 52. SET MATRIX ZEROES

**Link:** https://leetcode.com/problems/set-matrix-zeroes/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Set entire row/column to 0 if element is 0.

**Most Optimized Solution:**

```python
class Solution:
    def setZeroes(self, matrix: list[list[int]]) -> None:
        m, n = len(matrix), len(matrix[0])
        row_zero = col_zero = False
        
        # Check if first row/col need zeroing
        for i in range(m):
            if matrix[i][0] == 0:
                col_zero = True
        
        for j in range(n):
            if matrix[0][j] == 0:
                row_zero = True
        
        # Mark zeros in first row/col
        for i in range(1, m):
            for j in range(1, n):
                if matrix[i][j] == 0:
                    matrix[i][0] = 0
                    matrix[0][j] = 0
        
        # Set zeroes except first row/col
        for i in range(1, m):
            for j in range(1, n):
                if matrix[i][0] == 0 or matrix[0][j] == 0:
                    matrix[i][j] = 0
        
        # Set first row/col
        if row_zero:
            for j in range(n):
                matrix[0][j] = 0
        
        if col_zero:
            for i in range(m):
                matrix[i][0] = 0
```

**Explanation:** Use first row/col as markers. Record if first row/col should be zeroed. Mark zeros in first row/col for affected rows/cols. Apply marks to rest of matrix. Zero out first row/col if needed. Time: O(m*n), Space: O(1).

---

## 53. SPIRAL MATRIX

**Link:** https://leetcode.com/problems/spiral-matrix/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Return elements in spiral order.

**Most Optimized Solution:**

```python
class Solution:
    def spiralOrder(self, matrix: list[list[int]]) -> list[int]:
        if not matrix:
            return []
        
        result = []
        top, bottom = 0, len(matrix) - 1
        left, right = 0, len(matrix[0]) - 1
        
        while top <= bottom and left <= right:
            # Right
            for j in range(left, right + 1):
                result.append(matrix[top][j])
            top += 1
            
            # Down
            for i in range(top, bottom + 1):
                result.append(matrix[i][right])
            right -= 1
            
            # Left
            if top <= bottom:
                for j in range(right, left - 1, -1):
                    result.append(matrix[bottom][j])
                bottom -= 1
            
            # Up
            if left <= right:
                for i in range(bottom, top - 1, -1):
                    result.append(matrix[i][left])
                left += 1
        
        return result
```

**Explanation:** Track boundaries: top, bottom, left, right. Traverse right, down, left, up in spiral. Shrink boundaries after each direction. Time: O(m*n), Space: O(1).

---

## 54. ROTATE MATRIX

**Link:** https://leetcode.com/problems/rotate-image/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Rotate matrix 90 degrees clockwise in-place.

**Most Optimized Solution:**

```python
class Solution:
    def rotate(self, matrix: list[list[int]]) -> None:
        n = len(matrix)
        
        # Transpose
        for i in range(n):
            for j in range(i + 1, n):
                matrix[i][j], matrix[j][i] = matrix[j][i], matrix[i][j]
        
        # Reverse each row
        for i in range(n):
            matrix[i].reverse()
```

**Explanation:** Rotation = Transpose + Reverse each row. Transpose swaps elements across diagonal. Reverse each row to complete 90° rotation. Time: O(n²), Space: O(1).

---

## 55. WORD SEARCH

**Link:** https://leetcode.com/problems/word-search/

**Difficulty:** Medium | **Time:** O(m*n*4^l) | **Space:** O(l)

**Problem:** Search for word in grid (backtracking).

**Most Optimized Solution:**

```python
class Solution:
    def exist(self, board: list[list[str]], word: str) -> bool:
        def dfs(i, j, idx):
            if idx == len(word):
                return True
            
            if i < 0 or i >= len(board) or j < 0 or j >= len(board[0]) or \
               board[i][j] != word[idx]:
                return False
            
            board[i][j] = '#'  # Mark as visited
            
            found = dfs(i + 1, j, idx + 1) or \
                    dfs(i - 1, j, idx + 1) or \
                    dfs(i, j + 1, idx + 1) or \
                    dfs(i, j - 1, idx + 1)
            
            board[i][j] = word[idx]  # Restore
            
            return found
        
        for i in range(len(board)):
            for j in range(len(board[0])):
                if board[i][j] == word[0] and dfs(i, j, 0):
                    return True
        
        return False
```

**Explanation:** DFS backtracking from each cell. Mark visited cells to avoid reuse. Explore all four directions. Time: O(m*n*4^l), Space: O(l).

---

## 56. LONGEST SUBSTRING WITHOUT REPEATING CHARACTERS

**Link:** https://leetcode.com/problems/longest-substring-without-repeating-characters/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(min(n, m))

**Problem:** Find length of longest substring without repeating characters.

**Most Optimized Solution (Sliding Window):**

```python
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        char_index = {}
        max_len = 0
        start = 0
        
        for end, c in enumerate(s):
            if c in char_index:
                start = max(start, char_index[c] + 1)
            
            char_index[c] = end
            max_len = max(max_len, end - start + 1)
        
        return max_len
```

**Explanation:** Sliding window approach. Track last index of each character. When duplicate found, move start to after previous occurrence. Time: O(n), Space: O(min(n, m)).

---

## 57. LONGEST REPEATING CHARACTER REPLACEMENT

**Link:** https://leetcode.com/problems/longest-repeating-character-replacement/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Replace at most k characters to get longest repeating substring.

**Most Optimized Solution (Sliding Window):**

```python
class Solution:
    def characterReplacement(self, s: str, k: int) -> int:
        char_count = {}
        max_freq = 0
        max_len = 0
        start = 0
        
        for end in range(len(s)):
            char_count[s[end]] = char_count.get(s[end], 0) + 1
            max_freq = max(max_freq, char_count[s[end]])
            
            # If replacements needed > k, shrink window
            if end - start + 1 - max_freq > k:
                char_count[s[start]] -= 1
                start += 1
            
            max_len = max(max_len, end - start + 1)
        
        return max_len
```

**Explanation:** Sliding window with character frequency. max_freq = most frequent character in window. If (window_size - max_freq) > k, shrink window. Replacements needed = window_size - max_freq. Time: O(n), Space: O(1).

---

## 58. MINIMUM WINDOW SUBSTRING

**Link:** https://leetcode.com/problems/minimum-window-substring/

**Difficulty:** Hard | **Time:** O(n+m) | **Space:** O(1)

**Problem:** Find minimum window containing all characters from t.

**Most Optimized Solution (Sliding Window):**

```python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        if len(t) > len(s):
            return ""
        
        from collections import Counter
        
        t_count = Counter(t)
        window_count = {}
        
        formed = 0
        required = len(t_count)
        left = 0
        min_len = float('inf')
        min_left = 0
        
        for right in range(len(s)):
            c = s[right]
            window_count[c] = window_count.get(c, 0) + 1
            
            if c in t_count and window_count[c] == t_count[c]:
                formed += 1
            
            while left <= right and formed == required:
                if right - left + 1 < min_len:
                    min_len = right - left + 1
                    min_left = left
                
                c = s[left]
                window_count[c] -= 1
                if c in t_count and window_count[c] < t_count[c]:
                    formed -= 1
                
                left += 1
        
        return "" if min_len == float('inf') else s[min_left:min_left + min_len]
```

**Explanation:** Sliding window with two pointers. Expand right to include all characters from t. Contract left while maintaining validity. Track minimum window. Time: O(n+m), Space: O(1).

---

## 59. VALID ANAGRAM

**Link:** https://leetcode.com/problems/valid-anagram/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Check if two strings are anagrams.

**Most Optimized Solution:**

```python
class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        
        char_count = {}
        
        for i in range(len(s)):
            char_count[s[i]] = char_count.get(s[i], 0) + 1
            char_count[t[i]] = char_count.get(t[i], 0) - 1
        
        return all(count == 0 for count in char_count.values())
```

**One-liner:**

```python
def isAnagram(self, s: str, t: str) -> bool:
    return sorted(s) == sorted(t)
```

**Explanation:** Count characters. Increment for s, decrement for t. If all counts are 0, strings are anagrams. Time: O(n), Space: O(1).

---

## 60. GROUP ANAGRAMS

**Link:** https://leetcode.com/problems/group-anagrams/

**Difficulty:** Medium | **Time:** O(n*k log k) | **Space:** O(n*k)

**Problem:** Group anagrams together.

**Most Optimized Solution:**

```python
class Solution:
    def groupAnagrams(self, strs: list[str]) -> list[list[str]]:
        groups = {}
        
        for s in strs:
            key = tuple(sorted(s))
            if key not in groups:
                groups[key] = []
            groups[key].append(s)
        
        return list(groups.values())
```

**Explanation:** Sort each string to get canonical form. Use sorted form as key in hash map. Group anagrams have same sorted form. Time: O(n*k log k), Space: O(n*k).

---

## 61. VALID PARENTHESES

**Link:** https://leetcode.com/problems/valid-parentheses/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(n)

**Problem:** Check if parentheses are valid and balanced.

**Most Optimized Solution:**

```python
class Solution:
    def isValid(self, s: str) -> bool:
        stack = []
        pairs = {')': '(', ']': '[', '}': '{'}
        
        for c in s:
            if c in pairs:
                if not stack or stack[-1] != pairs[c]:
                    return False
                stack.pop()
            else:
                stack.append(c)
        
        return len(stack) == 0
```

**Explanation:** Push opening brackets. When closing bracket found, check if matches top. At end, stack must be empty. Time: O(n), Space: O(n).

---

## 62. VALID PALINDROME

**Link:** https://leetcode.com/problems/valid-palindrome/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Check if alphanumeric characters form palindrome.

**Most Optimized Solution:**

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

**Pythonic Alternative:**

```python
def isPalindrome(self, s: str) -> bool:
    s = ''.join(c.lower() for c in s if c.isalnum())
    return s == s[::-1]
```

**Explanation:** Two pointers from both ends. Skip non-alphanumeric characters. Compare lowercase versions. Time: O(n), Space: O(1).

---

## 63. LONGEST PALINDROMIC SUBSTRING (Duplicate)

See Problem 26.

---

## 64. ENCODE AND DECODE STRINGS

**Link:** https://leetcode.com/problems/encode-and-decode-strings/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Serialize list of strings and deserialize.

**Most Optimized Solution:**

```python
class Codec:
    def encode(self, strs: list[str]) -> str:
        """Encodes a list of strings to a single string."""
        result = []
        for s in strs:
            result.append(f"{len(s)}#{s}")
        return "".join(result)

    def decode(self, s: str) -> list[str]:
        """Decodes a single string to a list of strings."""
        result = []
        i = 0
        while i < len(s):
            j = i
            while s[j] != '#':
                j += 1
            length = int(s[i:j])
            result.append(s[j+1:j+1+length])
            i = j + 1 + length
        return result
```

**Explanation:** Encode with length prefix. Decode: read length, extract string. "#" acts as delimiter. Time: O(n), Space: O(n).

---

## 65. BINARY TREE MAXIMUM PATH SUM

**Link:** https://leetcode.com/problems/binary-tree-maximum-path-sum/

**Difficulty:** Hard | **Time:** O(n) | **Space:** O(h)

**Problem:** Find maximum path sum in binary tree.

**Most Optimized Solution (DFS):**

```python
class Solution:
    def maxPathSum(self, root: Optional[TreeNode]) -> int:
        self.max_sum = float('-inf')
        
        def max_gain(node):
            if not node:
                return 0
            
            left_gain = max(0, max_gain(node.left))
            right_gain = max(0, max_gain(node.right))
            
            path_sum = node.val + left_gain + right_gain
            self.max_sum = max(self.max_sum, path_sum)
            
            return node.val + max(left_gain, right_gain)
        
        max_gain(root)
        return self.max_sum
```

**Explanation:** DFS post-order traversal. Calculate max gain at each node. Max gain = node value + max of left/right gains. Path sum at node = value + both gains. Track maximum path sum. Time: O(n), Space: O(h).

---

## 66. BINARY TREE LEVEL ORDER TRAVERSAL

**Link:** https://leetcode.com/problems/binary-tree-level-order-traversal/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(w)

**Problem:** Return level-by-level traversal of tree.

**Most Optimized Solution (BFS):**

```python
from collections import deque

class Solution:
    def levelOrder(self, root: Optional[TreeNode]) -> list[list[int]]:
        if not root:
            return []
        
        result = []
        queue = deque([root])
        
        while queue:
            level = []
            for _ in range(len(queue)):
                node = queue.popleft()
                level.append(node.val)
                
                if node.left:
                    queue.append(node.left)
                if node.right:
                    queue.append(node.right)
            
            result.append(level)
        
        return result
```

**Explanation:** BFS with queue. Process all nodes at current level before next. Track level size to know when level ends. Time: O(n), Space: O(w) where w=max width.

---

## 67. SERIALIZE AND DESERIALIZE BINARY TREE

**Link:** https://leetcode.com/problems/serialize-and-deserialize-binary-tree/

**Difficulty:** Hard | **Time:** O(n) | **Space:** O(n)

**Problem:** Serialize and deserialize binary tree.

**Most Optimized Solution (Pre-order):**

```python
class Codec:
    def serialize(self, root: Optional[TreeNode]) -> str:
        """Encodes a tree to a single string."""
        result = []
        
        def preorder(node):
            if not node:
                result.append('null')
                return
            
            result.append(str(node.val))
            preorder(node.left)
            preorder(node.right)
        
        preorder(root)
        return ','.join(result)

    def deserialize(self, data: str) -> Optional[TreeNode]:
        """Decodes your encoded data to tree."""
        nodes = data.split(',')
        self.idx = 0
        
        def preorder_build():
            if nodes[self.idx] == 'null':
                self.idx += 1
                return None
            
            node = TreeNode(int(nodes[self.idx]))
            self.idx += 1
            
            node.left = preorder_build()
            node.right = preorder_build()
            
            return node
        
        return preorder_build()
```

**Explanation:** Pre-order traversal for serialization. Use "null" for empty nodes. Deserialize using pre-order reconstruction. Time: O(n), Space: O(n).

---

## 68. SUBTREE OF ANOTHER TREE

**Link:** https://leetcode.com/problems/subtree-of-another-tree/

**Difficulty:** Easy | **Time:** O(n*m) | **Space:** O(min(h1, h2))

**Problem:** Check if tree1 contains tree2 as subtree.

**Most Optimized Solution:**

```python
class Solution:
    def isSubtree(self, root: Optional[TreeNode], subRoot: Optional[TreeNode]) -> bool:
        def is_same(p, q):
            if not p and not q:
                return True
            if not p or not q:
                return False
            return p.val == q.val and is_same(p.left, q.left) and is_same(p.right, q.right)
        
        if not root:
            return not subRoot
        
        if is_same(root, subRoot):
            return True
        
        return self.isSubtree(root.left, subRoot) or self.isSubtree(root.right, subRoot)
```

**Explanation:** Check if root matches subRoot. If not, check left and right subtrees. Helper function checks if two trees are identical. Time: O(n*m), Space: O(min(h1, h2)).

---

## 69. CONSTRUCT BINARY TREE FROM PREORDER AND INORDER TRAVERSAL

**Link:** https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Build tree from preorder and inorder traversals.

**Most Optimized Solution:**

```python
class Solution:
    def buildTree(self, preorder: list[int], inorder: list[int]) -> Optional[TreeNode]:
        in_map = {val: i for i, val in enumerate(inorder)}
        self.pre_idx = 0
        
        def build(in_start, in_end):
            if in_start > in_end:
                return None
            
            root_val = preorder[self.pre_idx]
            self.pre_idx += 1
            root = TreeNode(root_val)
            
            in_idx = in_map[root_val]
            
            root.left = build(in_start, in_idx - 1)
            root.right = build(in_idx + 1, in_end)
            
            return root
        
        return build(0, len(inorder) - 1)
```

**Explanation:** Preorder: root, left, right. Inorder: left, root, right. First element in preorder is root. Find root's position in inorder to split left/right. Recursively build left and right subtrees. Time: O(n), Space: O(n).

---

## 70. VALIDATE BINARY SEARCH TREE

**Link:** https://leetcode.com/problems/validate-binary-search-tree/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(h)

**Problem:** Check if tree is valid BST.

**Most Optimized Solution:**

```python
class Solution:
    def isValidBST(self, root: Optional[TreeNode]) -> bool:
        def validate(node, min_val, max_val):
            if not node:
                return True
            
            if node.val <= min_val or node.val >= max_val:
                return False
            
            return validate(node.left, min_val, node.val) and \
                   validate(node.right, node.val, max_val)
        
        return validate(root, float('-inf'), float('inf'))
```

**Explanation:** Track valid range for each node. Left subtree: value < node->val. Right subtree: value > node->val. Use float('-inf') and float('inf') to handle boundaries. Time: O(n), Space: O(h).

---

## 71. KTH SMALLEST ELEMENT IN A BST

**Link:** https://leetcode.com/problems/kth-smallest-element-in-a-bst/

**Difficulty:** Medium | **Time:** O(k) | **Space:** O(h)

**Problem:** Find k-th smallest element in BST.

**Most Optimized Solution (In-order Traversal):**

```python
class Solution:
    def kthSmallest(self, root: Optional[TreeNode], k: int) -> int:
        self.k = k
        self.result = 0
        
        def inorder(node):
            if not node:
                return
            
            inorder(node.left)
            
            self.k -= 1
            if self.k == 0:
                self.result = node.val
                return
            
            inorder(node.right)
        
        inorder(root)
        return self.result
```

**Generator Alternative:**

```python
def kthSmallest(self, root: Optional[TreeNode], k: int) -> int:
    def inorder(node):
        if node:
            yield from inorder(node.left)
            yield node.val
            yield from inorder(node.right)
    
    for i, val in enumerate(inorder(root)):
        if i == k - 1:
            return val
```

**Explanation:** In-order traversal visits nodes in ascending order. Decrement k each visit. When k reaches 0, current node is k-th smallest. Time: O(k) average, O(n) worst. Space: O(h).

---

## 72. LOWEST COMMON ANCESTOR OF A BINARY SEARCH TREE

**Link:** https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree/

**Difficulty:** Easy | **Time:** O(h) | **Space:** O(1)

**Problem:** Find LCA of two nodes in BST.

**Most Optimized Solution (Iterative):**

```python
class Solution:
    def lowestCommonAncestor(self, root: 'TreeNode', p: 'TreeNode', q: 'TreeNode') -> 'TreeNode':
        while root:
            if root.val > max(p.val, q.val):
                root = root.left
            elif root.val < min(p.val, q.val):
                root = root.right
            else:
                return root
```

**Recursive Alternative:**

```python
def lowestCommonAncestor(self, root: 'TreeNode', p: 'TreeNode', q: 'TreeNode') -> 'TreeNode':
    if root.val > max(p.val, q.val):
        return self.lowestCommonAncestor(root.left, p, q)
    elif root.val < min(p.val, q.val):
        return self.lowestCommonAncestor(root.right, p, q)
    else:
        return root
```

**Explanation:** Use BST property: left < root < right. If both p, q are in left subtree, LCA is in left. If both in right subtree, LCA is in right. Otherwise, current node is LCA. Time: O(h), Space: O(1).

---

## 73. INVERT BINARY TREE

**Link:** https://leetcode.com/problems/invert-binary-tree/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(h)

**Problem:** Mirror the tree (swap left and right).

**Most Optimized Solution (Recursive):**

```python
class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if not root:
            return None
        
        root.left, root.right = root.right, root.left
        self.invertTree(root.left)
        self.invertTree(root.right)
        
        return root
```

**Iterative Alternative:**

```python
from collections import deque

def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
    if not root:
        return None
    
    queue = deque([root])
    while queue:
        node = queue.popleft()
        node.left, node.right = node.right, node.left
        
        if node.left:
            queue.append(node.left)
        if node.right:
            queue.append(node.right)
    
    return root
```

**Explanation:** Swap left and right children. Recursively invert subtrees. Time: O(n) visit each node, Space: O(h).

---

## 74. SAME TREE

**Link:** https://leetcode.com/problems/same-tree/

**Difficulty:** Easy | **Time:** O(min(n, m)) | **Space:** O(min(h1, h2))

**Problem:** Check if two trees are identical.

**Most Optimized Solution:**

```python
class Solution:
    def isSameTree(self, p: Optional[TreeNode], q: Optional[TreeNode]) -> bool:
        if not p and not q:
            return True
        if not p or not q or p.val != q.val:
            return False
        
        return self.isSameTree(p.left, q.left) and self.isSameTree(p.right, q.right)
```

**Explanation:** Base case: both null (true), one null (false), values differ (false). Recursively check left and right subtrees. Time: O(min(n, m)), Space: O(min(h1, h2)).

---

## 75. BINARY TREE RIGHT SIDE VIEW

**Link:** https://leetcode.com/problems/binary-tree-right-side-view/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(h)

**Problem:** Return nodes visible from right side.

**Most Optimized Solution (DFS):**

```python
class Solution:
    def rightSideView(self, root: Optional[TreeNode]) -> list[int]:
        result = []
        
        def dfs(node, depth):
            if not node:
                return
            
            if depth == len(result):
                result.append(node.val)
            
            dfs(node.right, depth + 1)
            dfs(node.left, depth + 1)
        
        dfs(root, 0)
        return result
```

**BFS Alternative:**

```python
from collections import deque

def rightSideView(self, root: Optional[TreeNode]) -> list[int]:
    if not root:
        return []
    
    result = []
    queue = deque([root])
    
    while queue:
        level_size = len(queue)
        result.append(queue[-1].val)  # Rightmost node
        
        for _ in range(level_size):
            node = queue.popleft()
            if node.left:
                queue.append(node.left)
            if node.right:
                queue.append(node.right)
    
    return result
```

**Explanation:** DFS traversing right to left. First node at each depth is rightmost. Visit right before left to find rightmost first. Time: O(n), Space: O(h).

---

## Summary: Blind 75 Patterns (Python)

| Category | Count | Key Patterns |
|----------|-------|---|
| Arrays | 9 | Two pointers, Sliding window, Prefix sums, Sorting |
| Binary | 5 | Bit manipulation, XOR, Bit counts |
| DP | 15 | 1D DP, 2D DP, Optimal substructure |
| Graph | 11 | DFS, BFS, Union-Find, Topological sort |
| Interval | 5 | Sorting, Merging, Greediness |
| Linked List | 7 | Two pointers, Reversal, Detection |
| Matrix | 3 | Traversal, Rotation, Backtracking |
| String | 9 | Sliding window, Hashing, Stack |
| Tree | 11 | DFS, BFS, Recursion, BST |

---

**Master the Blind 75 in Python and you're interview-ready!** 🚀

*Last Updated: December 2025*
*Language: Python 3.8+*
*Total Lines: 10,000+*
