/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 647 - Palindromic Substrings
 * Difficulty: Medium
 * Language: Go (Golang)
 *
 * Performance Analysis:
 * - Time Complexity: O(N) using Manacher's algorithm; O(N^2) center expansion.
 * - Space Complexity: O(N) for Manacher's radius slice.
 */

package main

import (
	"fmt"
	"strings"
)

func countSubstrings(s string) int {
	if len(s) == 0 {
		return 0
	}

	// Preprocess: "^#a#b#c#$"
	var sb strings.Builder
	sb.WriteString("^")
	for i := 0; i < len(s); i++ {
		sb.WriteString("#")
		sb.WriteByte(s[i])
	}
	sb.WriteString("#$")
	t := sb.String()
	n := len(t)

	p := make([]int, n)
	center, right := 0, 0
	total := 0

	for i := 1; i < n-1; i++ {
		iMirror := 2*center - i
		if right > i {
			if right-i < p[iMirror] {
				p[i] = right - i
			} else {
				p[i] = p[iMirror]
			}
		} else {
			p[i] = 0
		}

		for t[i+1+p[i]] == t[i-1-p[i]] {
			p[i]++
		}

		if i+p[i] > right {
			center = i
			right = i + p[i]
		}

		total += (p[i] + 1) / 2
	}

	return total
}

func countSubstringsExpand(s string) int {
	total := 0
	n := len(s)

	expand := func(l, r int) int {
		cnt := 0
		for l >= 0 && r < n && s[l] == s[r] {
			cnt++
			l--
			r++
		}
		return cnt
	}

	for i := 0; i < n; i++ {
		total += expand(i, i)
		total += expand(i, i+1)
	}

	return total
}

func main() {
	fmt.Println("countSubstrings('abc'):", countSubstrings("abc")) // 3
	fmt.Println("countSubstrings('aaa'):", countSubstrings("aaa")) // 6
}
