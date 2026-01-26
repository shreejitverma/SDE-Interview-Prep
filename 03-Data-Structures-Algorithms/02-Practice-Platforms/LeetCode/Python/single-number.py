# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

# Time:  O(n)
# Space: O(1)

import operator
from functools import reduce


class Solution(object):
    """
    :type nums: List[int]
    :rtype: int
    """
    def singleNumber(self, A):
        return reduce(operator.xor, A)

