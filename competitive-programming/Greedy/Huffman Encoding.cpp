/*
https://practice.geeksforgeeks.org/problems/huffman-encoding3345/1
https://youtu.be/co4_ahEDCho
Huffman Encoding 
Medium Accuracy: 37.28% Submissions: 5850 Points: 4
Given a string S of distinct character of size N and their corresponding frequency f[ ] i.e. character S[i] has f[i] frequency. Your task is to build the Huffman tree print all the huffman codes in preorder traversal of the tree.
Note: If two elements have same frequency, then the element which occur at first will be taken on the left of Binary Tree and other one to the right.

Example 1:

S = "abcdef"
f[] = {5, 9, 12, 13, 16, 45}
Output: 
0 100 101 1100 1101 111
Explanation:
HuffmanCodes will be:
f : 0
c : 100
d : 101
a : 1100
b : 1101
e : 111
Hence printing them in the PreOrder of Binary 
Tree.
Your Task:
You don't need to read or print anything. Your task is to complete the function huffmanCodes() which takes the given string S, frequency array f[ ] and number of characters N as input parameters and returns a vector of strings containing all huffman codes in order of preorder traversal of the tree.

Expected Time complexity: O(N * LogN) 
Expected Space complexity: O(N) 

Constraints:
1 ≤ N ≤ 26
*/

class Solution
{
    struct Node
    {
        int data;
        Node *left;
        Node *right;

        Node(int x)
        {
            data = x;
            left = right = NULL;
        }
    };
    // To compare two points
    struct compare
    {

        bool operator()(Node *l, Node *r)

        {
            return (l->data > r->data);
        }
    };

    void preOrder(vector<string> &res, Node *root, string s)
    {
        if (!root)
            return;

        if (!root->left && !root->right)
            res.push_back(s);
        preOrder(res, root->left, s + "0");
        preOrder(res, root->right, s + "1");
    }

public:
    vector<string> huffmanCodes(string S, vector<int> f, int N)
    {
        priority_queue<Node *, vector<Node *>, compare> pq;

        for (int i = 0; i < f.size(); i++)
        {
            Node *temp = new Node(f[i]);
            pq.push(temp);
        }

        vector<string> res;
        for (int i = 1; i < N; i++)
        {
            Node *x = pq.top();
            // cout << x << endl;
            pq.pop();
            Node *y = pq.top();
            // cout << y << endl;
            pq.pop();

            Node *temp = new Node(x->data + y->data);
            temp->left = x;
            temp->right = y;
            pq.push(temp);
        }

        preOrder(res, pq.top(), "");
        return res;
    }
};
