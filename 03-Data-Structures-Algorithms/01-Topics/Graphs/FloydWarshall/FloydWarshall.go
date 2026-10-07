/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Floyd-Warshall All-Pairs Shortest Path
 * Time Complexity: O(V^3)
 * Space Complexity: O(V^2)
 */

package main

const INF = 1e9

func FloydWarshall(graph [][]int) [][]int {
    v := len(graph)
    dist := make([][]int, v)
    for i := range dist {
        dist[i] = make([]int, v)
        copy(dist[i], graph[i])
    }

    for k := 0; k < v; k++ {
        for i := 0; i < v; i++ {
            for j := 0; j < v; j++ {
                if dist[i][k] != INF && dist[k][j] != INF && dist[i][k]+dist[k][j] < dist[i][j] {
                    dist[i][j] = dist[i][k] + dist[k][j]
                }
            }
        }
    }

    return dist
}
