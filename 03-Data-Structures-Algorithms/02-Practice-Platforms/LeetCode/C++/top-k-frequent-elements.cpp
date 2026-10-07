/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

#include <vector>
#include <unordered_map>

class Solution {
public:
    std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
        std::unordered_map<int, int> counts;
        for (int num : nums) {
            counts[num]++;
        }

        std::vector<std::vector<int>> buckets(nums.size() + 1);
        for (const auto& [num, freq] : counts) {
            buckets[freq].push_back(num);
        }

        std::vector<int> result;
        result.reserve(k);

        for (int i = static_cast<int>(buckets.size()) - 1; i >= 0 && static_cast<int>(result.size()) < k; --i) {
            for (int num : buckets[i]) {
                result.push_back(num);
                if (static_cast<int>(result.size()) == k) {
                    break;
                }
            }
        }

        return result;
    }
};
