/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

package main

func containsDuplicate(nums []int) bool {
    lookup := make(map[int]struct{}, len(nums))
    for _, num := range nums {
        if _, exists := lookup[num]; exists {
            return true
        }
        lookup[num] = struct{}{}
    }
    return false
}
