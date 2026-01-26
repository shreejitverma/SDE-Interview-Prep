/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: 0/1 Knapsack Problem (Dynamic Programming)
 * Description: Given weights and values of n items, put these items in a knapsack of capacity W 
 *           to get the maximum total value in the knapsack.
 * 
 * Complexity: O(N * W) Time, O(N * W) Space
 */

#include <iostream>
#include <vector>
#include <algorithm>

int knapsack(int W, const std::vector<int>& wt, const std::vector<int>& val, int n) {
    // dp[i][w] = Max value using first 'i' items with capacity 'w'
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1));

    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0) {
                dp[i][w] = 0; // Base case
            }
            else if (wt[i - 1] <= w) {
                // Choice: Include item OR Exclude item
                // val[i-1] + dp[i-1][w - wt[i-1]]  VS  dp[i-1][w]
                dp[i][w] = std::max(val[i - 1] + dp[i - 1][w - wt[i - 1]], 
                                    dp[i - 1][w]);
            }
            else {
                // Cannot include item
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    return dp[n][W];
}

int main() {
    std::vector<int> val = {60, 100, 120};
    std::vector<int> wt = {10, 20, 30};
    int W = 50;
    int n = val.size();

    std::cout << "Max Value: " << knapsack(W, wt, val, n) << "\n"; // Output: 220
    return 0;
}
