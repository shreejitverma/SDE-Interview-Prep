/*
https://practice.geeksforgeeks.org/problems/find-k-th-smallest-element-in-bst/1
k-th smallest element in BST 
Medium Accuracy: 49.44% Submissions: 49545 Points: 4
Given a BST and an integer K. Find the Kth Smallest element in the BST. 

Example 1:

Input:
      2
    /   \
   1     3
K = 2
Output: 2
Example 2:

Input:
        2
      /  \
     1    3
K = 5
Output: -1
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function KthSmallestElement() which takes the root of the BST and integer K as inputs and return the Kth smallest element in the BST, if no such element exists return -1.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1<=Number of nodes<=100000
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;
#define MAX_HEIGHT 100000

// Tree Node
struct Node
{
    int data;
    Node *left;
    Node *right;

    Node(int val)
    {
        data = val;
        left = right = NULL;
    }
};

// Function to Build Tree
Node *buildTree(string str)
{
    // Corner Case
    if (str.length() == 0 || str[0] == 'N')
        return NULL;

    // Creating vector of strings from input
    // string after spliting by space
    vector<string> ip;

    istringstream iss(str);
    for (string str; iss >> str;)
        ip.push_back(str);

    // Create the root of the tree
    Node *root = new Node(stoi(ip[0]));

    // Push the root to the queue
    queue<Node *> queue;
    queue.push(root);

    // Starting from the second element
    int i = 1;
    while (!queue.empty() && i < ip.size())
    {

        // Get and remove the front of the queue
        Node *currNode = queue.front();
        queue.pop();

        // Get the current node's value from the string
        string currVal = ip[i];

        // If the left child is not null
        if (currVal != "N")
        {

            // Create the left child for the current node
            currNode->left = new Node(stoi(currVal));

            // Push it to the queue
            queue.push(currNode->left);
        }

        // For the right child
        i++;
        if (i >= ip.size())
            break;
        currVal = ip[i];

        // If the right child is not null
        if (currVal != "N")
        {

            // Create the right child for the current node
            currNode->right = new Node(stoi(currVal));

            // Push it to the queue
            queue.push(currNode->right);
        }
        i++;
    }

    return root;
}

// } Driver Code Ends
/*Complete the function below

struct Node {
    int data;
    Node *left;
    Node *right;

    Node(int val) {
        data = val;
        left = right = NULL;
    }
};
*/

class Solution
{
public:
    // Return the Kth smallest element in the given BST
    int KthSmallestElement(Node *root, int k)
    {
        // add code here.
        {
            //add code here.
            if (root == NULL)
            {
                return -1;
            }
            Node *curr, *pre;
            curr = root;
            while (curr != NULL)
            {
                if (curr->left == NULL)
                {
                    k--;
                    if (k == 0)
                    {
                        return curr->data;
                    }
                    curr = curr->right;
                }
                else
                {
                    pre = curr->left;
                    while (pre->right != NULL && pre->right != curr)
                    {
                        pre = pre->right;
                    }
                    if (pre->right == NULL)
                    {
                        pre->right = curr;
                        curr = curr->left;
                    }
                    else
                    {
                        pre->right = NULL;
                        k--;
                        if (k == 0)
                        {
                            return curr->data;
                        }
                        curr = curr->right;
                    }
                }
            }
            return -1;
        }
    }
};

// { Driver Code Starts.
int main()
{

    int t;
    string tc;
    getline(cin, tc);
    t = stoi(tc);
    while (t--)
    {
        string s;
        getline(cin, s);
        Node *root = buildTree(s);

        getline(cin, s);
        int k = stoi(s);
        //  getline(cin, s);
        Solution obj;
        cout << obj.KthSmallestElement(root, k) << endl;
        // cout<<"~"<<endl;
    }
    return 0;
} // } Driver Code Ends