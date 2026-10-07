/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^(T/M + 1)) where N = candidates count, T = target, M = min(candidates)
// Space: O(T/M) auxiliary (recursion stack depth)

#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<int>> combinationSum(std::vector<int>& candidates, int target) {
        std::sort(candidates.begin(), candidates.end());
        std::vector<std::vector<int>> result;
        std::vector<int> current;
        backtrack(candidates, target, 0, current, result);
        return result;
    }

private:
    void backtrack(const std::vector<int>& candidates, int remain, size_t start,
                   std::vector<int>& current, std::vector<std::vector<int>>& result) {
        if (remain == 0) {
            result.push_back(current);
            return;
        }

        for (size_t i = start; i < candidates.size(); ++i) {
            if (candidates[i] > remain) {
                break; // Early pruning on sorted candidates
            }
            current.push_back(candidates[i]);
            backtrack(candidates, remain - candidates[i], i, current, result);
            current.pop_back();
        }
    }
};
