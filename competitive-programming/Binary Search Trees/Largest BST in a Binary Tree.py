'''https://www.geeksforgeeks.org/find-the-largest-subtree-in-a-tree-that-is-also-a-bst/
Largest BST 
Medium Accuracy: 30.88% Submissions: 48354 Points: 4
Given a binary tree. Find the size of its largest subtree that is a Binary Search Tree.
Note: Here Size is equal to the number of nodes in the subtree.

Example 1:

Input:
        1
      /   \
     4     4
   /   \
  6     8
Output: 1
Explanation: There's no sub-tree with size
greater than 1 which forms a BST. All the
leaf Nodes are the BSTs with size equal
to 1.
Example 2:

Input: 6 6 3 N 2 9 3 N 8 8 2
            6
        /       \
       6         3
        \      /   \
         2    9     3
          \  /  \
          8 8    2 
Output: 2
Explanation: The following sub-tree is a
BST of size 2: 
       2
    /    \ 
   N      8
Your Task:
You don't need to read input or print anything. Your task is to complete the function largestBst() that takes the root node of the Binary Tree as its input and returns the size of the largest subtree which is also the BST. If the complete Binary Tree is a BST, return the size of the complete Binary Tree. 

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(Height of the BST).

Constraints:
1 ≤ Number of nodes ≤ 105
1 ≤ Data of a node ≤ 106'''

'''Method 2 (Tricky and Efficient) 
In method 1, we traverse the tree in top-down manner and do BST test for every node. 
If we traverse the tree in bottom-up manner, then we can pass information about subtrees to the parent. 
The passed information can be used by the parent to do BST test 
(for parent node) only in constant time (or O(1) time). 
A left subtree need to tell the parent whether it is BST or not and also needs to pass maximum value in it. 
So that we can compare the maximum value with the parent’s data to check the BST property. 
Similarly, the right subtree need to pass the minimum value up the tree. 
The subtrees need to pass the following information up the tree for the finding the largest BST. 
1) Whether the subtree itself is BST or not (In the following code, is_bst_ref is used for this purpose) 
2) If the subtree is left subtree of its parent, then maximum value in it. 
And if it is right subtree then minimum value in it. 
3) Size of this subtree if this subtree is BST 
(In the following code, return value of largestBSTtil() is used for this purpose)
max_ref is used for passing the maximum value up the tree and min_ptr is used for 
passing minimum value up the tree. '''
# Helper function that allocates a new
# node with the given data and NULL left
# and right pointers.


class newNode:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None

# Returns size of the largest BST subtree
# in a Binary Tree (efficient version).


def largestBST(node):

    # Set the initial values for calling
    # largestBSTUtil()
    Min = [999999999999]  # For minimum value in
    # right subtree
    Max = [-999999999999]  # For maximum value in
    # left subtree

    max_size = [0]  # For size of the largest BST
    is_bst = [0]

    largestBSTUtil(node, Min, Max,
                   max_size, is_bst)

    return max_size[0]

# largestBSTUtil() updates max_size_ref[0]
# for the size of the largest BST subtree.
# Also, if the tree rooted with node is
# non-empty and a BST, then returns size of
# the tree. Otherwise returns 0.


def largestBSTUtil(node, min_ref, max_ref,
                   max_size_ref, is_bst_ref):

    # Base Case
    if node == None:
        is_bst_ref[0] = 1  # An empty tree is BST
        return 0  # Size of the BST is 0

    Min = 999999999999

    # A flag variable for left subtree property
    # i.e., max(root.left) < root.data
    left_flag = False

    # A flag variable for right subtree property
    # i.e., min(root.right) > root.data
    right_flag = False

    ls, rs = 0, 0    # To store sizes of left and
    # right subtrees

    # Following tasks are done by recursive
    # call for left subtree
    # a) Get the maximum value in left subtree
    #   (Stored in max_ref[0])
    # b) Check whether Left Subtree is BST or
    #    not (Stored in is_bst_ref[0])
    # c) Get the size of maximum size BST in
    #    left subtree (updates max_size[0])
    max_ref[0] = -999999999999
    ls = largestBSTUtil(node.left, min_ref, max_ref,
                        max_size_ref, is_bst_ref)
    if is_bst_ref[0] == 1 and node.data > max_ref[0]:
        left_flag = True

    # Before updating min_ref[0], store the min
    # value in left subtree. So that we have the
    # correct minimum value for this subtree
    Min = min_ref[0]

    # The following recursive call does similar
    # (similar to left subtree) task for right subtree
    min_ref[0] = 999999999999
    rs = largestBSTUtil(node.right, min_ref, max_ref,
                        max_size_ref, is_bst_ref)
    if is_bst_ref[0] == 1 and node.data < min_ref[0]:
        right_flag = True

    # Update min and max values for the
    # parent recursive calls
    if Min < min_ref[0]:
        min_ref[0] = Min
    if node.data < min_ref[0]:  # For leaf nodes
        min_ref[0] = node.data
    if node.data > max_ref[0]:
        max_ref[0] = node.data

    # If both left and right subtrees are BST.
    # And left and right subtree properties hold
    # for this node, then this tree is BST.
    # So return the size of this tree
    if left_flag and right_flag:
        if ls + rs + 1 > max_size_ref[0]:
            max_size_ref[0] = ls + rs + 1
        return ls + rs + 1
    else:

        # Since this subtree is not BST, set is_bst
        # flag for parent calls is_bst_ref[0] = 0;
        return 0


# Driver Code
if __name__ == '__main__':

    # Let us construct the following Tree
    #     50
    # /     \
    # 10     60
    # / \     / \
    # 5 20 55 70
    #         /     / \
    #     45     65 80
    root = newNode(50)
    root.left = newNode(10)
    root.right = newNode(60)
    root.left.left = newNode(5)
    root.left.right = newNode(20)
    root.right.left = newNode(55)
    root.right.left.left = newNode(45)
    root.right.right = newNode(70)
    root.right.right.left = newNode(65)
    root.right.right.right = newNode(80)

# The complete tree is not BST as 45 is in
# right subtree of 50. The following subtree
# is the largest BST
#     60
# / \
# 55     70
# /     / \
# 45     65 80

print("Size of the largest BST is",
      largestBST(root))

# Working


def solve(root):
    if root == None:
        return [True, 0, float("inf"), 0-float("inf")]
    if root.left == None and root.right == None:
        return [True, 1, root.data, root.data]
    l = solve(root.left)
    r = solve(root.right)
    if l[0] and r[0]:
        if root.data > l[3] and root.data < r[2]:
            x, y = l[2], r[3]
            if x == float("inf"):
                x = root.data
            if y == 0-float("inf"):
                y = root.data
            return [True, l[1]+r[1]+1, x, y]
    return [False, max(l[1], r[1]), 0, 0]


class Solution:
    # Return the size of the largest sub-tree which is also a BST
    def largestBst(self, root):
        # code here
        ans = solve(root)
        return ans[1]
