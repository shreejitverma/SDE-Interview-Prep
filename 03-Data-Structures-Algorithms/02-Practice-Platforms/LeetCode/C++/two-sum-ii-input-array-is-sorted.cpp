/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

#include <vector>

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& numbers, int target) {
        int left = 0, right = static_cast<int>(numbers.size()) - 1;
        
        while (left < right) {
            const int sum = numbers[left] + numbers[right];
            if (sum == target) {
                return {left + 1, right + 1};
            } else if (sum < target) {
                ++left;
            } else {
                --right;
            }
        }

        return {-1, -1};
    }
};
