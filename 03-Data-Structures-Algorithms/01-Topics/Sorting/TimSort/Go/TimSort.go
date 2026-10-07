/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: TimSort
 * Time Complexity: O(N log N) worst & average, O(N) best case
 * Space Complexity: O(N) auxiliary
 */

package main

const RUN = 32

func insertionSort(arr []int, left, right int) {
    for i := left + 1; i <= right; i++ {
        temp := arr[i]
        j := i - 1
        for j >= left && arr[j] > temp {
            arr[j+1] = arr[j]
            j--
        }
        arr[j+1] = temp
    }
}

func merge(arr []int, l, m, r int) {
    len1, len2 := m-l+1, r-m
    left := make([]int, len1)
    right := make([]int, len2)

    copy(left, arr[l:m+1])
    copy(right, arr[m+1:r+1])

    i, j, k := 0, 0, l
    for i < len1 && j < len2 {
        if left[i] <= right[j] {
            arr[k] = left[i]
            i++
        } else {
            arr[k] = right[j]
            j++
        }
        k++
    }
    for i < len1 {
        arr[k] = left[i]
        i++
        k++
    }
    for j < len2 {
        arr[k] = right[j]
        j++
        k++
    }
}

func timSort(arr []int) {
    n := len(arr)

    for i := 0; i < n; i += RUN {
        right := i + RUN - 1
        if right >= n {
            right = n - 1
        }
        insertionSort(arr, i, right)
    }

    for size := RUN; size < n; size *= 2 {
        for left := 0; left < n; left += 2 * size {
            mid := left + size - 1
            right := left + 2*size - 1
            if right >= n {
                right = n - 1
            }
            if mid < right {
                merge(arr, left, mid, right)
            }
        }
    }
}
