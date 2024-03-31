/*
https://leetcode.com/problems/non-overlapping-intervals/
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
-5 * 104 <= starti < endi <= 5 * 104
*/

//Greedy, maintain the shortest intervals
/*
the algorithm is not optimal!
consider [[-3,3],[4,8]] and [2,5],
this algorithm will choose [2,5],
which is sub-optimal!
*/
//WA
//15 / 18 test cases passed.
class Solution
{
public:
    int eraseOverlapIntervals(vector<vector<int> > &intervals)
    {
        set<int> points;

        int n = intervals.size();

        for (int i = 0; i < n; ++i)
        {
            points.insert(intervals[i][0]);
            points.insert(intervals[i][1]);
        }

        unordered_map<int, int> shrink;

        auto it = points.begin();
        for (int i = 0; i < points.size(); ++i, ++it)
        {
            shrink[*it] = i;
        }

        for (vector<int> &interval : intervals)
        {
            interval[0] = shrink[interval[0]];
            interval[1] = shrink[interval[1]];
        }

        sort(intervals.begin(), intervals.end(),
             [](vector<int> &a, vector<int> &b)
             {
                 return (a[1] - a[0] == b[1] - b[0]) ? a[0] < b[0] : a[1] - a[0] < b[1] - b[0];
             });

        unordered_set<int> used;

        int discarded = 0;

        for (int i = 0; i < n; ++i)
        {
            bool overlapping = false;
            for (int p = intervals[i][0]; p < intervals[i][1]; ++p)
            {
                if (used.find(p) != used.end())
                {
                    overlapping = true;
                    break;
                }
            }
            if (!overlapping)
            {
                for (int p = intervals[i][0]; p < intervals[i][1]; ++p)
                {
                    used.insert(p);
                }
            }
            else
            {
                ++discarded;
            }
        }

        return discarded;
    }
};

//Greedy, maintain the intervals with minimum right boundary
//proof: https://en.wikipedia.org/wiki/Interval_scheduling#Interval_Scheduling_Maximization
//http://lonati.di.unimi.it/algo/0910/lab/kowalski6.pdf
//https://leetcode.com/problems/non-overlapping-intervals/discuss/91713/Java%3A-Least-is-Most
//Runtime: 40 ms, faster than 59.83% of C++ online submissions for Non-overlapping Intervals.
//Memory Usage: 10.7 MB, less than 32.78% of C++ online submissions for Non-overlapping Intervals.
//time: O(NlogN)
class Solution
{
public:
    int eraseOverlapIntervals(vector<vector<int> > &intervals)
    {
        int n = intervals.size();

        sort(intervals.begin(), intervals.end(),
             [](vector<int> &a, vector<int> &b)
             {
                 return a[1] < b[1];
             });

        int discarded = 0;
        int last_end = INT_MIN;

        for (vector<int> interval : intervals)
        {
            if (last_end > interval[0])
                ++discarded;
            else
                last_end = interval[1];
        }

        return discarded;
    }
};