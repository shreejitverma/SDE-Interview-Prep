/*
https: // leetcode.com/problems/insert-interval/
57. Insert Interval
Medium

3758

290

Add to List

Share
You are given an array of non-overlapping intervals intervals where intervals[i] = [starti, endi] represent the start and the end of the ith interval and intervals is sorted in ascending order by starti. You are also given an interval newInterval = [start, end] that represents the start and end of another interval.

Insert newInterval into intervals such that intervals is still sorted in ascending order by starti and intervals still does not have any overlapping intervals(merge overlapping intervals if necessary).

Return intervals after the insertion.


Example 1:

Input: intervals = [[1, 3], [6, 9]], newInterval = [2, 5]
Output: [[1, 5], [6, 9]]
Example 2:

Input: intervals = [[1, 2], [3, 5], [6, 7], [8, 10], [12, 16]], newInterval = [4, 8]
Output: [[1, 2], [3, 10], [12, 16]]
Explanation: Because the new interval[4, 8] overlaps with [3, 5], [6, 7], [8, 10].
Example 3:

Input: intervals = [], newInterval = [5, 7]
Output: [[5, 7]]
Example 4:

Input: intervals = [[1, 5]], newInterval = [2, 3]
Output: [[1, 5]]
Example 5:

Input: intervals = [[1, 5]], newInterval = [2, 7]
Output: [[1, 7]]


Constraints:

0 <= intervals.length <= 104
intervals[i].length == 2
0 <= starti <= endi <= 105
intervals is sorted by starti in ascending order.
newInterval.length == 2
0 <= start <= end <= 105

*/

//Runtime: 32 ms, faster than 70.39% of C++ online submissions for Insert Interval.
//Memory Usage: 17.1 MB, less than 42.94% of C++ online submissions for Insert Interval.
class Solution
{
public:
    vector<vector<int> > insert(vector<vector<int> > &intervals, vector<int> &newInterval)
    {
        vector<int> start_dummy = {-1, newInterval[0]};
        vector<int> end_dummy = {newInterval[1], -1};

        //the smallest interval with end >= start_dummy[1]
        auto it1 = lower_bound(intervals.begin(), intervals.end(),
                               start_dummy,
                               [](const vector<int> &a, const vector<int> &b)
                               {
                                   return a[1] < b[1];
                               });

        if (it1 == intervals.end())
        {
            intervals.push_back(newInterval);
            return intervals;
        }
        // cout << (*it1)[0] << ", " << (*it1)[1] << endl;

        //the smallest interval with start > end_dummy[0]
        auto it2 = upper_bound(intervals.begin(), intervals.end(),
                               end_dummy,
                               [](const vector<int> &a, const vector<int> &b)
                               {
                                   return a[0] < b[0];
                               });
        if (it2 == intervals.begin())
        {
            intervals.insert(intervals.begin(), newInterval);
            return intervals;
        }

        //the largest interval with start <= end_dummy[0]
        it2 = prev(it2);

        // cout << (*it2)[0] << ", " << (*it2)[1] << endl;

        if (it1 == it2)
        {
            *it1 = {min((*it1)[0], newInterval[0]),
                    max((*it1)[1], newInterval[1])};
        }
        else if (it1 < it2)
        {
            vector<int> insertInterval = {
                min((*it1)[0], newInterval[0]),
                max((*it2)[1], newInterval[1])};
            *it1 = insertInterval;
            //erase [it1+1, it2]
            intervals.erase(it1 + 1, it2 + 1);
        }
        else
        {
            //it2 < it1
            //newInterval should be insert btw it2 and it1
            intervals.insert(it1, newInterval);
        }

        return intervals;
    }
};

//https://leetcode.com/problems/insert-interval/discuss/21602/Short-and-straight-forward-Java-solution
//Runtime: 28 ms, faster than 87.60% of C++ online submissions for Insert Interval.
//Memory Usage: 16.9 MB, less than 89.34% of C++ online submissions for Insert Interval.
class Solution
{
public:
    vector<vector<int> > insert(vector<vector<int> > &intervals, vector<int> &newInterval)
    {
        vector<vector<int> > ans;
        int i = 0, n = intervals.size();

        //smaller, no intersection
        while (i < n && intervals[i][1] < newInterval[0])
        {
            ans.push_back(intervals[i]);
            ++i;
        }

        //having intersection
        int start = newInterval[0], end = newInterval[1];
        while (i < n && intervals[i][0] <= end)
        {
            start = min(intervals[i][0], start);
            end = max(intervals[i][1], end);
            ++i;
        }

        ans.push_back({start, end});

        while (i < n)
        {
            ans.push_back(intervals[i]);
            ++i;
        }

        return ans;
    }
};