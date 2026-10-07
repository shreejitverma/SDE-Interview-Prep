/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(V * E)
// Space Complexity: O(V)

#[derive(Clone, Copy)]
pub struct Edge {
    pub src: usize,
    pub dest: usize,
    pub weight: i32,
}

pub struct Graph {
    pub v: usize,
    pub edges: Vec<Edge>,
}

impl Graph {
    pub fn new(v: usize) -> Self {
        Graph {
            v,
            edges: Vec::new(),
        }
    }

    pub fn add_edge(&mut self, src: usize, dest: usize, weight: i32) {
        self.edges.push(Edge { src, dest, weight });
    }

    pub fn bellman_ford(&self, src: usize) -> Option<Vec<i32>> {
        let mut dist = vec![i32::MAX; self.v];
        dist[src] = 0;

        // Relax edges |V| - 1 times
        for _ in 1..self.v {
            for edge in &self.edges {
                if dist[edge.src] != i32::MAX && dist[edge.src] + edge.weight < dist[edge.dest] {
                    dist[edge.dest] = dist[edge.src] + edge.weight;
                }
            }
        }

        // Check for negative-weight cycles
        for edge in &self.edges {
            if dist[edge.src] != i32::MAX && dist[edge.src] + edge.weight < dist[edge.dest] {
                eprintln!("Graph contains negative weight cycle");
                return None;
            }
        }

        Some(dist)
    }
}

fn main() {
    let mut g = Graph::new(5);
    g.add_edge(0, 1, -1);
    g.add_edge(0, 2, 4);
    g.add_edge(1, 2, 3);
    g.add_edge(1, 3, 2);
    g.add_edge(1, 4, 2);
    g.add_edge(3, 2, 5);
    g.add_edge(3, 1, 1);
    g.add_edge(4, 3, -3);

    if let Some(dist) = g.bellman_ford(0) {
        println!("Vertex   Distance from Source");
        for (i, d) in dist.iter().enumerate() {
            println!("{}\t\t{}", i, d);
        }
    }
}
