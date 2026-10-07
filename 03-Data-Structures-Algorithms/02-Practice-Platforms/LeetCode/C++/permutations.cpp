/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * N!)
// Space: O(N) auxiliary (recursion stack depth)

#include <vector>
#include <utility>

class Solution {
public:
    std::vector<std::vector<int>> permute(std::vector<int>& nums) {
        std::vector<std::vector<int>> result;
        backtrack(nums, 0, result);
        return result;
    }

private:
    void backtrack(std::vector<int>& nums, size_t first, std::vector<std::vector<int>>& result) {
        if (first == nums.size()) {
            result.push_back(nums);
            return;
        }

        for (size_t i = first; i < nums.size(); ++i) {
            std::swap(nums[first], nums[i]);
            backtrack(nums, first + 1, result);
            std::swap(nums[first], nums[i]);
        }
    }
};
