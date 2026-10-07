/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n^2)
// Space: O(1)

package main

import "sort"

func threeSum(nums []int) [][]int {
    var result [][]int
    sort.Ints(nums)
    n := len(nums)
    for i := 0; i < n-2; i++ {
        if nums[i] > 0 {
            break
        }
        if i > 0 && nums[i] == nums[i-1] {
            continue
        }
        left := i + 1
        right := n - 1
        for left < right {
            sum := nums[i] + nums[left] + nums[right]
            if sum == 0 {
                result = append(result, []int{nums[i], nums[left], nums[right]})
                for left < right && nums[left] == nums[left+1] {
                    left++
                }
                for left < right && nums[right] == nums[right-1] {
                    right--
                }
                left++
                right--
            } else if sum < 0 {
                left++
            } else {
                right--
            }
        }
    }
    return result
}
