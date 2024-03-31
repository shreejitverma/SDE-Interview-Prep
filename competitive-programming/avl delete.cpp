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

// Function to print the preorder
// traversal of the AVL tree
void printpreorder(struct AVLwithparent* root)
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

    // Update parent pointer of left
    // child of the root node
    if (tmpnode->right != NULL)
        tmpnode->right->par = root;

    // Update the right child of
    // tmpnode to root
    tmpnode->right = root;

    // Update parent pointer of tmpnode
    tmpnode->par = root->par;

    // Update the parent pointer of root
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

    // Update parent pointer of tmpnode
    tmpnode->par = root->par;

    // Update the parent pointer of root
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

// Function to balance the tree after
// deletion of a node
struct AVLwithparent* Balance(
    struct AVLwithparent* root)
{
    // Store the current height of
    // the left and right subtree
    int firstheight = 0;
    int secondheight = 0;

    if (root->left != NULL)
        firstheight = root->left->height;

    if (root->right != NULL)
        secondheight = root->right->height;

    // If current node is not balanced
    if (abs(firstheight - secondheight) == 2) {
        if (firstheight < secondheight) {

            // Store the height of the
            // left and right subtree
            // of the current node's
            // right subtree
            int rightheight1 = 0;
            int rightheight2 = 0;
            if (root->right->right != NULL)
                rightheight2 = root->right->right->height;

            if (root->right->left != NULL)
                rightheight1 = root->right->left->height;

            if (rightheight1 > rightheight2) {

                // Right Left Case
                root = RLR(root);
            }
            else {

                // Right Right Case
                root = RRR(root);
            }
        }
        else {

            // Store the height of the
            // left and right subtree
            // of the current node's
            // left subtree
            int leftheight1 = 0;
            int leftheight2 = 0;
            if (root->left->right != NULL)
                leftheight2 = root->left->right->height;

            if (root->left->left != NULL)
                leftheight1 = root->left->left->height;

            if (leftheight1 > leftheight2) {

                // Left Left Case
                root = LLR(root);
            }
            else {

                // Left Right Case
                root = LRR(root);
            }
        }
    }

    // Return the root node
    return root;
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
        if (root == NULL)
            cout << "Error in memory" << endl;
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

        // Store the heights of the left
        // and right subtree
        int firstheight = 0;
        int secondheight = 0;

        if (root->left != NULL)
            firstheight = root->left->height;

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

    // Case when given key is
    // already in tree
    else {
    }

    // Update the height of the
    // root node
    Updateheight(root);

    // Return the root node
    return root;
}

// Function to delete a node from
// the AVL tree
struct AVLwithparent* Delete(
    struct AVLwithparent* root,
    int key)
{
    if (root != NULL) {

        // If the node is found
        if (root->key == key) {

            // Replace root with its
            // left child
            if (root->right == NULL
                && root->left != NULL) {
                if (root->par != NULL) {
                    if (root->par->key
                        < root->key)
                        root->par->right = root->left;
                    else
                        root->par->left = root->left;

                    // Update the height
                    // of root's parent
                    Updateheight(root->par);
                }

                root->left->par = root->par;

                // Balance the node
                // after deletion
                root->left = Balance(
                    root->left);

                return root->left;
            }

            // Replace root with its
            // right child
            else if (root->left == NULL
                     && root->right != NULL) {
                if (root->par != NULL) {
                    if (root->par->key
                        < root->key)
                        root->par->right = root->right;
                    else
                        root->par->left = root->right;

                    // Update the height
                    // of the root's parent
                    Updateheight(root->par);
                }

                root->right->par = root->par;

                // Balance the node after
                // deletion
                root->right = Balance(root->right);
                return root->right;
            }

            // Remove the references of
            // the current node
            else if (root->left == NULL
                     && root->right == NULL) {
                if (root->par->key < root->key) {
                    root->par->right = NULL;
                }
                else {
                    root->par->left = NULL;
                }

                if (root->par != NULL)
                    Updateheight(root->par);

                root = NULL;
                return NULL;
            }

            // Otherwise, replace the
            // current node with its
            // successor and then
            // recursively call Delete()
            else {
                struct AVLwithparent* tmpnode = root;
                tmpnode = tmpnode->right;
                while (tmpnode->left != NULL) {
                    tmpnode = tmpnode->left;
                }

                int val = tmpnode->key;

                root->right
                    = Delete(root->right, tmpnode->key);

                root->key = val;

                // Balance the node
                // after deletion
                root = Balance(root);
            }
        }

        // Recur to the right subtree to
        // delete the current node
        else if (root->key < key) {
            root->right = Delete(root->right, key);

            root = Balance(root);
        }

        // Recur into the right subtree
        // to delete the current node
        else if (root->key > key) {
            root->left = Delete(root->left, key);

            root = Balance(root);
        }

        // Update height of the root
        if (root != NULL) {
            Updateheight(root);
        }
    }

    // Handle the case when the key to be
    // deleted could not be found
    else {
        cout << "Key to be deleted "
             << "could not be found\n";
    }

    // Return the root node
    return root;
}

// Driver Code
int main()
{
    struct AVLwithparent* root;
    root = NULL;

    // Function call to insert the nodes
    root = Insert(root, NULL, 9);
    root = Insert(root, NULL, 5);
    root = Insert(root, NULL, 10);
    root = Insert(root, NULL, 0);
    root = Insert(root, NULL, 6);

    // Print the tree before deleting node
    cout << "Before deletion:\n";
    printpreorder(root);

    // Function Call to delete node 10
    root = Delete(root, 10);

    // Print the tree after deleting node
    cout << "After deletion:\n";
    printpreorder(root);
}