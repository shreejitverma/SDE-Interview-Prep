/*
https://www.geeksforgeeks.org/construct-bst-from-given-preorder-traversa/
Method 2 ( O(n) time complexity ) 
The idea used here is inspired by method 3 of this post. The trick is to set a range {min .. max} for every node. Initialize the range as {INT_MIN .. INT_MAX}. The first node will definitely be in range, so create a root node. To construct the left subtree, set the range as {INT_MIN …root->data}. If a value is in the range {INT_MIN .. root->data}, the values are part of the left subtree. To construct the right subtree, set the range as {root->data..max .. INT_MAX}. 

Below is the implementation of the above idea:
*/

/* A O(n) program for construction
of BST from preorder traversal */
#include <bits/stdc++.h>
using namespace std;

/* A binary tree node has data, pointer to left child
and a pointer to right child */
class node
{
public:
    int data;
    node *left;
    node *right;
};

// A utility function to create a node
node *newNode(int data)
{
    node *temp = new node();

    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}

// A recursive function to construct
// BST from pre[]. preIndex is used
// to keep track of index in pre[].
node *constructTreeUtil(int pre[], int *preIndex, int key,
                        int min, int max, int size)
{
    // Base case
    if (*preIndex >= size)
        return NULL;

    node *root = NULL;

    // If current element of pre[] is in range, then
    // only it is part of current subtree
    if (key > min && key < max)
    {
        // Allocate memory for root of this
        // subtree and increment *preIndex
        root = newNode(key);
        *preIndex = *preIndex + 1;

        if (*preIndex < size)
        {
            // Construct the subtree under root
            // All nodes which are in range
            // {min .. key} will go in left
            // subtree, and first such node
            // will be root of left subtree.
            root->left = constructTreeUtil(pre, preIndex,
                                           pre[*preIndex],
                                           min, key, size);
        }
        if (*preIndex < size)
        {
            // All nodes which are in range
            // {key..max} will go in right
            // subtree, and first such node
            // will be root of right subtree.
            root->right = constructTreeUtil(pre, preIndex,
                                            pre[*preIndex],
                                            key, max, size);
        }
    }

    return root;
}

// The main function to construct BST
// from given preorder traversal.
// This function mainly uses constructTreeUtil()
node *constructTree(int pre[], int size)
{
    int preIndex = 0;
    return constructTreeUtil(pre, &preIndex, pre[0],
                             INT_MIN, INT_MAX, size);
}

// A utility function to print inorder
// traversal of a Binary Tree
void printInorder(node *node)
{
    if (node == NULL)
        return;
    printInorder(node->left);
    cout << node->data << " ";
    printInorder(node->right);
}

// Driver code
int main()
{
    int pre[] = {10, 5, 1, 7, 40, 50};
    int size = sizeof(pre) / sizeof(pre[0]);

    // Function call
    node *root = constructTree(pre, size);

    cout << "Inorder traversal of the constructed tree: \n";
    printInorder(root);

    return 0;
}
