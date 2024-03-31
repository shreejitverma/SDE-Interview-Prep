'''https: // practice.geeksforgeeks.org/problems/minimum-element-in-bst/1
Minimum element in BST 
Basic Accuracy: 62.66% Submissions: 57466 Points: 1
Given a Binary Search Tree. The task is to find the minimum element in this given BST.

Example 1:

Input:
           5
         /    \
        4      6
       /        \
      3          7
     /
    1
Output: 1
Example 2:

Input:
             9
              \
               10
                \
                 11
Output: 9
Your Task:
The task is to complete the function minValue() which takes root as the argument and returns the minimum element of BST. If the tree is empty, there is no minimum elemnt, so retutn -1 in that case.

Expected Time Complexity: O(Height of the BST)
Expected Auxiliary Space: O(Height of the BST).

Constraints:
1 <= N <= 104'''
# Python3 program to find maximum
# and minimum in a Binary Tree

# A class to create a new node

# Function to find the minimum element in the given BST.


def minValue(root):
    # Your code here
    if root is None:
        return float('inf')
    res = root.data
    lres = minValue(root.left)
    rres = minValue(root.right)
    if lres < res:
        res = lres
    if rres < res:
        res = rres
    return res


class newNode:
    def __init__(self, data):
        self.data = data
        self.left = self.right = None

# Returns maximum value in a
# given Binary Tree


def findMax(root):

    # Base case
    if (root == None):
        return float('-inf')

    # Return maximum of 3 values:
    # 1) Root's data 2) Max in Left Subtree
    # 3) Max in right subtree
    res = root.data
    lres = findMax(root.left)
    rres = findMax(root.right)
    if (lres > res):
        res = lres
    if (rres > res):
        res = rres
    return res


# Driver Code
if __name__ == '__main__':
    root = newNode(2)
    root.left = newNode(7)
    root.right = newNode(5)
    root.left.right = newNode(6)
    root.left.right.left = newNode(1)
    root.left.right.right = newNode(11)
    root.right.right = newNode(9)
    root.right.right.left = newNode(4)

    # Function call
    print("Maximum element is",
          findMax(root))
