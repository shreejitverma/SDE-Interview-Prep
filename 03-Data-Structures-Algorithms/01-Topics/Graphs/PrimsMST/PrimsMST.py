"""
Author: Shreejit Verma
GitHub: https://github.com/shreejitverma
"""

import heapq
from typing import List, Tuple

# Time Complexity: O(E log V)
# Space Complexity: O(V + E)

class PrimsMST:
    @staticmethod
    def find_mst_cost(v: int, adj: List[List[Tuple[int, int]]]) -> int:
        pq: List[Tuple[int, int]] = [(0, 0)]  # (weight, vertex)
        visited = [False] * v
        mst_cost = 0

        while pq:
            w, u = heapq.heappop(pq)
            if visited[u]:
                continue
            visited[u] = True
            mst_cost += w

            for nbr, edge_w in adj[u]:
                if not visited[nbr]:
                    heapq.heappush(pq, (edge_w, nbr))

        return mst_cost


def main() -> None:
    v = 5
    adj: List[List[Tuple[int, int]]] = [[] for _ in range(v)]

    edges = [
        (0, 1, 2),
        (0, 3, 6),
        (1, 2, 3),
        (1, 3, 8),
        (1, 4, 5),
        (2, 4, 7),
        (3, 4, 9),
    ]

    for u, w_v, w in edges:
        adj[u].append((w_v, w))
        adj[w_v].append((u, w))

    mst_cost = PrimsMST.find_mst_cost(v, adj)
    print(f"Total MST Cost: {mst_cost}")


if __name__ == "__main__":
    main()
