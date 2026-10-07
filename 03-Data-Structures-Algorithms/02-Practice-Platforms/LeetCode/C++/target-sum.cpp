#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

/*
 * Problem: LeetCode 494 - Target Sum
 * Difficulty: Medium
 * Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum
 *
 * Time Complexity: O(n * S) where S = (total_sum + target) / 2
 * Space Complexity: O(S)
 */

class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int total_sum = accumulate(nums.begin(), nums.end(), 0);

        // sum(P) - sum(N) = target
        // sum(P) + sum(N) = total_sum
        // 2 * sum(P) = total_sum + target
        if (total_sum < abs(target) || (total_sum + target) % 2 != 0) {
            return 0;
        }

        int subset_sum = (total_sum + target) / 2;
        vector<int> dp(subset_sum + 1, 0);
        dp[0] = 1;

        for (int num : nums) {
            for (int s = subset_sum; s >= num; --s) {
                dp[s] += dp[s - num];
            }
        }

        return dp[subset_sum];
    }
};
