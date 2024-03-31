'''https://practice.geeksforgeeks.org/problems/reverse-level-order-traversal/1
https://www.geeksforgeeks.org/reverse-level-order-traversal/
Reverse Level Order Traversal 
Easy Accuracy: 47.34% Submissions: 61244 Points: 2
Given a binary tree of size N, find its reverse level order traversal. ie- the traversal must begin from the last level.

Example 1:

Input :
        1
      /   \
     3     2

Output: 3 2 1
Explanation:
Traversing level 1 : 3 2
Traversing level 0 : 1
Example 2:

Input :
       10
      /  \
     20   30
    / \ 
   40  60

Output: 40 60 20 30 10
Explanation:
Traversing level 2 : 40 60
Traversing level 1 : 20 30
Traversing level 0 : 10

Your Task: 
You dont need to read input or print anything. Complete the function reverseLevelOrder() which takes the root of the tree as input parameter and returns a list containing the reverse level order traversal of the given tree.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
1 ≤ N ≤ 10^4'''

# Python program to print REVERSE level order traversal using
# stack and queue

from collections import deque
# A binary tree node


class Node:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None


# Given a binary tree, print its nodes in reverse level order


def reverseLevelOrder(root):
  # we can use a double ended queue which provides O(1) insert at the beginning
  # using the appendleft method
  # we do the regular level order traversal but instead of processing the
  # left child first we process the right child first and the we process the left child
  # of the current Node
  # we can do this One pass reduce the space usage not in terms of complexity but intuitively

    q = deque()
    q.append(root)
    ans = deque()
    while q:
        node = q.popleft()
        if node is None:
            continue

        ans.appendleft(node.data)

        if node.right:
            q.append(node.right)

        if node.left:
            q.append(node.left)

    return ans


# Driver program to test above function
root = Node(1)
root.left = Node(2)
root.right = Node(3)
root.left.left = Node(4)
root.left.right = Node(5)
root.right.left = Node(6)
root.right.right = Node(7)

print("Level Order traversal of binary tree is")
reverseLevelOrder(root)
