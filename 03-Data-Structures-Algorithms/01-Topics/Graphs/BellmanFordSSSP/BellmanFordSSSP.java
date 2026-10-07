/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(V * E)
// Space Complexity: O(V)

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class BellmanFordSSSP {
    static class Edge {
        int src, dest, weight;
        Edge(int src, int dest, int weight) {
            this.src = src;
            this.dest = dest;
            this.weight = weight;
        }
    }

    static class Graph {
        int V;
        List<Edge> edges;

        Graph(int V) {
            this.V = V;
            this.edges = new ArrayList<>();
        }

        void addEdge(int src, int dest, int weight) {
            edges.add(new Edge(src, dest, weight));
        }

        int[] bellmanFord(int src) {
            int[] dist = new int[V];
            Arrays.fill(dist, Integer.MAX_VALUE);
            dist[src] = 0;

            // Relax all edges |V| - 1 times
            for (int i = 1; i < V; ++i) {
                for (Edge edge : edges) {
                    if (dist[edge.src] != Integer.MAX_VALUE && dist[edge.src] + edge.weight < dist[edge.dest]) {
                        dist[edge.dest] = dist[edge.src] + edge.weight;
                    }
                }
            }

            // Check for negative-weight cycles
            for (Edge edge : edges) {
                if (dist[edge.src] != Integer.MAX_VALUE && dist[edge.src] + edge.weight < dist[edge.dest]) {
                    System.out.println("Graph contains negative weight cycle");
                    return null;
                }
            }

            return dist;
        }
    }

    public static void main(String[] args) {
        Graph g = new Graph(5);
        g.addEdge(0, 1, -1);
        g.addEdge(0, 2, 4);
        g.addEdge(1, 2, 3);
        g.addEdge(1, 3, 2);
        g.addEdge(1, 4, 2);
        g.addEdge(3, 2, 5);
        g.addEdge(3, 1, 1);
        g.addEdge(4, 3, -3);

        int[] dist = g.bellmanFord(0);
        if (dist != null) {
            System.out.println("Vertex   Distance from Source");
            for (int i = 0; i < dist.length; ++i) {
                System.out.println(i + "\t\t" + dist[i]);
            }
        }
    }
}
