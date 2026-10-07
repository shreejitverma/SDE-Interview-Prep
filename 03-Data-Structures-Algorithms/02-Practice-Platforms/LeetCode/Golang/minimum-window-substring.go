/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m + n)
// Space: O(1) auxiliary

package main

import "math"

func minWindow(s string, t string) string {
    if len(s) < len(t) {
        return ""
    }

    var count [128]int
    for i := 0; i < len(t); i++ {
        count[t[i]]++
    }

    remain := len(t)
    left := 0
    minStart := 0
    minLen := math.MaxInt32

    for right := 0; right < len(s); right++ {
        if count[s[right]] > 0 {
            remain--
        }
        count[s[right]]--

        for remain == 0 {
            if right-left+1 < minLen {
                minLen = right - left + 1
                minStart = left
            }

            count[s[left]]++
            if count[s[left]] > 0 {
                remain++
            }
            left++
        }
    }

    if minLen == math.MaxInt32 {
        return ""
    }
    return s[minStart : minStart+minLen]
}
