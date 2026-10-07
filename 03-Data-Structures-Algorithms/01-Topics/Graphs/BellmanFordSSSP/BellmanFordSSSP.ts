/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(V * E)
// Space Complexity: O(V)

interface Edge {
    src: number;
    dest: number;
    weight: number;
}

class BellmanFordGraph {
    v: number;
    edges: Edge[];

    constructor(v: number) {
        this.v = v;
        this.edges = [];
    }

    addEdge(src: number, dest: number, weight: number): void {
        this.edges.push({ src, dest, weight });
    }

    bellmanFord(src: number): number[] | null {
        const dist: number[] = new Array(this.v).fill(Infinity);
        dist[src] = 0;

        // Relax edges |V| - 1 times
        for (let i = 1; i < this.v; i++) {
            for (const { src: u, dest: v, weight: w } of this.edges) {
                if (dist[u] !== Infinity && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                }
            }
        }

        // Check for negative-weight cycles
        for (const { src: u, dest: v, weight: w } of this.edges) {
            if (dist[u] !== Infinity && dist[u] + w < dist[v]) {
                console.log("Graph contains negative weight cycle");
                return null;
            }
        }

        return dist;
    }
}

// Example usage
const g = new BellmanFordGraph(5);
g.addEdge(0, 1, -1);
g.addEdge(0, 2, 4);
g.addEdge(1, 2, 3);
g.addEdge(1, 3, 2);
g.addEdge(1, 4, 2);
g.addEdge(3, 2, 5);
g.addEdge(3, 1, 1);
g.addEdge(4, 3, -3);

const distances = g.bellmanFord(0);
if (distances !== null) {
    console.log("Vertex   Distance from Source");
    for (let i = 0; i < distances.length; i++) {
        console.log(`${i}\t\t${distances[i]}`);
    }
}
