/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H) recursion stack

package main

import "math"

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func isValidBST(root *TreeNode) bool {
	return validate(root, math.MinInt64, math.MaxInt64)
}

func validate(node *TreeNode, low, high int64) bool {
	if node == nil {
		return true
	}
	val := int64(node.Val)
	if val <= low || val >= high {
		return false
	}
	return validate(node.Left, low, val) && validate(node.Right, val, high)
}
