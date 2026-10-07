/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

func characterReplacement(s: string, k: int) int {
    count := make([]int, 26)
    maxCount := 0
    left := 0
    maxLen := 0

    for right := 0; right < len(s); right++ {
        idx := s[right] - 'A'
        count[idx]++
        if count[idx] > maxCount {
            maxCount = count[idx]
        }

        if (right - left + 1) - maxCount > k {
            count[s[left]-'A']--
            left++
        }

        if (right - left + 1) > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
