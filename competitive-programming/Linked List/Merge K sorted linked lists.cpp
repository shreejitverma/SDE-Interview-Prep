/*
https://practice.geeksforgeeks.org/problems/merge-k-sorted-linked-lists/1
Merge K sorted linked lists 
Medium Accuracy: 41.42% Submissions: 38460 Points: 4
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
// { Driver Code Starts
// C++ program to merge k sorted arrays of size n each
#include <bits/stdc++.h>
using namespace std;

// A Linked List node
struct Node
{
    int data;
    Node *next;

    Node(int x)
    {
        data = x;
        next = NULL;
    }
};

/* Function to print nodes in a given linked list */
void printList(Node *node)
{
    while (node != NULL)
    {
        printf("%d ", node->data);
        node = node->next;
    }
    cout << endl;
}

// } Driver Code Ends
/*Linked list Node structure

struct Node
{
	int data;
	Node* next;
	
	Node(int x){
	    data = x;
	    next = NULL;
	}
	
};
*/
typedef pair<int, Node *> node;

class Solution
{

public:
    //Function to merge K sorted linked list.
    Node *mergeKLists(Node *arr[], int K)
    {
        //Here result is the final head of the created linkedlist

        //And resultHead is the last pointer where we will add the
        //sorted element coming form minheap to its next
        Node *result;
        Node *resultHead;

        //To check for the main result head of the linkedlist we will return
        bool checkFirst = true;

        priority_queue<node, vector<node>, greater<node> > q;
        //First we will push the head nodes data and pointers of all the linkedlist
        for (int i = 0; i < K; i++)
        {
            Node *temp = arr[i];
            q.push({temp->data, temp});
        }

        while (!q.empty())
        {
            int value = q.top().first;
            Node *headNode = q.top().second;

            q.pop();
            Node *n = new Node(value);

            //If initially checkFIrst is true then it is the result head node
            if (checkFirst)
            {
                result = n;
                resultHead = n;
                checkFirst = false;
            }
            else
            {
                resultHead->next = n;
                resultHead = resultHead->next;
            }

            if (headNode->next != NULL)
            {
                q.push({headNode->next->data, headNode->next});
            }
        }

        return result;
    }
};

// { Driver Code Starts.
// Driver program to test above functions
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int N;
        cin >> N;
        struct Node *arr[N];
        for (int j = 0; j < N; j++)
        {
            int n;
            cin >> n;

            int x;
            cin >> x;
            arr[j] = new Node(x);
            Node *curr = arr[j];
            n--;

            for (int i = 0; i < n; i++)
            {
                cin >> x;
                Node *temp = new Node(x);
                curr->next = temp;
                curr = temp;
            }
        }
        Solution obj;
        Node *res = obj.mergeKLists(arr, N);
        printList(res);
    }

    return 0;
}
// } Driver Code Ends