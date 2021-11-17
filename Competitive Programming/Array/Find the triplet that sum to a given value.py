'''https://practice.geeksforgeeks.org/problems/triplet-sum-in-array-1587115621/1
Triplet Sum in Array 
Medium Accuracy: 49.0% Submissions: 74975 Points: 4
Given an array arr of size n and an integer X. Find if there's a triplet in the array which sums up to the given integer X.


Example 1:

Input:
n = 6, X = 13
arr[] = [1 4 45 6 10 8]
Output:
1
Explanation:
The triplet {1, 4, 8} in 
the array sums up to 13.
Example 2:

Input:
n = 5, X = 10
arr[] = [1 2 4 3 6]
Output:
1
Explanation:
The triplet {1, 3, 6} in 
the array sums up to 10.

Your Task:
You don't need to read input or print anything. Your task is to complete the function find3Numbers() which takes the array arr[], the size of the array (n) and the sum (X) as inputs and returns True if there exists a triplet in the array arr[] which sums up to X and False otherwise.


Expected Time Complexity: O(n2)
Expected Auxiliary Space: O(1)


Constraints:
1 ≤ n ≤ 103
1 ≤ A[i] ≤ 105'''

# User function Template for python3
import atexit
import sys
import io


class Solution:

    # Function to find if there exists a triplet in the
    # array A[] which sums up to X.
    def find3Numbers(self, A, n, X):
        # Your Code Here
        A.sort()
        for i in range(n):
            point = A[i]
            j = i + 1
            k = n - 1
            while (j < k):
                a = point + A[j] + A[k]
                if (a == X):
                    return True
                elif (a < X):
                    j += 1
                else:
                    k -= 1
        return False

# {
#  Driver Code Starts
# Initial Template for Python 3


_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())


if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        n, X = map(int, input().strip().split())
        A = list(map(int, input().strip().split()))
        ob = Solution()
        if(ob.find3Numbers(A, n, X)):
            print(1)
        else:
            print(0)
# } Driver Code Ends
