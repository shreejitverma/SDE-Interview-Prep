/*
https://www.geeksforgeeks.org/find-largest-subtree-sum-tree/
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
Implementation : 
*/

// C++ program to find largest subtree
// sum in a given binary tree.
#include <bits/stdc++.h>
using namespace std;

// Structure of a tree node.
struct Node
{
    int key;
    Node *left, *right;
};

// Function to create new tree node.
Node *newNode(int key)
{
    Node *temp = new Node;
    temp->key = key;
    temp->left = temp->right = NULL;
    return temp;
}

// Helper function to find largest
// subtree sum recursively.
int findLargestSubtreeSumUtil(Node *root, int &ans)
{
    // If current node is null then
    // return 0 to parent node.
    if (root == NULL)
        return 0;

    // Subtree sum rooted at current node.
    int currSum = root->key +
                  findLargestSubtreeSumUtil(root->left, ans) + findLargestSubtreeSumUtil(root->right, ans);

    // Update answer if current subtree
    // sum is greater than answer so far.
    ans = max(ans, currSum);

    // Return current subtree sum to
    // its parent node.
    return currSum;
}

// Function to find largest subtree sum.
int findLargestSubtreeSum(Node *root)
{
    // If tree does not exist,
    // then answer is 0.
    if (root == NULL)
        return 0;

    // Variable to store maximum subtree sum.
    int ans = INT_MIN;

    // Call to recursive function to
    // find maximum subtree sum.
    findLargestSubtreeSumUtil(root, ans);

    return ans;
}

// Driver function
int main()
{
    /*
               1
             /   \
            /     \
          -2       3
          / \     /  \
         /   \   /    \
        4     5 -6     2
    */

    Node *root = newNode(1);
    root->left = newNode(-2);
    root->right = newNode(3);
    root->left->left = newNode(4);
    root->left->right = newNode(5);
    root->right->left = newNode(-6);
    root->right->right = newNode(2);

    cout << findLargestSubtreeSum(root);
    return 0;
}

/*
Output: 
7
 

Time Complexity: O(n), where n is number of nodes. 
Auxiliary Space: O(n), function call stack size. 
*/