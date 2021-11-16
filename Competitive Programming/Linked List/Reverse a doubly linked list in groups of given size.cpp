/*
https://www.geeksforgeeks.org/reverse-doubly-linked-list-groups-given-size/
Given a doubly linked list containing n nodes. The problem is to reverse every group of k nodes in the list.
 

Examples: 
Prerequisite: Reverse a doubly linked list | Set-2.
Approach: Create a recursive function say reverse(head, k). This function receives the head or the first node of each group of k nodes. It reverses those group of k nodes by applying the approach discussed in Reverse a doubly linked list | Set-2. After reversing the group of k nodes the function checks whether next group of nodes exists in the list or not. If group exists then it makes a recursive call to itself with the first node of the next group and makes the necessary adjustments with the next and previous links of that group. Finally it returns the new head node of the reversed group. 
*/
// C++ implementation to reverse a doubly linked list
// in groups of given size
#include <bits/stdc++.h>

using namespace std;

// a node of the doubly linked list
struct Node
{
    int data;
    Node *next, *prev;
};

// function to get a new node
Node *getNode(int data)
{
    // allocate space
    Node *new_node = (Node *)malloc(sizeof(Node));

    // put in the data
    new_node->data = data;
    new_node->next = new_node->prev = NULL;
    return new_node;
}

// function to insert a node at the beginging
// of the Doubly Linked List
void push(Node **head_ref, Node *new_node)
{
    // since we are adding at the beginning,
    // prev is always NULL
    new_node->prev = NULL;

    // link the old list off the new node
    new_node->next = (*head_ref);

    // change prev of head node to new node
    if ((*head_ref) != NULL)
        (*head_ref)->prev = new_node;

    // move the head to point to the new node
    (*head_ref) = new_node;
}

// function to reverse a doubly linked list
// in groups of given size
Node *revListInGroupOfGivenSize(Node *head, int k)
{
    Node *current = head;
    Node *next = NULL;
    Node *newHead = NULL;
    int count = 0;

    // reversing the current group of k
    // or less than k nodes by adding
    // them at the beginning of list
    // 'newHead'
    while (current != NULL && count < k)
    {
        next = current->next;
        push(&newHead, current);
        current = next;
        count++;
    }

    // if next group exists then making the desired
    // adjustments in the link
    if (next != NULL)
    {
        head->next = revListInGroupOfGivenSize(next, k);
        head->next->prev = head;
    }

    // pointer to the new head of the
    // reversed group
    return newHead;
}

// Function to print nodes in a
// given doubly linked list
void printList(Node *head)
{
    while (head != NULL)
    {
        cout << head->data << " ";
        head = head->next;
    }
}

// Driver program to test above
int main()
{
    // Start with the empty list
    Node *head = NULL;

    // Create doubly linked: 10<->8<->4<->2
    push(&head, getNode(2));
    push(&head, getNode(4));
    push(&head, getNode(8));
    push(&head, getNode(10));

    int k = 2;

    cout << "Original list: ";
    printList(head);

    // Reverse doubly linked list in groups of
    // size 'k'
    head = revListInGroupOfGivenSize(head, k);

    cout << "\nModified list: ";
    printList(head);

    return 0;
}
/*
Output
Original list: 10 8 4 2 
Modified list: 8 10 2 4 
Time Complexity: O(n).
 

We can further simplify the implementation of this algorithm using the same idea with recursion in just one function.
*/
#include <iostream>
using namespace std;
struct Node
{
    int data;
    Node *next, *prev;
};
// function to add Node at the end of a Doubly LinkedList
Node *insertAtEnd(Node *head, int data)
{

    Node *new_node = new Node();
    new_node->data = data;
    new_node->next = NULL;
    Node *temp = head;
    if (head == NULL)
    {
        new_node->prev = NULL;
        head = new_node;
        return head;
    }

    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = new_node;
    new_node->prev = temp;
    return head;
}
// function to print Doubly LinkedList
void printDLL(Node *head)
{
    while (head != NULL)
    {
        cout << head->data << " ";
        head = head->next;
    }
    cout << endl;
}
// function to Reverse a doubly linked list
// in groups of given size
Node *reverseByN(Node *head, int k)
{
    if (!head)
        return NULL;
    head->prev = NULL;
    Node *temp, *curr = head, *newHead;
    int count = 0;
    while (curr != NULL && count < k)
    {
        newHead = curr;
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
        count++;
    }
    // checking if the reversed LinkedList size is
    // equal to K or not
    // if it is not equal to k that means we have reversed
    // the last set of size K and we don't need to call the
    // recursive function
    if (count >= k)
    {
        Node *rest = reverseByN(curr, k);
        head->next = rest;
        if (rest != NULL)
            // it is required for prev link otherwise u wont
            // be backtrack list due to broken links
            rest->prev = head;
    }
    return newHead;
}
int main()
{
    Node *head;
    for (int i = 1; i <= 10; i++)
    {
        head = insertAtEnd(head, i);
    }
    printDLL(head);
    int n = 4;
    head = reverseByN(head, n);
    printDLL(head);
}