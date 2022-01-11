#include <iostream>
using namespace std;
class Node
{
public:
    int value;
    Node *next;
    Node()
    {
        Node *next = NULL;
    }

    Node(int value)
    {
        this->value = value;
    }

    Node(int value, Node *next)
    {
        this->value = value;
        this->next = next;
    }
};
class LL : public Node
{

private:
    Node *head;

    Node *tail;

    int size;

public:
    LL()
    {
        this->size = 0;
    }
    void insertFirst(int val)
    {
        Node *node = new Node(val);
        node->next = head;
        head = node;

        if (tail == NULL)
        {
            tail = head;
        }
        size += 1;
    }

    void insertLast(int val)
    {
        if (tail == NULL)
        {
            insertFirst(val);
            return;
        }
        Node *node = new Node(val);
        tail->next = node;
        tail = node;
        size++;
    }

    void insert(int val, int index)
    {
        if (index == 0)
        {
            insertFirst(val);
            return;
        }
        if (index == size)
        {
            insertLast(val);
            return;
        }

        Node *temp = head;
        for (int i = 1; i < index; i++)
        {
            temp = temp->next;
        }

        Node *node = new Node(val, temp->next);
        temp->next = node;

        size++;
    }

    // insert using recursion
    void insertRec(int val, int index)
    {
        head = insertRec(val, index, head);
    }
    Node *insertRec(int val, int index, Node *node)
    {
        if (index == 0)
        {
            Node *temp = new Node(val, node);
            size++;
            return temp;
        }

        node->next = insertRec(val, index - 1, node->next);
        return node;
    }

    int deletedLast()
    {
        if (size <= 1)
        {
            return deletedFirst();
        }

        Node *secondLast = get(size - 2);
        int val = tail->value;
        tail = secondLast;
        tail->next = NULL;
        return val;
    }

    int deleted(int index)
    {
        if (index == 0)
        {
            return deletedFirst();
        }
        if (index == size - 1)
        {
            return deletedLast();
        }

        Node *prev = get(index - 1);
        int val = prev->next->value;

        prev->next = prev->next->next;

        return val;
    }

    Node *find(int value)
    {
        Node *node = head;
        while (node != NULL)
        {
            if (node->value == value)
            {
                return node;
            }
            node = node->next;
        }
        return NULL;
    }

    Node *get(int index)
    {
        Node *node = head;
        for (int i = 0; i < index; i++)
        {
            node = node->next;
        }
        return node;
    }

    int deletedFirst()
    {
        int val = head->value;
        head = head->next;
        if (head == NULL)
        {
            tail = NULL;
        }
        size--;
        return val;
    }

    void display()
    {
        Node *temp = head;
        while (temp != NULL)
        {
            cout << temp->value << " -> ";
            temp = temp->next;
        }
        cout << "END" << endl;
    }

    // https://leetcode->com/problems/remove-duplicates-from-sorted-list
    void
    duplicates()
    {
        Node *node = head;

        while (node->next != NULL)
        {
            if (node->value == node->next->value)
            {
                node->next = node->next->next;
                size--;
            }
            else
            {
                node = node->next;
            }
        }
        tail = node;
        tail->next = NULL;
    }

    // https://leetcode->com/problems/merge-two-sorted-lists/submissions/
    LL *merge(LL *first, LL *second)
    {
        Node *f = first->head;
        Node *s = second->head;

        LL *ans = new LL();

        while (f != NULL && s != NULL)
        {
            if (f->value < s->value)
            {
                ans->insertLast(f->value);
                f = f->next;
            }
            else
            {
                ans->insertLast(s->value);
                s = s->next;
            }
        }

        while (f != NULL)
        {
            ans->insertLast(f->value);
            f = f->next;
        }

        while (s != NULL)
        {
            ans->insertLast(s->value);
            s = s->next;
        }

        return ans;
    }

    void bubbleSort()
    {
        bubbleSort(size - 1, 0);
    }

    void bubbleSort(int row, int col)
    {
        if (row == 0)
        {
            return;
        }

        if (col < row)
        {
            Node *first = get(col);
            Node *second = get(col + 1);

            if (first->value > second->value)
            {
                // swap
                if (first == head)
                {
                    head = second;
                    first->next = second->next;
                    second->next = first;
                }
                else if (second == tail)
                {
                    Node *prev = get(col - 1);
                    prev->next = second;
                    tail = first;
                    first->next = NULL;
                    second->next = tail;
                }
                else
                {
                    Node *prev = get(col - 1);
                    prev->next = second;
                    first->next = second->next;
                    second->next = first;
                }
            }
            bubbleSort(row, col + 1);
        }
        else
        {
            bubbleSort(row - 1, 0);
        }
    }

    // recursion reverse
    void reverse(Node *node)
    {
        if (node == tail)
        {
            head = tail;
            return;
        }
        reverse(node->next);
        tail->next = node;
        tail = node;
        tail->next = NULL;
    }

    // in place reversal of linked list
    // google, microsoft, apple, amazon: https://leetcode.com/problems/reverse-linked-list/
    void reverse()
    {
        if (size < 2)
        {
            return;
        }

        Node *prev = NULL;
        Node *present = head;
        Node *next = present->next;

        while (present != NULL)
        {
            present->next = prev;
            prev = present;
            present = next;
            if (next != NULL)
            {
                next = next->next;
            }
        }
        head = prev;
    }
};
int main()
{
    LL *first = new LL();
    LL *second = new LL();

    first->insertLast(1);
    first->insertLast(3);
    first->insertLast(5);

    second->insertLast(1);
    second->insertLast(2);
    second->insertLast(9);
    second->insertLast(14);

    LL *ans = new LL();
    ans->merge(first, second);
    ans->display();

    LL *list = new LL();
    for (int i = 7; i > 0; i--)
    {
        list->insertLast(i);
    }
    list->display();
    list->bubbleSort();
    list->display();
}
