/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(1) auxiliary

package main

import "sort"

func merge(intervals [][]int) [][]int {
    if len(intervals) <= 1 {
        return intervals
    }

    sort.Slice(intervals, func(i, j int) bool {
        return intervals[i][0] < intervals[j][0]
    })

    var result [][]int
    result = append(result, intervals[0])

    for i := 1; i < len(intervals); i++ {
        last := &result[len(result)-1]
        curr := intervals[i]

        if curr[0] <= (*last)[1] {
            if curr[1] > (*last)[1] {
                (*last)[1] = curr[1]
            }
        } else {
            result = append(result, curr)
        }
    }

    return result
}
