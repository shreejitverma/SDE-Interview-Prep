/*
https://practice.geeksforgeeks.org/problems/add-1-to-a-number-represented-as-linked-list/1
Add 1 to a number represented as linked list 
Easy Accuracy: 51.33% Submissions: 74755 Points: 2
A number N is represented in Linked List such that each digit corresponds to a node in linked list. You need to add 1 to it.

Example 1:

Input:
LinkedList: 4->5->6
Output: 457 
Example 2:

Input:
LinkedList: 1->2->3
Output: 124 
Your Task:
Your task is to complete the function addOne() which takes the head of the linked list as the only argument and returns the head of the modified linked list. The driver code prints the number.
Note: The head represents the left-most digit of the number.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1 <= N <= 101000
*/

// { Driver Code Starts
//Initial template for C++

#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int data;
    struct Node *next;

    Node(int x)
    {
        data = x;
        next = NULL;
    }
};

void printList(Node *node)
{
    while (node != NULL)
    {
        cout << node->data;
        node = node->next;
    }
    cout << "\n";
}

// } Driver Code Ends
//User function template for C++

/* 

struct Node
{
    int data;
    struct Node* next;
    
    Node(int x){
        data = x;
        next = NULL;
    }
};

*/

class Solution
{
public:
    Node *reverse(Node *head)
    {
        Node *prev = NULL;
        Node *curr = head;

        while (curr != NULL)
        {
            Node *next = curr->next;

            curr->next = prev;
            prev = curr;

            curr = next;
        }

        return prev;
    }

    Node *addOne(Node *head)
    {
        head = reverse(head);

        Node *curr = head, *prev = head;

        int sum = (curr->data + 1);
        curr->data = sum % 10;
        int carry = sum / 10;

        curr = curr->next;

        while (curr != NULL)
        {
            sum = (curr->data + carry);
            curr->data = sum % 10;
            carry = sum / 10;

            prev = curr;

            curr = curr->next;
        }

        if (carry)
        {
            prev->next = new Node(carry);
        }

        head = reverse(head);

        return head;
    }
};

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        string s;
        cin >> s;

        Node *head = new Node(s[0] - '0');
        Node *tail = head;
        for (int i = 1; i < s.size(); i++)
        {
            tail->next = new Node(s[i] - '0');
            tail = tail->next;
        }
        Solution ob;
        head = ob.addOne(head);
        printList(head);
    }
    return 0;
}
// } Driver Code Ends