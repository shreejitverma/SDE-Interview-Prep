/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N^3)
// Space Complexity: O(N^2)

#include <iostream>
#include <vector>
#include <climits>

int matrixChainOrder(const std::vector<int>& p) {
    int n = static_cast<int>(p.size()) - 1; // Number of matrices
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));

    // L is chain length
    for (int L = 2; L <= n; ++L) {
        for (int i = 1; i <= n - L + 1; ++i) {
            int j = i + L - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; ++k) {
                int cost = dp[i][k] + dp[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                }
            }
        }
    }

    return dp[1][n];
}

int main() {
    std::vector<int> arr = {1, 2, 3, 4, 3};
    std::cout << "Minimum number of multiplications is " << matrixChainOrder(arr) << std::endl;
    return 0;
}
