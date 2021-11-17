/*
https://www.geeksforgeeks.org/flatten-bst-to-sorted-list-increasing-order/
Given a binary search tree, the task is to flatten it to a sorted list. Precisely, the value of each node must be lesser than the values of all the nodes at its right, and its left node must be NULL after flattening. We must do it in O(H) extra space where ‘H’ is the height of BST.

Examples: 

Input: 
          5 
        /   \ 
       3     7 
      / \   / \ 
     2   4 6   8
Output: 2 3 4 5 6 7 8
Input:
      1
       \
        2
         \
          3
           \
            4
             \
              5
Output: 1 2 3 4 5
Recommended: Please try your approach on {IDE} first, before moving on to the solution.
Approach: A simple approach will be to recreate the BST from its in-order traversal. This will take O(N) extra space where N is the number of nodes in BST. 



To improve upon that, we will simulate in-order traversal of a binary tree as follows:  

Create a dummy node.
Create a variable called ‘prev’ and make it point to the dummy node.
Perform in-order traversal and at each step. 
Set prev -> right = curr
Set prev -> left = NULL
Set prev = curr
This will improve the space complexity to O(H) in worst case as in-order traversal takes O(H) extra space.

Below is the implementation of the above approach: 
*/

// C++ implementation of the approach
#include <bits/stdc++.h>
using namespace std;

// Node of the binary tree
struct node
{
    int data;
    node *left;
    node *right;
    node(int data)
    {
        this->data = data;
        left = NULL;
        right = NULL;
    }
};

// Function to print flattened
// binary Tree
void print(node *parent)
{
    node *curr = parent;
    while (curr != NULL)
        cout << curr->data << " ", curr = curr->right;
}

// Function to perform in-order traversal
// recursively
void inorder(node *curr, node *&prev)
{
    // Base case
    if (curr == NULL)
        return;
    inorder(curr->left, prev);
    prev->left = NULL;
    prev->right = curr;
    prev = curr;
    inorder(curr->right, prev);
}

// Function to flatten binary tree using
// level order traversal
node *flatten(node *parent)
{
    // Dummy node
    node *dummy = new node(-1);

    // Pointer to previous element
    node *prev = dummy;

    // Calling in-order traversal
    inorder(parent, prev);

    prev->left = NULL;
    prev->right = NULL;
    node *ret = dummy->right;

    // Delete dummy node
    delete dummy;
    return ret;
}

// Driver code
int main()
{
    node *root = new node(5);
    root->left = new node(3);
    root->right = new node(7);
    root->left->left = new node(2);
    root->left->right = new node(4);
    root->right->left = new node(6);
    root->right->right = new node(8);

    // Calling required function
    print(flatten(root));

    return 0;
}

// Time Complexity: O(N)
// Auxiliary Space: O(N)