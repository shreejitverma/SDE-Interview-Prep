/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Kruskal's Minimum Spanning Tree
 * Time Complexity: O(E log E)
 * Space Complexity: O(V + E)
 */

class DSU {
    parent: number[];
    rank: number[];

    constructor(n: number) {
        this.parent = Array.from({ length: n }, (_, i) => i);
        this.rank = new Array(n).fill(0);
    }

    find(i: number): number {
        if (this.parent[i] === i) return i;
        this.parent[i] = this.find(this.parent[i]);
        return this.parent[i];
    }

    union(i: number, j: number): boolean {
        const rootI = this.find(i);
        const rootJ = this.find(j);
        if (rootI === rootJ) return false;

        if (this.rank[rootI] < this.rank[rootJ]) {
            this.parent[rootI] = rootJ;
        } else if (this.rank[rootI] > this.rank[rootJ]) {
            this.parent[rootJ] = rootI;
        } else {
            this.parent[rootJ] = rootI;
            this.rank[rootI]++;
        }
        return true;
    }
}

export interface Edge {
    u: number;
    v: number;
    weight: number;
}

export function kruskalMST(n: number, edges: Edge[]): { totalWeight: number; mst: Edge[] } {
    edges.sort((a, b) => a.weight - b.weight);
    const dsu = new DSU(n);
    const mst: Edge[] = [];
    let totalWeight = 0;

    for (const edge of edges) {
        if (dsu.union(edge.u, edge.v)) {
            mst.push(edge);
            totalWeight += edge.weight;
            if (mst.length === n - 1) break;
        }
    }

    return { totalWeight, mst };
}
