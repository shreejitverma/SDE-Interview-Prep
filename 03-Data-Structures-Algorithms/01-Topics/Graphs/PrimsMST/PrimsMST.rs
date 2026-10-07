/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(E log V)
// Space Complexity: O(V + E)

use std::cmp::Ordering;
use std::collections::BinaryHeap;

#[derive(Copy, Clone, Eq, PartialEq)]
struct State {
    weight: i32,
    vertex: usize,
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        // Reverse ordering for min-heap
        other.weight.cmp(&self.weight)
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Clone)]
pub struct Edge {
    pub to: usize,
    pub weight: i32,
}

pub fn prims_mst(v: usize, adj: &[Vec<Edge>]) -> i32 {
    let mut visited = vec![false; v];
    let mut pq = BinaryHeap::new();
    pq.push(State { weight: 0, vertex: 0 });

    let mut mst_cost = 0;

    while let Some(State { weight, vertex }) = pq.pop() {
        if visited[vertex] {
            continue;
        }

        visited[vertex] = true;
        mst_cost += weight;

        for edge in &adj[vertex] {
            if !visited[edge.to] {
                pq.push(State {
                    weight: edge.weight,
                    vertex: edge.to,
                });
            }
        }
    }

    mst_cost
}

fn main() {
    let v = 5;
    let mut adj = vec![Vec::new(); v];
    let edges = [
        (0, 1, 2),
        (0, 3, 6),
        (1, 2, 3),
        (1, 3, 8),
        (1, 4, 5),
        (2, 4, 7),
        (3, 4, 9),
    ];

    for &(u, to, w) in &edges {
        adj[u].push(Edge { to, weight: w });
        adj[to].push(Edge { to: u, weight: w });
    }

    println!("Total MST Cost: {}", prims_mst(v, &adj));
}
