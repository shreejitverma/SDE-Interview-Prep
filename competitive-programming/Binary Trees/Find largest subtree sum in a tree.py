'''https://www.geeksforgeeks.org/find-largest-subtree-sum-tree/
https://youtu.be/7opoOv7SVko
Given a binary tree, task is to find subtree with maximum sum in tree.
Examples: 
 

Input :       1
            /   \
           2      3
          / \    / \
         4   5  6   7
Output : 28
As all the tree elements are positive,
the largest subtree sum is equal to
sum of all tree elements.

Input :       1
            /    \
          -2      3
          / \    /  \
         4   5  -6   2
Output : 7
Subtree with largest sum is :  -2
                             /  \ 
                            4    5
Also, entire tree sum is also 7.

Approach : Do post order traversal of the binary tree. At every node, find left subtree value and right subtree value recursively. The value of subtree rooted at current node is equal to sum of current node value, left node subtree sum and right node subtree sum. Compare current subtree sum with overall maximum subtree sum so far.
Implementation : '''

# Python3 program to find largest subtree
# sum in a given binary tree.

# Function to create new tree node.


class newNode:
    def __init__(self, key):
        self.key = key
        self.left = self.right = None

# Helper function to find largest
# subtree sum recursively.


def findLargestSubtreeSumUtil(root, ans):

    # If current node is None then
    # return 0 to parent node.
    if (root == None):
        return 0

    # Subtree sum rooted at current node.
    currSum = (root.key +
               findLargestSubtreeSumUtil(root.left, ans) +
               findLargestSubtreeSumUtil(root.right, ans))

    # Update answer if current subtree
    # sum is greater than answer so far.
    ans[0] = max(ans[0], currSum)

    # Return current subtree sum to
    # its parent node.
    return currSum

# Function to find largest subtree sum.


def findLargestSubtreeSum(root):

    # If tree does not exist,
    # then answer is 0.
    if (root == None):
        return 0

    # Variable to store maximum subtree sum.
    ans = [-999999999999]

    # Call to recursive function to
    # find maximum subtree sum.
    findLargestSubtreeSumUtil(root, ans)

    return ans[0]


# Driver Code
if __name__ == '__main__':

    #
    #         1
    #         / \
    #     /     \
    #     -2     3
    #     / \     / \
    #     / \ / \
    # 4     5 -6     2
    root = newNode(1)
    root.left = newNode(-2)
    root.right = newNode(3)
    root.left.left = newNode(4)
    root.left.right = newNode(5)
    root.right.left = newNode(-6)
    root.right.right = newNode(2)

    print(findLargestSubtreeSum(root))

'''Output: 
7
 

Time Complexity: O(n), where n is number of nodes. 
Auxiliary Space: O(n), function call stack size. '''
