# The Blind 75 LeetCode Problems - Complete C++ Solutions Guide

A curated list of 75 essential LeetCode problems that cover all major algorithmic concepts. Each problem includes optimal time/space complexity and production-grade C++ solutions.

---

## Table of Contents

### Arrays
1. Two Sum
2. Best Time to Buy and Sell Stock
3. Contains Duplicate
4. Product of Array Except Self
5. Maximum Subarray
6. Maximum Product Subarray
7. Search in Rotated Sorted Array
8. 3Sum
9. Container With Most Water

### Binary
10. Sum of Two Integers
11. Number of 1 Bits
12. Counting Bits
13. Missing Number
14. Reverse Bits

### Dynamic Programming
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

### Graph
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

### Interval
41. Insert Interval
42. Merge Intervals
43. Non-overlapping Intervals
44. Meeting Rooms
45. Meeting Rooms II

### Linked List
46. Reverse Linked List
47. Detect Cycle in Linked List
48. Merge Two Sorted Lists
49. Merge K Sorted Lists
50. Remove Nth Node From End of List
51. Reorder List
52. Set Matrix Zeroes

### Matrix
53. Spiral Matrix
54. Rotate Matrix
55. Word Search

### String
56. Longest Substring Without Repeating Characters
57. Longest Repeating Character Replacement
58. Minimum Window Substring
59. Valid Anagram
60. Group Anagrams
61. Valid Parentheses
62. Valid Palindrome
63. Longest Palindromic Substring (Duplicate)
64. Encode and Decode Strings

### Tree
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

```cpp
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> seen;  // value -> index
        
        for (int i = 0; i < nums.size(); ++i) {
            int complement = target - nums[i];
            
            if (seen.count(complement)) {
                return {seen[complement], i};
            }
            
            seen[nums[i]] = i;
        }
        
        return {};
    }
};
```

**Explanation:**
- Use a hash map to store values we've seen with their indices
- For each number, calculate complement needed (target - current)
- If complement exists in map, we found the pair
- Time: O(n) single pass, Space: O(n) for hash map

---

## 2. BEST TIME TO BUY AND SELL STOCK

**Link:** https://leetcode.com/problems/best-time-to-buy-and-sell-stock/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Find maximum profit from buying and selling a stock once.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int minPrice = INT_MAX;
        int maxProfit = 0;
        
        for (int price : prices) {
            minPrice = min(minPrice, price);
            maxProfit = max(maxProfit, price - minPrice);
        }
        
        return maxProfit;
    }
};
```

**Explanation:**
- Track minimum price seen so far and maximum profit
- For each price, calculate profit if sold at that price (price - minPrice)
- Time: O(n) single pass, Space: O(1) constant

---

## 3. CONTAINS DUPLICATE

**Link:** https://leetcode.com/problems/contains-duplicate/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(n)

**Problem:** Determine if array contains any duplicate.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        unordered_set<int> seen;
        
        for (int num : nums) {
            if (seen.count(num)) {
                return true;
            }
            seen.insert(num);
        }
        
        return false;
    }
};
```

**Explanation:**
- Use hash set to track seen numbers
- If we encounter a number already in set, it's a duplicate
- Time: O(n), Space: O(n) for hash set

---

## 4. PRODUCT OF ARRAY EXCEPT SELF

**Link:** https://leetcode.com/problems/product-of-array-except-self/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Return array where result[i] is product of all elements except nums[i].

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        int n = nums.size();
        vector<int> result(n, 1);
        
        // Calculate prefix products
        int prefix = 1;
        for (int i = 0; i < n; ++i) {
            result[i] = prefix;
            prefix *= nums[i];
        }
        
        // Calculate suffix products and multiply
        int suffix = 1;
        for (int i = n - 1; i >= 0; --i) {
            result[i] *= suffix;
            suffix *= nums[i];
        }
        
        return result;
    }
};
```

**Explanation:**
- First pass: calculate prefix product (all elements to left)
- Second pass: calculate suffix product (all elements to right) and multiply
- Time: O(n) two passes, Space: O(1) excluding output array

---

## 5. MAXIMUM SUBARRAY

**Link:** https://leetcode.com/problems/maximum-subarray/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find contiguous subarray with largest sum (Kadane's Algorithm).

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int maxCurrent = nums[0];
        int maxGlobal = nums[0];
        
        for (int i = 1; i < nums.size(); ++i) {
            maxCurrent = max(nums[i], maxCurrent + nums[i]);
            maxGlobal = max(maxGlobal, maxCurrent);
        }
        
        return maxGlobal;
    }
};
```

**Explanation:**
- Kadane's Algorithm: track max sum ending at current position
- If adding current element to previous sum is worse than current element alone, reset
- Time: O(n), Space: O(1)

---

## 6. MAXIMUM PRODUCT SUBARRAY

**Link:** https://leetcode.com/problems/maximum-product-subarray/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find contiguous subarray with largest product.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int maxProd = nums[0];
        int minProd = nums[0];
        int result = nums[0];
        
        for (int i = 1; i < nums.size(); ++i) {
            // Negative number flips max/min
            if (nums[i] < 0) {
                swap(maxProd, minProd);
            }
            
            maxProd = max(nums[i], maxProd * nums[i]);
            minProd = min(nums[i], minProd * nums[i]);
            
            result = max(result, maxProd);
        }
        
        return result;
    }
};
```

**Explanation:**
- Track both max and min product (negative * negative = positive)
- Swap when encountering negative (to handle flipping)
- Time: O(n), Space: O(1)

---

## 7. SEARCH IN ROTATED SORTED ARRAY

**Link:** https://leetcode.com/problems/search-in-rotated-sorted-array/

**Difficulty:** Medium | **Time:** O(log n) | **Space:** O(1)

**Problem:** Search for target in rotated sorted array.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int left = 0, right = nums.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (nums[mid] == target) {
                return mid;
            }
            
            // Determine which half is sorted
            if (nums[left] <= nums[mid]) {
                // Left half is sorted
                if (nums[left] <= target && target < nums[mid]) {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            } else {
                // Right half is sorted
                if (nums[mid] < target && target <= nums[right]) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }
        
        return -1;
    }
};
```

**Explanation:**
- Binary search but determine which half is sorted
- Check if target is in sorted half
- Narrow search space based on target location
- Time: O(log n) binary search, Space: O(1)

---

## 8. 3SUM

**Link:** https://leetcode.com/problems/3sum/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Find all unique triplets that sum to zero.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> result;
        if (nums.size() < 3) return result;
        
        sort(nums.begin(), nums.end());
        
        for (int i = 0; i < nums.size() - 2; ++i) {
            // Skip duplicates
            if (i > 0 && nums[i] == nums[i-1]) continue;
            
            // If positive, no solution possible
            if (nums[i] > 0) break;
            
            int left = i + 1;
            int right = nums.size() - 1;
            
            while (left < right) {
                int sum = nums[i] + nums[left] + nums[right];
                
                if (sum == 0) {
                    result.push_back({nums[i], nums[left], nums[right]});
                    
                    // Skip duplicates
                    while (left < right && nums[left] == nums[left+1]) left++;
                    while (left < right && nums[right] == nums[right-1]) right--;
                    
                    left++;
                    right--;
                } else if (sum < 0) {
                    left++;
                } else {
                    right--;
                }
            }
        }
        
        return result;
    }
};
```

**Explanation:**
- Sort array first
- Fix one element and use two-pointer for remaining two
- Skip duplicates to avoid duplicate triplets
- Time: O(n²) due to two-pointer, Space: O(1) excluding output

---

## 9. CONTAINER WITH MOST WATER

**Link:** https://leetcode.com/problems/container-with-most-water/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Find two lines that form container with most water.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int maxArea(vector<int>& height) {
        int left = 0;
        int right = height.size() - 1;
        int maxArea = 0;
        
        while (left < right) {
            int area = min(height[left], height[right]) * (right - left);
            maxArea = max(maxArea, area);
            
            // Move pointer pointing to smaller height
            if (height[left] < height[right]) {
                left++;
            } else {
                right--;
            }
        }
        
        return maxArea;
    }
};
```

**Explanation:**
- Two-pointer approach from both ends
- Area = min(left, right) * distance
- Move pointer pointing to smaller height (only way to improve)
- Time: O(n), Space: O(1)

---

## 10. SUM OF TWO INTEGERS

**Link:** https://leetcode.com/problems/sum-of-two-integers/

**Difficulty:** Medium | **Time:** O(1) | **Space:** O(1)

**Problem:** Add two integers without using + or - operators.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int getSum(int a, int b) {
        while (b != 0) {
            int carry = (unsigned int)(a & b) << 1;
            a = a ^ b;
            b = carry;
        }
        return a;
    }
};
```

**Explanation:**
- XOR gives sum without carry: a ^ b
- AND and left shift give carry: (a & b) << 1
- Repeat until carry is 0
- Use unsigned for left shift to handle overflow
- Time: O(1) constant iterations, Space: O(1)

---

## 11. NUMBER OF 1 BITS

**Link:** https://leetcode.com/problems/number-of-1-bits/

**Difficulty:** Easy | **Time:** O(1) | **Space:** O(1)

**Problem:** Count number of 1 bits in binary representation.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int hammingWeight(uint32_t n) {
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }
};
```

**Explanation:**
- Check rightmost bit with AND 1
- Right shift to check next bit
- Count until n becomes 0
- Alternative: n & (n-1) removes rightmost 1 bit each iteration
- Time: O(1) max 32 iterations, Space: O(1)

---

## 12. COUNTING BITS

**Link:** https://leetcode.com/problems/counting-bits/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Return array where i-th element is count of 1s in binary of i.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<int> countBits(int n) {
        vector<int> result;
        result.emplace_back(0);
        for (int i = 1; i <= n; ++i) {
            result.emplace_back(result[(i & i-1)]+1);
        }
        return result;
    }
};
```

**Explanation:**
- DP approach: result[i] = result[i >> 1] + (i & 1)
- i >> 1 is i divided by 2
- i & 1 is 1 if i is odd, 0 if even
- Reuse previous results
- Time: O(n), Space: O(1) excluding output

---

## 13. MISSING NUMBER

**Link:** https://leetcode.com/problems/missing-number/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Given array containing n distinct numbers from 0 to n, find missing one.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int missingNumber(vector<int>& nums) {
        long long n = nums.size();
        long long expected = n * (n + 1) / 2;
        long long actual = 0;
        
        for (int num : nums) {
            actual += num;
        }
        
        return expected - actual;
    }
};
```

**Explanation:**
- Sum of 0 to n is n*(n+1)/2
- Subtract actual sum from expected sum
- Use long long to prevent overflow
- Time: O(n), Space: O(1)


**Cyclic Sort:**
```cpp
class Solution {
public:
    int missingNumber(vector<int>& nums) {
       int n = nums.size();
       int i = 0;

       while(i<n){
            if(nums[i] < n && nums[i] != nums[nums[i]]){
                swap(nums[i], nums[nums[i]]);
            }
            else{
                i++;
            }
       }

       for(int i=0; i<n; i++){
            if(nums[i] != i){
                return i;
            }
       }
       return n;
    }
};
```
---

## 14. REVERSE BITS

**Link:** https://leetcode.com/problems/reverse-bits/

**Difficulty:** Easy | **Time:** O(1) | **Space:** O(1)

**Problem:** Reverse bits of 32-bit unsigned integer.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    uint32_t reverseBits(uint32_t n) {
        uint32_t result = 0;
        
        for (int i = 0; i < 32; ++i) {
            result <<= 1;  // Shift left to make space
            result |= n & 1;  // Add rightmost bit
            n >>= 1;  // Shift n right
        }
        
        return result;
    }
};
```

**Explanation:**
- Build result by shifting left and adding bits from n
- Extract rightmost bit of n with AND 1
- Shift n right to process next bit
- Time: O(1) exactly 32 iterations, Space: O(1)

---

## 15. CLIMBING STAIRS

**Link:** https://leetcode.com/problems/climbing-stairs/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Climb n stairs taking 1 or 2 steps at a time. How many ways?

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        
        int prev = 1, curr = 2;
        
        for (int i = 3; i <= n; ++i) {
            int next = prev + curr;
            prev = curr;
            curr = next;
        }
        
        return curr;
    }
};
```

**Explanation:**
- DP: ways[i] = ways[i-1] + ways[i-2]
- Each step can reach from 1 or 2 steps before
- Optimize space: only track last two values
- Time: O(n), Space: O(1)

---

## 16. COIN CHANGE

**Link:** https://leetcode.com/problems/coin-change/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Find minimum number of coins to make amount.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> dp(amount + 1, INT_MAX);
        dp[0] = 0;
        
        for (int i = 1; i <= amount; ++i) {
            for (int coin : coins) {
                if (coin <= i && dp[i - coin] != INT_MAX) {
                    dp[i] = min(dp[i], dp[i - coin] + 1);
                }
            }
        }
        
        return dp[amount] == INT_MAX ? -1 : dp[amount];
    }
};
```

**Explanation:**
- DP: dp[i] = minimum coins to make amount i
- For each amount, try all coins
- dp[i] = min(dp[i], dp[i - coin] + 1)
- Time: O(n*m) where n=amount, m=coins, Space: O(n)

---

## 17. LONGEST INCREASING SUBSEQUENCE

**Link:** https://leetcode.com/problems/longest-increasing-subsequence/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(n)

**Problem:** Find length of longest increasing subsequence.

**Most Optimized Solution (Binary Search):**

```cpp
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        vector<int> tails;
        
        for (int num : nums) {
            auto it = lower_bound(tails.begin(), tails.end(), num);
            
            if (it == tails.end()) {
                tails.push_back(num);
            } else {
                *it = num;
            }
        }
        
        return tails.size();
    }
};
```

**Explanation:**
- Maintain sorted array of smallest tails of increasing subsequences
- For each number, use binary search to find position
- Either add to end (longer subsequence) or replace (potentially better)
- Time: O(n log n), Space: O(n)

---

## 18. LONGEST COMMON SUBSEQUENCE

**Link:** https://leetcode.com/problems/longest-common-subsequence/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find length of longest common subsequence of two strings.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        int m = text1.size(), n = text2.size();
        vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
        
        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (text1[i-1] == text2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                } else {
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
                }
            }
        }
        
        return dp[m][n];
    }
};
```

**Explanation:**
- DP: dp[i][j] = LCS length of first i chars of text1 and first j of text2
- If chars match: dp[i][j] = dp[i-1][j-1] + 1
- Else: dp[i][j] = max(dp[i-1][j], dp[i][j-1])
- Time: O(m*n), Space: O(m*n)

---

## 19. WORD BREAK

**Link:** https://leetcode.com/problems/word-break/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(n)

**Problem:** Determine if string can be segmented using dictionary.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
        unordered_set<string> dict(wordDict.begin(), wordDict.end());
        vector<bool> dp(s.size() + 1, false);
        dp[0] = true;
        
        for (int i = 1; i <= s.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                if (dp[j] && dict.count(s.substr(j, i - j))) {
                    dp[i] = true;
                    break;
                }
            }
        }
        
        return dp[s.size()];
    }
};
```

**Explanation:**
- DP: dp[i] = can first i chars be segmented
- For each position, check all substrings ending at position
- If substring is in dict and dp[start] is true, dp[i] = true
- Time: O(n²), Space: O(n)

---

## 20. COMBINATION SUM IV

**Link:** https://leetcode.com/problems/combination-sum-iv/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Find number of combinations that sum to target.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int combinationSum4(vector<int>& nums, int target) {
        vector<long long> dp(target + 1, 0);
        dp[0] = 1;
        
        for (int i = 1; i <= target; ++i) {
            for (int num : nums) {
                if (num <= i) {
                    dp[i] += dp[i - num];
                }
            }
        }
        
        return dp[target];
    }
};
```

**Explanation:**
- DP: dp[i] = number of combinations to sum to i
- For each amount, sum up combinations from all nums
- dp[i] += dp[i - num]
- Use long long to handle large numbers
- Time: O(n*m), Space: O(n)

---

## 21. HOUSE ROBBER

**Link:** https://leetcode.com/problems/house-robber/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Rob houses to maximize money (can't rob adjacent houses).

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int rob(vector<int>& nums) {
        int prev1 = 0, prev2 = 0;
        
        for (int num : nums) {
            int curr = max(prev1 + num, prev2);
            prev2 = prev1;
            prev1 = curr;
        }
        
        return prev1;
    }
};
```

**Explanation:**
- DP: dp[i] = max money up to house i
- Either rob current + max up to i-2, or skip current
- dp[i] = max(dp[i-1], dp[i-2] + nums[i])
- Optimize space: only track last two values
- Time: O(n), Space: O(1)

---

## 22. HOUSE ROBBER II

**Link:** https://leetcode.com/problems/house-robber-ii/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Rob houses in circle (first and last adjacent).

**Most Optimized Solution:**

```cpp
class Solution {
private:
    int rob_range(vector<int>& nums, int start, int end) {
        int prev1 = 0, prev2 = 0;
        for (int i = start; i <= end; ++i) {
            int curr = max(prev1 + nums[i], prev2);
            prev2 = prev1;
            prev1 = curr;
        }
        return prev1;
    }
    
public:
    int rob(vector<int>& nums) {
        if (nums.size() == 1) return nums[0];
        
        // Either rob houses [0, n-2] or [1, n-1]
        return max(rob_range(nums, 0, nums.size() - 2),
                   rob_range(nums, 1, nums.size() - 1));
    }
};
```

**Explanation:**
- Houses arranged in circle (first and last are adjacent)
- Can't rob both first and last
- Solve two scenarios: exclude first or exclude last
- Take maximum of both
- Time: O(n), Space: O(1)

---

## 23. DECODE WAYS

**Link:** https://leetcode.com/problems/decode-ways/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Count number of ways to decode string of digits.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int numDecodings(string s) {
        if (s.empty() || s[0] == '0') return 0;
        
        int prev2 = 1, prev1 = 1;
        
        for (int i = 1; i < s.size(); ++i) {
            int curr = 0;
            
            // Single digit
            if (s[i] != '0') {
                curr += prev1;
            }
            
            // Two digits
            if (s[i-1] == '1' || (s[i-1] == '2' && s[i] < '7')) {
                curr += prev2;
            }
            
            prev2 = prev1;
            prev1 = curr;
        }
        
        return prev1;
    }
};
```

**Explanation:**
- DP: ways[i] = ways to decode first i characters
- Can decode single digit (1-9)
- Can decode two digits (10-26)
- dp[i] = dp[i-1] (if single valid) + dp[i-2] (if pair valid)
- Optimize space: track last two values
- Time: O(n), Space: O(1)

---

## 24. COIN CHANGE 2

**Link:** https://leetcode.com/problems/coin-change-2/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(n)

**Problem:** Number of combinations to make target amount.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int change(int amount, vector<int>& coins) {
        vector<int> dp(amount + 1, 0);
        dp[0] = 1;
        
        for (int coin : coins) {
            for (int i = coin; i <= amount; ++i) {
                dp[i] += dp[i - coin];
            }
        }
        
        return dp[amount];
    }
};
```

**Explanation:**
- DP: dp[i] = combinations to make amount i
- Iterate through coins (not amounts) to avoid counting duplicates
- For each coin, update all amounts it can contribute to
- dp[i] += dp[i - coin]
- Time: O(n*m), Space: O(n)

---

## 25. PARTITION EQUAL SUBSET SUM

**Link:** https://leetcode.com/problems/partition-equal-subset-sum/

**Difficulty:** Medium | **Time:** O(n*m) | **Space:** O(m)

**Problem:** Partition array into two equal sum subsets.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int total = 0;
        for (int num : nums) total += num;
        
        // Odd sum can't be partitioned
        if (total % 2 != 0) return false;
        
        int target = total / 2;
        vector<bool> dp(target + 1, false);
        dp[0] = true;
        
        for (int num : nums) {
            for (int i = target; i >= num; --i) {
                dp[i] = dp[i] || dp[i - num];
            }
        }
        
        return dp[target];
    }
};
```

**Explanation:**
- Problem reduces to: find subset with sum = total/2
- Use 0/1 knapsack DP
- dp[i] = can we achieve sum i
- Iterate backwards to avoid using same item twice
- Time: O(n*m), Space: O(m)

---

## 26. LONGEST PALINDROMIC SUBSTRING

**Link:** https://leetcode.com/problems/longest-palindromic-substring/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Find longest palindromic substring.

**Most Optimized Solution (Expand Around Center):**

```cpp
class Solution {
private:
    pair<int, int> expand_around_center(string& s, int left, int right) {
        while (left >= 0 && right < s.size() && s[left] == s[right]) {
            left--;
            right++;
        }
        return {left + 1, right - 1};
    }
    
public:
    string longestPalindrome(string s) {
        int max_start = 0, max_len = 0;
        
        for (int i = 0; i < s.size(); ++i) {
            // Odd length palindrome
            auto [start1, end1] = expand_around_center(s, i, i);
            if (end1 - start1 + 1 > max_len) {
                max_start = start1;
                max_len = end1 - start1 + 1;
            }
            
            // Even length palindrome
            auto [start2, end2] = expand_around_center(s, i, i + 1);
            if (end2 - start2 + 1 > max_len) {
                max_start = start2;
                max_len = end2 - start2 + 1;
            }
        }
        
        return s.substr(max_start, max_len);
    }
};
```

**Explanation:**
- Expand around each center (single and double)
- Track longest palindrome found
- Better than O(n³) DP approach
- Time: O(n²), Space: O(1)

---

## 27. PALINDROMIC SUBSTRINGS

**Link:** https://leetcode.com/problems/palindromic-substrings/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(1)

**Problem:** Count number of palindromic substrings.

**Most Optimized Solution:**

```cpp
class Solution {
private:
    int expand_around_center(string& s, int left, int right) {
        int count = 0;
        while (left >= 0 && right < s.size() && s[left] == s[right]) {
            count++;
            left--;
            right++;
        }
        return count;
    }
    
public:
    int countSubstrings(string s) {
        int count = 0;
        
        for (int i = 0; i < s.size(); ++i) {
            count += expand_around_center(s, i, i);      // Odd length
            count += expand_around_center(s, i, i + 1);  // Even length
        }
        
        return count;
    }
};
```

**Explanation:**
- Expand around each center
- Count palindromes found at each expansion
- Time: O(n²), Space: O(1)

---

## 28. NUMBER OF LONGEST INCREASING SUBSEQUENCE

**Link:** https://leetcode.com/problems/number-of-longest-increasing-subsequence/

**Difficulty:** Medium | **Time:** O(n²) | **Space:** O(n)

**Problem:** Count number of longest increasing subsequences.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int findNumberOfLIS(vector<int>& nums) {
        int n = nums.size();
        vector<int> length(n, 1);  // LIS length ending at i
        vector<int> count(n, 1);   // Number of LIS ending at i
        
        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < i; ++j) {
                if (nums[j] < nums[i]) {
                    if (length[j] + 1 > length[i]) {
                        length[i] = length[j] + 1;
                        count[i] = count[j];
                    } else if (length[j] + 1 == length[i]) {
                        count[i] += count[j];
                    }
                }
            }
        }
        
        int max_len = *max_element(length.begin(), length.end());
        int result = 0;
        
        for (int i = 0; i < n; ++i) {
            if (length[i] == max_len) {
                result += count[i];
            }
        }
        
        return result;
    }
};
```

**Explanation:**
- Track LIS length ending at each position
- Track count of LIS with that length
- When extending, update count based on whether we found longer or equal
- Sum counts where length equals maximum LIS length
- Time: O(n²), Space: O(n)

---

## 29. MAXIMAL SQUARE

**Link:** https://leetcode.com/problems/maximal-square/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find largest square of 1s in matrix.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int maximalSquare(vector<vector<char>>& matrix) {
        if (matrix.empty()) return 0;
        
        int m = matrix.size(), n = matrix[0].size();
        vector<vector<int>> dp(m, vector<int>(n, 0));
        int max_side = 0;
        
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (matrix[i][j] == '1') {
                    if (i == 0 || j == 0) {
                        dp[i][j] = 1;
                    } else {
                        dp[i][j] = min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]}) + 1;
                    }
                    max_side = max(max_side, dp[i][j]);
                }
            }
        }
        
        return max_side * max_side;
    }
};
```

**Explanation:**
- DP: dp[i][j] = side length of square with bottom-right at (i,j)
- If current is '1': dp[i][j] = min(top, left, diagonal) + 1
- This ensures we can form a square
- Time: O(m*n), Space: O(m*n)

---

## 30. NUMBER OF ISLANDS

**Link:** https://leetcode.com/problems/number-of-islands/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Count number of islands (connected 1s).

**Most Optimized Solution (DFS):**

```cpp
class Solution {
private:
    void dfs(vector<vector<char>>& grid, int i, int j) {
        if (i < 0 || i >= grid.size() || j < 0 || j >= grid[0].size() ||
            grid[i][j] != '1') {
            return;
        }
        
        grid[i][j] = '0';  // Mark as visited
        
        dfs(grid, i + 1, j);
        dfs(grid, i - 1, j);
        dfs(grid, i, j + 1);
        dfs(grid, i, j - 1);
    }
    
public:
    int numIslands(vector<vector<char>>& grid) {
        int count = 0;
        
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[0].size(); ++j) {
                if (grid[i][j] == '1') {
                    dfs(grid, i, j);
                    count++;
                }
            }
        }
        
        return count;
    }
};
```

**Explanation:**
- Use DFS to mark connected 1s
- Each DFS call finds one complete island
- Count number of DFS calls
- Time: O(m*n), Space: O(m*n) for recursion

---

## 31. CLONE GRAPH

**Link:** https://leetcode.com/problems/clone-graph/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Deep copy an undirected graph.

**Most Optimized Solution (BFS):**

```cpp
class Solution {
public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;
        
        unordered_map<Node*, Node*> cloned;
        queue<Node*> q;
        
        q.push(node);
        cloned[node] = new Node(node->val);
        
        while (!q.empty()) {
            Node* curr = q.front();
            q.pop();
            
            for (Node* neighbor : curr->neighbors) {
                if (cloned.find(neighbor) == cloned.end()) {
                    cloned[neighbor] = new Node(neighbor->val);
                    q.push(neighbor);
                }
                cloned[curr]->neighbors.push_back(cloned[neighbor]);
            }
        }
        
        return cloned[node];
    }
};
```

**Explanation:**
- Use BFS with hash map to track cloned nodes
- Create new node when first encountering
- Connect cloned nodes based on original edges
- Time: O(n+e) where n=nodes, e=edges
- Space: O(n) for hash map

---

## 32. COURSE SCHEDULE

**Link:** https://leetcode.com/problems/course-schedule/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Detect cycle in directed graph (course prerequisites).

**Most Optimized Solution (Topological Sort):**

```cpp
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> indegree(numCourses, 0);
        vector<vector<int>> graph(numCourses);
        
        // Build graph
        for (auto& prereq : prerequisites) {
            graph[prereq[1]].push_back(prereq[0]);
            indegree[prereq[0]]++;
        }
        
        queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }
        
        int count = 0;
        while (!q.empty()) {
            int course = q.front();
            q.pop();
            count++;
            
            for (int next : graph[course]) {
                indegree[next]--;
                if (indegree[next] == 0) {
                    q.push(next);
                }
            }
        }
        
        return count == numCourses;
    }
};
```

**Explanation:**
- Build directed graph of course dependencies
- Use topological sort with indegree
- If all courses can be sorted, no cycle exists
- Time: O(n+e), Space: O(n)

---

## 33. COURSE SCHEDULE II

**Link:** https://leetcode.com/problems/course-schedule-ii/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Return course order if possible, else empty array.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> indegree(numCourses, 0);
        vector<vector<int>> graph(numCourses);
        
        for (auto& prereq : prerequisites) {
            graph[prereq[1]].push_back(prereq[0]);
            indegree[prereq[0]]++;
        }
        
        queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }
        
        vector<int> result;
        while (!q.empty()) {
            int course = q.front();
            q.pop();
            result.push_back(course);
            
            for (int next : graph[course]) {
                indegree[next]--;
                if (indegree[next] == 0) {
                    q.push(next);
                }
            }
        }
        
        return result.size() == numCourses ? result : vector<int>();
    }
};
```

**Explanation:**
- Topological sort returns valid course order
- If all courses included, return order; else empty
- Time: O(n+e), Space: O(n)

---

## 34. ALIEN DICTIONARY

**Link:** https://leetcode.com/problems/alien-dictionary/

**Difficulty:** Hard | **Time:** O(n*l+k) | **Space:** O(k)

**Problem:** Order of alien alphabet from sorted words.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    string alienOrder(vector<string>& words) {
        unordered_map<char, set<char>> graph;
        unordered_map<char, int> indegree;
        
        // Initialize all characters
        for (string& word : words) {
            for (char c : word) {
                if (indegree.find(c) == indegree.end()) {
                    indegree[c] = 0;
                }
            }
        }
        
        // Build graph
        for (int i = 0; i < words.size() - 1; ++i) {
            string w1 = words[i], w2 = words[i+1];
            int min_len = min(w1.size(), w2.size());
            
            for (int j = 0; j < min_len; ++j) {
                if (w1[j] != w2[j]) {
                    if (graph[w1[j]].find(w2[j]) == graph[w1[j]].end()) {
                        graph[w1[j]].insert(w2[j]);
                        indegree[w2[j]]++;
                    }
                    break;
                }
            }
        }
        
        // Topological sort
        queue<char> q;
        for (auto& [c, in] : indegree) {
            if (in == 0) q.push(c);
        }
        
        string result;
        while (!q.empty()) {
            char c = q.front();
            q.pop();
            result += c;
            
            for (char next : graph[c]) {
                indegree[next]--;
                if (indegree[next] == 0) {
                    q.push(next);
                }
            }
        }
        
        return result.size() == indegree.size() ? result : "";
    }
};
```

**Explanation:**
- Compare adjacent words to find order constraints
- Build directed graph and use topological sort
- Time: O(n*l+k), Space: O(k) where k=alphabet size

---

## 35. GRAPH VALID TREE

**Link:** https://leetcode.com/problems/graph-valid-tree/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Check if graph is valid tree (no cycle, connected).

**Most Optimized Solution (Union-Find):**

```cpp
class Solution {
public:
    bool validTree(int n, vector<vector<int>>& edges) {
        if (edges.size() != n - 1) return false;  // Tree has n-1 edges
        
        vector<int> parent(n);
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
        
        function<int(int)> find = [&](int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };
        
        for (auto& edge : edges) {
            int x = find(edge[0]);
            int y = find(edge[1]);
            
            if (x == y) return false;  // Cycle detected
            parent[x] = y;
        }
        
        return true;
    }
};
```

**Explanation:**
- Tree has exactly n-1 edges
- Use Union-Find to detect cycles
- If two vertices already have same root, edge creates cycle
- Time: O(n+e), Space: O(n)

---

## 36. NUMBER OF CONNECTED COMPONENTS IN AN UNDIRECTED GRAPH

**Link:** https://leetcode.com/problems/number-of-connected-components-in-an-undirected-graph/

**Difficulty:** Medium | **Time:** O(n+e) | **Space:** O(n)

**Problem:** Count connected components.

**Most Optimized Solution (Union-Find):**

```cpp
class Solution {
public:
    int countComponents(int n, vector<vector<int>>& edges) {
        vector<int> parent(n);
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
        
        function<int(int)> find = [&](int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };
        
        int components = n;
        for (auto& edge : edges) {
            int x = find(edge[0]);
            int y = find(edge[1]);
            
            if (x != y) {
                parent[x] = y;
                components--;
            }
        }
        
        return components;
    }
};
```

**Explanation:**
- Start with n components (each node separate)
- For each edge, if nodes are in different components, merge them
- Decrease component count when merging
- Time: O(n+e), Space: O(n)

---

## 37. LONGEST CONSECUTIVE

**Link:** https://leetcode.com/problems/longest-consecutive/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Find length of longest consecutive elements sequence.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        if (nums.empty()) return 0;
        
        unordered_set<int> numSet(nums.begin(), nums.end());
        int maxLen = 0;
        
        for (int num : numSet) {
            // Only start counting from sequence start
            if (numSet.find(num - 1) == numSet.end()) {
                int length = 1;
                while (numSet.find(num + length) != numSet.end()) {
                    length++;
                }
                maxLen = max(maxLen, length);
            }
        }
        
        return maxLen;
    }
};
```

**Explanation:**
- Use set for O(1) lookup
- Only start counting from sequence beginning (num-1 doesn't exist)
- For each sequence start, count length
- Time: O(n), Space: O(n)

---

## 38. PACIFIC ATLANTIC WATER FLOW

**Link:** https://leetcode.com/problems/pacific-atlantic-water-flow/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find cells where water flows to both oceans.

**Most Optimized Solution (Reverse DFS):**

```cpp
class Solution {
private:
    void dfs(vector<vector<int>>& heights, vector<vector<bool>>& visited, 
             int i, int j, int prev_height) {
        if (i < 0 || i >= heights.size() || j < 0 || j >= heights[0].size() ||
            visited[i][j] || heights[i][j] < prev_height) {
            return;
        }
        
        visited[i][j] = true;
        
        dfs(heights, visited, i + 1, j, heights[i][j]);
        dfs(heights, visited, i - 1, j, heights[i][j]);
        dfs(heights, visited, i, j + 1, heights[i][j]);
        dfs(heights, visited, i, j - 1, heights[i][j]);
    }
    
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        int m = heights.size(), n = heights[0].size();
        vector<vector<bool>> pacific(m, vector<bool>(n, false));
        vector<vector<bool>> atlantic(m, vector<bool>(n, false));
        
        // Start from borders
        for (int i = 0; i < m; ++i) {
            dfs(heights, pacific, i, 0, INT_MIN);
            dfs(heights, atlantic, i, n - 1, INT_MIN);
        }
        
        for (int j = 0; j < n; ++j) {
            dfs(heights, pacific, 0, j, INT_MIN);
            dfs(heights, atlantic, m - 1, j, INT_MIN);
        }
        
        vector<vector<int>> result;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (pacific[i][j] && atlantic[i][j]) {
                    result.push_back({i, j});
                }
            }
        }
        
        return result;
    }
};
```

**Explanation:**
- Reverse approach: start from oceans, find cells reachable
- Water flows to ocean if path has non-increasing heights
- Find cells reachable from both oceans
- Time: O(m*n), Space: O(m*n)

---

## 39. WALLS AND GATES

**Link:** https://leetcode.com/problems/walls-and-gates/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Fill distances from gates to empty rooms.

**Most Optimized Solution (Multi-source BFS):**

```cpp
class Solution {
public:
    void wallsAndGates(vector<vector<int>>& rooms) {
        queue<pair<int, int>> q;
        int m = rooms.size(), n = rooms[0].size();
        
        // Add all gates to queue
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (rooms[i][j] == 0) {
                    q.push({i, j});
                }
            }
        }
        
        vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            
            for (auto [dx, dy] : directions) {
                int nx = x + dx, ny = y + dy;
                
                if (nx >= 0 && nx < m && ny >= 0 && ny < n &&
                    rooms[nx][ny] > rooms[x][y] + 1) {
                    rooms[nx][ny] = rooms[x][y] + 1;
                    q.push({nx, ny});
                }
            }
        }
    }
};
```

**Explanation:**
- Multi-source BFS starting from all gates
- Breadth-first ensures shortest distance
- Update room distance if shorter path found
- Time: O(m*n), Space: O(m*n)

---

## 40. ROTTING ORANGES

**Link:** https://leetcode.com/problems/rotting-oranges/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(m*n)

**Problem:** Find time until all fresh oranges rot.

**Most Optimized Solution (Multi-source BFS):**

```cpp
class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        queue<pair<int, int>> q;
        int fresh = 0;
        int m = grid.size(), n = grid[0].size();
        
        // Add all rotten oranges to queue
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == 2) {
                    q.push({i, j});
                } else if (grid[i][j] == 1) {
                    fresh++;
                }
            }
        }
        
        vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        int minutes = 0;
        
        while (!q.empty() && fresh > 0) {
            minutes++;
            int size = q.size();
            
            for (int i = 0; i < size; ++i) {
                auto [x, y] = q.front();
                q.pop();
                
                for (auto [dx, dy] : directions) {
                    int nx = x + dx, ny = y + dy;
                    
                    if (nx >= 0 && nx < m && ny >= 0 && ny < n &&
                        grid[nx][ny] == 1) {
                        grid[nx][ny] = 2;
                        fresh--;
                        q.push({nx, ny});
                    }
                }
            }
        }
        
        return fresh == 0 ? minutes : -1;
    }
};
```

**Explanation:**
- Multi-source BFS from all rotten oranges
- Each level represents one minute
- Count fresh oranges, decrease when rotting
- Return minutes when all fresh are rotten
- Time: O(m*n), Space: O(m*n)

---

## 41. INSERT INTERVAL

**Link:** https://leetcode.com/problems/insert-interval/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Insert interval into list of non-overlapping intervals.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        vector<vector<int>> result;
        int i = 0;
        int n = intervals.size();
        
        // Add intervals that end before new interval starts
        while (i < n && intervals[i][1] < newInterval[0]) {
            result.push_back(intervals[i]);
            i++;
        }
        
        // Merge overlapping intervals
        while (i < n && intervals[i][0] <= newInterval[1]) {
            newInterval[0] = min(newInterval[0], intervals[i][0]);
            newInterval[1] = max(newInterval[1], intervals[i][1]);
            i++;
        }
        
        result.push_back(newInterval);
        
        // Add remaining intervals
        while (i < n) {
            result.push_back(intervals[i]);
            i++;
        }
        
        return result;
    }
};
```

**Explanation:**
- Add non-overlapping intervals before new interval
- Merge all overlapping intervals with new interval
- Add remaining non-overlapping intervals
- Time: O(n), Space: O(n)

---

## 42. MERGE INTERVALS

**Link:** https://leetcode.com/problems/merge-intervals/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Merge overlapping intervals.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());
        
        vector<vector<int>> result;
        result.push_back(intervals[0]);
        
        for (int i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] <= result.back()[1]) {
                result.back()[1] = max(result.back()[1], intervals[i][1]);
            } else {
                result.push_back(intervals[i]);
            }
        }
        
        return result;
    }
};
```

**Explanation:**
- Sort intervals by start time
- Merge if current start <= previous end
- Update end to max of two ends
- Time: O(n log n), Space: O(1)

---

## 43. NON-OVERLAPPING INTERVALS

**Link:** https://leetcode.com/problems/non-overlapping-intervals/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Remove minimum intervals to make non-overlapping.

**Most Optimized Solution (Greedy):**

```cpp
class Solution {
public:
    int eraseOverlapIntervals(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end(), 
             [](const vector<int>& a, const vector<int>& b) {
                 return a[1] < b[1];  // Sort by end time
             });
        
        int removed = 0;
        int prev_end = INT_MIN;
        
        for (auto& interval : intervals) {
            if (interval[0] < prev_end) {
                removed++;  // Overlaps, remove this one
            } else {
                prev_end = interval[1];  // Update end time
            }
        }
        
        return removed;
    }
};
```

**Explanation:**
- Greedy approach: keep intervals with earliest end times
- Sort by end time, not start
- If current start < previous end, remove current
- Time: O(n log n), Space: O(1)

---

## 44. MEETING ROOMS

**Link:** https://leetcode.com/problems/meeting-rooms/

**Difficulty:** Easy | **Time:** O(n log n) | **Space:** O(1)

**Problem:** Check if person can attend all meetings (non-overlapping).

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool canAttendMeetings(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());
        
        for (int i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] < intervals[i-1][1]) {
                return false;
            }
        }
        
        return true;
    }
};
```

**Explanation:**
- Sort by start time
- Check if any meetings overlap
- If sorted meeting starts before previous ends, overlap exists
- Time: O(n log n), Space: O(1)

---

## 45. MEETING ROOMS II

**Link:** https://leetcode.com/problems/meeting-rooms-ii/

**Difficulty:** Medium | **Time:** O(n log n) | **Space:** O(n)

**Problem:** Minimum conference rooms needed.

**Most Optimized Solution (Sweep Line):**

```cpp
class Solution {
public:
    int minMeetingRooms(vector<vector<int>>& intervals) {
        vector<pair<int, int>> events;
        
        for (auto& interval : intervals) {
            events.push_back({interval[0], 1});   // Start: +1 room
            events.push_back({interval[1], -1});  // End: -1 room
        }
        
        sort(events.begin(), events.end());
        
        int rooms = 0, max_rooms = 0;
        
        for (auto& event : events) {
            rooms += event.second;
            max_rooms = max(max_rooms, rooms);
        }
        
        return max_rooms;
    }
};
```

**Explanation:**
- Create events: +1 for start, -1 for end
- Sort events by time
- Track current rooms needed
- Max rooms needed at any time is answer
- Time: O(n log n), Space: O(n)

---

## 46. REVERSE LINKED LIST

**Link:** https://leetcode.com/problems/reverse-linked-list/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Reverse a singly linked list.

**Most Optimized Solution (Iterative):**

```cpp
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        
        while (curr) {
            ListNode* next = curr->next;  // Save next
            curr->next = prev;             // Reverse link
            prev = curr;                   // Move prev
            curr = next;                   // Move curr
        }
        
        return prev;
    }
};
```

**Explanation:**
- Three pointers: prev, curr, next
- Iterate through list, reversing links
- Time: O(n), Space: O(1)

---

## 47. DETECT CYCLE IN LINKED LIST

**Link:** https://leetcode.com/problems/linked-list-cycle/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Detect if linked list has cycle.

**Most Optimized Solution (Floyd's Algorithm):**

```cpp
class Solution {
public:
    bool hasCycle(ListNode *head) {
        if (!head || !head->next) return false;
        
        ListNode* slow = head;
        ListNode* fast = head;
        
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            
            if (slow == fast) {
                return true;
            }
        }
        
        return false;
    }
};
```

**Explanation:**
- Floyd's cycle detection: slow moves 1 step, fast moves 2
- If cycle exists, they eventually meet
- Time: O(n), Space: O(1)

---

## 48. MERGE TWO SORTED LISTS

**Link:** https://leetcode.com/problems/merge-two-sorted-lists/

**Difficulty:** Easy | **Time:** O(n+m) | **Space:** O(1)

**Problem:** Merge two sorted lists into one.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy(0);
        ListNode* curr = &dummy;
        
        while (list1 && list2) {
            if (list1->val <= list2->val) {
                curr->next = list1;
                list1 = list1->next;
            } else {
                curr->next = list2;
                list2 = list2->next;
            }
            curr = curr->next;
        }
        
        curr->next = list1 ? list1 : list2;
        
        return dummy.next;
    }
};
```

**Explanation:**
- Use dummy node to simplify logic
- Compare heads of both lists
- Attach smaller one to result
- Attach remaining list when one is exhausted
- Time: O(n+m), Space: O(1)

---

## 49. MERGE K SORTED LISTS

**Link:** https://leetcode.com/problems/merge-k-sorted-lists/

**Difficulty:** Hard | **Time:** O(n log k) | **Space:** O(k)

**Problem:** Merge k sorted lists.

**Most Optimized Solution (Min Heap):**

```cpp
class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        // Custom comparator for min heap
        auto cmp = [](ListNode* a, ListNode* b) {
            return a->val > b->val;
        };
        
        priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);
        
        // Add first node of each list
        for (ListNode* list : lists) {
            if (list) {
                pq.push(list);
            }
        }
        
        ListNode dummy(0);
        ListNode* curr = &dummy;
        
        while (!pq.empty()) {
            ListNode* smallest = pq.top();
            pq.pop();
            
            curr->next = smallest;
            curr = curr->next;
            
            if (smallest->next) {
                pq.push(smallest->next);
            }
        }
        
        return dummy.next;
    }
};
```

**Explanation:**
- Use min heap to track smallest nodes
- Always pop smallest, add its next
- Continue until heap empty
- Time: O(n log k) where n=total nodes, k=lists
- Space: O(k) for heap

---

## 50. REMOVE NTH NODE FROM END OF LIST

**Link:** https://leetcode.com/problems/remove-nth-node-from-end-of-list/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Remove n-th node from end of list.

**Most Optimized Solution (Two Pointers):**

```cpp
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode dummy(0);
        dummy.next = head;
        
        ListNode* first = &dummy;
        ListNode* second = &dummy;
        
        // Move first n+1 steps ahead
        for (int i = 0; i <= n; ++i) {
            first = first->next;
        }
        
        // Move both until first reaches end
        while (first) {
            first = first->next;
            second = second->next;
        }
        
        second->next = second->next->next;
        
        return dummy.next;
    }
};
```

**Explanation:**
- Use dummy node to handle head removal
- Create two pointers n+1 steps apart
- Move both until first reaches end
- Remove node by skipping it
- Time: O(n), Space: O(1)

---

## 51. REORDER LIST

**Link:** https://leetcode.com/problems/reorder-list/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Reorder list: L0->L1->...->Ln to L0->Ln->L1->Ln-1...

**Most Optimized Solution:**

```cpp
class Solution {
public:
    void reorderList(ListNode* head) {
        if (!head || !head->next) return;
        
        // Find middle
        ListNode* slow = head, *fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        
        // Reverse second half
        ListNode* prev = nullptr;
        while (slow) {
            ListNode* next = slow->next;
            slow->next = prev;
            prev = slow;
            slow = next;
        }
        
        // Merge two halves
        ListNode* l1 = head, *l2 = prev;
        while (l2->next) {
            ListNode* tmp1 = l1->next;
            ListNode* tmp2 = l2->next;
            
            l1->next = l2;
            l2->next = tmp1;
            
            l1 = tmp1;
            l2 = tmp2;
        }
    }
};
```

**Explanation:**
- Find middle of list
- Reverse second half
- Merge two halves alternately
- Time: O(n), Space: O(1)

---

## 52. SET MATRIX ZEROES

**Link:** https://leetcode.com/problems/set-matrix-zeroes/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Set entire row/column to 0 if element is 0.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    void setZeroes(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        bool row_zero = false, col_zero = false;
        
        // Check if first row/col needs zeroing
        for (int i = 0; i < m; ++i) {
            if (matrix[i][0] == 0) col_zero = true;
        }
        for (int j = 0; j < n; ++j) {
            if (matrix[0][j] == 0) row_zero = true;
        }
        
        // Mark zeros in first row/col
        for (int i = 1; i < m; ++i) {
            for (int j = 1; j < n; ++j) {
                if (matrix[i][j] == 0) {
                    matrix[i][0] = 0;
                    matrix[0][j] = 0;
                }
            }
        }
        
        // Set zeroes except first row/col
        for (int i = 1; i < m; ++i) {
            for (int j = 1; j < n; ++j) {
                if (matrix[i][0] == 0 || matrix[0][j] == 0) {
                    matrix[i][j] = 0;
                }
            }
        }
        
        // Set first row/col
        if (row_zero) {
            for (int j = 0; j < n; ++j) {
                matrix[0][j] = 0;
            }
        }
        if (col_zero) {
            for (int i = 0; i < m; ++i) {
                matrix[i][0] = 0;
            }
        }
    }
};
```

**Explanation:**
- Use first row/col as markers
- Record if first row/col should be zeroed
- Mark zeros in first row/col for affected rows/cols
- Apply marks to rest of matrix
- Zero out first row/col if needed
- Time: O(m*n), Space: O(1)

---

## 53. SPIRAL MATRIX

**Link:** https://leetcode.com/problems/spiral-matrix/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Return elements in spiral order.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        vector<int> result;
        if (matrix.empty()) return result;
        
        int top = 0, bottom = matrix.size() - 1;
        int left = 0, right = matrix[0].size() - 1;
        
        while (top <= bottom && left <= right) {
            // Right
            for (int j = left; j <= right; ++j) {
                result.push_back(matrix[top][j]);
            }
            top++;
            
            // Down
            for (int i = top; i <= bottom; ++i) {
                result.push_back(matrix[i][right]);
            }
            right--;
            
            // Left
            if (top <= bottom) {
                for (int j = right; j >= left; --j) {
                    result.push_back(matrix[bottom][j]);
                }
                bottom--;
            }
            
            // Up
            if (left <= right) {
                for (int i = bottom; i >= top; --i) {
                    result.push_back(matrix[i][left]);
                }
                left++;
            }
        }
        
        return result;
    }
};
```

**Explanation:**
- Track boundaries: top, bottom, left, right
- Traverse right, down, left, up in spiral
- Shrink boundaries after each direction
- Time: O(m*n), Space: O(1)

---

## 54. ROTATE MATRIX

**Link:** https://leetcode.com/problems/rotate-image/

**Difficulty:** Medium | **Time:** O(m*n) | **Space:** O(1)

**Problem:** Rotate matrix 90 degrees clockwise in-place.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    void rotate(vector<vector<int>>& matrix) {
        int n = matrix.size();
        
        // Transpose
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                swap(matrix[i][j], matrix[j][i]);
            }
        }
        
        // Reverse each row
        for (int i = 0; i < n; ++i) {
            reverse(matrix[i].begin(), matrix[i].end());
        }
    }
};
```

**Explanation:**
- Rotation = Transpose + Reverse each row
- Transpose swaps elements across diagonal
- Reverse each row to complete 90° rotation
- Time: O(n²), Space: O(1)

---

## 55. WORD SEARCH

**Link:** https://leetcode.com/problems/word-search/

**Difficulty:** Medium | **Time:** O(m*n*4^l) | **Space:** O(l)

**Problem:** Search for word in grid (backtracking).

**Most Optimized Solution:**

```cpp
class Solution {
private:
    bool dfs(vector<vector<char>>& board, string& word, int idx, int i, int j) {
        if (idx == word.size()) {
            return true;
        }
        
        if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size() ||
            board[i][j] != word[idx]) {
            return false;
        }
        
        board[i][j] = '#';  // Mark as visited
        
        bool found = dfs(board, word, idx + 1, i + 1, j) ||
                     dfs(board, word, idx + 1, i - 1, j) ||
                     dfs(board, word, idx + 1, i, j + 1) ||
                     dfs(board, word, idx + 1, i, j - 1);
        
        board[i][j] = word[idx];  // Restore
        
        return found;
    }
    
public:
    bool exist(vector<vector<char>>& board, string word) {
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[0].size(); ++j) {
                if (board[i][j] == word[0] && 
                    dfs(board, word, 0, i, j)) {
                    return true;
                }
            }
        }
        return false;
    }
};
```

**Explanation:**
- DFS backtracking from each cell
- Mark visited cells to avoid reuse
- Explore all four directions
- Time: O(m*n*4^l), Space: O(l)

---

## 56. LONGEST SUBSTRING WITHOUT REPEATING CHARACTERS

**Link:** https://leetcode.com/problems/longest-substring-without-repeating-characters/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(min(n, m))

**Problem:** Find length of longest substring without repeating characters.

**Most Optimized Solution (Sliding Window):**

```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        unordered_map<char, int> char_index;
        int max_len = 0;
        int start = 0;
        
        for (int end = 0; end < s.size(); ++end) {
            if (char_index.find(s[end]) != char_index.end()) {
                start = max(start, char_index[s[end]] + 1);
            }
            
            char_index[s[end]] = end;
            max_len = max(max_len, end - start + 1);
        }
        
        return max_len;
    }
};
```

**Explanation:**
- Sliding window approach
- Track last index of each character
- When duplicate found, move start to after previous occurrence
- Time: O(n), Space: O(min(n, m))

---

## 57. LONGEST REPEATING CHARACTER REPLACEMENT

**Link:** https://leetcode.com/problems/longest-repeating-character-replacement/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(1)

**Problem:** Replace at most k characters to get longest repeating substring.

**Most Optimized Solution (Sliding Window):**

```cpp
class Solution {
public:
    int characterReplacement(string s, int k) {
        unordered_map<char, int> char_count;
        int max_len = 0;
        int max_freq = 0;
        int start = 0;
        
        for (int end = 0; end < s.size(); ++end) {
            char_count[s[end]]++;
            max_freq = max(max_freq, char_count[s[end]]);
            
            // If replacements needed > k, shrink window
            if (end - start + 1 - max_freq > k) {
                char_count[s[start]]--;
                start++;
            }
            
            max_len = max(max_len, end - start + 1);
        }
        
        return max_len;
    }
};
```

**Explanation:**
- Sliding window with character frequency
- max_freq = most frequent character in window
- If (window_size - max_freq) > k, shrink window
- Replacements needed = window_size - max_freq
- Time: O(n), Space: O(1)

---

## 58. MINIMUM WINDOW SUBSTRING

**Link:** https://leetcode.com/problems/minimum-window-substring/

**Difficulty:** Hard | **Time:** O(n+m) | **Space:** O(1)

**Problem:** Find minimum window containing all characters from t.

**Most Optimized Solution (Sliding Window):**

```cpp
class Solution {
public:
    string minWindow(string s, string t) {
        if (t.size() > s.size()) return "";
        
        unordered_map<char, int> t_count, window_count;
        for (char c : t) {
            t_count[c]++;
        }
        
        int formed = 0, required = t_count.size();
        int left = 0;
        int min_len = INT_MAX, min_left = 0;
        
        for (int right = 0; right < s.size(); ++right) {
            char c = s[right];
            window_count[c]++;
            
            if (t_count.count(c) && window_count[c] == t_count[c]) {
                formed++;
            }
            
            while (left <= right && formed == required) {
                if (right - left + 1 < min_len) {
                    min_len = right - left + 1;
                    min_left = left;
                }
                
                char c = s[left];
                window_count[c]--;
                if (t_count.count(c) && window_count[c] < t_count[c]) {
                    formed--;
                }
                
                left++;
            }
        }
        
        return min_len == INT_MAX ? "" : s.substr(min_left, min_len);
    }
};
```

**Explanation:**
- Sliding window with two pointers
- Expand right to include all characters from t
- Contract left while maintaining validity
- Track minimum window
- Time: O(n+m), Space: O(1)

---

## 59. VALID ANAGRAM

**Link:** https://leetcode.com/problems/valid-anagram/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Check if two strings are anagrams.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool isAnagram(string s, string t) {
        if (s.size() != t.size()) return false;
        
        int char_count[26] = {0};
        
        for (int i = 0; i < s.size(); ++i) {
            char_count[s[i] - 'a']++;
            char_count[t[i] - 'a']--;
        }
        
        for (int count : char_count) {
            if (count != 0) return false;
        }
        
        return true;
    }
};
```

**Explanation:**
- Fixed size array for 26 letters
- Increment for s, decrement for t
- If all counts are 0, strings are anagrams
- Time: O(n), Space: O(1)

---

## 60. GROUP ANAGRAMS

**Link:** https://leetcode.com/problems/group-anagrams/

**Difficulty:** Medium | **Time:** O(n*k log k) | **Space:** O(n*k)

**Problem:** Group anagrams together.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> groups;
        
        for (string str : strs) {
            string sorted_str = str;
            sort(sorted_str.begin(), sorted_str.end());
            groups[sorted_str].push_back(str);
        }
        
        vector<vector<string>> result;
        for (auto& [key, group] : groups) {
            result.push_back(group);
        }
        
        return result;
    }
};
```

**Explanation:**
- Sort each string to get canonical form
- Use sorted form as key in hash map
- Group anagrams have same sorted form
- Time: O(n*k log k), Space: O(n*k)

---

## 61. VALID PARENTHESES

**Link:** https://leetcode.com/problems/valid-parentheses/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(n)

**Problem:** Check if parentheses are valid and balanced.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool isValid(string s) {
        stack<char> st;
        unordered_map<char, char> pairs = {
            {')', '('}, {']', '['}, {'}', '{'}
        };
        
        for (char c : s) {
            if (pairs.count(c)) {
                if (st.empty() || st.top() != pairs[c]) {
                    return false;
                }
                st.pop();
            } else {
                st.push(c);
            }
        }
        
        return st.empty();
    }
};
```

**Explanation:**
- Push opening brackets
- When closing bracket found, check if matches top
- At end, stack must be empty
- Time: O(n), Space: O(n)

---

## 62. VALID PALINDROME

**Link:** https://leetcode.com/problems/valid-palindrome/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(1)

**Problem:** Check if alphanumeric characters form palindrome.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool isPalindrome(string s) {
        int left = 0, right = s.size() - 1;
        
        while (left < right) {
            while (left < right && !isalnum(s[left])) {
                left++;
            }
            while (left < right && !isalnum(s[right])) {
                right--;
            }
            
            if (tolower(s[left]) != tolower(s[right])) {
                return false;
            }
            
            left++;
            right--;
        }
        
        return true;
    }
};
```

**Explanation:**
- Two pointers from both ends
- Skip non-alphanumeric characters
- Compare lowercase versions
- Time: O(n), Space: O(1)

---

## 63. LONGEST PALINDROMIC SUBSTRING (Duplicate)

See Problem 26.

---

## 64. ENCODE AND DECODE STRINGS

**Link:** https://leetcode.com/problems/encode-and-decode-strings/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Serialize list of strings and deserialize.

**Most Optimized Solution:**

```cpp
class Codec {
public:
    string encode(vector<string>& strs) {
        string result;
        for (const string& str : strs) {
            result += to_string(str.size()) + "#" + str;
        }
        return result;
    }
    
    vector<string> decode(string s) {
        vector<string> result;
        int i = 0;
        
        while (i < s.size()) {
            int j = i;
            while (s[j] != '#') {
                j++;
            }
            
            int len = stoi(s.substr(i, j - i));
            result.push_back(s.substr(j + 1, len));
            i = j + 1 + len;
        }
        
        return result;
    }
};
```

**Explanation:**
- Encode: store length + "#" + string
- Decode: read length, extract string
- "#" acts as delimiter
- Time: O(n), Space: O(n)

---

## 65. BINARY TREE MAXIMUM PATH SUM

**Link:** https://leetcode.com/problems/binary-tree-maximum-path-sum/

**Difficulty:** Hard | **Time:** O(n) | **Space:** O(h)

**Problem:** Find maximum path sum in binary tree.

**Most Optimized Solution (DFS):**

```cpp
class Solution {
private:
    int max_sum = INT_MIN;
    
    int max_gain(TreeNode* node) {
        if (!node) return 0;
        
        int left_gain = max(0, max_gain(node->left));
        int right_gain = max(0, max_gain(node->right));
        
        int path_sum = node->val + left_gain + right_gain;
        max_sum = max(max_sum, path_sum);
        
        return node->val + max(left_gain, right_gain);
    }
    
public:
    int maxPathSum(TreeNode* root) {
        max_gain(root);
        return max_sum;
    }
};
```

**Explanation:**
- DFS post-order traversal
- Calculate max gain at each node
- Max gain = node value + max of left/right gains
- Path sum at node = value + both gains
- Track maximum path sum
- Time: O(n), Space: O(h)

---

## 66. BINARY TREE LEVEL ORDER TRAVERSAL

**Link:** https://leetcode.com/problems/binary-tree-level-order-traversal/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(w)

**Problem:** Return level-by-level traversal of tree.

**Most Optimized Solution (BFS):**

```cpp
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> result;
        if (!root) return result;
        
        queue<TreeNode*> q;
        q.push(root);
        
        while (!q.empty()) {
            int size = q.size();
            vector<int> level;
            
            for (int i = 0; i < size; ++i) {
                TreeNode* node = q.front();
                q.pop();
                
                level.push_back(node->val);
                
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }
            
            result.push_back(level);
        }
        
        return result;
    }
};
```

**Explanation:**
- BFS with queue
- Process all nodes at current level before next
- Track level size to know when level ends
- Time: O(n), Space: O(w) where w=max width

---

## 67. SERIALIZE AND DESERIALIZE BINARY TREE

**Link:** https://leetcode.com/problems/serialize-and-deserialize-binary-tree/

**Difficulty:** Hard | **Time:** O(n) | **Space:** O(n)

**Problem:** Serialize and deserialize binary tree.

**Most Optimized Solution (Pre-order):**

```cpp
class Codec {
private:
    void serialize_helper(TreeNode* node, string& data) {
        if (!node) {
            data += "null,";
            return;
        }
        
        data += to_string(node->val) + ",";
        serialize_helper(node->left, data);
        serialize_helper(node->right, data);
    }
    
    TreeNode* deserialize_helper(vector<string>& nodes, int& idx) {
        if (nodes[idx] == "null") {
            idx++;
            return nullptr;
        }
        
        TreeNode* node = new TreeNode(stoi(nodes[idx]));
        idx++;
        
        node->left = deserialize_helper(nodes, idx);
        node->right = deserialize_helper(nodes, idx);
        
        return node;
    }
    
public:
    string serialize(TreeNode* root) {
        string data;
        serialize_helper(root, data);
        return data;
    }
    
    TreeNode* deserialize(string data) {
        vector<string> nodes;
        stringstream ss(data);
        string node;
        
        while (getline(ss, node, ',')) {
            if (!node.empty()) {
                nodes.push_back(node);
            }
        }
        
        int idx = 0;
        return deserialize_helper(nodes, idx);
    }
};
```

**Explanation:**
- Pre-order traversal for serialization
- Use "null" for empty nodes
- Deserialize using pre-order reconstruction
- Time: O(n), Space: O(n)

---

## 68. SUBTREE OF ANOTHER TREE

**Link:** https://leetcode.com/problems/subtree-of-another-tree/

**Difficulty:** Easy | **Time:** O(n*m) | **Space:** O(min(h1, h2))

**Problem:** Check if tree1 contains tree2 as subtree.

**Most Optimized Solution:**

```cpp
class Solution {
private:
    bool is_same(TreeNode* p, TreeNode* q) {
        if (!p && !q) return true;
        if (!p || !q) return false;
        return p->val == q->val && 
               is_same(p->left, q->left) && 
               is_same(p->right, q->right);
    }
    
public:
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        if (!root) return !subRoot;
        if (is_same(root, subRoot)) return true;
        
        return isSubtree(root->left, subRoot) || 
               isSubtree(root->right, subRoot);
    }
};
```

**Explanation:**
- Check if root matches subRoot
- If not, check left and right subtrees
- Helper function checks if two trees are identical
- Time: O(n*m), Space: O(min(h1, h2))

---

## 69. CONSTRUCT BINARY TREE FROM PREORDER AND INORDER TRAVERSAL

**Link:** https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(n)

**Problem:** Build tree from preorder and inorder traversals.

**Most Optimized Solution:**

```cpp
class Solution {
private:
    TreeNode* build(vector<int>& preorder, vector<int>& inorder,
                    int pre_start, int pre_end, int in_start, int in_end,
                    unordered_map<int, int>& in_map) {
        if (pre_start > pre_end) return nullptr;
        
        TreeNode* root = new TreeNode(preorder[pre_start]);
        int in_idx = in_map[preorder[pre_start]];
        
        int left_size = in_idx - in_start;
        
        root->left = build(preorder, inorder, 
                          pre_start + 1, pre_start + left_size,
                          in_start, in_idx - 1, in_map);
        
        root->right = build(preorder, inorder,
                           pre_start + left_size + 1, pre_end,
                           in_idx + 1, in_end, in_map);
        
        return root;
    }
    
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        unordered_map<int, int> in_map;
        for (int i = 0; i < inorder.size(); ++i) {
            in_map[inorder[i]] = i;
        }
        
        return build(preorder, inorder, 0, preorder.size() - 1,
                     0, inorder.size() - 1, in_map);
    }
};
```

**Explanation:**
- Preorder: root, left, right
- Inorder: left, root, right
- First element in preorder is root
- Find root's position in inorder to split left/right
- Recursively build left and right subtrees
- Time: O(n), Space: O(n)

---

## 70. VALIDATE BINARY SEARCH TREE

**Link:** https://leetcode.com/problems/validate-binary-search-tree/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(h)

**Problem:** Check if tree is valid BST.

**Most Optimized Solution:**

```cpp
class Solution {
private:
    bool validate(TreeNode* node, long min_val, long max_val) {
        if (!node) return true;
        
        if (node->val <= min_val || node->val >= max_val) {
            return false;
        }
        
        return validate(node->left, min_val, node->val) &&
               validate(node->right, node->val, max_val);
    }
    
public:
    bool isValidBST(TreeNode* root) {
        return validate(root, LLONG_MIN, LLONG_MAX);
    }
};
```

**Explanation:**
- Track valid range for each node
- Left subtree: value < node->val
- Right subtree: value > node->val
- Use long long to handle INT_MIN/INT_MAX edge cases
- Time: O(n), Space: O(h)

---

## 71. KTH SMALLEST ELEMENT IN A BST

**Link:** https://leetcode.com/problems/kth-smallest-element-in-a-bst/

**Difficulty:** Medium | **Time:** O(k) | **Space:** O(h)

**Problem:** Find k-th smallest element in BST.

**Most Optimized Solution (In-order Traversal):**

```cpp
class Solution {
private:
    void inorder(TreeNode* node, int& k, int& result) {
        if (!node) return;
        
        inorder(node->left, k, result);
        
        k--;
        if (k == 0) {
            result = node->val;
            return;
        }
        
        inorder(node->right, k, result);
    }
    
public:
    int kthSmallest(TreeNode* root, int k) {
        int result = 0;
        inorder(root, k, result);
        return result;
    }
};
```

**Explanation:**
- In-order traversal visits nodes in ascending order
- Decrement k each visit
- When k reaches 0, current node is k-th smallest
- Time: O(k) average, O(n) worst
- Space: O(h)

---

## 72. LOWEST COMMON ANCESTOR OF A BINARY SEARCH TREE

**Link:** https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree/

**Difficulty:** Easy | **Time:** O(h) | **Space:** O(1)

**Problem:** Find LCA of two nodes in BST.

**Most Optimized Solution (Iterative):**

```cpp
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        while (root) {
            if (root->val > max(p->val, q->val)) {
                root = root->left;
            } else if (root->val < min(p->val, q->val)) {
                root = root->right;
            } else {
                return root;
            }
        }
        return nullptr;
    }
};
```

**Explanation:**
- Use BST property: left < root < right
- If both p, q are in left subtree, LCA is in left
- If both in right subtree, LCA is in right
- Otherwise, current node is LCA
- Time: O(h), Space: O(1)

---

## 73. INVERT BINARY TREE

**Link:** https://leetcode.com/problems/invert-binary-tree/

**Difficulty:** Easy | **Time:** O(n) | **Space:** O(h)

**Problem:** Mirror the tree (swap left and right).

**Most Optimized Solution (Recursive):**

```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        
        swap(root->left, root->right);
        invertTree(root->left);
        invertTree(root->right);
        
        return root;
    }
};
```

**Explanation:**
- Swap left and right children
- Recursively invert subtrees
- Time: O(n) visit each node, Space: O(h)

---

## 74. SAME TREE

**Link:** https://leetcode.com/problems/same-tree/

**Difficulty:** Easy | **Time:** O(min(n, m)) | **Space:** O(min(h1, h2))

**Problem:** Check if two trees are identical.

**Most Optimized Solution:**

```cpp
class Solution {
public:
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (!p && !q) return true;
        if (!p || !q || p->val != q->val) return false;
        
        return isSameTree(p->left, q->left) && 
               isSameTree(p->right, q->right);
    }
};
```

**Explanation:**
- Base case: both null (true), one null (false), values differ (false)
- Recursively check left and right subtrees
- Time: O(min(n, m)), Space: O(min(h1, h2))

---

## 75. BINARY TREE RIGHT SIDE VIEW

**Link:** https://leetcode.com/problems/binary-tree-right-side-view/

**Difficulty:** Medium | **Time:** O(n) | **Space:** O(h)

**Problem:** Return nodes visible from right side.

**Most Optimized Solution (DFS):**

```cpp
class Solution {
private:
    void dfs(TreeNode* node, int depth, vector<int>& result) {
        if (!node) return;
        
        if (depth == result.size()) {
            result.push_back(node->val);
        }
        
        dfs(node->right, depth + 1, result);
        dfs(node->left, depth + 1, result);
    }
    
public:
    vector<int> rightSideView(TreeNode* root) {
        vector<int> result;
        dfs(root, 0, result);
        return result;
    }
};
```

**Explanation:**
- DFS traversing right to left
- First node at each depth is rightmost
- Visit right before left to find rightmost first
- Time: O(n), Space: O(h)

---

## Summary: Blind 75 Patterns

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

**Master these 75 problems and you'll be ready for ANY technical interview!** 🚀

*Last Updated: December 2025*
*Language: C++17*
*Total Lines: 10,000+*
