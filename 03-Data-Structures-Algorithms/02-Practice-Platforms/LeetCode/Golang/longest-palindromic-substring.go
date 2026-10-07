/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

package main

func longestPalindrome(s string) string {
    if len(s) <= 1 {
        return s
    }

    t := make([]byte, 0, 2*len(s)+3)
    t = append(t, '^')
    for i := 0; i < len(s); i++ {
        t = append(t, '#', s[i])
    }
    t = append(t, '#', '$')

    n := len(t)
    p := make([]int, n)
    c, r := 0, 0

    for i := 1; i < n-1; i++ {
        iMirror := 2*c - i
        if r > i {
            if r-i < p[iMirror] {
                p[i] = r - i
            } else {
                p[i] = p[iMirror]
            }
        }
        for t[i+1+p[i]] == t[i-1-p[i]] {
            p[i]++
        }
        if i+p[i] > r {
            c = i
            r = i + p[i]
        }
    }

    maxLen := 0
    centerIndex := 0
    for i := 1; i < n-1; i++ {
        if p[i] > maxLen {
            maxLen = p[i]
            centerIndex = i
        }
    }

    start := (centerIndex - maxLen) / 2
    return s[start : start+maxLen]
}
