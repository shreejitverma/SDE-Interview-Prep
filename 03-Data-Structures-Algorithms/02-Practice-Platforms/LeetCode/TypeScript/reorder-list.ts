/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

class ListNode {
    val: number;
    next: ListNode | null;
    constructor(val?: number, next?: ListNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.next = (next === undefined ? null : next);
    }
}

function reorderList(head: ListNode | null): void {
    if (!head || !head.next) {
        return;
    }

    // 1. Find middle of linked list
    let slow: ListNode = head;
    let fast: ListNode | null = head;
    while (fast && fast.next && fast.next.next) {
        slow = slow.next!;
        fast = fast.next.next;
    }

    // 2. Reverse second half
    let second: ListNode | null = reverseList(slow.next);
    slow.next = null;

    // 3. Interleave two halves
    let first: ListNode | null = head;
    while (second) {
        const tmp1: ListNode | null = first!.next;
        const tmp2: ListNode | null = second.next;

        first!.next = second;
        second.next = tmp1;

        first = tmp1;
        second = tmp2;
    }
}

function reverseList(head: ListNode | null): ListNode | null {
    let prev: ListNode | null = null;
    let curr: ListNode | null = head;
    while (curr) {
        const next: ListNode | null = curr.next;
        curr.next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
