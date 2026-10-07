/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 269 - Alien Dictionary
 * Difficulty: Hard
 * Language: Go (Golang)
 *
 * Performance Analysis:
 * - Time Complexity: O(C) where C is the total count of characters across all words.
 * - Space Complexity: O(1) auxiliary space bounded by alphabet size 26.
 */

package main

import (
	"fmt"
	"strings"
)

func alienOrder(words []string) string {
	if len(words) == 0 {
		return ""
	}

	var present [26]bool
	var inDegree [26]int
	adj := make([][]int, 26)
	uniqueCount := 0

	for _, w := range words {
		for i := 0; i < len(w); i++ {
			c := int(w[i] - 'a')
			if !present[c] {
				present[c] = true
				uniqueCount++
			}
		}
	}

	for i := 0; i < len(words)-1; i++ {
		w1, w2 := words[i], words[i+1]

		// Prefix edge case
		if len(w1) > len(w2) && strings.HasPrefix(w1, w2) {
			return ""
		}

		minLen := len(w1)
		if len(w2) < minLen {
			minLen = len(w2)
		}

		for j := 0; j < minLen; j++ {
			c1 := int(w1[j] - 'a')
			c2 := int(w2[j] - 'a')
			if c1 != c2 {
				adj[c1] = append(adj[c1], c2)
				inDegree[c2]++
				break
			}
		}
	}

	var queue []int
	for i := 0; i < 26; i++ {
		if present[i] && inDegree[i] == 0 {
			queue = append(queue, i)
		}
	}

	var result []byte
	head := 0
	for head < len(queue) {
		u := queue[head]
		head++
		result = append(result, byte('a'+u))

		for _, v := range adj[u] {
			inDegree[v]--
			if inDegree[v] == 0 {
				queue = append(queue, v)
			}
		}
	}

	if len(result) < uniqueCount {
		return ""
	}

	return string(result)
}

func main() {
	words1 := []string{"wrt", "wrf", "er", "ett", "rftt"}
	fmt.Println("Order 1:", alienOrder(words1)) // "wertf"

	words2 := []string{"z", "x"}
	fmt.Println("Order 2:", alienOrder(words2)) // "zx"

	words3 := []string{"z", "x", "z"}
	fmt.Println("Order 3:", alienOrder(words3)) // ""
}
