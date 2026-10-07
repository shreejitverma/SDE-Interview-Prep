/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Dijkstra's Single Source Shortest Path (container/heap)
 * Time Complexity: O((V + E) log V)
 * Space Complexity: O(V + E)
 */

package main

import (
    "container/heap"
    "math"
)

type Edge struct {
    to     int
    weight int
}

type Item struct {
    node int
    dist int
}

type PriorityQueue []*Item

func (pq PriorityQueue) Len() int           { return len(pq) }
func (pq PriorityQueue) Less(i, j int) bool { return pq[i].dist < pq[j].dist }
func (pq PriorityQueue) Swap(i, j int)      { pq[i], pq[j] = pq[j], pq[i] }
func (pq *PriorityQueue) Push(x any)        { *pq = append(*pq, x.(*Item)) }
func (pq *PriorityQueue) Pop() any {
    old := *pq
    n := len(old)
    item := old[n-1]
    *pq = old[0 : n-1]
    return item
}

func Dijkstra(n int, adj [][]Edge, src int) []int {
    dist := make([]int, n)
    for i := range dist {
        dist[i] = math.MaxInt32
    }
    dist[src] = 0

    pq := &PriorityQueue{}
    heap.Init(pq)
    heap.Push(pq, &Item{node: src, dist: 0})

    for pq.Len() > 0 {
        curr := heap.Pop(pq).(*Item)
        u := curr.node
        d := curr.dist

        if d > dist[u] {
            continue
        }

        for _, e := range adj[u] {
            v := e.to
            w := e.weight
            if dist[u]+w < dist[v] {
                dist[v] = dist[u] + w
                heap.Push(pq, &Item{node: v, dist: dist[v]})
            }
        }
    }

    return dist
}
