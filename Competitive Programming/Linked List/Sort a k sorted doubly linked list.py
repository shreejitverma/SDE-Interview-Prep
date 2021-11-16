'''
https://www.geeksforgeeks.org/sort-k-sorted-doubly-linked-list/
Given a doubly linked list containing n nodes, where each node is at most k away from its target position in the list. The problem is to sort the given doubly linked list. 
For example, let us consider k is 2, a node at position 7 in the sorted doubly linked list, can be at positions 5, 6, 7, 8, 9 in the given doubly linked list.
Examples:
Naive Approach: Sort the given doubly linked list using the insertion sort technique. While inserting each element in the sorted part of the list, there will be at most k swaps to place the element to its correct position since it is at most k steps away from its correct position.'''
