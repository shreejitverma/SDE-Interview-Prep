/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Dijkstra's Single Source Shortest Path
 * Time Complexity: O((V + E) log V)
 * Space Complexity: O(V + E)
 */

interface Edge {
    to: number;
    weight: number;
}

export function dijkstra(n: number, adj: Edge[][], src: number): number[] {
    const dist = new Array(n).fill(Infinity);
    dist[src] = 0;

    // Simple priority queue emulation using binary min-heap
    const heap: [number, number][] = [[0, src]]; // [dist, node]

    while (heap.length > 0) {
        heap.sort((a, b) => a[0] - b[0]);
        const [d, u] = heap.shift()!;

        if (d > dist[u]) continue;

        for (const { to, weight } of adj[u]) {
            if (dist[u] + weight < dist[to]) {
                dist[to] = dist[u] + weight;
                heap.push([dist[to], to]);
            }
        }
    }

    return dist;
}
