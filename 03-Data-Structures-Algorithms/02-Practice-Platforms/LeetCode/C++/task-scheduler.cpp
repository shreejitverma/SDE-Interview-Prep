#include <vector>
#include <algorithm>

using namespace std;

/*
 * Problem: LeetCode 621 - Task Scheduler
 * Difficulty: Medium
 * Concepts: Greedy, Counting, Math
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1) (fixed 26-element array)
 */

class Solution {
public:
    int leastInterval(vector<char>& tasks, int n) {
        int freq[26] = {0};
        int max_freq = 0;

        for (char task : tasks) {
            int count = ++freq[task - 'A'];
            max_freq = max(max_freq, count);
        }

        int max_count = 0;
        for (int count : freq) {
            if (count == max_freq) {
                ++max_count;
            }
        }

        int calculated_intervals = (max_freq - 1) * (n + 1) + max_count;
        return max(static_cast<int>(tasks.size()), calculated_intervals);
    }
};
