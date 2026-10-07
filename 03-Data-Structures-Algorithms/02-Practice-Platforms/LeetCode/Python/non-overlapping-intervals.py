# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N log N)
# Space: O(1) or O(N) depending on Timsort

from typing import List

class Solution:
    def eraseOverlapIntervals(self, intervals: List[List[int]]) -> int:
        if not intervals:
            return 0

        # Sort intervals by end time ascending
        intervals.sort(key=lambda x: x[1])

        removals = 0
        prev_end = intervals[0][1]

        for i in range(1, len(intervals)):
            if intervals[i][0] < prev_end:
                removals += 1
            else:
                prev_end = intervals[i][1]

        return removals
