# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N^(T/M + 1)) where N = candidates count, T = target, M = min(candidates)
# Space: O(T/M) auxiliary (recursion stack depth)

from typing import List


class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        candidates.sort()
        result: List[List[int]] = []
        path: List[int] = []

        def backtrack(remain: int, start: int) -> None:
            if remain == 0:
                result.append(list(path))
                return

            for i in range(start, len(candidates)):
                if candidates[i] > remain:
                    break
                path.append(candidates[i])
                backtrack(remain - candidates[i], i)
                path.pop()

        backtrack(target, 0)
        return result
