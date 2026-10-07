/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(min(M, N))
// Space: O(1)

class Solution {
    public int uniquePaths(int m, int n) {
        int totalSteps = m + n - 2;
        int k = Math.min(m - 1, n - 1);
        long result = 1;

        for (int i = 1; i <= k; i++) {
            result = result * (totalSteps - k + i) / i;
        }

        return (int) result;
    }
}
