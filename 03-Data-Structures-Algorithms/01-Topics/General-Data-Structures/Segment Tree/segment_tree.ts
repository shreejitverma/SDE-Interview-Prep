/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Build O(N), Query O(log N), Update O(log N)
// Space Complexity: O(N)

class SegmentTree {
    private tree: number[];
    private n: number;

    constructor(arr: number[]) {
        this.n = arr.length;
        this.tree = new Array(4 * this.n).fill(0);
        if (this.n > 0) {
            this.build(arr, 1, 0, this.n - 1);
        }
    }

    private build(arr: number[], node: number, start: number, end: number): void {
        if (start === end) {
            this.tree[node] = arr[start];
            return;
        }
        const mid = Math.floor((start + end) / 2);
        this.build(arr, 2 * node, start, mid);
        this.build(arr, 2 * node + 1, mid + 1, end);
        this.tree[node] = this.tree[2 * node] + this.tree[2 * node + 1];
    }

    public update(idx: number, val: number): void {
        this.updateNode(1, 0, this.n - 1, idx, val);
    }

    private updateNode(node: number, start: number, end: number, idx: number, val: number): void {
        if (start === end) {
            this.tree[node] = val;
            return;
        }
        const mid = Math.floor((start + end) / 2);
        if (idx <= mid) {
            this.updateNode(2 * node, start, mid, idx, val);
        } else {
            this.updateNode(2 * node + 1, mid + 1, end, idx, val);
        }
        this.tree[node] = this.tree[2 * node] + this.tree[2 * node + 1];
    }

    public query(left: number, right: number): number {
        return this.queryNode(1, 0, this.n - 1, left, right);
    }

    private queryNode(node: number, start: number, end: number, l: number, r: number): number {
        if (r < start || end < l) {
            return 0;
        }
        if (l <= start && end <= r) {
            return this.tree[node];
        }
        const mid = Math.floor((start + end) / 2);
        const leftSum = this.queryNode(2 * node, start, mid, l, r);
        const rightSum = this.queryNode(2 * node + 1, mid + 1, end, l, r);
        return leftSum + rightSum;
    }
}

// Example driver
const arr = [1, 3, 5, 7, 9, 11];
const st = new SegmentTree(arr);

console.log("Sum in range [1, 3]:", st.query(1, 3)); // 3 + 5 + 7 = 15
st.update(1, 10);
console.log("Sum in range [1, 3] after update:", st.query(1, 3)); // 10 + 5 + 7 = 22
