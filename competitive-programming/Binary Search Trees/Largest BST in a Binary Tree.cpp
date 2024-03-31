/*
https://www.geeksforgeeks.org/find-the-largest-subtree-in-a-tree-that-is-also-a-bst/
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
1 ≤ Data of a node ≤ 106
*/

/*
Method 2 (Tricky and Efficient) 
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
passing minimum value up the tree. 
*/

// C++ program of above approach
#include <bits/stdc++.h>

using namespace std;

/* A binary tree node has data,
pointer to left child and a pointer
to right child */
class node
{
public:
    int data;
    node *left;
    node *right;

    /* Constructor that allocates
    a new node with the given data
    and NULL left and right pointers. */
    node(int data)
    {
        this->data = data;
        this->left = NULL;
        this->right = NULL;
    }
};

int largestBSTUtil(node *node, int *min_ref, int *max_ref,
                   int *max_size_ref, bool *is_bst_ref);

/* Returns size of the largest BST
subtree in a Binary Tree
(efficient version). */
int largestBST(node *node)
{
    // Set the initial values for
    // calling largestBSTUtil()
    int min = INT_MAX; // For minimum value in right subtree
    int max = INT_MIN; // For maximum value in left subtree

    int max_size = 0; // For size of the largest BST
    bool is_bst = 0;

    largestBSTUtil(node, &min, &max,
                   &max_size, &is_bst);

    return max_size;
}

/* largestBSTUtil() updates *max_size_ref
for the size of the largest BST subtree.
Also, if the tree rooted with node is
non-empty and a BST, then returns size
of the tree. Otherwise returns 0.*/
int largestBSTUtil(node *node, int *min_ref, int *max_ref,
                   int *max_size_ref, bool *is_bst_ref)
{

    /* Base Case */
    if (node == NULL)
    {
        *is_bst_ref = 1; // An empty tree is BST
        return 0;        // Size of the BST is 0
    }

    int min = INT_MAX;

    /* A flag variable for left subtree property
        i.e., max(root->left) < root->data */
    bool left_flag = false;

    /* A flag variable for right subtree property
        i.e., min(root->right) > root->data */
    bool right_flag = false;

    int ls, rs; // To store sizes of left and right subtrees

    /* Following tasks are done by
    recursive call for left subtree
        a) Get the maximum value in left
        subtree (Stored in *max_ref)
        b) Check whether Left Subtree is
        BST or not (Stored in *is_bst_ref)
        c) Get the size of maximum size BST
        in left subtree (updates *max_size) */
    *max_ref = INT_MIN;
    ls = largestBSTUtil(node->left, min_ref, max_ref,
                        max_size_ref, is_bst_ref);
    if (*is_bst_ref == 1 && node->data > *max_ref)
        left_flag = true;

    /* Before updating *min_ref, store the min
    value in left subtree. So that we have the
    correct minimum value for this subtree */
    min = *min_ref;

    /* The following recursive call
    does similar (similar to left subtree)
    task for right subtree */
    *min_ref = INT_MAX;
    rs = largestBSTUtil(node->right, min_ref,
                        max_ref, max_size_ref, is_bst_ref);
    if (*is_bst_ref == 1 && node->data < *min_ref)
        right_flag = true;

    // Update min and max values for
    // the parent recursive calls
    if (min < *min_ref)
        *min_ref = min;
    if (node->data < *min_ref) // For leaf nodes
        *min_ref = node->data;
    if (node->data > *max_ref)
        *max_ref = node->data;

    /* If both left and right subtrees are BST.
    And left and right subtree properties hold
    for this node, then this tree is BST.
    So return the size of this tree */
    if (left_flag && right_flag)
    {
        if (ls + rs + 1 > *max_size_ref)
            *max_size_ref = ls + rs + 1;
        return ls + rs + 1;
    }
    else
    {
        // Since this subtree is not BST,
        // set is_bst flag for parent calls
        *is_bst_ref = 0;
        return 0;
    }
}

/* Driver code*/
int main()
{
    /* Let us construct the following Tree
            50
        / \
        10 60
        / \ / \
    5 20 55 70
                / / \
            45 65 80
    */

    node *root = new node(50);
    root->left = new node(10);
    root->right = new node(60);
    root->left->left = new node(5);
    root->left->right = new node(20);
    root->right->left = new node(55);
    root->right->left->left = new node(45);
    root->right->right = new node(70);
    root->right->right->left = new node(65);
    root->right->right->right = new node(80);

    /* The complete tree is not BST
    as 45 is in right subtree of 50.
    The following subtree is the largest BST
            60
        / \
        55 70
    / / \
    45 65 80
    */
    cout << " Size of the largest BST is " << largestBST(root);

    return 0;
}