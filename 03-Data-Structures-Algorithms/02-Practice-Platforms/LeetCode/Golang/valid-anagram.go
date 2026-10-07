/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

func isAnagram(s string, t string) bool {
    if len(s) != len(t) {
        return false
    }
    var counts [26]int
    for i := 0; i < len(s); i++ {
        counts[s[i]-'a']++
        counts[t[i]-'a']--
    }
    for _, c := range counts {
        if c != 0 {
            return false
        }
    }
    return true
}
