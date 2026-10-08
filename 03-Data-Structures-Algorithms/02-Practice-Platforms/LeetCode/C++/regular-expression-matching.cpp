#include <string>
#include <vector>

using namespace std;

/*
 * Problem: LeetCode 10 - Regular Expression Matching
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String, Recursion
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n)
 */

class Solution {
public:
    bool isMatch(string s, string p) {
        int m = static_cast<int>(s.length());
        int n = static_cast<int>(p.length());

        // dp[i][j] means s[0..i) matches p[0..j)
        vector<vector<bool>> dp(m + 1, vector<bool>(n + 1, false));
        dp[0][0] = true;

        // Initialize matching empty s against patterns like a*, a*b*, etc.
        for (int j = 2; j <= n; ++j) {
            if (p[j - 1] == '*') {
                dp[0][j] = dp[0][j - 2];
            }
        }

        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (p[j - 1] == '*') {
                    // Match 0 of preceding element
                    dp[i][j] = dp[i][j - 2];
                    // Match 1 or more if preceding character matches
                    if (p[j - 2] == '.' || p[j - 2] == s[i - 1]) {
                        dp[i][j] = dp[i][j] || dp[i - 1][j];
                    }
                } else if (p[j - 1] == '.' || p[j - 1] == s[i - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                }
            }
        }

        return dp[m][n];
    }
};
