# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

# Time:  O(n)
# Space: O(1)

import collections


class Solution(object):
    def canPermutePalindrome(self, s):
        """
        :type s: str
        :rtype: bool
        """
        return sum(v % 2 for v in collections.Counter(s).values()) < 2

