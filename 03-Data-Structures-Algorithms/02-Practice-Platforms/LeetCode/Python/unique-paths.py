# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(min(M, N))
# Space: O(1)

import math


class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        return math.comb(m + n - 2, min(m - 1, n - 1))
