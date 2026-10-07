/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N * W)
// Space Complexity: O(W) with 1D array space optimization

package main

import "fmt"

func knapsack01(weights []int, values []int, capacity int) int {
	n := len(weights)
	dp := make([]int, capacity+1)

	for i := 0; i < n; i++ {
		wt := weights[i]
		val := values[i]
		for w := capacity; w >= wt; w-- {
			if dp[w-wt]+val > dp[w] {
				dp[w] = dp[w-wt] + val
			}
		}
	}

	return dp[capacity]
}

func main() {
	weights := []int{10, 20, 30}
	values := []int{60, 100, 120}
	capacity := 50

	fmt.Println("Max value in 0-1 Knapsack:", knapsack01(weights, values, capacity)) // 220
}
