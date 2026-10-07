/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

import "unicode"

func isPalindrome(s string) bool {
	runes := []rune(s)
	left := 0
	right := len(runes) - 1

	for left < right {
		for left < right && !isAlphanumeric(runes[left]) {
			left++
		}
		for left < right && !isAlphanumeric(runes[right]) {
			right--
		}

		if unicode.ToLower(runes[left]) != unicode.ToLower(runes[right]) {
			return false
		}

		left++
		right--
	}

	return true
}

func isAlphanumeric(r rune) bool {
	return unicode.IsLetter(r) || unicode.IsDigit(r)
}
