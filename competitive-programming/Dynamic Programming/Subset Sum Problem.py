'''https://practice.geeksforgeeks.org/problems/subset-sum-problem2014/1
https://www.geeksforgeeks.org/partition-problem-dp-18/
Partition Equal Subset Sum 
Medium Accuracy: 38.0% Submissions: 75383 Points: 4
Given an array arr[] of size N, check if it can be partitioned into two parts such that the sum of elements in both parts is the same.

Example 1:

Input: N = 4
arr = {1, 5, 11, 5}
Output: YES
Explaination: 
The two parts are {1, 5, 5} and {11}.
Example 2:

Input: N = 3
arr = {1, 3, 5}
Output: NO
Explaination: This array can never be 
partitioned into two such parts.

Your Task:
You do not need to read input or print anything. Your task is to complete the function equalPartition() which takes the value N and the array as input parameters and returns 1 if the partition is possible. Otherwise, returns 0.


Expected Time Complexity: O(N*sum of elements)
Expected Auxiliary Space: O(N*sum of elements)


Constraints:
1 ≤ N ≤ 100
1 ≤ arr[i] ≤ 1000


Recursive Solution 
Following is the recursive property of the second step mentioned above. 

Let isSubsetSum(arr, n, sum/2) be the function that returns true if 
there is a subset of arr[0..n-1] with sum equal to sum/2

The isSubsetSum problem can be divided into two subproblems
 a) isSubsetSum() without considering last element 
    (reducing n to n-1)
 b) isSubsetSum considering the last element 
    (reducing sum/2 by arr[n-1] and n to n-1)
If any of the above subproblems return true, then return true. 
isSubsetSum (arr, n, sum/2) = isSubsetSum (arr, n-1, sum/2) ||
                              isSubsetSum (arr, n-1, sum/2 - arr[n-1])
Below is the implementation of the above code: '''

# A recursive Python3 program for
# partition problem

# A utility function that returns
# true if there is a subset of
# arr[] with sun equal to given sum


def isSubsetSum(arr, n, sum):
    # Base Cases
    if sum == 0:
        return True
    if n == 0 and sum != 0:
        return False

    # If last element is greater than sum, then
    # ignore it
    if arr[n-1] > sum:
        return isSubsetSum(arr, n-1, sum)

    ''' else, check if sum can be obtained by any of
    the following
    (a) including the last element
    (b) excluding the last element'''

    return isSubsetSum(arr, n-1, sum) or isSubsetSum(arr, n-1, sum-arr[n-1])

# Returns true if arr[] can be partitioned in two
# subsets of equal sum, otherwise false


def findPartion(arr, n):
    # Calculate sum of the elements in array
    sum = 0
    for i in range(0, n):
        sum += arr[i]
    # If sum is odd, there cannot be two subsets
    # with equal sum
    if sum % 2 != 0:
        return false

    # Find if there is subset with sum equal to
    # half of total sum
    return isSubsetSum(arr, n, sum // 2)


# Driver code
arr = [3, 1, 5, 9, 12]
n = len(arr)

# Function call
if findPartion(arr, n) == True:
    print("Can be divided into two subsets of equal sum")
else:
    print("Can not be divided into two subsets of equal sum")

'''
Output
Can be divided into two subsets of equal sum
Time Complexity: O(2^n) In the worst case, this solution tries two possibilities (whether to include or exclude) 
for every element.

Dynamic Programming Solution 
The problem can be solved using dynamic programming when the sum of the elements is not too big. 
We can create a 2D array part[][] of size (sum/2 + 1)*(n+1). And we can construct the solution 
in a bottom-up manner such that every filled entry has the following property  

part[i][j] = true if a subset of {arr[0], arr[1], ..arr[j-1]} has sum 
             equal to i, otherwise false'''

# Dynamic Programming based python
# program to partition problem

# Returns true if arr[] can be
# partitioned in two subsets of
# equal sum, otherwise false


def findPartition(arr, n):
    sum = 0
    i, j = 0, 0

    # calculate sum of all elements
    for i in range(n):
        sum += arr[i]

    if sum % 2 != 0:
        return false

    part = [[True for i in range(n + 1)]
            for j in range(sum // 2 + 1)]

    # initialize top row as true
    for i in range(0, n + 1):
        part[0][i] = True

    # initialize leftmost column,
    # except part[0][0], as 0
    for i in range(1, sum // 2 + 1):
        part[i][0] = False

    # fill the partition table in
    # bottom up manner
    for i in range(1, sum // 2 + 1):

        for j in range(1, n + 1):
            part[i][j] = part[i][j - 1]

            if i >= arr[j - 1]:
                part[i][j] = (part[i][j] or
                              part[i - arr[j - 1]][j - 1])

    return part[sum // 2][n]


# Driver Code
arr = [3, 1, 1, 2, 2, 1]
n = len(arr)

# Function call
if findPartition(arr, n) == True:
    print("Can be divided into two",
          "subsets of equal sum")
else:
    print("Can not be divided into ",
          "two subsets of equal sum")


'''Output
Can be divided into two subsets of equal sum
Following diagram shows the values in the partition table.  





Time Complexity: O(sum*n) 
Auxiliary Space: O(sum*n) 

Please note that this solution will not be feasible for arrays with big sum.

Dynamic Programming Solution (Space Complexity Optimized)

Instead of creating a 2-D array of size (sum/2 + 1)*(n  + 1), we can solve this problem using an array of size (sum/2 + 1 ) only. 

part[j] = true if there is a subset with sum equal to j, otherwise false.

Below is the implementation of the above approach: '''

# A Dynamic Programming based
# Python3 program to partition problem

# Returns true if arr[] can be partitioned
# in two subsets of equal sum, otherwise false


def findPartiion(arr, n):
    Sum = 0

    # Calculate sum of all elements
    for i in range(n):
        Sum += arr[i]
    if (Sum % 2 != 0):
        return 0
    part = [0] * ((Sum // 2) + 1)

    # Initialize the part array as 0
    for i in range((Sum // 2) + 1):
        part[i] = 0

    # Fill the partition table in bottom up manner
    for i in range(n):

        # the element to be included
        # in the sum cannot be
        # greater than the sum
        for j in range(Sum // 2, arr[i] - 1, -1):

            # check if sum - arr[i]
            # could be formed
            # from a subset
            # using elements
            # before index i
            if (part[j - arr[i]] == 1 or j == arr[i]):
                part[j] = 1

    return part[Sum // 2]


# Drive code
arr = [1, 3, 3, 2, 3, 2]
n = len(arr)

# Function call
if (findPartiion(arr, n) == 1):
    print("Can be divided into two subsets of equal sum")
else:
    print("Can not be divided into two subsets of equal sum")

'''Output
Can be divided into two subsets of equal sum
Time Complexity: O(sum * n)
Auxiliary Space: O(sum)

'''
