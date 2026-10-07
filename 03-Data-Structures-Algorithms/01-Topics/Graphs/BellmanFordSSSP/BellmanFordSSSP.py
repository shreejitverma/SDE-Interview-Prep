"""
Author: Shreejit Verma
GitHub: https://github.com/shreejitverma
"""

from typing import List, Tuple, Optional

# Time Complexity: O(V * E)
# Space Complexity: O(V)

class Edge:
    def __init__(self, src: int, dest: int, weight: int):
        self.src = src
        self.dest = dest
        self.weight = weight


class Graph:
    def __init__(self, v: int, edges: Optional[List[Edge]] = None):
        self.v = v
        self.edges = edges if edges is not None else []

    def add_edge(self, src: int, dest: int, weight: int) -> None:
        self.edges.append(Edge(src, dest, weight))

    def bellman_ford(self, src: int) -> Optional[List[float]]:
        dist = [float("inf")] * self.v
        dist[src] = 0

        # Relax all edges |V| - 1 times
        for _ in range(self.v - 1):
            for edge in self.edges:
                u, v, w = edge.src, edge.dest, edge.weight
                if dist[u] != float("inf") and dist[u] + w < dist[v]:
                    dist[v] = dist[u] + w

        # Detect negative-weight cycles
        for edge in self.edges:
            u, v, w = edge.src, edge.dest, edge.weight
            if dist[u] != float("inf") and dist[u] + w < dist[v]:
                print("Graph contains negative weight cycle")
                return None

        return dist


def main() -> None:
    g = Graph(5)
    g.add_edge(0, 1, -1)
    g.add_edge(0, 2, 4)
    g.add_edge(1, 2, 3)
    g.add_edge(1, 3, 2)
    g.add_edge(1, 4, 2)
    g.add_edge(3, 2, 5)
    g.add_edge(3, 1, 1)
    g.add_edge(4, 3, -3)

    distances = g.bellman_ford(0)
    if distances is not None:
        print("Vertex Distance from Source:")
        for i, d in enumerate(distances):
            print(f"{i}\t\t{d}")


if __name__ == "__main__":
    main()
