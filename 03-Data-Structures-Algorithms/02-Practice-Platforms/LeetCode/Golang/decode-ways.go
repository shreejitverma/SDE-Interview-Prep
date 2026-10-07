/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

func numDecodings(s string) int {
	if len(s) == 0 || s[0] == '0' {
		return 0
	}

	prev2 := 1
	prev1 := 1

	for i := 1; i < len(s); i++ {
		current := 0

		// Single digit decode
		if s[i] != '0' {
			current += prev1
		}

		// Two digit decode
		twoDigit := int(s[i-1]-'0')*10 + int(s[i]-'0')
		if twoDigit >= 10 && twoDigit <= 26 {
			current += prev2
		}

		prev2 = prev1
		prev1 = current
	}

	return prev1
}
