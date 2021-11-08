'''https://practice.geeksforgeeks.org/problems/merge-k-sorted-linked-lists/1
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
1 <= K <= 103'''

# User function Template for python3
'''
	Your task is to merge the given k sorted
	linked lists into one list and return
	the the new formed linked list class.

	Function Arguments:
	    arr is a list containing the n linkedlist head pointers
	    n is an integer value
    
    node class:
    
class Node:
    def __init__(self,x):
        self.data = x
        self.nxt = None
'''




from heapq import heapify, heappop, heappush
def merge(h1, h2):
    if h1 == None and h2 == None:
        return None
    if h1 == None:
        return h2
    if h2 == None:
        return h1
    t1, t2 = h1, h2
    ans = None
    a = None
    while t1 != None and t2 != None:
        if t1.data < t2.data:
            if ans == None:
                ans = t1
                a = ans
                t1 = t1.next
                a.next = None
            else:
                a.next = t1
                a = a.next
                t1 = t1.next
                a.next = None
        else:
            if ans == None:
                ans = t2
                a = ans
                t2 = t2.next
                a.next = None
            else:
                a.next = t2
                a = a.next
                t2 = t2.next
                a.next = None
    if t1 != None:
        a.next = t1
    else:
        a.next = t2
    return ans


class Solution:
    # Function to merge K sorted linked list.
    def mergeKLists(self, arr, K):
        # code here
        # return head of merged list
        K -= 1
        while K != 0:
            i = 0
            j = K
            while i < j:
                arr[i] = merge(arr[i], arr[j])
                i += 1
                j -= 1
                if i >= j:
                    K = j
        return arr[0]


# Another One


class Solution:
    # Function to merge K sorted linked list.
    # Time complexity -> O(n.klogk)
    def mergeKLists(self, lists, k):
        dummy = Node(0)
        ref = dummy
        h = []
        heapify(h)
        for lst in lists:  # k
            temp = lst
            while temp:  # nlogn
                heappush(h, temp.data)
                temp = temp.next
        while h:
            data = heappop(h)
            nxt = Node(data)
            dummy.next = nxt
            dummy = dummy.next
        return ref.next
