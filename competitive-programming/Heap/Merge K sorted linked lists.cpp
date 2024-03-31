/*
https://practice.geeksforgeeks.org/problems/merge-k-sorted-linked-lists/1
Merge K sorted linked lists 
Medium Accuracy: 41.42% Submissions: 38262 Points: 4
Given K sorted linked lists of different sizes. The task is to merge them in such a way that after merging they will be a single sorted linked list.

Example 1:

Input:
K = 4
value = {{1,2,3},{4 5},{5 6},{7,8}}
Output: 1 2 3 4 5 5 6 7 8
Explanation:
The test case has 4 sorted linked 
list of size 3, 2, 2, 2
1st    list     1 -> 2-> 3
2nd   list      4->5
3rd    list      5->6
4th    list      7->8
The merged list will be
1->2->3->4->5->5->6->7->8.
Example 2:

Input:
K = 3
value = {{1,3},{4,5,6},{8}}
Output: 1 3 4 5 6 8
Explanation:
The test case has 3 sorted linked
list of size 2, 3, 1.
1st list 1 -> 3
2nd list 4 -> 5 -> 6
3rd list 8
The merged list will be
1->3->4->5->6->8.
Your Task:
The task is to complete the function mergeKList() which merges the K given lists into a sorted one. The printing is done automatically by the driver code.

Expected Time Complexity: O(nk Logk)
Expected Auxiliary Space: O(k)
Note: n is the maximum size of all the k link list

Constraints
1 <= K <= 103
*/

Node *merge(Node *head1, Node *head2)
{

    if (!head1 and head2)
        return head2;
    if (head1 and !head2)
        return head1;

    if (!head1 and !head2)
        return head1;

    Node *reshead = NULL;
    Node *curr = NULL;

    Node *ptr1 = head1;
    Node *ptr2 = head2;

    while (ptr1 and ptr2)
    {

        if (ptr1->data <= ptr2->data)
        {
            if (!reshead)
            {
                reshead = ptr1;
                curr = ptr1;
            }

            else
            {
                curr->next = ptr1;
                curr = curr->next;
            }

            ptr1 = ptr1->next;
        }

        else
        {

            if (!reshead)
            {
                reshead = ptr2;
                curr = ptr2;
            }

            else
            {
                curr->next = ptr2;
                curr = curr->next;
            }

            ptr2 = ptr2->next;
        }
    }

    while (ptr1)
    {
        curr->next = ptr1;
        curr = curr->next;
        ptr1 = ptr1->next;
    }

    while (ptr2)
    {
        curr->next = ptr2;
        curr = curr->next;
        ptr2 = ptr2->next;
    }

    if (!ptr1 and !ptr2)
        curr->next = NULL;

    return reshead;
}