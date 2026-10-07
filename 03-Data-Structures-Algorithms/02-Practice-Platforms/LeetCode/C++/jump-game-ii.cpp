#include <vector>
#include <algorithm>

using namespace std;

/*
 * Problem: LeetCode 45 - Jump Game II
 * Difficulty: Medium
 * Concepts: Greedy, BFS, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
public:
    int jump(vector<int>& nums) {
        int n = static_cast<int>(nums.size());
        if (n <= 1) {
            return 0;
        }

        int jumps = 0;
        int current_end = 0;
        int farthest = 0;

        for (int i = 0; i < n - 1; ++i) {
            farthest = max(farthest, i + nums[i]);
            if (i == current_end) {
                ++jumps;
                current_end = farthest;
                if (current_end >= n - 1) {
                    break;
                }
            }
        }

        return jumps;
    }
};
