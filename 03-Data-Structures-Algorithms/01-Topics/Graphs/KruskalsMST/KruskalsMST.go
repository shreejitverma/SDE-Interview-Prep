/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Kruskal's Minimum Spanning Tree
 * Time Complexity: O(E log E)
 * Space Complexity: O(V + E)
 */

package main

import "sort"

type DSU struct {
    parent []int
    rank   []int
}

func NewDSU(n int) *DSU {
    parent := make([]int, n)
    rank := make([]int, n)
    for i := range parent {
        parent[i] = i
    }
    return &DSU{parent: parent, rank: rank}
}

func (d *DSU) Find(i int) int {
    if d.parent[i] == i {
        return i
    }
    d.parent[i] = d.Find(d.parent[i])
    return d.parent[i]
}

func (d *DSU) Union(i, j int) bool {
    rootI := d.Find(i)
    rootJ := d.Find(j)
    if rootI == rootJ {
        return false
    }
    if d.rank[rootI] < d.rank[rootJ] {
        d.parent[rootI] = rootJ
    } else if d.rank[rootI] > d.rank[rootJ] {
        d.parent[rootJ] = rootI
    } else {
        d.parent[rootJ] = rootI
        d.rank[rootI]++
    }
    return true
}

type Edge struct {
    U, V, Weight int
}

func KruskalMST(n int, edges []Edge) (int, []Edge) {
    sort.Slice(edges, func(i, j int) bool {
        return edges[i].Weight < edges[j].Weight
    })

    dsu := NewDSU(n)
    var mst []Edge
    totalWeight := 0

    for _, e := range edges {
        if dsu.Union(e.U, e.V) {
            mst = append(mst, e)
            totalWeight += e.Weight
            if len(mst) == n-1 {
                break
            }
        }
    }

    return totalWeight, mst
}
