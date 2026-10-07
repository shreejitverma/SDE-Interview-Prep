/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log K)
// Space: O(1) auxiliary

package main

type ListNode struct {
	Val  int
	Next *ListNode
}

func mergeKLists(lists []*ListNode) *ListNode {
	if len(lists) == 0 {
		return nil
	}

	interval := 1
	for interval < len(lists) {
		for i := 0; i+interval < len(lists); i += interval * 2 {
			lists[i] = mergeTwoLists(lists[i], lists[i+interval])
		}
		interval *= 2
	}

	return lists[0]
}

func mergeTwoLists(l1, l2 *ListNode) *ListNode {
	dummy := &ListNode{}
	curr := dummy

	for l1 != nil && l2 != nil {
		if l1.Val <= l2.Val {
			curr.Next = l1
			l1 = l1.Next
		} else {
			curr.Next = l2
			l2 = l2.Next
		}
		curr = curr.Next
	}

	if l1 != nil {
		curr.Next = l1
	} else {
		curr.Next = l2
	}

	return dummy.Next
}
