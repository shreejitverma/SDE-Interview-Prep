/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Counting Sort
 * Time Complexity: O(N + K)
 * Space Complexity: O(N + K)
 */

package main

func countingSort(arr []int) []int {
    if len(arr) == 0 {
        return arr
    }

    minVal, maxVal := arr[0], arr[0]
    for _, v := range arr {
        if v < minVal {
            minVal = v
        }
        if v > maxVal {
            maxVal = v
        }
    }

    k := maxVal - minVal + 1
    count := make([]int, k)
    for _, v := range arr {
        count[v-minVal]++
    }

    for i := 1; i < k; i++ {
        count[i] += count[i-1]
    }

    output := make([]int, len(arr))
    for i := len(arr) - 1; i >= 0; i-- {
        output[count[arr[i]-minVal]-1] = arr[i]
        count[arr[i]-minVal]--
    }

    return output
}
