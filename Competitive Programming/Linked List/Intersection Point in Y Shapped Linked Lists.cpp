/*
https: // practice.geeksforgeeks.org/problems/intersection-point-in-y-shapped-linked-lists/1
Intersection Point in Y Shapped Linked Lists
Medium Accuracy: 49.55 % Submissions: 100k + Points: 4
Given two singly linked lists of size N and M, write a program to get the point where two linked lists intersect each other.


Example 1:

Input:
LinkList1 = 3 -> 6 -> 9 -> common
LinkList2 = 10 -> common
common = 15 -> 30 -> NULL
Output: 15
Explanation:
Y ShapedLinked List
Example 2:

Input:
Linked List 1 = 4 -> 1 -> common
Linked List 2 = 5 -> 6 -> 1 -> common
common = 8 -> 4 -> 5 -> NULL
Output: 8
Explanation:

4              5
| |
1              6
 /
       8 - ---- 1
         |
          4
           |
        5
        |
        NULL
Your Task:
You don't need to read input or print anything. The task is to complete the function intersetPoint() which takes the pointer to the head of linklist1(head1) and linklist2(head2) as input parameters and returns data value of a node where two linked lists intersect. If linked list do not merge at any point, then it should return -1.
Challenge: Try to solve the problem without using any extra space.


Expected Time Complexity: O(N+M)
Expected Auxiliary Space: O(1)


Constraints:
1 ≤ N + M ≤ 2*105
-1000 ≤ value ≤ 1000

*/

// { Driver Code Starts
#include <iostream>

#include <bits/stdc++.h>
using namespace std;

/* Link list Node */
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

int intersectPoint(struct Node *head1, struct Node *head2);

Node *inputList(int size)
{
    if (size == 0)
        return NULL;

    int val;
    cin >> val;

    Node *head = new Node(val);
    Node *tail = head;

    for (int i = 0; i < size - 1; i++)
    {
        cin >> val;
        tail->next = new Node(val);
        tail = tail->next;
    }

    return head;
}

/* Driver program to test above function*/
int main()
{
    int T, n1, n2, n3;

    cin >> T;
    while (T--)
    {
        cin >> n1 >> n2 >> n3;

        Node *head1 = inputList(n1);
        Node *head2 = inputList(n2);
        Node *common = inputList(n3);

        Node *temp = head1;
        while (temp != NULL && temp->next != NULL)
            temp = temp->next;
        if (temp != NULL)
            temp->next = common;

        temp = head2;
        while (temp != NULL && temp->next != NULL)
            temp = temp->next;
        if (temp != NULL)
            temp->next = common;

        cout << intersectPoint(head1, head2) << endl;
    }
    return 0;
}

// } Driver Code Ends

/* Linked List Node
struct Node {
  int data;
  struct Node *next;
  Node(int x) {
    data = x;
    next = NULL;
  }
}; */

//Function to find intersection point in Y shaped Linked Lists.
int intersectPoint(Node *head1, Node *head2)
{

    if (head1 == NULL || head2 == NULL)
    {
        return -1;
    }
    Node *a = head1;
    Node *b = head2;
    if (a == NULL)
    {
        return -1;
    }
    while (a != b)
    {
        a = a == NULL ? head2 : a->next;
        b = b == NULL ? head1 : b->next;
    }
    return a->data;
}
