"""
Author: Shreejit Verma
GitHub: https://github.com/shreejitverma

Algorithm: Floyd-Warshall All-Pairs Shortest Path
Time Complexity: O(V^3)
Space Complexity: O(V^2)
"""

def floyd_warshall(graph: list[list[float]]) -> list[list[float]]:
    v = len(graph)
    dist = [row[:] for row in graph]

    for k in range(v):
        for i in range(v):
            for j in range(v):
                if dist[i][k] + dist[k][j] < dist[i][j]:
                    dist[i][j] = dist[i][k] + dist[k][j]

    return dist
