'''https://leetcode.com/problems/merge-intervals/
56. Merge Intervals
Medium

10310

447

Add to List

Share
Given an array of intervals where intervals[i] = [starti, endi], merge all overlapping intervals, and return an array of the non-overlapping intervals that cover all the intervals in the input.

 

Example 1:

Input: intervals = [[1,3],[2,6],[8,10],[15,18]]
Output: [[1,6],[8,10],[15,18]]
Explanation: Since intervals [1,3] and [2,6] overlaps, merge them into [1,6].
Example 2:

Input: intervals = [[1,4],[4,5]]
Output: [[1,5]]
Explanation: Intervals [1,4] and [4,5] are considered overlapping.
 

Constraints:

1 <= intervals.length <= 104
intervals[i].length == 2
0 <= starti <= endi <= 104'''

from typing import List


class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        if not intervals:
            return []
        intervals = sorted(intervals, key=lambda x: x[0])
        new_res = []
        left, right = intervals[0][0], intervals[0][1]
        for item in intervals[1:]:
            if left <= item[0] <= right:
                right = max(right, item[1])
            else:
                new_res.append([left, right])
                left, right = item[0], item[1]
        new_res.append([left, right])
        return new_res
