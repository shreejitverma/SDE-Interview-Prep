/*https://practice.geeksforgeeks.org/problems/reverse-level-order-traversal/1
https://www.geeksforgeeks.org/reverse-level-order-traversal/
Reverse Level Order Traversal 
Easy Accuracy: 47.34% Submissions: 61244 Points: 2
Given a binary tree of size N, find its reverse level order traversal. ie- the traversal must begin from the last level.

Example 1:

Input :
        1
      /   \
     3     2

Output: 3 2 1
Explanation:
Traversing level 1 : 3 2
Traversing level 0 : 1
Example 2:

Input :
       10
      /  \
     20   30
    / \ 
   40  60

Output: 40 60 20 30 10
Explanation:
Traversing level 2 : 40 60
Traversing level 1 : 20 30
Traversing level 0 : 10

Your Task: 
You dont need to read input or print anything. Complete the function reverseLevelOrder() which takes the root of the tree as input parameter and returns a list containing the reverse level order traversal of the given tree.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
1 ≤ N ≤ 10^4*/

// A C++ program to print REVERSE level order traversal using stack and queue
// This approach is adopted from following link
// http://tech-queries.blogspot.in/2008/12/level-order-tree-traversal-in-reverse.html
#include <bits/stdc++.h>
using namespace std;

/* A binary tree node has data, pointer to left and right children */
struct node
{
    int data;
    struct node *left;
    struct node *right;
};

/* Given a binary tree, print its nodes in reverse level order */
void reverseLevelOrder(node *root)
{
    stack<node *> S;
    queue<node *> Q;
    Q.push(root);

    // Do something like normal level order traversal order. Following are the
    // differences with normal level order traversal
    // 1) Instead of printing a node, we push the node to stack
    // 2) Right subtree is visited before left subtree
    while (Q.empty() == false)
    {
        /* Dequeue node and make it root */
        root = Q.front();
        Q.pop();
        S.push(root);

        /* Enqueue right child */
        if (root->right)
            Q.push(root->right); // NOTE: RIGHT CHILD IS ENQUEUED BEFORE LEFT

        /* Enqueue left child */
        if (root->left)
            Q.push(root->left);
    }

    // Now pop all items from stack one by one and print them
    while (S.empty() == false)
    {
        root = S.top();
        cout << root->data << " ";
        S.pop();
    }
}

/* Helper function that allocates a new node with the
   given data and NULL left and right pointers. */
node *newNode(int data)
{
    node *temp = new node;
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;

    return (temp);
}

/* Driver program to test above functions*/
int main()
{
    struct node *root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(4);
    root->left->right = newNode(5);
    root->right->left = newNode(6);
    root->right->right = newNode(7);

    cout << "Level Order traversal of binary tree is \n";
    reverseLevelOrder(root);

    return 0;
}

/*   
struct Node
{
    int data;
    Node* left;
    Node* right;
}; */

// vector<int> reverseLevelOrder(Node *root)
// {
//     vector<int> v;
//     stack <Node *> S;
//     queue <Node *> Q;
//     Q.push(root);

//     // Do something like normal level order traversal order. Following are the
//     // differences with normal level order traversal
//     // 1) Instead of printing a node, we push the node to stack
//     // 2) Right subtree is visited before left subtree
//     while (Q.empty() == false)
//     {
//         /* Dequeue node and make it root */
//         root = Q.front();
//         Q.pop();
//         S.push(root);

//         /* Enqueue right child */
//         if (root->right)
//             Q.push(root->right); // NOTE: RIGHT CHILD IS ENQUEUED BEFORE LEFT

//         /* Enqueue left child */
//         if (root->left)
//             Q.push(root->left);
//     }

//     // Now pop all items from stack one by one and print them
//     while (S.empty() == false)
//     {
//         root = S.top();
//         v.push_back(root->data);
//         S.pop();
//     }
//     return v;
// }