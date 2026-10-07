/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Kruskal's Minimum Spanning Tree
 * Time Complexity: O(E log E)
 * Space Complexity: O(V + E)
 */

struct DSU {
    parent: Vec<usize>,
    rank: Vec<usize>,
}

impl DSU {
    fn new(n: usize) -> Self {
        DSU {
            parent: (0..n).collect(),
            rank: vec![0; n],
        }
    }

    fn find(&mut self, i: usize) -> usize {
        if self.parent[i] == i {
            i
        } else {
            let p = self.parent[i];
            self.parent[i] = self.find(p);
            self.parent[i]
        }
    }

    fn union(&mut self, i: usize, j: usize) -> bool {
        let root_i = self.find(i);
        let root_j = self.find(j);
        if root_i == root_j {
            return false;
        }
        if self.rank[root_i] < self.rank[root_j] {
            self.parent[root_i] = root_j;
        } else if self.rank[root_i] > self.rank[root_j] {
            self.parent[root_j] = root_i;
        } else {
            self.parent[root_j] = root_i;
            self.rank[root_i] += 1;
        }
        true
    }
}

#[derive(Clone, Copy, Debug)]
pub struct Edge {
    pub u: usize,
    pub v: usize,
    pub weight: i64,
}

pub fn kruskal_mst(n: usize, mut edges: Vec<Edge>) -> (i64, Vec<Edge>) {
    edges.sort_by_key(|e| e.weight);
    let mut dsu = DSU::new(n);
    let mut mst = Vec::new();
    let mut total_weight = 0;

    for edge in edges {
        if dsu.union(edge.u, edge.v) {
            total_weight += edge.weight;
            mst.push(edge);
            if mst.len() == n - 1 {
                break;
            }
        }
    }

    (total_weight, mst)
}
