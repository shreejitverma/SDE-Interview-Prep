/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Floyd-Warshall All-Pairs Shortest Path
 * Time Complexity: O(V^3)
 * Space Complexity: O(V^2)
 */

pub const INF: i64 = i64::MAX / 2;

pub fn floyd_warshall(graph: &[Vec<i64>]) -> Vec<Vec<i64>> {
    let v = graph.len();
    let mut dist = graph.to_vec();

    for k in 0..v {
        for i in 0..v {
            for j in 0..v {
                if dist[i][k] + dist[k][j] < dist[i][j] {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    dist
}
