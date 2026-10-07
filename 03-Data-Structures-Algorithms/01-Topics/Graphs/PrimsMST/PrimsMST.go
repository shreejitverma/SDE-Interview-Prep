/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(E log V)
// Space Complexity: O(V + E)

package main

import (
	"container/heap"
	"fmt"
)

type Edge struct {
	To, Weight int
}

type Item struct {
	Weight, Vertex int
}

type PriorityQueue []Item

func (pq PriorityQueue) Len() int           { return len(pq) }
func (pq PriorityQueue) Less(i, j int) bool { return pq[i].Weight < pq[j].Weight }
func (pq PriorityQueue) Swap(i, j int)      { pq[i], pq[j] = pq[j], pq[i] }
func (pq *PriorityQueue) Push(x any)        { *pq = append(*pq, x.(Item)) }
func (pq *PriorityQueue) Pop() any {
	old := *pq
	n := len(old)
	item := old[n-1]
	*pq = old[0 : n-1]
	return item
}

func primsMST(v int, adj [][]Edge) int {
	visited := make([]bool, v)
	pq := &PriorityQueue{}
	heap.Init(pq)
	heap.Push(pq, Item{Weight: 0, Vertex: 0})

	mstCost := 0

	for pq.Len() > 0 {
		top := heap.Pop(pq).(Item)
		u := top.Vertex
		if visited[u] {
			continue
		}

		visited[u] = true
		mstCost += top.Weight

		for _, edge := range adj[u] {
			if !visited[edge.To] {
				heap.Push(pq, Item{Weight: edge.Weight, Vertex: edge.To})
			}
		}
	}

	return mstCost
}

func main() {
	v := 5
	adj := make([][]Edge, v)
	edges := [][]int{
		{0, 1, 2},
		{0, 3, 6},
		{1, 2, 3},
		{1, 3, 8},
		{1, 4, 5},
		{2, 4, 7},
		{3, 4, 9},
	}

	for _, e := range edges {
		u, to, w := e[0], e[1], e[2]
		adj[u] = append(adj[u], Edge{To: to, Weight: w})
		adj[to] = append(adj[to], Edge{To: u, Weight: w})
	}

	fmt.Println("Total MST Cost:", primsMST(v, adj))
}
