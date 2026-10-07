/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log K)
// Space: O(1) auxiliary

class ListNode {
    val: number;
    next: ListNode | null;
    constructor(val?: number, next?: ListNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.next = (next === undefined ? null : next);
    }
}

function mergeKLists(lists: Array<ListNode | null>): ListNode | null {
    if (!lists || lists.length === 0) {
        return null;
    }

    function mergeTwoLists(l1: ListNode | null, l2: ListNode | null): ListNode | null {
        const dummy = new ListNode(0);
        let curr = dummy;

        while (l1 && l2) {
            if (l1.val <= l2.val) {
                curr.next = l1;
                l1 = l1.next;
            } else {
                curr.next = l2;
                l2 = l2.next;
            }
            curr = curr.next;
        }

        curr.next = l1 ? l1 : l2;
        return dummy.next;
    }

    let interval = 1;
    while (interval < lists.length) {
        for (let i = 0; i + interval < lists.length; i += interval * 2) {
            lists[i] = mergeTwoLists(lists[i], lists[i + interval]);
        }
        interval *= 2;
    }

    return lists[0];
}
