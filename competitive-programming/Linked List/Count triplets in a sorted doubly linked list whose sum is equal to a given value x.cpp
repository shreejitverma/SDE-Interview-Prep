/*
https://www.geeksforgeeks.org/count-triplets-sorted-doubly-linked-list-whose-sum-equal-given-value-x/
Given a sorted doubly linked list of distinct nodes(no two nodes have the same data) and a value x. Count triplets in the list that sum up to a given value x.

Examples:
Method 1 (Naive Approach): 
Using three nested loops generate all triplets and check whether elements in the triplet sum up to x or not.
*/
/*
Method 3 Efficient Approach(Use of two pointers): 
Traverse the doubly linked list from left to right. For each current node during the traversal, initialize two pointers first = pointer to the node next to the current node and last = pointer to the last node of the list. Now, count pairs in the list from first to last pointer that sum up to value (x – current node’s data) (algorithm described in this post). Add this count to the total_count of triplets. Pointer to the last node can be found only once in the beginning.

Output: 

Count = 2
Time Complexity: O(n2) 
Auxiliary Space: O(1)
*/

// C++ implementation to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
#include <bits/stdc++.h>

using namespace std;

// structure of node of doubly linked list
struct Node
{
    int data;
    struct Node *next, *prev;
};

// function to count pairs whose sum equal to given 'value'
int countPairs(struct Node *first, struct Node *second, int value)
{
    int count = 0;

    // The loop terminates when either of two pointers
    // become NULL, or they cross each other (second->next
    // == first), or they become same (first == second)
    while (first != NULL && second != NULL &&
           first != second && second->next != first)
    {

        // pair found
        if ((first->data + second->data) == value)
        {

            // increment count
            count++;

            // move first in forward direction
            first = first->next;

            // move second in backward direction
            second = second->prev;
        }

        // if sum is greater than 'value'
        // move second in backward direction
        else if ((first->data + second->data) > value)
            second = second->prev;

        // else move first in forward direction
        else
            first = first->next;
    }

    // required count of pairs
    return count;
}

// function to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
int countTriplets(struct Node *head, int x)
{
    // if list is empty
    if (head == NULL)
        return 0;

    struct Node *current, *first, *last;
    int count = 0;

    // get pointer to the last node of
    // the doubly linked list
    last = head;
    while (last->next != NULL)
        last = last->next;

    // traversing the doubly linked list
    for (current = head; current != NULL; current = current->next)
    {

        // for each current node
        first = current->next;

        // count pairs with sum(x - current->data) in the range
        // first to last and add it to the 'count' of triplets
        count += countPairs(first, last, x - current->data);
    }

    // required count of triplets
    return count;
}

// A utility function to insert a new node at the
// beginning of doubly linked list
void insert(struct Node **head, int data)
{
    // allocate node
    struct Node *temp = new Node();

    // put in the data
    temp->data = data;
    temp->next = temp->prev = NULL;

    if ((*head) == NULL)
        (*head) = temp;
    else
    {
        temp->next = *head;
        (*head)->prev = temp;
        (*head) = temp;
    }
}

// Driver program to test above
int main()
{
    // start with an empty doubly linked list
    struct Node *head = NULL;

    // insert values in sorted order
    insert(&head, 9);
    insert(&head, 8);
    insert(&head, 6);
    insert(&head, 5);
    insert(&head, 4);
    insert(&head, 2);
    insert(&head, 1);

    int x = 17;

    cout << "Count = "
         << countTriplets(head, x);
    return 0;
}
// C++ implementation to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
#include <bits/stdc++.h>

using namespace std;

// structure of node of doubly linked list
struct Node
{
    int data;
    struct Node *next, *prev;
};

// function to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
int countTriplets(struct Node *head, int x)
{
    struct Node *ptr1, *ptr2, *ptr3;
    int count = 0;

    // generate all possible triplets
    for (ptr1 = head; ptr1 != NULL; ptr1 = ptr1->next)
        for (ptr2 = ptr1->next; ptr2 != NULL; ptr2 = ptr2->next)
            for (ptr3 = ptr2->next; ptr3 != NULL; ptr3 = ptr3->next)

                // if elements in the current triplet sum up to 'x'
                if ((ptr1->data + ptr2->data + ptr3->data) == x)

                    // increment count
                    count++;

    // required count of triplets
    return count;
}

// A utility function to insert a new node at the
// beginning of doubly linked list
void insert(struct Node **head, int data)
{
    // allocate node
    struct Node *temp = new Node();

    // put in the data
    temp->data = data;
    temp->next = temp->prev = NULL;

    if ((*head) == NULL)
        (*head) = temp;
    else
    {
        temp->next = *head;
        (*head)->prev = temp;
        (*head) = temp;
    }
}

// Driver program to test above
int main()
{
    // start with an empty doubly linked list
    struct Node *head = NULL;

    // insert values in sorted order
    insert(&head, 9);
    insert(&head, 8);
    insert(&head, 6);
    insert(&head, 5);
    insert(&head, 4);
    insert(&head, 2);
    insert(&head, 1);

    int x = 17;

    cout << "Count = "
         << countTriplets(head, x);
    return 0;
}

/*
Output: 

Count = 2
Time Complexity: O(n3) 
Auxiliary Space: O(1)
*/

/*
Method 2 (Hashing): 
Create a hash table with (key, value) tuples represented as (node data, node pointer) tuples. Traverse the doubly linked list and store each node’s data and its pointer pair(tuple) in the hash table. Now, generate each possible pair of nodes. For each pair of nodes, calculate the p_sum(sum of data in the two nodes) and check whether (x-p_sum) exists in the hash table or not. If it exists, then also verify that the two nodes in the pair are not same to the node associated with (x-p_sum) in the hash table and finally increment count. Return (count / 3) as each triplet is counted 3 times in the above process.
*/

// C++ implementation to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
#include <bits/stdc++.h>

using namespace std;

// structure of node of doubly linked list
struct Node
{
    int data;
    struct Node *next, *prev;
};

// function to count triplets in a sorted doubly linked list
// whose sum is equal to a given value 'x'
int countTriplets(struct Node *head, int x)
{
    struct Node *ptr, *ptr1, *ptr2;
    int count = 0;

    // unordered_map 'um' implemented as hash table
    unordered_map<int, Node *> um;

    // insert the <node data, node pointer> tuple in 'um'
    for (ptr = head; ptr != NULL; ptr = ptr->next)
        um[ptr->data] = ptr;

    // generate all possible pairs
    for (ptr1 = head; ptr1 != NULL; ptr1 = ptr1->next)
        for (ptr2 = ptr1->next; ptr2 != NULL; ptr2 = ptr2->next)
        {

            // p_sum - sum of elements in the current pair
            int p_sum = ptr1->data + ptr2->data;

            // if 'x-p_sum' is present in 'um' and either of the two nodes
            // are not equal to the 'um[x-p_sum]' node
            if (um.find(x - p_sum) != um.end() && um[x - p_sum] != ptr1 && um[x - p_sum] != ptr2)

                // increment count
                count++;
        }

    // required count of triplets
    // division by 3 as each triplet is counted 3 times
    return (count / 3);
}

// A utility function to insert a new node at the
// beginning of doubly linked list
void insert(struct Node **head, int data)
{
    // allocate node
    struct Node *temp = new Node();

    // put in the data
    temp->data = data;
    temp->next = temp->prev = NULL;

    if ((*head) == NULL)
        (*head) = temp;
    else
    {
        temp->next = *head;
        (*head)->prev = temp;
        (*head) = temp;
    }
}

// Driver program to test above
int main()
{
    // start with an empty doubly linked list
    struct Node *head = NULL;

    // insert values in sorted order
    insert(&head, 9);
    insert(&head, 8);
    insert(&head, 6);
    insert(&head, 5);
    insert(&head, 4);
    insert(&head, 2);
    insert(&head, 1);

    int x = 17;

    cout << "Count = "
         << countTriplets(head, x);
    return 0;
}

/*
Output: 

Count = 2
Time Complexity: O(n2) 
Auxiliary Space: O(n)
*/