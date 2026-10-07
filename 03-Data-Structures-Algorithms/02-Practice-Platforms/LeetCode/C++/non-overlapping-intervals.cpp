/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) auxiliary

#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int eraseOverlapIntervals(vector<vector<int>>& intervals) {
        if (intervals.empty()) {
            return 0;
        }

        // Sort intervals by end time ascending
        sort(intervals.begin(), intervals.end(), [](const vector<int>& a, const vector<int>& b) {
            return a[1] < b[1];
        });

        int removals = 0;
        int prev_end = intervals[0][1];

        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] < prev_end) {
                ++removals;
            } else {
                prev_end = intervals[i][1];
            }
        }

        return removals;
    }
};
