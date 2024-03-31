'''https://practice.geeksforgeeks.org/problems/first-and-last-occurrences-of-x3116/1
First and last occurrences of x 
Basic Accuracy: 53.04% Submissions: 41057 Points: 1
Given a sorted array arr containing n elements with possibly duplicate elements, the task is to find indexes of first and last occurrences of an element x in the given array.

Example 1:

Input:
n=9, x=5
arr[] = { 1, 3, 5, 5, 5, 5, 67, 123, 125 }
Output:  2 5
Explanation: First occurrence of 5 is at index 2 and last
             occurrence of 5 is at index 5. 
 

Example 2:

Input:
n=9, x=7
arr[] = { 1, 3, 5, 5, 5, 5, 7, 123, 125 }
Output:  6 6 

Your Task:
Since, this is a function problem. You don't need to take any input, as it is already accomplished by the driver code. You just need to complete the function find() that takes array arr, integer n and integer x as parameters and returns the required answer.
Note: If the number x is not found in the array just return both index as -1.

 

Expected Time Complexity: O(logN)
Expected Auxiliary Space: O(1).

 

Constraints:
1 ≤ N ≤ 107

'''
# User function Template for python3


def find(arr, n, x):
    # code here
    b = []
    for i in range(n):
        if arr[i] == x:
            b.append(i)
        m = len(b)
    if m == 0:
        return (-1, -1)
    return (b[0], b[m-1])


# {
#  Driver Code Starts
t = int(input())
for _ in range(0, t):
    l = list(map(int, input().split()))
    n = l[0]
    x = l[1]
    arr = list(map(int, input().split()))
    ans = find(arr, n, x)
    print(*ans)
# } Driver Code Ends
