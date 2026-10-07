/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

package main

import "math"

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func maxPathSum(root *TreeNode) int {
	maxSum := math.MinInt32

	var maxGain func(node *TreeNode) int
	maxGain = func(node *TreeNode) int {
		if node == nil {
			return 0
		}

		leftGain := maxGain(node.Left)
		if leftGain < 0 {
			leftGain = 0
		}

		rightGain := maxGain(node.Right)
		if rightGain < 0 {
			rightGain = 0
		}

		currentPathSum := node.Val + leftGain + rightGain
		if currentPathSum > maxSum {
			maxSum = currentPathSum
		}

		if leftGain > rightGain {
			return node.Val + leftGain
		}
		return node.Val + rightGain
	}

	maxGain(root)
	return maxSum
}
