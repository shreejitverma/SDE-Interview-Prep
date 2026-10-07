/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Floyd-Warshall All-Pairs Shortest Path
 * Time Complexity: O(V^3)
 * Space Complexity: O(V^2)
 */

export function floydWarshall(graph: number[][]): number[][] {
    const v = graph.length;
    const dist: number[][] = graph.map(row => [...row]);

    for (let k = 0; k < v; k++) {
        for (let i = 0; i < v; i++) {
            for (let j = 0; j < v; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    return dist;
}
