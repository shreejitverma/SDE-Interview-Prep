/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * 4^N)
// Space: O(N) auxiliary (recursion stack)

package main

var mapping = []string{
	"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz",
}

func letterCombinations(digits string) []string {
	var result []string
	if len(digits) == 0 {
		return result
	}

	path := make([]byte, len(digits))

	var backtrack func(index int)
	backtrack = func(index int) {
		if index == len(digits) {
			result = append(result, string(path))
			return
		}

		letters := mapping[digits[index]-'0']
		for i := 0; i < len(letters); i++ {
			path[index] = letters[i]
			backtrack(index + 1)
		}
	}

	backtrack(0)
	return result
}
