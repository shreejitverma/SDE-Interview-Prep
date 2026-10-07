/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(n)

package main

import "sort"

func lengthOfLIS(nums []int) int {
    if len(nums) == 0 {
        return 0
    }
    tails := make([]int, 0, len(nums))
    for _, x := range nums {
        idx := sort.Search(len(tails), func(i int) bool {
            return tails[i] >= x
        })
        if idx == len(tails) {
            tails = append(tails, x)
        } else {
            tails[idx] = x
        }
    }
    return len(tails)
}
