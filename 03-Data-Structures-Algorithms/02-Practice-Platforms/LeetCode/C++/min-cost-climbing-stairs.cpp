/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

#include <vector>
#include <algorithm>

class Solution {
public:
    int minCostClimbingStairs(const std::vector<int>& cost) {
        int prev2 = 0;
        int prev1 = 0;

        for (int c : cost) {
            const int curr = c + std::min(prev1, prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        return std::min(prev1, prev2);
    }
};
