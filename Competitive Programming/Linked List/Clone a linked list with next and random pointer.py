'''https://practice.geeksforgeeks.org/problems/clone-a-linked-list-with-next-and-random-pointer/1
Clone a linked list with next and random pointer
Hard Accuracy: 49.62% Submissions: 34607 Points: 8
You are given a special linked list with N nodes where each node has a next pointer pointing to its next node. You are also given M random pointers, where you will be given M number of pairs denoting two nodes a and b  i.e. a->arb = b.

Construct a copy of the given list. The copy should consist of exactly N new nodes, where each new node has its value set to the value of its corresponding original node. Both the next and random pointer of the new nodes should point to new nodes in the copied list such that the pointers in the original list and copied list represent the same list state. None of the pointers in the new list should point to nodes in the original list.

For example, if there are two nodes X and Y in the original list, where X.random --> Y, then for the corresponding two nodes x and y in the copied list, x.random --> y.

Return the head of the copied linked list.

ArbitLinked List1

Example 1:

Input:
N = 4, M = 2
value = {1,2,3,4}
pairs = {{1,2},{2,4}}
Output: 1
Explanation: In this test case, there
are 4 nodes in linked list.  Among these
4 nodes,  2 nodes have arbitrary pointer
set, rest two nodes have arbitrary pointer
as NULL. Second line tells us the value
of four nodes. The third line gives the
information about arbitrary pointers.
The first node arbitrary pointer is set to
node 2.  The second node arbitrary pointer
is set to node 4.
Example 2:

Input:
N = 4, M = 2
value[] = {1,3,5,9}
pairs[] = {{1,1},{3,4}}
Output: 1
Explanation: In the given testcase ,
applying the method as stated in the
above example, the output will be 1.

Your Task:
The task is to complete the function copyList() which takes one argument the head of the linked list to be cloned and should return the head of the cloned linked list.

NOTE : 
1. If there is any node whose arbitrary pointer is not given then it's by default NULL. 
2. Your solution return an output 1 if your clone linked list is correct, else it returns 0.

Expected Time Complexity : O(n)
Expected Auxilliary Space : O(1)

Constraints:
1 <= N <= 100
1 <= M <= N
1 <= a, b <= 100'''

# User function Template for python3

'''
class Node:
    def __init__(self, data):   # data -> value stored in node
        self.data = data
        self.next = None
        self.arb=None
        
param: head:  head of linkedList to copy
return: the head of the copied linked list the #output will be 1 if successfully copied
'''


class Solution:
    # Function to clone a linked list with next and random pointer.
    def copyList(self, head):
        # code here
        if head == None:
            return None

        def add_duplicates(head):
            node = head
            while node != None:
                d = node.data
                nex = node.next
                node.next = Node(d)
                node.next.next = nex
                node = nex

        def add_random(head):
            node = head
            while node != None:
                if node.arb != None:
                    node.next.arb = node.arb.next
                node = node.next.next

        def remove(head):
            head1 = head
            head2 = head.next
            node1 = head1
            node2 = head2
            node = head.next.next
            while node != None:
                node1.next = node
                node2.next = node.next
                node1 = node1.next
                node2 = node2.next
                node = node.next.next
            node1.next = None
            head = head1
            return head1, head2

        add_duplicates(head)
        add_random(head)
        head1, head2 = remove(head)
        return head2


# {
#  Driver Code Starts
class Node:
    def __init__(self, data):   # data -> value stored in node
        self.data = data
        self.next = None
        self.arb = None


class LinkedList:
    def __init__(self):
        self.head = None


def insert(tail, data):
    tail.next = Node(data)
    return tail.next


def setarb(head, a, b):
    h = head
    i = 1
    while i < a and h:
        h = h.next
        i += 1
    an = h

    h = head
    i = 1
    while i < b and h:
        h = h.next
        i += 1

    if an:
        an.arb = h


def validation(head, res):

    headp = head
    resp = res

    d = {}

    while head and res:
        if head == res:
            return
        if head.data != res.data:
            return

        if head.arb:
            if not res.arb:
                return

            if head.arb.data != res.arb.data:
                return

        elif res.arb:
            return
        if head not in d:
            d[head] = res
        head = head.next
        res = res.next

    if not head and res:
        return
    elif head and not res:
        return

    head = headp
    res = resp
    while head:
        if head == res:
            return
        if head.arb:
            if head.arb not in d:
                return
            if d[head.arb] != res.arb:
                return
        head = head.next
        res = res.next

    return True


if __name__ == '__main__':
    t = int(input())
    for cases in range(t):
        __n, __m = list(map(int, input().strip().split()))
        __nodes = list(map(int, input().strip().split()))
        __aarb = list(map(int, input().strip().split()))
        __ll = LinkedList()
        __ll2 = LinkedList()
        __ll.head = Node(__nodes[0])
        __ll2.head = Node(__nodes[0])
        __tail = __ll.head
        __tail2 = __ll2.head

        for x in __nodes[1:]:
            __tail = insert(__tail, x)
            __tail2 = insert(__tail2, x)

        for i in range(0, len(__aarb), 2):
            setarb(__ll.head, __aarb[i], __aarb[i+1])
            setarb(__ll2.head, __aarb[i], __aarb[i+1])

        obj = Solution()
        __res = obj.copyList(__ll.head)
        if validation(__ll.head, __res) and validation(__ll2.head, __res):
            print(1)
        else:
            print(0)
# } Driver Code Ends
