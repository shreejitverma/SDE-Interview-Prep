'''https://leetcode.com/problems/reorganize-string/
767. Reorganize String
Medium

3787

163

Add to List

Share
Given a string s, rearrange the characters of s so that any two adjacent characters are not the same.

Return any possible rearrangement of s or return "" if not possible.

 

Example 1:

Input: s = "aab"
Output: "aba"
Example 2:

Input: s = "aaab"
Output: ""
 

Constraints:

1 <= s.length <= 500
s consists of lowercase English letters.'''
from collections import defaultdict


class Solution:
    def reorganizeString(self, S: str) -> str:
        """
        piles by max char and circular append
        """
        counter = defaultdict(int)
        for c in S:
            counter[c] += 1

        lst = [
            (-n, n, c)
            for c, n in counter.items()
        ]
        lst.sort()
        piles = []
        _, n, c = lst[0]
        for i in range(n):
            piles.append([c])

        cnt = 0
        for _, n, c in lst[1:]:
            for _ in range(n):
                piles[cnt].append(c)
                cnt = (cnt + 1) % len(piles)

        if len(piles) > 1 and len(piles[-2]) == 1:
            return ""

        return "".join(
            map(lambda x: "".join(x), piles)
        )


if __name__ == "__main__":
    assert Solution().reorganizeString("vvvlo") == "vlvov"
    assert Solution().reorganizeString("aab") == "aba"
    assert Solution().reorganizeString("aaab") == ""
# Time:  O(nloga) = O(n), a is the size of alphabet
# Space: O(a) = O(1)

# Given a string S, check if the letters can be rearranged
# so that two characters that are adjacent to each other are not the same.
#
# If possible, output any possible result.  If not possible, return the empty string.
#
# Example 1:
#
# Input: S = "aab"
# Output: "aba"
# Example 2:
#
# Input: S = "aaab"
# Output: ""
#
# Note:
# - S will consist of lowercase letters and have length in range [1, 500].


class Solution(object):
    def reorganizeString(self, S):
        """
        :type S: str
        :rtype: str
        """
        counts = collections.Counter(S)
        if any(v > (len(S)+1)/2 for k, v in counts.iteritems()):
            return ""

        result = []
        max_heap = []
        for k, v in counts.iteritems():
            heapq.heappush(max_heap, (-v, k))
        while len(max_heap) > 1:
            count1, c1 = heapq.heappop(max_heap)
            count2, c2 = heapq.heappop(max_heap)
            if not result or c1 != result[-1]:
                result.extend([c1, c2])
                if count1+1:
                    heapq.heappush(max_heap, (count1+1, c1))
                if count2+1:
                    heapq.heappush(max_heap, (count2+1, c2))
        return "".join(result) + (max_heap[0][1] if max_heap else '')
