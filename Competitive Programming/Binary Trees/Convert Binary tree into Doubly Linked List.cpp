/*
https://practice.geeksforgeeks.org/problems/binary-tree-to-dll/1
Binary Tree to DLL 
Hard Accuracy: 41.34% Submissions: 79521 Points: 8
Given a Binary Tree (BT), convert it to a Doubly Linked List(DLL) In-Place. The left and right pointers in nodes are to be used as previous and next pointers respectively in converted DLL. The order of nodes in DLL must be same as Inorder of the given Binary Tree. The first node of Inorder traversal (leftmost node in BT) must be the head node of the DLL.

TreeToList

Example 1:

Input:
      1
    /  \
   3    2
Output:
3 1 2 
2 1 3 
Explanation: DLL would be 3<=>1<=>2
Example 2:

Input:
       10
      /   \
     20   30
   /   \
  40   60
Output:
40 20 60 10 30 
30 10 60 20 40
Explanation:  DLL would be 
40<=>20<=>60<=>10<=>30.
Your Task:
You don't have to take input. Complete the function bToDLL() that takes root node of the tree as a parameter and returns the head of DLL . The driver code prints the DLL both ways.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(H).
Note: H is the height of the tree and this space is used implicitly for the recursion stack.

Constraints:
1 ≤ Number of nodes ≤ 105
1 ≤ Data of a node ≤ 105
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// Tree Node
struct Node
{
    int data;
    Node *left;
    Node *right;
};

// Utility function to create a new Tree Node
Node *newNode(int val)
{
    Node *temp = new Node;
    temp->data = val;
    temp->left = NULL;
    temp->right = NULL;

    return temp;
}

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
    Node *root = newNode(stoi(ip[0]));

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
            currNode->left = newNode(stoi(currVal));

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
            currNode->right = newNode(stoi(currVal));

            // Push it to the queue
            queue.push(currNode->right);
        }
        i++;
    }

    return root;
}

// } Driver Code Ends
/* Structure for tree and linked list

struct Node
{
    int data;
    struct Node* left;
    struct Node* right;
    
    Node(int x){
        data = x;
        left = right = NULL;
    }
};
 */

// This function should return head to the DLL
class Solution
{
public:
    //Function to convert binary tree to doubly linked list and return it.
    void bDLL(Node *root, Node *&head, Node *&prev)
    {
        if (!root)
            return;
        bDLL(root->left, head, prev);
        root->left = prev;
        if (!prev)
            head = root;
        else
        {
            prev->right = root;
        }
        prev = root;
        bDLL(root->right, head, prev);
    }
    // This function should return head to the DLL
    Node *bToDLL(Node *root)
    {
        Node *head = NULL;
        Node *prev = NULL;
        bDLL(root, head, prev);
        return head;
    }
};

// { Driver Code Starts.

/* Function to print nodes in a given doubly linked list */
void printList(Node *node)
{
    Node *prev = NULL;
    while (node != NULL)
    {
        cout << node->data << " ";
        prev = node;
        node = node->right;
    }
    cout << endl;
    while (prev != NULL)
    {
        cout << prev->data << " ";
        prev = prev->left;
    }
    cout << endl;
}

void inorder(Node *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        cout << root->data;
        inorder(root->right);
    }
}

/* Driver program to test size function*/
int main()
{
    int t;
    cin >> t;
    getchar();

    while (t--)
    {
        string inp;
        getline(cin, inp);
        Node *root = buildTree(inp);

        Solution ob;
        Node *head = ob.bToDLL(root);
        printList(head);
    }
    return 0;
}

// } Driver Code Ends