'''https://leetcode.com/problems/missing-number/
268. Missing Number
Easy

4055

2698

Add to List

Share
Given an array nums containing n distinct numbers in the range [0, n], return the only number in the range that is missing from the array.

 

Example 1:

Input: nums = [3,0,1]
Output: 2
Explanation: n = 3 since there are 3 numbers, so all numbers are in the range [0,3]. 2 is the missing number in the range since it does not appear in nums.
Example 2:

Input: nums = [0,1]
Output: 2
Explanation: n = 2 since there are 2 numbers, so all numbers are in the range [0,2]. 2 is the missing number in the range since it does not appear in nums.
Example 3:

Input: nums = [9,6,4,2,3,5,7,0,1]
Output: 8
Explanation: n = 9 since there are 9 numbers, so all numbers are in the range [0,9]. 8 is the missing number in the range since it does not appear in nums.
Example 4:

Input: nums = [0]
Output: 1
Explanation: n = 1 since there is 1 number, so all numbers are in the range [0,1]. 1 is the missing number in the range since it does not appear in nums.
 

Constraints:

n == nums.length
1 <= n <= 104
0 <= nums[i] <= n
All the numbers of nums are unique.
 

Follow up: 
Could you implement a solution using only O(1) extra space complexity and O(n) runtime complexity?'''

from typing import List


def missingNumber(nums: List[int]) -> int:
    nums = sorted(nums)
    for idx, item in enumerate(nums):
        if idx != item:
            return idx
    return nums[-1]+1


def CoountingSort(nums: List[int]) -> int:
    arr = [None for _ in range(len(nums)+1)]
    for item in nums:
        arr[item] = item
    for idx, item in enumerate(arr):
        if item is None:
            return idx+1


def Bit_Manipulation(nums: List[int]) -> int:
    missing = len(nums)
    for i, num in enumerate(nums):
        missing ^= i ^ num
    return missing


def Gauss_Formula(nums: List[int]) -> int:
    expected_sum = len(nums)*(len(nums)+1)//2
    actual_sum = sum(nums)
    return expected_sum - actual_sum
