/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(log(m * n))
// Space: O(1)

#include <vector>

class Solution {
public:
    bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
        if (matrix.empty() || matrix[0].empty()) {
            return false;
        }

        const int m = static_cast<int>(matrix.size());
        const int n = static_cast<int>(matrix[0].size());
        int left = 0;
        int right = m * n - 1;

        while (left <= right) {
            const int mid = left + (right - left) / 2;
            const int val = matrix[mid / n][mid % n];
            if (val == target) {
                return true;
            } else if (val < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        return false;
    }
};
