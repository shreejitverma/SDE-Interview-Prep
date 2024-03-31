/*https://www.geeksforgeeks.org/create-a-mirror-tree-from-the-given-binary-tree/
Create a mirror tree from the given binary tree
Difficulty Level : Easy
Last Updated : 12 Oct, 2021
Geek Week
Given a binary tree, the task is to create a new binary tree which is a mirror image of the given binary tree.

Examples: 

Input:
        5
       / \
      3   6
     / \
    2   4
Output:
Inorder of original tree: 2 3 4 5 6 
Inorder of mirror tree: 6 5 4 3 2
Mirror tree will be:
  5
 / \
6   3
   / \
  4   2

Input:
        2
       / \
      1   8
     /     \
    12      9
Output:
Inorder of original tree: 12 1 2 8 9 
Inorder of mirror tree: 9 8 2 1 12*/
#include <iostream>
using namespace std;

typedef struct treenode
{
    int val;
    struct treenode *left;
    struct treenode *right;
} node;

// Helper function that
// allocates a new node with the
// given data and NULL left and right pointers
node *createNode(int val)
{
    node *newNode = (node *)malloc(sizeof(node));
    newNode->val = val;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Function to print the inrder traversal
void inorder(node *root)
{
    if (root == NULL)
        return;
    inorder(root->left);
    printf("%d ", root->val);
    inorder(root->right);
}

// Function to convert to  mirror tree
treenode *mirrorTree(node *root)
{
    // Base Case
    if (root == NULL)
        return root;
    node *t = root->left;
    root->left = root->right;
    root->right = t;

    if (root->left)
        mirrorTree(root->left);
    if (root->right)
        mirrorTree(root->right);

    return root;
}

// Driver Code
int main()
{

    node *tree = createNode(5);
    tree->left = createNode(3);
    tree->right = createNode(6);
    tree->left->left = createNode(2);
    tree->left->right = createNode(4);
    printf("Inorder of original tree: ");
    inorder(tree);

    // Function call
    mirrorTree(tree);

    printf("\nInorder of Mirror tree: ");
    inorder(tree);
    return 0;
}