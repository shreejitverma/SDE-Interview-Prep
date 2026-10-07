/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

/**
 * Definition for a Node.
 * type Node struct {
 *     Val int
 *     Next *Node
 *     Random *Node
 * }
 */

type Node struct {
	Val    int
	Next   *Node
	Random *Node
}

func copyRandomList(head *Node) *Node {
	if head == nil {
		return nil
	}

	// 1. Interleave cloned nodes
	curr := head
	for curr != nil {
		copyNode := &Node{
			Val:  curr.Val,
			Next: curr.Next,
		}
		curr.Next = copyNode
		curr = copyNode.Next
	}

	// 2. Assign random pointers
	curr = head
	for curr != nil {
		if curr.Random != nil {
			curr.Next.Random = curr.Random.Next
		}
		curr = curr.Next.Next
	}

	// 3. Separate original and copied lists
	orig := head
	copyHead := head.Next
	copyCurr := copyHead

	for orig != nil {
		orig.Next = orig.Next.Next
		if copyCurr.Next != nil {
			copyCurr.Next = copyCurr.Next.Next
		}
		orig = orig.Next
		copyCurr = copyCurr.Next
	}

	return copyHead
}
