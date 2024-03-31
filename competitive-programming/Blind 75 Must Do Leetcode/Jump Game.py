'''https://leetcode.com/problems/jump-game/
55. Jump Game
Medium

8538

507

Add to List

Share
You are given an integer array nums. You are initially positioned at the array's first index, and each element in the array represents your maximum jump length at that position.

Return true if you can reach the last index, or false otherwise.

 

Example 1:

Input: nums = [2,3,1,1,4]
Output: true
Explanation: Jump 1 step from index 0 to 1, then 3 steps to the last index.
Example 2:

Input: nums = [3,2,1,0,4]
Output: false
Explanation: You will always arrive at index 3 no matter what. Its maximum jump length is 0, which makes it impossible to reach the last index.
 

Constraints:

1 <= nums.length <= 104
0 <= nums[i] <= 105'''

# Time:  O(n)
# Space: O(1)


from typing import List


class Solution(object):
    # @param A, a list of integers
    # @return a boolean
    def canJump(self, A):
        reachable = 0
        for i, length in enumerate(A):
            if i > reachable:
                break
            reachable = max(reachable, i + length)
        return reachable >= len(A) - 1


class Solution:
    def canJump(self, nums: List[int]) -> bool:
        far = 0
        for i in range(len(nums)):
            if i <= far:
                far = max(far, nums[i] + i)
            else:
                return False
        return True

    def funcname(self, nums):
        m = 0
        for i, n in enumerate(nums):
            if i > m:
                return False
            m = max(m, i+n)
        return True


if __name__ == "__main__":
    s = Solution()
    print(s.canJump([2, 0, 0]))
