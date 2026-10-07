/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

/**
 * Definition for _Node.
 * class _Node {
 *     val: number
 *     next: _Node | null
 *     random: _Node | null
 *     constructor(val?: number, next?: _Node, random?: _Node) {
 *         this.val = (val===undefined ? 0 : val)
 *         this.next = (next===undefined ? null : next)
 *         this.random = (random===undefined ? null : random)
 *     }
 * }
 */

class _Node {
    val: number;
    next: _Node | null;
    random: _Node | null;
    constructor(val?: number, next?: _Node | null, random?: _Node | null) {
        this.val = val === undefined ? 0 : val;
        this.next = next === undefined ? null : next;
        this.random = random === undefined ? null : random;
    }
}

function copyRandomList(head: _Node | null): _Node | null {
    if (!head) return null;

    // 1. Interleave cloned nodes
    let curr: _Node | null = head;
    while (curr) {
        const copy = new _Node(curr.val, curr.next, null);
        curr.next = copy;
        curr = copy.next;
    }

    // 2. Assign random pointers
    curr = head;
    while (curr) {
        if (curr.random) {
            curr.next!.random = curr.random.next;
        }
        curr = curr.next!.next;
    }

    // 3. Separate the lists
    let orig: _Node | null = head;
    const copyHead = head.next;
    let copy: _Node | null = copyHead;

    while (orig) {
        orig.next = orig.next ? orig.next.next : null;
        if (copy && copy.next) {
            copy.next = copy.next.next;
        }
        orig = orig.next;
        copy = copy ? copy.next : null;
    }

    return copyHead;
}
