/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Floyd-Warshall All-Pairs Shortest Path
 * Time Complexity: O(V^3)
 * Space Complexity: O(V^2)
 */

public class FloydWarshall {
    public static final long INF = Long.MAX_VALUE / 2;

    public static long[][] floydWarshall(long[][] graph) {
        int v = graph.length;
        long[][] dist = new long[v][v];

        for (int i = 0; i < v; i++) {
            System.arraycopy(graph[i], 0, dist[i], 0, v);
        }

        for (int k = 0; k < v; k++) {
            for (int i = 0; i < v; i++) {
                for (int j = 0; j < v; j++) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
        return dist;
    }
}
