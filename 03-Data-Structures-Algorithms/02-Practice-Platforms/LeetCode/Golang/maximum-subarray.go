/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

func maxSubArray(nums []int) int {
    currentSum := nums[0]
    maxSum := nums[0]
    for i := 1; i < len(nums); i++ {
        if currentSum+nums[i] > nums[i] {
            currentSum += nums[i]
        } else {
            currentSum = nums[i]
        }
        if currentSum > maxSum {
            maxSum = currentSum
        }
    }
    return maxSum
}
