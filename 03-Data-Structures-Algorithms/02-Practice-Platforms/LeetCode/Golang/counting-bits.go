/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 338 - Counting Bits
 * Difficulty: Easy
 * Language: Go (Golang)
 *
 * Performance Analysis:
 * - Time Complexity: O(N) single-pass dynamic programming.
 * - Space Complexity: O(1) auxiliary space (excluding output slice).
 */

package main

import "fmt"

func countBits(n int) []int {
	ans := make([]int, n+1)
	for i := 1; i <= n; i++ {
		ans[i] = ans[i>>1] + (i & 1)
	}
	return ans
}

func countBitsKernighan(n int) []int {
	ans := make([]int, n+1)
	for i := 1; i <= n; i++ {
		ans[i] = ans[i&(i-1)] + 1
	}
	return ans
}

func main() {
	fmt.Println("countBits(2):", countBits(2)) // [0 1 1]
	fmt.Println("countBits(5):", countBits(5)) // [0 1 1 2 1 2]
}
