/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

#include <vector>
#include <algorithm>

class Solution {
public:
    int rob(std::vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        if (nums.size() == 1) {
            return nums[0];
        }

        return std::max(robRange(nums, 0, nums.size() - 1),
                        robRange(nums, 1, nums.size()));
    }

private:
    int robRange(const std::vector<int>& nums, size_t start, size_t end) {
        int prev2 = 0;
        int prev1 = 0;

        for (size_t i = start; i < end; ++i) {
            int current = std::max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }
};
