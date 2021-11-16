'''https://www.geeksforgeeks.org/find-pairs-given-sum-doubly-linked-list/
Given a sorted doubly linked list of positive distinct elements, the task is to find pairs in a doubly-linked list whose sum is equal to given value x, without using any extra space? 
Example:
Input : head : 1 <-> 2 <-> 4 <-> 5 <-> 6 <-> 8 <-> 9
        x = 7
Output: (6, 1), (5,2)
The expected time complexity is O(n) and auxiliary space is O(1).

A simple approach for this problem is to one by one pick each node and find a second element whose sum is equal to x in the remaining list by traversing in the forward direction. The time complexity for this problem will be O(n^2), n is the total number of nodes in the doubly linked list.

An efficient solution for this problem is the same as this article. Here is the algorithm :  

Initialize two pointer variables to find the candidate elements in the sorted doubly linked list. Initialize first with the start of the doubly linked list i.e; first=head and initialize second with the last node of the doubly linked list i.e; second=last_node.
We initialize first and second pointers as first and last nodes. Here we don’t have random access, so to find the second pointer, we traverse the list to initialize the second.
If current sum of first and second is less than x, then we move first in forward direction. If current sum of first and second element is greater than x, then we move second in backward direction.
Loop termination conditions are also different from arrays. The loop terminates when two pointers cross each other (second->next = first), or they become the same (first == second).
The case when no pairs are present will be handled by the condition “first==second”'''

# Python3 program to find a pair with
# given sum x.

# Structure of node of doubly linked list


class Node:

    def __init__(self, x):

        self.data = x
        self.next = None
        self.prev = None

# Function to find pair whose sum
# equal to given value x.


def pairSum(head, x):

    # Set two pointers, first to the
    # beginning of DLL and second to
    # the end of DLL.
    first = head
    second = head

    while (second.next != None):
        second = second.next

    # To track if we find a pair or not
    found = False

    # The loop terminates when they
    # cross each other (second.next ==
    # first), or they become same
    # (first == second)
    while (first != second and second.next != first):

        # Pair found
        if ((first.data + second.data) == x):
            found = True
            print("(", first.data, ",",
                  second.data, ")")

            # Move first in forward direction
            first = first.next

            # Move second in backward direction
            second = second.prev
        else:
            if ((first.data + second.data) < x):
                first = first.next
            else:
                second = second.prev

    # If pair is not present
    if (found == False):
        print("No pair found")

# A utility function to insert a new node
# at the beginning of doubly linked list


def insert(head, data):

    temp = Node(data)

    if not head:
        head = temp
    else:
        temp.next = head
        head.prev = temp
        head = temp

    return head


# Driver code
if __name__ == '__main__':

    head = None
    head = insert(head, 9)
    head = insert(head, 8)
    head = insert(head, 6)
    head = insert(head, 5)
    head = insert(head, 4)
    head = insert(head, 2)
    head = insert(head, 1)
    x = 7

    pairSum(head, x)
