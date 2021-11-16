/*
https://practice.geeksforgeeks.org/problems/clone-a-linked-list-with-next-and-random-pointer/1
Clone a linked list with next and random pointer
Hard Accuracy: 49.62% Submissions: 34607 Points: 8
You are given a special linked list with N nodes where each node has a next pointer pointing to its next node. You are also given M random pointers, where you will be given M number of pairs denoting two nodes a and b  i.e. a->arb = b.

Construct a copy of the given list. The copy should consist of exactly N new nodes, where each new node has its value set to the value of its corresponding original node. Both the next and random pointer of the new nodes should point to new nodes in the copied list such that the pointers in the original list and copied list represent the same list state. None of the pointers in the new list should point to nodes in the original list.

For example, if there are two nodes X and Y in the original list, where X.random --> Y, then for the corresponding two nodes x and y in the copied list, x.random --> y.

Return the head of the copied linked list.

ArbitLinked List1

Example 1:

Input:
N = 4, M = 2
value = {1,2,3,4}
pairs = {{1,2},{2,4}}
Output: 1
Explanation: In this test case, there
are 4 nodes in linked list.  Among these
4 nodes,  2 nodes have arbitrary pointer
set, rest two nodes have arbitrary pointer
as NULL. Second line tells us the value
of four nodes. The third line gives the
information about arbitrary pointers.
The first node arbitrary pointer is set to
node 2.  The second node arbitrary pointer
is set to node 4.
Example 2:

Input:
N = 4, M = 2
value[] = {1,3,5,9}
pairs[] = {{1,1},{3,4}}
Output: 1
Explanation: In the given testcase ,
applying the method as stated in the
above example, the output will be 1.

Your Task:
The task is to complete the function copyList() which takes one argument the head of the linked list to be cloned and should return the head of the cloned linked list.

NOTE : 
1. If there is any node whose arbitrary pointer is not given then it's by default NULL. 
2. Your solution return an output 1 if your clone linked list is correct, else it returns 0.

Expected Time Complexity : O(n)
Expected Auxilliary Space : O(1)

Constraints:
1 <= N <= 100
1 <= M <= N
1 <= a, b <= 100
*/

// { Driver Code Starts
#include <bits/stdc++.h>

using namespace std;
/* Link list Node */
struct Node
{
    int data;
    Node *next;
    Node *arb;

    Node(int x)
    {
        data = x;
        next = NULL;
        arb = NULL;
    }
};

// } Driver Code Ends
class Solution
{
public:
    Node *copyList(Node *head)
    {
        map<Node *, Node *> mp;
        Node *curr = head;
        Node *newHead = new Node(0);
        Node *org = newHead;
        while (curr != NULL)
        {
            Node *next = new Node(curr->data);
            org->next = next;
            mp[curr] = next;
            curr = curr->next;
            org = org->next;
        }
        newHead = newHead->next;
        Node *c1 = head;
        Node *c2 = newHead;
        while (c1 != NULL)
        {
            c2->arb = c1->arb ? mp[c1->arb] : NULL;
            c2 = c2->next;
            c1 = c1->next;
        }
        return newHead;
    }
};

// { Driver Code Starts.

void print(Node *root)
{
    Node *temp = root;
    while (temp != NULL)
    {
        int k;
        if (temp->arb == NULL)
            k = -1;
        else
            k = temp->arb->data;
        cout << temp->data << " " << k << " ";
        temp = temp->next;
    }
}

void append(Node **head_ref, Node **tail_ref, int new_data)
{

    Node *new_node = new Node(new_data);
    if (*head_ref == NULL)
    {
        *head_ref = new_node;
    }
    else
        (*tail_ref)->next = new_node;
    *tail_ref = new_node;
}

bool validation(Node *head, Node *res)
{

    Node *temp1 = head;
    Node *temp2 = res;

    int len1 = 0, len2 = 0;
    while (temp1 != NULL)
    {
        len1++;
        temp1 = temp1->next;
    }
    while (temp2 != NULL)
    {
        len2++;
        temp2 = temp2->next;
    }

    /*if lengths not equal */

    if (len1 != len2)
        return false;

    temp1 = head;
    temp2 = res;
    map<Node *, Node *> a;
    while (temp1 != NULL)
    {

        if (temp1 == temp2)
            return false;

        if (temp1->data != temp2->data)
            return false;
        if (temp1->arb != NULL and temp2->arb != NULL)
        {
            if (temp1->arb->data != temp2->arb->data)
                return false;
        }
        else if (temp1->arb != NULL and temp2->arb == NULL)
            return false;
        else if (temp1->arb == NULL and temp2->arb != NULL)
            return false;
        a[temp1] = temp2;
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    temp1 = head;
    temp2 = res;
    while (temp1 != NULL)
    {

        if (temp1->arb != NULL and temp2->arb != NULL)
        {
            if (a[temp1->arb] != temp2->arb)
                return false;
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }
    return true;
}

int main()
{

    int T, i, n, l, k;
    Node *generated_addr = NULL;
    /*reading input stuff*/
    cin >> T;
    while (T--)
    {
        generated_addr = NULL;
        struct Node *head = NULL, *tail = NULL;
        struct Node *head2 = NULL, *tail2 = NULL;
        cin >> n >> k;
        for (i = 1; i <= n; i++)
        {
            cin >> l;
            append(&head, &tail, l);
            append(&head2, &tail2, l);
        }
        for (int i = 0; i < k; i++)
        {
            int a, b;
            cin >> a >> b;

            Node *tempA = head;
            Node *temp2A = head2;
            int count = -1;

            while (tempA != NULL)
            {
                count++;
                if (count == a - 1)
                    break;
                tempA = tempA->next;
                temp2A = temp2A->next;
            }
            Node *tempB = head;
            Node *temp2B = head2;
            count = -1;

            while (tempB != NULL)
            {
                count++;
                if (count == b - 1)
                    break;
                tempB = tempB->next;
                temp2B = temp2B->next;
            }

            // when both a is greater than N
            if (a <= n)
            {
                tempA->arb = tempB;
                temp2A->arb = temp2B;
            }
        }
        /*read finished*/

        generated_addr = head;
        Solution ob;
        struct Node *res = ob.copyList(head);
        if (validation(head2, res) && validation(head, res))
            cout << validation(head2, res) << endl;
        else
            cout << 0 << endl;
    }
    return 0;
} // } Driver Code Ends