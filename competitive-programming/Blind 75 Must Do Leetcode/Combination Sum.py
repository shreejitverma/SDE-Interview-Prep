'''https://leetcode.com/problems/combination-sum/
39. Combination Sum
Medium

7761

186

Add to List

Share
Given an array of distinct integers candidates and a target integer target, return a list of all unique combinations of candidates where the chosen numbers sum to target. You may return the combinations in any order.

The same number may be chosen from candidates an unlimited number of times. Two combinations are unique if the frequency of at least one of the chosen numbers is different.

It is guaranteed that the number of unique combinations that sum up to target is less than 150 combinations for the given input.

 

Example 1:

Input: candidates = [2,3,6,7], target = 7
Output: [[2,2,3],[7]]
Explanation:
2 and 3 are candidates, and 2 + 2 + 3 = 7. Note that 2 can be used multiple times.
7 is a candidate, and 7 = 7.
These are the only two combinations.
Example 2:

Input: candidates = [2,3,5], target = 8
Output: [[2,2,2,2],[2,3,3],[3,5]]
Example 3:

Input: candidates = [2], target = 1
Output: []
Example 4:

Input: candidates = [1], target = 1
Output: [[1]]
Example 5:

Input: candidates = [1], target = 2
Output: [[1,1]]
 

Constraints:

1 <= candidates.length <= 30
1 <= candidates[i] <= 200
All elements of candidates are distinct.
1 <= target <= 500'''


# Time:  O(k * n^k)
# Space: O(k)

from typing import List
import bisect


class Solution(object):
    # @param candidates, a list of integers
    # @param target, integer
    # @return a list of lists of integers
    def combinationSum(self, candidates, target):
        result = []
        self.combinationSumRecu(sorted(candidates), result, 0, [], target)
        return result

    def combinationSumRecu(self, candidates, result, start, intermediate, target):
        if target == 0:
            result.append(list(intermediate))
        while start < len(candidates) and candidates[start] <= target:
            intermediate.append(candidates[start])
            self.combinationSumRecu(
                candidates, result, start, intermediate, target - candidates[start])
            intermediate.pop()
            start += 1


#


class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        candidates.sort()
        res = []
        self.biSearch(candidates, target, [], res)
        return res

    def search(self, candidates, target, path, res):
        if target < 0:
            return
        if target == 0:
            res.append(path)
            return
        for idx, item in enumerate(candidates):
            self.search(candidates[idx:], target-item, path+[item], res)

    def biSearch(self, candidates, target, path, res):
        if not candidates or target < 0:
            return
        if target == 0:
            res.append(path)
            return
        id_s = bisect.bisect_left(candidates, target)

        for idx, item in enumerate(candidates[:id_s+1]):
            self.biSearch(candidates[idx:id_s+1],
                          target-item, path+[item], res)


if __name__ == "__main__":
    s = Solution()
    res = s.combinationSum([2, 3, 5], 8)
    print(res)
