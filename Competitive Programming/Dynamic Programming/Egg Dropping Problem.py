'''https://practice.geeksforgeeks.org/problems/egg-dropping-puzzle-1587115620/1
https://www.geeksforgeeks.org/egg-dropping-puzzle-dp-11/
Egg Dropping Puzzle 
Medium Accuracy: 54.38% Submissions: 30023 Points: 4
You are given N identical eggs and you have access to a K-floored building from 1 to K.

There exists a floor f where 0 <= f <= K such that any egg dropped at a floor higher than f will break, and any egg dropped at or below floor f will not break. There are few rules given below. 

An egg that survives a fall can be used again.
A broken egg must be discarded.
The effect of a fall is the same for all eggs.
If the egg doesn't break at a certain floor, it will not break at any floor below.
If the eggs breaks at a certain floor, it will break at any floor above.
Return the minimum number of moves that you need to determine with certainty what the value of f is.

For more description on this problem see wiki page

Example 1:

Input:
N = 1, K = 2
Output: 2
Explanation: 
1. Drop the egg from floor 1. If it 
   breaks, we know that f = 0.
2. Otherwise, drop the egg from floor 2.
   If it breaks, we know that f = 1.
3. If it does not break, then we know f = 2.
4. Hence, we need at minimum 2 moves to
   determine with certainty what the value of f is.
Example 2:

Input:
N = 2, K = 10
Output: 4
Your Task:
Complete the function eggDrop() which takes two positive integer N and K as input parameters and returns the minimum number of attempts you need in order to find the critical floor.

Expected Time Complexity : O(N*K)
Expected Auxiliary Space: O(N*K)

Constraints:
1<=N<=200
1<=K<=200'''

# A Dynamic Programming based Python Program for the Egg Dropping Puzzle
INT_MAX = 32767

# Function to get minimum number of trials needed in worst
# case with n eggs and k floors


def eggDrop(n, k):
    # A 2D table where entry eggFloor[i][j] will represent minimum
    # number of trials needed for i eggs and j floors.
    eggFloor = [[0 for x in range(k + 1)] for x in range(n + 1)]

    # We need one trial for one floor and0 trials for 0 floors
    for i in range(1, n + 1):
        eggFloor[i][1] = 1
        eggFloor[i][0] = 0

    # We always need j trials for one egg and j floors.
    for j in range(1, k + 1):
        eggFloor[1][j] = j

    # Fill rest of the entries in table using optimal substructure
    # property
    for i in range(2, n + 1):
        for j in range(2, k + 1):
            eggFloor[i][j] = INT_MAX
            for x in range(1, j + 1):
                res = 1 + max(eggFloor[i-1][x-1], eggFloor[i][j-x])
                if res < eggFloor[i][j]:
                    eggFloor[i][j] = res

    # eggFloor[n][k] holds the result
    return eggFloor[n][k]


# Driver program to test to pront printDups
n = 2
k = 36
print("Minimum number of trials in worst case with" + str(n) + "eggs and "
      + str(k) + " floors is " + str(eggDrop(n, k)))

'''
How many floors we can cover with x trials? 
When we drop an egg, two cases arise. 

If egg breaks, then we are left with x-1 trials and n-1 eggs.
If egg does not break, then we are left with x-1 trials and n eggs
Let maxFloors(x, n) be the maximum number of floors 
that we can cover with x trials and n eggs. From above 
two cases, we can write.

maxFloors(x, n) = maxFloors(x-1, n-1) + maxFloors(x-1, n) + 1
For all x >= 1 and n >= 1

Base cases : 
We can't cover any floor with 0 trials or 0 eggs
maxFloors(0, n) = 0
maxFloors(x, 0) = 0

Since we need to cover k floors, 
maxFloors(x, n) >= k           ----------(1)

The above recurrence simplifies to following,
Refer this for proof.

maxFloors(x, n) = &Sum;xCi
                  1 <= i <= n   ----------(2)
Here C represents Binomial Coefficient.

From above two equations, we can say.
&Sum;xCj  >= k
1 <= i <= n
Basically we need to find minimum value of x
that satisfies above inequality. We can find
such x using Binary Search.'''


# Python3 program to find minimum
# number of trials in worst case.

# Find sum of binomial coefficients
# xCi (where i varies from 1 to n).
# If the sum becomes more than K
def binomialCoeff(x, n, k):

    sum = 0
    term = 1
    i = 1
    while(i <= n and sum < k):
        term *= x - i + 1
        term /= i
        sum += term
        i += 1
    return sum

# Do binary search to find minimum
# number of trials in worst case.


def minTrials(n, k):

    # Initialize low and high as
    # 1st and last floors
    low = 1
    high = k

    # Do binary search, for every
    # mid, find sum of binomial
    # coefficients and check if
    # the sum is greater than k or not.
    while (low < high):

        mid = int((low + high) / 2)
        if (binomialCoeff(mid, n, k) < k):
            low = mid + 1
        else:
            high = mid

    return int(low)


# Driver Code
print(minTrials(2, 10))
