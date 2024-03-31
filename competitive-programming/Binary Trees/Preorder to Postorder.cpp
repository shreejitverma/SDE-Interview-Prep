/*
https://practice.geeksforgeeks.org/problems/preorder-to-postorder4423/1
Preorder to Postorder 
Medium Accuracy: 56.79% Submissions: 13376 Points: 4
Given an array arr[] of N nodes representing preorder traversal of BST. The task is to print its postorder traversal.

Example 1:

Input:
N = 5
arr[]  = {40,30,35,80,100}
Output: 35 30 100 80 40
Explanation: PreOrder: 40 30 35 80 100
InOrder: 30 35 40 80 100
Therefore, the BST will be:
              40
           /      \
         30       80
           \        \   
           35      100
Hence, the postOrder traversal will
be: 35 30 100 80 40
Example 2:

Input:
N = 8
arr[]  = {40,30,32,35,80,90,100,120}
Output: 35 32 30 120 100 90 80 40
Your Task:
You need to complete the given function and return the root of the tree. The driver code will then use this root to print the post order traversal.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(N).

Constraints:
1 <= N <= 103
1 <= arr[i] <= 104
*/

// { Driver Code Starts
//Initial template for C++

/* A O(n) iterative program for construction of BST from preorder traversal */
#include <bits/stdc++.h>
using namespace std;

/* A binary tree node has data, pointer to left child
   and a pointer to right child */
typedef struct Node
{
    int data;
    struct Node *left, *right;
} Node;

// A Stack has array of Nodes, capacity, and top
typedef struct Stack
{
    int top;
    int capacity;
    Node **array;
} Stack;

// A utility function to create a new tree node
Node *newNode(int data)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

// A utility function to create a stack of given capacity
Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->top = -1;
    stack->capacity = capacity;
    stack->array = (Node **)malloc(stack->capacity * sizeof(Node *));
    return stack;
}

// A utility function to check if stack is full
int isFull(Stack *stack)
{
    return stack->top == stack->capacity - 1;
}

// A utility function to check if stack is empty
int isEmpty(Stack *stack)
{
    return stack->top == -1;
}

// A utility function to push an item to stack
void push(Stack *stack, Node *item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}

// A utility function to remove an item from stack
Node *pop(Stack *stack)
{
    if (isEmpty(stack))
        return NULL;
    return stack->array[stack->top--];
}

// A utility function to get top node of stack
Node *peek(Stack *stack)
{
    return stack->array[stack->top];
}

bool canRepresentBST(int pre[], int n)
{
    // Create an empty stack
    stack<int> s;

    // Initialize current root as minimum possible
    // value
    int root = INT_MIN;

    // Traverse given array
    for (int i = 0; i < n; i++)
    {
        // If we find a node who is on right side
        // and smaller than root, return false
        if (pre[i] < root)
            return false;

        // If pre[i] is in right subtree of stack top,
        // Keep removing items smaller than pre[i]
        // and make the last removed item as new
        // root.
        while (!s.empty() && s.top() < pre[i])
        {
            root = s.top();
            s.pop();
        }

        // At this point either stack is empty or
        // pre[i] is smaller than root, push pre[i]
        s.push(pre[i]);
    }
    return true;
}

// A utility function to print postorder traversal of a Binary Tree
void printPostorder(Node *node)
{
    if (node == NULL)
        return;
    printPostorder(node->left);
    printPostorder(node->right);
    printf("%d ", node->data);
}

// The main function that constructs BST from pre[]
Node *post_order(int *, int);

int main()
{
    // Note that size of arr[] is considered 100 according to
    // the constraints mentioned in problem statement.
    int arr[1000], x, t, n;

    // Input the number of test cases you want to run
    cin >> t;

    // One by one run for all input test cases
    while (t--)
    {
        // Input the size of the array
        cin >> n;

        // Input the array
        for (int i = 0; i < n; i++)
            cin >> arr[i];

        printPostorder(post_order(arr, n));
        cout << endl;
    }
    return 0;
}
// } Driver Code Ends

//User function template in C++

Node *insert(Node *head, int n)
{
    if (!head)
        return newNode(n);

    if (head->data > n)
        head->left = insert(head->left, n);
    else
        head->right = insert(head->right, n);
    return head;
}
//Function that constructs BST from its preorder traversal.
Node *post_order(int pre[], int size)
{
    //code here
    Node *head = NULL;

    for (int i = 0; i < size; i++)
    {
        head = insert(head, pre[i]);
    }

    return head;
}
