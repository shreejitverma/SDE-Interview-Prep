/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addNum: O(log N), findMedian: O(1)
// Space: O(N)

class BinaryHeap {
    private data: number[] = [];
    private compare: (a: number, b: number) => boolean;

    constructor(compare: (a: number, b: number) => boolean) {
        this.compare = compare;
    }

    push(val: number): void {
        this.data.push(val);
        this.bubbleUp(this.data.length - 1);
    }

    pop(): number | undefined {
        if (this.data.length === 0) return undefined;
        const top = this.data[0];
        const last = this.data.pop()!;
        if (this.data.length > 0) {
            this.data[0] = last;
            this.bubbleDown(0);
        }
        return top;
    }

    peek(): number | undefined {
        return this.data[0];
    }

    size(): number {
        return this.data.length;
    }

    private bubbleUp(idx: number): void {
        while (idx > 0) {
            const parent = Math.floor((idx - 1) / 2);
            if (this.compare(this.data[idx], this.data[parent])) {
                [this.data[idx], this.data[parent]] = [this.data[parent], this.data[idx]];
                idx = parent;
            } else {
                break;
            }
        }
    }

    private bubbleDown(idx: number): void {
        const len = this.data.length;
        while (true) {
            let target = idx;
            const left = 2 * idx + 1;
            const right = 2 * idx + 2;

            if (left < len && this.compare(this.data[left], this.data[target])) {
                target = left;
            }
            if (right < len && this.compare(this.data[right], this.data[target])) {
                target = right;
            }
            if (target !== idx) {
                [this.data[idx], this.data[target]] = [this.data[target], this.data[idx]];
                idx = target;
            } else {
                break;
            }
        }
    }
}

class MedianFinder {
    private maxHeap: BinaryHeap;
    private minHeap: BinaryHeap;

    constructor() {
        this.maxHeap = new BinaryHeap((a, b) => a > b);
        this.minHeap = new BinaryHeap((a, b) => a < b);
    }

    addNum(num: number): void {
        if (this.maxHeap.size() === 0 || num <= this.maxHeap.peek()!) {
            this.maxHeap.push(num);
        } else {
            this.minHeap.push(num);
        }

        if (this.maxHeap.size() > this.minHeap.size() + 1) {
            this.minHeap.push(this.maxHeap.pop()!);
        } else if (this.minHeap.size() > this.maxHeap.size()) {
            this.maxHeap.push(this.minHeap.pop()!);
        }
    }

    findMedian(): number {
        if (this.maxHeap.size() > this.minHeap.size()) {
            return this.maxHeap.peek()!;
        }
        return (this.maxHeap.peek()! + this.minHeap.peek()!) / 2;
    }
}
