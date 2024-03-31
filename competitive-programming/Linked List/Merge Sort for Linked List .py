'''https: // practice.geeksforgeeks.org/problems/sort-a-linked-list/1
Merge Sort for Linked List
Medium Accuracy: 52.29 % Submissions: 33658 Points: 4
Given Pointer/Reference to the head of the linked list, the task is to Sort the given linked list using Merge Sort.
Note: If the length of linked list is odd, then the extra node should go in the first list while splitting.

Example 1:

Input:
N = 5
value[] = {3, 5, 2, 4, 1}
Output: 1 2 3 4 5
Explanation: After sorting the given
linked list, the resultant matrix
will be 1 -> 2 -> 3 -> 4 -> 5.
Example 2:

Input:
N = 3
value[] = {9, 15, 0}
Output: 0 9 15
Explanation: After sorting the given
linked list, resultant will be
0 -> 9 -> 15.
Your Task:
For C++ and Python: The task is to complete the function mergeSort() which sort the linked list using merge sort function.
For Java: The task is to complete the function mergeSort() and return the node which can be used to print the sorted linked list.

Expected Time Complexity: O(N*Log(N))
Expected Auxiliary Space: O(N)

Constraints:
1 <= T <= 100
1 <= N <= 105
'''

# User function Template for python3

'''
    :param head: head of unsorted linked list 
    :return: head of sorted linkd list
    
    # Node Class
    class Node:
        def __init__(self, data):  # data -> value stored in node
            self.data = data
            self.next = None
'''




import sys
import io
import atexit
class Solution:
    # Function to sort the given linked list using Merge Sort.
    def mergeSort(self, head):

        if head is None or head.next is None:
            return head
        mid = self.middleOfLL(head)
        nextHead = mid.next
        mid.next = None

        first = self.mergeSort(head)
        second = self.mergeSort(nextHead)

        return self.merge2SortedLists(first, second)

    def middleOfLL(self, head):
        slow = fast = head

        while fast.next and fast.next.next:
            slow = slow.next
            fast = fast.next.next
        return slow

    def merge2SortedLists(self, head1, head2):
        if not head1:
            return head2

        if not head2:
            return head1

        dummyNode = Node(-1)
        l1 = head1
        l2 = head2
        prev = dummyNode

        while l1 != None and l2 != None:
            if l1.data > l2.data:
                prev.next = l2
                l2 = l2.next
            else:
                prev.next = l1
                l1 = l1.next

            prev = prev.next

        if l1 is None:
            prev.next = l2
        else:
            prev.next = l1

        return dummyNode.next


# {
#  Driver Code Starts
# Initial Template for Python 3

# Contributed by : Nagendra Jha

# Node Class

class Node:
    def __init__(self, data):  # data -> value stored in node
        self.data = data
        self.next = None


# Linked List Class
class LinkedList:
    def __init__(self):
        self.head = None
        self.tail = None

    # creates a new node with given value and appends it at the end of the linked list
    def append(self, new_value):
        new_node = Node(new_value)
        if self.head is None:
            self.head = new_node
            self.tail = new_node
            return
        self.tail.next = new_node
        self.tail = new_node

# prints the elements of linked list starting with head


def printList(head):
    if head is None:
        print(' ')
        return
    curr_node = head
    while curr_node:
        print(curr_node.data, end=" ")
        curr_node = curr_node.next
    print(' ')


if __name__ == '__main__':
    t = int(input())
    for cases in range(t):
        n = int(input())
        p = LinkedList()  # create a new linked list 'a'.
        nodes_p = list(map(int, input().strip().split()))
        for x in nodes_p:
            p.append(x)  # add to the end of the list

        printList(Solution().mergeSort(p.head))

# } Driver Code Ends
