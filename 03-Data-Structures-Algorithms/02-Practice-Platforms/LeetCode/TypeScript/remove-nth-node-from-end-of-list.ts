/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

class ListNode {
    val: number;
    next: ListNode | null;
    constructor(val?: number, next?: ListNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.next = (next === undefined ? null : next);
    }
}

function removeNthFromEnd(head: ListNode | null, n: number): ListNode | null {
    const dummy = new ListNode(0, head);
    let fast: ListNode | null = dummy;
    let slow: ListNode | null = dummy;

    for (let i = 0; i < n; i++) {
        if (fast) fast = fast.next;
    }

    while (fast && fast.next) {
        fast = fast.next;
        slow = slow!.next;
    }

    if (slow && slow.next) {
        slow.next = slow.next.next;
    }

    return dummy.next;
}
