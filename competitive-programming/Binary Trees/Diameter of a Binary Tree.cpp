/*https://www.geeksforgeeks.org/diameter-of-a-binary-tree/
https://practice.geeksforgeeks.org/problems/diameter-of-binary-tree/1
Diameter of a Binary Tree
Easy Accuracy: 50.01% Submissions: 100k+ Points: 2
The diameter of a tree (sometimes called the width) is the number of nodes on the longest path between two end nodes. The diagram below shows two trees each with diameter nine, the leaves that form the ends of the longest path are shaded (note that there is more than one path in each tree of length nine, but no path longer than nine nodes).



Example 1:

Input:
       1
     /  \
    2    3
Output: 3
Example 2:

Input:
         10
        /   \
      20    30
    /   \
   40   60
Output: 4
Your Task:
You need to complete the function diameter() that takes root as parameter and returns the diameter.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(Height of the Tree).

Constraints:
1 <= Number of nodes <= 10000
1 <= Data of a node <= 1000*/

// Recursive optimized C program to find the diameter of a
// Binary Tree
// #include <bits/stdc++.h>
#include <iostream>
using namespace std;

// A binary tree node has data, pointer to left child
// and a pointer to right child
struct node
{
    int data;
    struct node *left, *right;
};
struct node *newNode(int data)
{
    struct node *Node = (struct node *)malloc(sizeof(struct node));
    Node->data = data;
    Node->left = NULL;
    Node->right = NULL;

    return (Node);
}

// function to create a new node of tree and returns pointer
struct node *newNode(int data);

// returns max of two integers
int max(int a, int b) { return (a > b) ? a : b; }

// function to Compute height of a tree.
int height(struct node *Node);

// Function to get diameter of a binary tree
int diameter(struct node *tree)
{
    // base case where tree is empty
    if (tree == NULL)
        return 0;

    // get the height of left and right sub-trees
    int lheight = height(tree->left);
    int rheight = height(tree->right);

    // get the diameter of left and right sub-trees
    int ldiameter = diameter(tree->left);
    int rdiameter = diameter(tree->right);

    // Return max of following three
    // 1) Diameter of left subtree
    // 2) Diameter of right subtree
    // 3) Height of left subtree + height of right subtree + 1
    return max(lheight + rheight + 1,
               max(ldiameter, rdiameter));
}

// UTILITY FUNCTIONS TO TEST diameter() FUNCTION

// The function Compute the "height" of a tree. Height is
// the number f nodes along the longest path from the root
// node down to the farthest leaf node.
int height(struct node *Node)
{
    // base case tree is empty
    if (Node == NULL)
        return 0;

    // If tree is not empty then height = 1 + max of left
    // height and right heights
    return 1 + max(height(Node->left), height(Node->right));
}

// Helper function that allocates a new node with the
// given data and NULL left and right pointers.

// Driver Code
int main()
{

    /* Constructed binary tree is
            1
            / \
        2     3
        / \
    4     5
    */
    struct node *root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(4);
    root->left->right = newNode(5);

    // Function Call
    cout << "Diameter of the given binary tree is " << diameter(root);

    return 0;
}