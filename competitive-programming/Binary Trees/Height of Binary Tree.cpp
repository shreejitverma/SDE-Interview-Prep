/*Height of Binary Tree 
Medium Accuracy: 65.76% Submissions: 100k+ Points: 4
Given a binary tree, find its height.


Example 1:

Input:
     1
    /  \
   2    3
Output: 2
Example 2:

Input:
  2
   \
    1
   /
 3
Output: 3   

Your Task:
You don't need to read input or print anything. Your task is to complete the function height() which takes root node of the tree as input parameter and returns an integer denoting the height of the tree. If the tree is empty, return 0. 


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
1 <= Number of nodes <= 105
1 <= Data of a node <= 105
*/
class Solution
{
public:
    int maxDepth(TreeNode *root)
    {
        // Start typing your C/C++ solution below
        // DO NOT write int main() function
        if (!root)
            return 0;
        int nLeftDeep = 1;
        int nRightDeep = 1;
        if (root->left)
            nLeftDeep = nLeftDeep + maxDepth(root->left);
        if (root->right)
            nRightDeep = nRightDeep + maxDepth(root->right);
        if (nLeftDeep > nRightDeep)
            return nLeftDeep;
        else
            return nRightDeep;
    }
};
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// A Tree node
struct Node
{
    int key;
    struct Node *left, *right;
};

// Utility function to create a new node
Node *newNode(int key)
{
    Node *temp = new Node;
    temp->key = key;
    temp->left = temp->right = NULL;
    return (temp);
}

/*Function to find the height(depth) of the tree*/
int height(struct Node *root)
{

    //Initialising a variable to count the
    //height of tree
    int depth = 0;

    queue<Node *> q;

    //Pushing first level element along with NULL
    q.push(root);
    q.push(NULL);
    while (!q.empty())
    {
        Node *temp = q.front();
        q.pop();

        //When NULL encountered, increment the value
        if (temp == NULL)
        {
            depth++;
        }

        //If NULL not encountered, keep moving
        if (temp != NULL)
        {
            if (temp->left)
            {
                q.push(temp->left);
            }
            if (temp->right)
            {
                q.push(temp->right);
            }
        }

        //If queue still have elements left,
        //push NULL again to the queue.
        else if (!q.empty())
        {
            q.push(NULL);
        }
    }
    return depth;
}

// Driver program
int main()
{
    // Let us create Binary Tree shown in above example
    Node *root = newNode(1);
    root->left = newNode(12);
    root->right = newNode(13);

    root->right->left = newNode(14);
    root->right->right = newNode(15);

    root->right->left->left = newNode(21);
    root->right->left->right = newNode(22);
    root->right->right->left = newNode(23);
    root->right->right->right = newNode(24);

    cout << "Height(Depth) of tree is: " << height(root);
}