'''https://www.geeksforgeeks.org/reverse-doubly-linked-list-groups-given-size/
Given a doubly linked list containing n nodes. The problem is to reverse every group of k nodes in the list.
 

Examples: 
Prerequisite: Reverse a doubly linked list | Set-2.
Approach: Create a recursive function say reverse(head, k). This function receives the head or the first node of each group of k nodes. It reverses those group of k nodes by applying the approach discussed in Reverse a doubly linked list | Set-2. After reversing the group of k nodes the function checks whether next group of nodes exists in the list or not. If group exists then it makes a recursive call to itself with the first node of the next group and makes the necessary adjustments with the next and previous links of that group. Finally it returns the new head node of the reversed group. '''

# Python implementation to reverse a doubly linked list
# in groups of given size

# Link list node


class Node:

    def __init__(self, data):
        self.data = data
        self.next = next

# function to get a new node


def getNode(data):

    # allocate space
    new_node = Node(0)

    # put in the data
    new_node.data = data
    new_node.next = new_node.prev = None
    return new_node

# function to insert a node at the beginging
# of the Doubly Linked List


def push(head_ref, new_node):

    # since we are adding at the beginning,
    # prev is always None
    new_node.prev = None

    # link the old list off the new node
    new_node.next = (head_ref)

    # change prev of head node to new node
    if ((head_ref) != None):
        (head_ref).prev = new_node

    # move the head to point to the new node
    (head_ref) = new_node
    return head_ref

# function to reverse a doubly linked list
# in groups of given size


def revListInGroupOfGivenSize(head, k):

    current = head
    next = None
    newHead = None
    count = 0

    # reversing the current group of k
    # or less than k nodes by adding
    # them at the beginning of list
    # 'newHead'
    while (current != None and count < k):

        next = current.next
        newHead = push(newHead, current)
        current = next
        count = count + 1

    # if next group exists then making the desired
    # adjustments in the link
    if (next != None):

        head.next = revListInGroupOfGivenSize(next, k)
        head.next.prev = head

    # pointer to the new head of the
    # reversed group
    return newHead

# Function to print nodes in a
# given doubly linked list


def printList(head):

    while (head != None):
        print(head.data, end=" ")
        head = head.next

# Driver program to test above


# Start with the empty list
head = None

# Create doubly linked: 10<.8<.4<.2
head = push(head, getNode(2))
head = push(head, getNode(4))
head = push(head, getNode(8))
head = push(head, getNode(10))

k = 2

print("Original list: ")
printList(head)

# Reverse doubly linked list in groups of
# size 'k'
head = revListInGroupOfGivenSize(head, k)

print("\nModified list: ")
printList(head)
