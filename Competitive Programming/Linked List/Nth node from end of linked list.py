'''https://practice.geeksforgeeks.org/problems/nth-node-from-end-of-linked-list/1
Nth node from end of linked list 
Easy Accuracy: 46.6% Submissions: 100k+ Points: 2
Given a linked list consisting of L nodes and given a number N. The task is to find the Nth node from the end of the linked list.

Example 1:

Input:
N = 2
LinkedList: 1->2->3->4->5->6->7->8->9
Output: 8
Explanation: In the first example, there
are 9 nodes in linked list and we need
to find 2nd node from end. 2nd node
from end os 8.  
Example 2:

Input:
N = 5
LinkedList: 10->5->100->5
Output: -1
Explanation: In the second example, there
are 4 nodes in the linked list and we
need to find 5th from the end. Since 'n'
is more than the number of nodes in the
linked list, the output is -1.
Your Task:
The task is to complete the function getNthFromLast() which takes two arguments: reference to head and N and you need to return Nth from the end or -1 in case node doesn't exist..

Note:
Try to solve in single traversal.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1 <= L <= 106
1 <= N <= 106'''

# User function Template for python3
'''
	Your task is to return the data stored in
	the nth node from end of linked list.
	
	Function Arguments: head (reference to head of the list), n (pos of node from end)
	Return Type: Integer or -1 if no such node exits.

	{
		# Node Class
		class Node:
		    def __init__(self, data):   # data -> value stored in node
		        self.data = data
		        self.next = None
	}
'''
# Function to find the data of nth node from the end of a linked list




import atexit
import sys
import io
def getNthFromLast(head, n):
    # code here
    curr_node = head
    nth_node = head

    # traversing first n elements with first pointer.
    while n:
        if n and curr_node == None:
            return -1
        curr_node = curr_node.next
        n -= 1

    # now traversing with both pointers and when first pointer gives null
    # we have got the nth node from end in second pointer since the first
    # pointer had already traversed n nodes and thus had difference of n
    # nodes with second pointer.
    while curr_node:
        curr_node = curr_node.next
        nth_node = nth_node.next

    # returning the data of nth node from end.
    return nth_node.data

# {
#  Driver Code Starts
# Initial Template for Python 3
# Contributed by : Nagendra Jha


_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())

# Node Class


class Node:
    def __init__(self, data):   # data -> value stored in node
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


if __name__ == '__main__':
    t = int(input())
    for cases in range(t):
        n, nth_node = map(int, input().strip().split())
        a = LinkedList()  # create a new linked list 'a'.
        nodes_a = list(map(int, input().strip().split()))
        for x in nodes_a:
            a.append(x)  # add to the end of the list
        print(getNthFromLast(a.head, nth_node))
# } Driver Code Ends
