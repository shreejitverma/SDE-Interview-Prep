#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
 * Problem: LeetCode 72 - Edit Distance
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(min(m, n))
 */

class Solution {
public:
    int minDistance(string word1, string word2) {
        int m = static_cast<int>(word1.length());
        int n = static_cast<int>(word2.length());

        // Ensure word2 is the shorter string for O(min(m, n)) space
        if (m < n) {
            return minDistance(word2, word1);
        }

        vector<int> dp(n + 1);
        for (int j = 0; j <= n; ++j) {
            dp[j] = j;
        }

        for (int i = 1; i <= m; ++i) {
            int prev_diag = dp[0];
            dp[0] = i;

            for (int j = 1; j <= n; ++j) {
                int temp = dp[j];
                if (word1[i - 1] == word2[j - 1]) {
                    dp[j] = prev_diag;
                } else {
                    dp[j] = 1 + min({dp[j], dp[j - 1], prev_diag});
                }
                prev_diag = temp;
            }
        }

        return dp[n];
    }
};
