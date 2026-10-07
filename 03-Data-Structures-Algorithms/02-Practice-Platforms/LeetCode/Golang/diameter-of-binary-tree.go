/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 543 - Diameter of Binary Tree
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(H) where H is tree height
 */

package main

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func diameterOfBinaryTree(root *TreeNode) int {
	maxDiameter := 0

	var maxDepth func(node *TreeNode) int
	maxDepth = func(node *TreeNode) int {
		if node == nil {
			return 0
		}

		leftDepth := maxDepth(node.Left)
		rightDepth := maxDepth(node.Right)

		if leftDepth+rightDepth > maxDiameter {
			maxDiameter = leftDepth + rightDepth
		}

		if leftDepth > rightDepth {
			return 1 + leftDepth
		}
		return 1 + rightDepth
	}

	maxDepth(root)
	return maxDiameter
}
