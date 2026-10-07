/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(log(min(M, N)))
// Space: O(1)

package main

import "math"

func findMedianSortedArrays(nums1 []int, nums2 []int) float64 {
	if len(nums1) > len(nums2) {
		return findMedianSortedArrays(nums2, nums1)
	}

	m := len(nums1)
	n := len(nums2)
	low := 0
	high := m

	for low <= high {
		partitionX := (low + high) / 2
		partitionY := (m + n + 1) / 2 - partitionX

		maxLeftX := math.MinInt32
		if partitionX > 0 {
			maxLeftX = nums1[partitionX-1]
		}

		minRightX := math.MaxInt32
		if partitionX < m {
			minRightX = nums1[partitionX]
		}

		maxLeftY := math.MinInt32
		if partitionY > 0 {
			maxLeftY = nums2[partitionY-1]
		}

		minRightY := math.MaxInt32
		if partitionY < n {
			minRightY = nums2[partitionY]
		}

		if maxLeftX <= minRightY && maxLeftY <= minRightX {
			if (m+n)%2 != 0 {
				return float64(max(maxLeftX, maxLeftY))
			}
			return float64(max(maxLeftX, maxLeftY)+min(minRightX, minRightY)) / 2.0
		} else if maxLeftX > minRightY {
			high = partitionX - 1
		} else {
			low = partitionX + 1
		}
	}

	return 0.0
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
