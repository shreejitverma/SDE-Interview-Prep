/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(min(M, N))
// Space: O(1)

#include <algorithm>

class Solution {
public:
    int uniquePaths(int m, int n) {
        // Compute (m + n - 2) choose (min(m - 1, n - 1))
        int total_steps = m + n - 2;
        int k = std::min(m - 1, n - 1);
        long long result = 1;

        for (int i = 1; i <= k; ++i) {
            result = result * (total_steps - k + i) / i;
        }

        return static_cast<int>(result);
    }
};
