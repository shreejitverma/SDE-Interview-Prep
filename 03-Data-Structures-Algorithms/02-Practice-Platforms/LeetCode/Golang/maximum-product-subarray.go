/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

func maxProduct(nums []int) int {
    maxProd := nums[0]
    minProd := nums[0]
    result := nums[0]
    for i := 1; i < len(nums); i++ {
        x := nums[i]
        if x < 0 {
            maxProd, minProd = minProd, maxProd
        }
        if x > maxProd*x {
            maxProd = x
        } else {
            maxProd = maxProd * x
        }
        if x < minProd*x {
            minProd = x
        } else {
            minProd = minProd * x
        }
        if maxProd > result {
            result = maxProd
        }
    }
    return result
}
