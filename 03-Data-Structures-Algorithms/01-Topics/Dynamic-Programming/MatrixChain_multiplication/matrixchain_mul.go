/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N^3)
// Space Complexity: O(N^2)

package main

import (
	"fmt"
	"math"
)

func matrixChainOrder(p []int) int {
	n := len(p) - 1
	dp := make([][]int, n+1)
	for i := range dp {
		dp[i] = make([]int, n+1)
	}

	for L := 2; L <= n; L++ {
		for i := 1; i <= n-L+1; i++ {
			j := i + L - 1
			dp[i][j] = math.MaxInt32
			for k := i; k < j; k++ {
				cost := dp[i][k] + dp[k+1][j] + p[i-1]*p[k]*p[j]
				if cost < dp[i][j] {
					dp[i][j] = cost
				}
			}
		}
	}

	return dp[1][n]
}

func main() {
	arr := []int{1, 2, 3, 4, 3}
	fmt.Println("Minimum number of multiplications is", matrixChainOrder(arr))
}
