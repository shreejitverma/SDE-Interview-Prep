/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Build O(N), Query O(log N), Update O(log N)
// Space Complexity: O(N)

package main

import "fmt"

type SegmentTree struct {
	tree []int
	n    int
}

func NewSegmentTree(arr []int) *SegmentTree {
	n := len(arr)
	st := &SegmentTree{
		tree: make([]int, 4*n),
		n:    n,
	}
	if n > 0 {
		st.build(arr, 1, 0, n-1)
	}
	return st
}

func (st *SegmentTree) build(arr []int, node, start, end int) {
	if start == end {
		st.tree[node] = arr[start]
		return
	}
	mid := (start + end) / 2
	st.build(arr, 2*node, start, mid)
	st.build(arr, 2*node+1, mid+1, end)
	st.tree[node] = st.tree[2*node] + st.tree[2*node+1]
}

func (st *SegmentTree) Update(idx, val int) {
	st.updateNode(1, 0, st.n-1, idx, val)
}

func (st *SegmentTree) updateNode(node, start, end, idx, val int) {
	if start == end {
		st.tree[node] = val
		return
	}
	mid := (start + end) / 2
	if idx <= mid {
		st.updateNode(2*node, start, mid, idx, val)
	} else {
		st.updateNode(2*node+1, mid+1, end, idx, val)
	}
	st.tree[node] = st.tree[2*node] + st.tree[2*node+1]
}

func (st *SegmentTree) Query(left, right int) int {
	return st.queryNode(1, 0, st.n-1, left, right)
}

func (st *SegmentTree) queryNode(node, start, end, l, r int) int {
	if r < start || end < l {
		return 0
	}
	if l <= start && end <= r {
		return st.tree[node]
	}
	mid := (start + end) / 2
	leftSum := st.queryNode(2*node, start, mid, l, r)
	rightSum := st.queryNode(2*node+1, mid+1, end, l, r)
	return leftSum + rightSum
}

func main() {
	arr := []int{1, 3, 5, 7, 9, 11}
	st := NewSegmentTree(arr)

	fmt.Println("Sum in range [1, 3]:", st.Query(1, 3)) // 3 + 5 + 7 = 15
	st.Update(1, 10)
	fmt.Println("Sum in range [1, 3] after update:", st.Query(1, 3)) // 10 + 5 + 7 = 22
}
