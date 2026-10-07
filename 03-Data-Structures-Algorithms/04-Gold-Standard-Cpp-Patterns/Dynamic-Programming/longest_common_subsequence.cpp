/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Longest Common Subsequence (LCS) & Sequence Reconstruction
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation demonstrating O(min(M, N)) rolling-array
 *              space optimization and exact sequence reconstruction.
 * 
 * Complexity:
 * - Length only: O(M * N) Time, O(min(M, N)) Space
 * - Full Reconstruction: O(M * N) Time, O(M * N) Space
 */

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <cassert>

class LCSSolver {
public:
    // Pure O(min(M, N)) space calculation for subsequence length
    static size_t length(std::string_view s1, std::string_view s2) {
        if (s1.length() < s2.length()) {
            std::swap(s1, s2); // Ensure s2 is the shorter string
        }
        size_t n = s2.length();
        std::vector<size_t> prev(n + 1, 0);
        std::vector<size_t> curr(n + 1, 0);

        for (char c1 : s1) {
            for (size_t j = 1; j <= n; ++j) {
                if (c1 == s2[j - 1]) {
                    curr[j] = prev[j - 1] + 1;
                } else {
                    curr[j] = std::max(prev[j], curr[j - 1]);
                }
            }
            std::swap(prev, curr);
        }
        return prev[n];
    }

    // Reconstructs one optimal longest common subsequence string
    static std::string reconstruct(std::string_view s1, std::string_view s2) {
        size_t m = s1.length();
        size_t n = s2.length();
        std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1, 0));

        for (size_t i = 1; i <= m; ++i) {
            for (size_t j = 1; j <= n; ++j) {
                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        // Backtrack from dp[m][n]
        std::string result;
        size_t i = m, j = n;
        while (i > 0 && j > 0) {
            if (s1[i - 1] == s2[j - 1]) {
                result.push_back(s1[i - 1]);
                --i;
                --j;
            } else if (dp[i - 1][j] >= dp[i][j - 1]) {
                --i;
            } else {
                --j;
            }
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
};

int main() {
    std::string s1 = "ABCBDAB";
    std::string s2 = "BDCABA";

    size_t len = LCSSolver::length(s1, s2);
    assert(len == 4);

    std::string subseq = LCSSolver::reconstruct(s1, s2);
    assert(subseq.length() == 4);
    // Valid 4-character LCS includes "BDAB", "BCBA", "BCAB"
    std::cout << "Computed LCS: " << subseq << "\n";

    std::cout << "All LCS tests passed successfully.\n";
    return 0;
}
