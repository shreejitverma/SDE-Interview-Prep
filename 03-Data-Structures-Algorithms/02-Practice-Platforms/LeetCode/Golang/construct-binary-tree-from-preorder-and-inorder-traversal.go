/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N) auxiliary

package main

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func buildTree(preorder []int, inorder []int) *TreeNode {
	inMap := make(map[int]int, len(inorder))
	for i, v := range inorder {
		inMap[v] = i
	}

	preIndex := 0

	var build func(inStart, inEnd int) *TreeNode
	build = func(inStart, inEnd int) *TreeNode {
		if inStart > inEnd {
			return nil
		}

		rootVal := preorder[preIndex]
		preIndex++
		root := &TreeNode{Val: rootVal}
		mid := inMap[rootVal]

		root.Left = build(inStart, mid-1)
		root.Right = build(mid+1, inEnd)

		return root
	}

	return build(0, len(inorder)-1)
}
