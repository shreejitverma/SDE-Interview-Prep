#include <iostream>
using namespace std;

struct ListNode
{
    int data;
    struct ListNode *next;
};

int ListLength(struct ListNode *head)
{
    struct ListNode *current = head;
    int count = 0;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}
void InsertInLinkedList(struct ListNode **head, int data, int position)
{
    int k = 1;
    struct ListNode *p, *q, *newNode;
    newNode = (ListNode *)malloc(sizeof(struct ListNode));
    if (!newNode)
    {
        printf("Memory Error");
        return;
    }
    newNode->data = data;
    p = *head;
    //Inserting at the beginning
    if (position == 1)
    {
        newNode->next = p;
        *head = newNode;
    }
    else
    {
        //Traverse the list until the position where we want to insert
        while ((p != NULL) && (k < position))
        {
            k++;
            q = p;
            p = p->next;
        }
        q->next = newNode;
        //more optimum way to do this
        newNode->next = p;
    }
}

void DeleteNodeFromLinkedList(struct ListNode **head, int position)
{
    int k = 1;
    struct ListNode *p, *q;
    if (*head == NULL)
    {
        printf("List Empty");
        return;
    }
    p = *head;
    if (position == 1)
    { /* from the beginning */
        *head = (*head)->next;
        free(p);
        return;
    }
    else
    { //Traverse the list until arriving at the position from which we want to delete
        while ((p != NULL) && (k < position))
        {
            k++;
            q = p;
            p = p->next;
        }
        if (p == NULL)
        {
            /* At the end */
            printf("Position does not exist.");
        }

        else
        { /* From the middle */
            q->next = p->next;
            free(p);
        }
    }
}

void DeleteLinkedList(struct ListNode **head)
{
    struct ListNode *auxilaryNode, *iterator;
    iterator = *head;
    while (iterator)
    {
        auxilaryNode = iterator->next;
        free(iterator);
        iterator = auxilaryNode;
    }
    *head = NULL; // to affect the real head back in the caller.
}

int main()
{

    return 0;
}