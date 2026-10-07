/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

#include <vector>
#include <unordered_set>
#include <algorithm>

class Solution {
public:
    int longestConsecutive(std::vector<int>& nums) {
        std::unordered_set<int> num_set(nums.begin(), nums.end());
        int longest_streak = 0;

        for (int num : num_set) {
            // Only attempt to build sequences from the start of a streak
            if (!num_set.count(num - 1)) {
                int current_num = num;
                int current_streak = 1;

                while (num_set.count(current_num + 1)) {
                    current_num += 1;
                    current_streak += 1;
                }

                longest_streak = std::max(longest_streak, current_streak);
            }
        }

        return longest_streak;
    }
};
