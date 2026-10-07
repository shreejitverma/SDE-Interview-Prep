/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H) where H is tree height
// Space: O(1) auxiliary

package main

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func lowestCommonAncestor(root, p, q *TreeNode) *TreeNode {
	small := p.Val
	large := q.Val
	if small > large {
		small, large = large, small
	}

	curr := root
	for curr != nil {
		if curr.Val > large {
			curr = curr.Left
		} else if curr.Val < small {
			curr = curr.Right
		} else {
			return curr
		}
	}

	return nil
}
