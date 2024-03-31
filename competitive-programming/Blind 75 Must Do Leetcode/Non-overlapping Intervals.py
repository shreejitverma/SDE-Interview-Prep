'''https://leetcode.com/problems/non-overlapping-intervals/
435. Non-overlapping Intervals
Medium

2867

79

Add to List

Share
Given an array of intervals intervals where intervals[i] = [starti, endi], return the minimum number of intervals you need to remove to make the rest of the intervals non-overlapping.

 

Example 1:

Input: intervals = [[1,2],[2,3],[3,4],[1,3]]
Output: 1
Explanation: [1,3] can be removed and the rest of the intervals are non-overlapping.
Example 2:

Input: intervals = [[1,2],[1,2],[1,2]]
Output: 2
Explanation: You need to remove two [1,2] to make the rest of the intervals non-overlapping.
Example 3:

Input: intervals = [[1,2],[2,3]]
Output: 0
Explanation: You don't need to remove any of the intervals since they're already non-overlapping.
 

Constraints:

1 <= intervals.length <= 105
intervals[i].length == 2
-5 * 104 <= starti < endi <= 5 * 104'''

# Time:  O(nlogn)
# Space: O(1)


class Solution(object):
    def eraseOverlapIntervals(self, intervals: List[List[int]]) -> int:
        size = len(intervals)
        if size <= 1:
            return 0
        intervals.sort(key=lambda x: x[1])
        first_end = intervals[0][1]
        res = 0
        for i in range(1, size):
            start, end = intervals[i]
            if start >= first_end:
                first_end = end
            else:
                res += 1
        return res
