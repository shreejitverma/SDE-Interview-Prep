'''https://practice.geeksforgeeks.org/problems/array-subset-of-another-array2317/1
Array Subset of another array 
Easy Accuracy: 51.91% Submissions: 29218 Points: 2
Given two arrays: a1[0..n-1] of size n and a2[0..m-1] of size m. Task is to check whether a2[] is a subset of a1[] or not. Both the arrays can be sorted or unsorted. It may be assumed that elements in both array are distinct.
 

Example 1:

Input:
a1[] = {11, 1, 13, 21, 3, 7}
a2[] = {11, 3, 7, 1}
Output:
Yes
Explanation:
a2[] is a subset of a1[]

Example 2:

Input:
a1[] = {1, 2, 3, 4, 5, 6}
a2[] = {1, 2, 4}
Output:
Yes
Explanation:
a2[] is a subset of a1[]

Example 3:

Input:
a1[] = {10, 5, 2, 23, 19}
a2[] = {19, 5, 3}
Output:
No
Explanation:
a2[] is not a subset of a1[]
 

Your Task:  
You don't need to read input or print anything. Your task is to complete the function isSubset() which takes the array a1[], a2[], its size n and m as inputs and return "Yes" if arr2 is subset of arr1 else return "No" if arr2 is not subset of arr1.

 

Expected Time Complexity: O(n)
Expected Auxiliary Space: O(n)


Constraints:
1 <= n,m <= 105
1 <= a1[i], a2[j] <= 105'''

# User function Template for python3


def isSubset(a1, a2, n, m):
    if set(a1) | set(a2) == set(a1):
        return 'Yes'
    return 'No'


# {
#  Driver Code Starts
# Initial Template for Python 3


def main():

    T = int(input())

    while(T > 0):
        sz = [int(x) for x in input().strip().split()]
        n, m = sz[0], sz[1]
        a1 = [int(x) for x in input().strip().split()]
        a2 = [int(x) for x in input().strip().split()]

        print(isSubset(a1, a2, n, m))

        T -= 1


if __name__ == "__main__":
    main()

# } Driver Code Ends
