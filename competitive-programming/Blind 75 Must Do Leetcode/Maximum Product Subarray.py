'''https://leetcode.com/problems/maximum-product-subarray/
152. Maximum Product Subarray
Medium

8690

272

Add to List

Share
Given an integer array nums, find a contiguous non-empty subarray within the array that has the largest product, and return the product.

It is guaranteed that the answer will fit in a 32-bit integer.

A subarray is a contiguous subsequence of the array.

 

Example 1:

Input: nums = [2,3,-2,4]
Output: 6
Explanation: [2,3] has the largest product 6.
Example 2:

Input: nums = [-2,0,-1]
Output: 0
Explanation: The result cannot be 2, because [-2,-1] is not a subarray.
 

Constraints:

1 <= nums.length <= 2 * 104
-10 <= nums[i] <= 10
The product of any prefix or suffix of nums is guaranteed to fit in a 32-bit integer.'''


class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        i_max, i_min = 1, 1
        res = -float('inf')
        for num in nums:
            i_max, i_min = (i_max, i_min) if num > 0 else(i_min, i_max)
            i_max = max(i_max * num, num)
            i_min = min(i_min * num, num)
            res = max(i_max, res)
        return res
