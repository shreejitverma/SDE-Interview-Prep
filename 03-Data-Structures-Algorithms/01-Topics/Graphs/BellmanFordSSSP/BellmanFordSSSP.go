/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(V * E)
// Space Complexity: O(V)

package main

import (
	"fmt"
	"math"
)

type Edge struct {
	Src, Dest, Weight int
}

type Graph struct {
	V     int
	Edges []Edge
}

func NewGraph(v int) *Graph {
	return &Graph{V: v, Edges: make([]Edge, 0)}
}

func (g *Graph) AddEdge(src, dest, weight int) {
	g.Edges = append(g.Edges, Edge{Src: src, Dest: dest, Weight: weight})
}

func (g *Graph) BellmanFord(src int) []int {
	dist := make([]int, g.V)
	for i := range dist {
		dist[i] = math.MaxInt32
	}
	dist[src] = 0

	// Relax all edges |V| - 1 times
	for i := 1; i < g.V; i++ {
		for _, e := range g.Edges {
			if dist[e.Src] != math.MaxInt32 && dist[e.Src]+e.Weight < dist[e.Dest] {
				dist[e.Dest] = dist[e.Src] + e.Weight
			}
		}
	}

	// Detect negative cycles
	for _, e := range g.Edges {
		if dist[e.Src] != math.MaxInt32 && dist[e.Src]+e.Weight < dist[e.Dest] {
			fmt.Println("Graph contains negative weight cycle")
			return nil
		}
	}

	return dist
}

func main() {
	g := NewGraph(5)
	g.AddEdge(0, 1, -1)
	g.AddEdge(0, 2, 4)
	g.AddEdge(1, 2, 3)
	g.AddEdge(1, 3, 2)
	g.AddEdge(1, 4, 2)
	g.AddEdge(3, 2, 5)
	g.AddEdge(3, 1, 1)
	g.AddEdge(4, 3, -3)

	dist := g.BellmanFord(0)
	if dist != nil {
		fmt.Println("Vertex   Distance from Source")
		for i, d := range dist {
			fmt.Printf("%d\t\t%d\n", i, d)
		}
	}
}
