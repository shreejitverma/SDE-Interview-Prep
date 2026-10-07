/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N * W)
// Space Complexity: O(W) with 1D array space optimization

function knapsack01(weights: number[], values: number[], capacity: number): number {
    const n = weights.length;
    const dp = new Array<number>(capacity + 1).fill(0);

    for (let i = 0; i < n; i++) {
        const wt = weights[i];
        const val = values[i];
        for (let w = capacity; w >= wt; w--) {
            dp[w] = Math.max(dp[w], dp[w - wt] + val);
        }
    }

    return dp[capacity];
}

// Example usage
const weights = [10, 20, 30];
const values = [60, 100, 120];
const capacity = 50;

console.log("Max value in 0-1 Knapsack:", knapsack01(weights, values, capacity)); // 220
