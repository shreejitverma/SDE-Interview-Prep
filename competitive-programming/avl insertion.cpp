// C++ program for the above approach
#include <bits/stdc++.h>
using namespace std;

// AVL tree node
struct AVLwithparent {
    struct AVLwithparent* left;
    struct AVLwithparent* right;
    int key;
    struct AVLwithparent* par;
    int height;
};

// Function to update the height of
// a node according to its children's
// node's heights
void Updateheight(
    struct AVLwithparent* root)
{
    if (root != NULL) {

        // Store the height of the
        // current node
        int val = 1;

        // Store the height of the left
        // and right substree
        if (root->left != NULL)
            val = root->left->height + 1;

        if (root->right != NULL)
            val = max(
                val, root->right->height + 1);

        // Update the height of the
        // current node
        root->height = val;
    }
}

// Function to handle Left Left Case
struct AVLwithparent* LLR(
    struct AVLwithparent* root)
{
    // Create a reference to the
    // left child
    struct AVLwithparent* tmpnode = root->left;

    // Update the left child of the
    // root to the right child of the
    // current left child of the root
    root->left = tmpnode->right;

    // Update parent pointer of the
    // left child of the root node
    if (tmpnode->right != NULL)
        tmpnode->right->par = root;

    // Update the right child of
    // tmpnode to root
    tmpnode->right = root;

    // Update parent pointer of
    // the tmpnode
    tmpnode->par = root->par;

    // Update the parent pointer
    // of the root
    root->par = tmpnode;

    // Update tmpnode as the left or the
    // right child of its parent pointer
    // according to its key value
    if (tmpnode->par != NULL
        && root->key < tmpnode->par->key) {
        tmpnode->par->left = tmpnode;
    }
    else {
        if (tmpnode->par != NULL)
            tmpnode->par->right = tmpnode;
    }

    // Make tmpnode as the new root
    root = tmpnode;

    // Update the heights
    Updateheight(root->left);
    Updateheight(root->right);
    Updateheight(root);
    Updateheight(root->par);

    // Return the root node
    return root;
}

// Function to handle Right Right Case
struct AVLwithparent* RRR(
    struct AVLwithparent* root)
{
    // Create a reference to the
    // right child
    struct AVLwithparent* tmpnode = root->right;

    // Update the right child of the
    // root as the left child of the
    // current right child of the root
    root->right = tmpnode->left;

    // Update parent pointer of the
    // right child of the root node
    if (tmpnode->left != NULL)
        tmpnode->left->par = root;

    // Update the left child of the
    // tmpnode to root
    tmpnode->left = root;

    // Update parent pointer of
    // the tmpnode
    tmpnode->par = root->par;

    // Update the parent pointer
    // of the root
    root->par = tmpnode;

    // Update tmpnode as the left or
    // the right child of its parent
    // pointer according to its key value
    if (tmpnode->par != NULL
        && root->key < tmpnode->par->key) {
        tmpnode->par->left = tmpnode;
    }
    else {
        if (tmpnode->par != NULL)
            tmpnode->par->right = tmpnode;
    }

    // Make tmpnode as the new root
    root = tmpnode;

    // Update the heights
    Updateheight(root->left);
    Updateheight(root->right);
    Updateheight(root);
    Updateheight(root->par);

    // Return the root node
    return root;
}

// Function to handle Left Right Case
struct AVLwithparent* LRR(
    struct AVLwithparent* root)
{
    root->left = RRR(root->left);
    return LLR(root);
}

// Function to handle right left case
struct AVLwithparent* RLR(
    struct AVLwithparent* root)
{
    root->right = LLR(root->right);
    return RRR(root);
}

// Function to insert a node in
// the AVL tree
struct AVLwithparent* Insert(
    struct AVLwithparent* root,
    struct AVLwithparent* parent,
    int key)
{

    if (root == NULL) {

        // Create and assign values
        // to a new node
        root = new struct AVLwithparent;

        // If the root is NULL
        if (root == NULL) {
            cout << "Error in memory"
                 << endl;
        }

        // Otherwise
        else {
            root->height = 1;
            root->left = NULL;
            root->right = NULL;
            root->par = parent;
            root->key = key;
        }
    }

    else if (root->key > key) {

        // Recur to the left subtree
        // to insert the node
        root->left = Insert(root->left,
                            root, key);

        // Store the heights of the
        // left and right subtree
        int firstheight = 0;
        int secondheight = 0;

        if (root->left != NULL)
            firstheight = root->left->height;

        if (root->right != NULL)
            secondheight = root->right->height;

        // Balance the tree if the
        // current node is not balanced
        if (abs(firstheight
                - secondheight)
            == 2) {

            if (root->left != NULL
                && key < root->left->key) {

                // Left Left Case
                root = LLR(root);
            }
            else {

                // Left Right Case
                root = LRR(root);
            }
        }
    }

    else if (root->key < key) {

        // Recur to the right subtree
        // to insert the node
        root->right = Insert(root->right,
                             root, key);

        // Store the heights of the
        // left and right subtree
        int firstheight = 0;
        int secondheight = 0;

        if (root->left != NULL)
            firstheight
                = root->left->height;

        if (root->right != NULL)
            secondheight = root->right->height;

        // Balance the tree if the
        // current node is not balanced
        if (abs(firstheight - secondheight) == 2) {
            if (root->right != NULL
                && key < root->right->key) {

                // Right Left Case
                root = RLR(root);
            }
            else {

                // Right Right Case
                root = RRR(root);
            }
        }
    }

    // Case when given key is already
    // in the tree
    else {
    }

    // Update the height of the
    // root node
    Updateheight(root);

    // Return the root node
    return root;
}

// Function to print the preorder
// traversal of the AVL tree
void printpreorder(
    struct AVLwithparent* root)
{
    // Print the node's value along
    // with its parent value
    cout << "Node: " << root->key
         << ", Parent Node: ";

    if (root->par != NULL)
        cout << root->par->key << endl;
    else
        cout << "NULL" << endl;

    // Recur to the left subtree
    if (root->left != NULL) {
        printpreorder(root->left);
    }

    // Recur to the right subtree
    if (root->right != NULL) {
        printpreorder(root->right);
    }
}

// Driver Code
int main()
{
    struct AVLwithparent* root;
    root = NULL;

    // Function Call to insert nodes
    root = Insert(root, NULL, 10);
    root = Insert(root, NULL, 20);
    root = Insert(root, NULL, 30);
    root = Insert(root, NULL, 40);
    root = Insert(root, NULL, 50);
    root = Insert(root, NULL, 25);

    // Function call to print the tree
    printpreorder(root);
}