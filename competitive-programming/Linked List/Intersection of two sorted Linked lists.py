'''https://practice.geeksforgeeks.org/problems/intersection-of-two-sorted-linked-lists/1
Intersection of two sorted Linked lists 
Easy Accuracy: 29.44% Submissions: 55237 Points: 2
Given two lists sorted in increasing order, create a new list representing the intersection of the two lists. The new list should be made with its own memory — the original lists should not be changed.
Note: The list elements are not necessarily distinct.

Example 1:

Input:
L1 = 1->2->3->4->6
L2 = 2->4->6->8
Output: 2 4 6
Explanation: For the given first two
linked list, 2, 4 and 6 are the elements
in the intersection.
Example 2:

Input:
L1 = 10->20->40->50
L2 = 15->40
Output: 40
Your Task:
The task is to complete the function intersection() which should find the intersection of two linked list and add all the elements in intersection to the third linked list and return the head of the third linked list.

Expected Time Complexity : O(n+m)
Expected Auxilliary Space : O(n+m)
Note: n,m are the size of the linked lists.

Constraints:
1 <= size of linked lists <= 5000
1 <= Data in linked list nodes <= 1000'''

# User function Template for python3

''' structure of node:

class Node:
    def __init__(self,data):
        self.data=data
        self.next=None

'''


def findIntersection(head1, head2):
    # return head
    temp1 = head1
    temp2 = head2
    flag = 0
    temp = None
    while temp1 != None and temp2 != None:
        if temp1.data == temp2.data:
            newnode = Node(temp1.data)
            if temp == None:
                head = newnode
                temp = newnode
                temp1 = temp1.next
                temp2 = temp2.next
            else:
                temp.next = newnode
                temp = newnode
                temp1 = temp1.next
                temp2 = temp2.next
        elif temp1.data > temp2.data:
            temp2 = temp2.next
        else:
            temp1 = temp1.next
    return head

# {
#  Driver Code Starts
# Initial Template for Python 3


class Node:
    def __init__(self, data):
        self.data = data
        self.next = None


class linkedList:
    def __init__(self):
        self.head = None
        self.tail = None

    def insert(self, data):
        if self.head is None:
            self.head = Node(data)
            self.tail = self.head
        else:
            self.tail.next = Node(data)
            self.tail = self.tail.next


def printList(head):
    while head:
        print(head.data, end=' ')
        head = head.next


if __name__ == '__main__':
    t = int(input())
    for _ in range(t):

        n1, n2 = [int(x) for x in input().split()]
        arr1 = [int(x) for x in input().split()]
        ll1 = linkedList()
        for i in arr1:
            ll1.insert(i)

        arr2 = [int(x) for x in input().split()]
        ll2 = linkedList()
        for i in arr2:
            ll2.insert(i)

        result = findIntersection(ll1.head, ll2.head)
        printList(result)
        print()

# } Driver Code Ends
