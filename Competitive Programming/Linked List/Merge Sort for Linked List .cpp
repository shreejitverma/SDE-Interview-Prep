/*
https: // practice.geeksforgeeks.org/problems/sort-a-linked-list/1
Merge Sort for Linked List
Medium Accuracy: 52.29 % Submissions: 33658 Points: 4
Given Pointer/Reference to the head of the linked list, the task is to Sort the given linked list using Merge Sort.
Note: If the length of linked list is odd, then the extra node should go in the first list while splitting.

Example 1:

Input:
N = 5
value[] = {3, 5, 2, 4, 1}
Output: 1 2 3 4 5
Explanation: After sorting the given
linked list, the resultant matrix
will be 1 -> 2 -> 3 -> 4 -> 5.
Example 2:

Input:
N = 3
value[] = {9, 15, 0}
Output: 0 9 15
Explanation: After sorting the given
linked list, resultant will be
0 -> 9 -> 15.
Your Task:
For C++ and Python: The task is to complete the function mergeSort() which sort the linked list using merge sort function.
For Java: The task is to complete the function mergeSort() and return the node which can be used to print the sorted linked list.

Expected Time Complexity: O(N*Log(N))
Expected Auxiliary Space: O(N)

Constraints:
1 <= T <= 100
1 <= N <= 105

*/

// { Driver Code Starts
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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

// } Driver Code Ends
/* Structure of the linked list node is as
struct Node 
{
    int data;
    struct Node* next;
    Node(int x) { data = x;  next = NULL; }
};
*/

class Solution
{
public:
    //Function to sort the given linked list using Merge Sort.
    Node *merge(Node *l1, Node *l2)
    {
        if (!l1)
            return l2;
        if (!l2)
            return l1;

        if (l1->data < l2->data)
        {
            l1->next = merge(l1->next, l2);
            return l1;
        }
        else
        {
            l2->next = merge(l1, l2->next);
            return l2;
        }
    }
    Node *mergeSort(Node *head)
    {
        // your code here
        if (head == NULL || head->next == NULL)
            return head;

        Node *slow = head;
        Node *fast = head->next;

        while (fast && fast->next)
        {
            slow = slow->next;
            fast = fast->next->next;
        }

        Node *newHead = slow->next;
        slow->next = NULL;

        return merge(mergeSort(head), mergeSort(newHead));
    }
};

// { Driver Code Starts.

void printList(Node *node)
{
    while (node != NULL)
    {
        printf("%d ", node->data);
        node = node->next;
    }
    printf("\n");
}

void push(struct Node **head_ref, int new_data)
{
    Node *new_node = new Node(new_data);

    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}

int main()
{
    long test;
    cin >> test;
    while (test--)
    {
        struct Node *a = NULL;
        long n, tmp;
        cin >> n;
        for (int i = 0; i < n; i++)
        {
            cin >> tmp;
            push(&a, tmp);
        }
        Solution obj;
        a = obj.mergeSort(a);
        printList(a);
    }
    return 0;
} // } Driver Code Ends