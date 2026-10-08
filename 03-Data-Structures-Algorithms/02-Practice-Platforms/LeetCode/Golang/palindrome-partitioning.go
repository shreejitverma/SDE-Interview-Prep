package main

/*
 * Problem: LeetCode 131 - Palindrome Partitioning
 * Difficulty: Medium
 * Concepts: Backtracking, String, Dynamic Programming
 *
 * Time Complexity: O(n * 2^n)
 * Space Complexity: O(n) recursion stack
 */

func partition(s string) [][]string {
	var result [][]string
	var current []string

	isPalindrome := func(left, right int) bool {
		for left < right {
			if s[left] != s[right] {
				return false
			}
			left++
			right--
		}
		return true
	}

	var backtrack func(start int)
	backtrack = func(start int) {
		if start == len(s) {
			combination := make([]string, len(current))
			copy(combination, current)
			result = append(result, combination)
			return
		}

		for end := start; end < len(s); end++ {
			if isPalindrome(start, end) {
				current = append(current, s[start:end+1])
				backtrack(end + 1)
				current = current[:len(current)-1]
			}
		}
	}

	backtrack(0)
	return result
}
