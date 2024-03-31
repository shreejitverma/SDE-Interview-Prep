'''
https://www.geeksforgeeks.org/write-a-c-program-to-find-the-maximum-depth-or-height-of-a-tree/
https://practice.geeksforgeeks.org/problems/height-of-binary-tree/1
Height of Binary Tree 
Medium Accuracy: 65.76% Submissions: 100k+ Points: 4
Given a binary tree, find its height.


Example 1:

Input:
     1
    /  \
   2    3
Output: 2
Example 2:

Input:
  2
   \
    1
   /
 3
Output: 3   

Your Task:
You don't need to read input or print anything. Your task is to complete the function height() which takes root node of the tree as input parameter and returns an integer denoting the height of the tree. If the tree is empty, return 0. 


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
1 <= Number of nodes <= 105
1 <= Data of a node <= 105
'''

# Python3 program to find the maximum depth of tree

# A binary tree node


class Node:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None

# Compute the "maxDepth" of a tree -- the number of nodes
# along the longest path from the root node down to the
# farthest leaf node


def maxDepth(node):
    if node is None:
        return 0

    else:

        # Compute the depth of each subtree
        lDepth = maxDepth(node.left)
        rDepth = maxDepth(node.right)

        # Use the larger one
        if (lDepth > rDepth):
            return lDepth+1
        else:
            return rDepth+1


# Driver program to test above function
root = Node(1)
root.left = Node(2)
root.right = Node(3)
root.left.left = Node(4)
root.left.right = Node(5)


print("Height of tree is %d" % (maxDepth(root)))
