'''https://practice.geeksforgeeks.org/problems/common-elements1132/1
Common elements 
Easy Accuracy: 38.69% Submissions: 81306 Points: 2
Given three arrays sorted in increasing order. Find the elements that are common in all three arrays.
Note: can you take care of the duplicates without using any additional Data Structure?

Example 1:

Input:
n1 = 6; A = {1, 5, 10, 20, 40, 80}
n2 = 5; B = {6, 7, 20, 80, 100}
n3 = 8; C = {3, 4, 15, 20, 30, 70, 80, 120}
Output: 20 80
Explanation: 20 and 80 are the only
common elements in A, B and C.
 

Your Task:  
You don't need to read input or print anything. Your task is to complete the function commonElements() which take the 3 arrays A[], B[], C[] and their respective sizes n1, n2 and n3 as inputs and returns an array containing the common element present in all the 3 arrays in sorted order. 
If there are no such elements return an empty array. In this case the output will be printed as -1.

 

Expected Time Complexity: O(n1 + n2 + n3)
Expected Auxiliary Space: O(n1 + n2 + n3)

 

Constraints:
1 <= n1, n2, n3 <= 10^5
The array elements can be both positive or negative integers.'''

# User function Template for python3


class Solution:
    def commonElements(self, A, B, C, n1, n2, n3):
        # your code here
        return sorted(list(set(A) & set(B) & set(C)))

# {
#  Driver Code Starts
# Initial Template for Python 3


t = int(input())
for tc in range(t):
    n1, n2, n3 = list(map(int, input().split()))
    A = list(map(int, input().split()))
    B = list(map(int, input().split()))
    C = list(map(int, input().split()))

    ob = Solution()

    res = ob.commonElements(A, B, C, n1, n2, n3)

    if len(res) == 0:
        print(-1)
    else:
        for i in range(len(res)):
            print(res[i], end=" ")
        print()

# } Driver Code Ends
