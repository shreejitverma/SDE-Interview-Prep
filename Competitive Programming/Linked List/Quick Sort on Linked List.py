'''https://practice.geeksforgeeks.org/problems/quick-sort-on-linked-list/1
Quick Sort on Linked List 
Medium Accuracy: 66.58% Submissions: 9064 Points: 4
Sort the given Linked List using quicksort. which takes O(n^2) time in worst case and O(nLogn) in average and best cases, otherwise you may get TLE.

Input:
In this problem, method takes 1 argument: address of the head of the linked list. The function should not read any input from stdin/console.
The struct Node has a data part which stores the data and a next pointer which points to the next element of the linked list.
There are multiple test cases. For each test case, this method will be called individually.

Output:
Set *headRef to head of resultant linked list.

User Task:
The task is to complete the function quickSort() which should set the *headRef to head of the resultant linked list.

Constraints:
1<=T<=100
1<=N<=200

Note: If you use "Test" or "Expected Output Button" use below example format

Example:
Input:
2
3
1 6 2
4
1 9 3 8

Output:
1 2 6
1 3 8 9

Explanation:
Testcase 1: After sorting the nodes, we have 1, 2 and 6.
Testcase 2: After sorting the nodes, we have 1, 3, 8 and 9.'''

# User function Template for python3
from collections import defaultdict


def quickSort(head):
    # return head after sorting

    # Use a pivot
    # Partition around pivot
    # Sort both halves.

    head, tail = quick_sort(head)

    return head


def quick_sort(head):
    if not head or not head.next:
        return head, head

    first_half_head, first_half_tail, pivot, second_half_head, second_half_tail = partition(
        head)

    first_half_head, first_half_tail = quick_sort(first_half_head)
    second_half_head, second_half_tail = quick_sort(second_half_head)

    pivot.next = second_half_head

    # first half is empty
    if not first_half_tail:
        return pivot, second_half_tail
    # second half is empty
    first_half_tail.next = pivot
    if not second_half_head:
        return first_half_head, pivot
    else:
        return first_half_head, second_half_tail


def partition(head):
    first_half_head = Node(-1)
    first_half_tail = first_half_head

    second_half_head = Node(-1)
    second_half_tail = second_half_head

    cur = head.next
    while cur:
        if cur.data < head.data:
            first_half_tail.next = cur
            first_half_tail = first_half_tail.next
        else:
            second_half_tail.next = cur
            second_half_tail = second_half_tail.next
        cur = cur.next

    first_half_tail.next = None
    second_half_tail.next = None

    return first_half_head.next, first_half_tail, head, second_half_head.next, second_half_tail


# {
#  Driver Code Starts
# Initial Template for Python 3

# contributed by RavinderSinghPB


class Node:
    def __init__(self, data):
        self.data = data
        self.next = None


class Llist:
    def __init__(self):
        self.head = None

    def insert(self, data, tail):
        node = Node(data)

        if not self.head:
            self.head = node
            return node

        tail.next = node
        return node


def nodeID(head, dic):
    while head:
        dic[head.data].append(id(head))
        head = head.next


def printList(head, dic):
    while head:
        if id(head) not in dic[head.data]:
            print("Do'nt swap data, swap pointer/node")
            return
        print(head.data, end=' ')
        head = head.next


if __name__ == '__main__':
    t = int(input())

    for tcs in range(t):
        n = int(input())

        arr = [int(x) for x in input().split()]

        ll = Llist()
        tail = None

        for nodeData in arr:
            tail = ll.insert(nodeData, tail)

        dic = defaultdict(list)   # dictonary to keep data and id of node
        nodeID(ll.head, dic)     # putting data and its id

        resHead = quickSort(ll.head)
        printList(resHead, dic)  # verifying and printing
        print()


# } Driver Code Ends
