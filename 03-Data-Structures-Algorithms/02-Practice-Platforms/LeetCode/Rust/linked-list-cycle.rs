/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1) auxiliary

// Definition for singly-linked list with raw pointers for cyclic structures.
pub struct ListNode {
    pub val: i32,
    pub next: *const ListNode,
}

impl Solution {
    pub fn has_cycle(head: *const ListNode) -> bool {
        if head.is_null() {
            return false;
        }

        unsafe {
            let mut slow = head;
            let mut fast = head;

            while !fast.is_null() && !(*fast).next.is_null() {
                slow = (*slow).next;
                fast = (*(*fast).next).next;

                if slow == fast {
                    return true;
                }
            }
        }

        false
    }
}
