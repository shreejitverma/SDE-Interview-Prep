/*
https://leetcode.com/problems/reverse-linked-list/
206. Reverse Linked List
Easy

8957

158

Add to List

Share
Given the head of a singly linked list, reverse the list, and return the reversed list.

 

Example 1:


Input: head = [1,2,3,4,5]
Output: [5,4,3,2,1]
Example 2:


Input: head = [1,2]
Output: [2,1]
Example 3:

Input: head = []
Output: []
 

Constraints:

The number of nodes in the list is the range [0, 5000].
-5000 <= Node.val <= 5000
*/

//Iterative solution
//Time: O(n)
//Space: O(1)
class Solution {
    public ListNode reverseList(ListNode head) {
        ListNode curr = head, prev = null, temp = null;
        while (curr != null) {
            temp = curr.next;// Store the new head of the sub-linkedlist
            curr.next = prev;// Reverse happens here
            prev = curr; // Move curr & prev to the next place
            curr = temp;
        }
        return prev;
    }
}
/**
 * Definition for singly-linked list. public class ListNode { int val; ListNode
 * next; ListNode(int x) { val = x; } }
 */
