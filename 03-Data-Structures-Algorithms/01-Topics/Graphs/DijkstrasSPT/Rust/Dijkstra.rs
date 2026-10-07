/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Dijkstra's Single Source Shortest Path (BinaryHeap)
 * Time Complexity: O((V + E) log V)
 * Space Complexity: O(V + E)
 */

use std::cmp::Ordering;
use std::collections::BinaryHeap;

#[derive(Copy, Clone, Eq, PartialEq)]
struct State {
    cost: usize,
    position: usize,
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        // Reverse ordering to make BinaryHeap a min-heap
        other.cost.cmp(&self.cost)
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

pub struct Edge {
    pub to: usize,
    pub cost: usize,
}

pub fn dijkstra(n: usize, adj: &[Vec<Edge>], src: usize) -> Vec<usize> {
    let mut dist = vec![usize::MAX; n];
    let mut heap = BinaryHeap::new();

    dist[src] = 0;
    heap.push(State { cost: 0, position: src });

    while let Some(State { cost, position }) = heap.pop() {
        if cost > dist[position] {
            continue;
        }

        for edge in &adj[position] {
            let next = State {
                cost: cost + edge.cost,
                position: edge.to,
            };

            if next.cost < dist[next.position] {
                dist[next.position] = next.cost;
                heap.push(next);
            }
        }
    }

    dist
}
