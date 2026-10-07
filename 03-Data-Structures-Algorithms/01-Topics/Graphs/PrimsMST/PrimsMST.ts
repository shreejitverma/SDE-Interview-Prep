/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(E log V)
// Space Complexity: O(V + E)

interface Edge {
    to: number;
    weight: number;
}

class MinHeap<T> {
    private data: T[] = [];
    private compare: (a: T, b: T) => number;

    constructor(compare: (a: T, b: T) => number) {
        this.compare = compare;
    }

    push(val: T): void {
        this.data.push(val);
        this.bubbleUp(this.data.length - 1);
    }

    pop(): T | undefined {
        if (this.data.length === 0) return undefined;
        const top = this.data[0];
        const bottom = this.data.pop()!;
        if (this.data.length > 0) {
            this.data[0] = bottom;
            this.bubbleDown(0);
        }
        return top;
    }

    isEmpty(): boolean {
        return this.data.length === 0;
    }

    private bubbleUp(idx: number): void {
        while (idx > 0) {
            const pIdx = Math.floor((idx - 1) / 2);
            if (this.compare(this.data[idx], this.data[pIdx]) < 0) {
                [this.data[idx], this.data[pIdx]] = [this.data[pIdx], this.data[idx]];
                idx = pIdx;
            } else {
                break;
            }
        }
    }

    private bubbleDown(idx: number): void {
        const len = this.data.length;
        while (true) {
            let smallest = idx;
            const left = 2 * idx + 1;
            const right = 2 * idx + 2;

            if (left < len && this.compare(this.data[left], this.data[smallest]) < 0) {
                smallest = left;
            }
            if (right < len && this.compare(this.data[right], this.data[smallest]) < 0) {
                smallest = right;
            }
            if (smallest !== idx) {
                [this.data[idx], this.data[smallest]] = [this.data[smallest], this.data[idx]];
                idx = smallest;
            } else {
                break;
            }
        }
    }
}

function primsMST(v: number, adj: Edge[][]): number {
    const visited = new Array<boolean>(v).fill(false);
    const pq = new MinHeap<[number, number]>((a, b) => a[0] - b[0]); // [weight, vertex]
    pq.push([0, 0]);

    let mstCost = 0;

    while (!pq.isEmpty()) {
        const [w, u] = pq.pop()!;
        if (visited[u]) continue;

        visited[u] = true;
        mstCost += w;

        for (const edge of adj[u]) {
            if (!visited[edge.to]) {
                pq.push([edge.weight, edge.to]);
            }
        }
    }

    return mstCost;
}

// Example usage
const V = 5;
const adjList: Edge[][] = Array.from({ length: V }, () => []);
const edges: [number, number, number][] = [
    [0, 1, 2],
    [0, 3, 6],
    [1, 2, 3],
    [1, 3, 8],
    [1, 4, 5],
    [2, 4, 7],
    [3, 4, 9]
];

for (const [u, v, w] of edges) {
    adjList[u].push({ to: v, weight: w });
    adjList[v].push({ to: u, weight: w });
}

console.log("Total MST Cost:", primsMST(V, adjList));
