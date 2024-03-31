'''https://practice.geeksforgeeks.org/problems/mobile-numeric-keypad5456/1
https://www.geeksforgeeks.org/mobile-numeric-keypad-problem/
Mobile numeric keypad 
Medium Accuracy: 49.71% Submissions: 9962 Points: 4
Given the mobile numeric keypad. You can only press buttons that are up, left, right, or down to the current button. You are not allowed to press bottom row corner buttons (i.e. * and # ). Given a number N, the task is to find out the number of possible numbers of the given length.

Example 1:

Input: 1
Output: 10
Explanation: Number of possible numbers 
would be 10 (0, 1, 2, 3, …., 9)  
Example 2:
Input: N = 2
Output: 36
Explanation: Possible numbers: 00, 08, 11,
12, 14, 22, 21, 23, 25 and so on.
If we start with 0, valid numbers 
will be 00, 08 (count: 2)
If we start with 1, valid numbers 
will be 11, 12, 14 (count: 3)
If we start with 2, valid numbers 
will be 22, 21, 23,25 (count: 4)
If we start with 3, valid numbers 
will be 33, 32, 36 (count: 3)
If we start with 4, valid numbers 
will be 44,41,45,47 (count: 4)
If we start with 5, valid numbers 
will be 55,54,52,56,58 (count: 5) 
and so on..

Your Task:  
You don't need to read input or print anything. Complete the function getCount() which takes N as input parameter and returns the integer value

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)

Constraints:
1 ≤ N ≤ 25'''

'''A Space Optimized Solution: 
The above dynamic programming approach also runs in O(n) time and requires O(n) auxiliary space, 
as only one for loop runs n times, other for loops runs for constant time. 
We can see that nth iteration needs data from (n-1)th iteration only, so we need not keep the data from older iterations. We can have a space efficient dynamic programming approach with just two arrays of size 10. 
Thanks to Nik for suggesting this solution.
'''

# A Space Optimized Python program to count
# number of possible numbers
# of given length

# Return count of all possible numbers
# of length n
# in a given numeric keyboard


def getCount(keypad, n):

    if(not keypad or n <= 0):
        return 0
    if(n == 1):
        return 10

    # odd[i], even[i] arrays represent
    # count of numbers starting
    # with digit i for any length j
    odd = [0]*10
    even = [0]*10
    i = 0
    j = 0
    useOdd = 0
    totalCount = 0

    for i in range(10):
        odd[i] = 1  # for j = 1

    for j in range(2, n+1):  # Bottom Up calculation from j = 2 to n

        useOdd = 1 - useOdd

        # Here we are explicitly writing lines for each number 0
        # to 9. But it can always be written as DFS on 4X3 grid
        # using row, column array valid moves
        if(useOdd == 1):

            even[0] = odd[0] + odd[8]
            even[1] = odd[1] + odd[2] + odd[4]
            even[2] = odd[2] + odd[1] + odd[3] + odd[5]
            even[3] = odd[3] + odd[2] + odd[6]
            even[4] = odd[4] + odd[1] + odd[5] + odd[7]
            even[5] = odd[5] + odd[2] + odd[4] + odd[8] + odd[6]
            even[6] = odd[6] + odd[3] + odd[5] + odd[9]
            even[7] = odd[7] + odd[4] + odd[8]
            even[8] = odd[8] + odd[0] + odd[5] + odd[7] + odd[9]
            even[9] = odd[9] + odd[6] + odd[8]

        else:

            odd[0] = even[0] + even[8]
            odd[1] = even[1] + even[2] + even[4]
            odd[2] = even[2] + even[1] + even[3] + even[5]
            odd[3] = even[3] + even[2] + even[6]
            odd[4] = even[4] + even[1] + even[5] + even[7]
            odd[5] = even[5] + even[2] + even[4] + even[8] + even[6]
            odd[6] = even[6] + even[3] + even[5] + even[9]
            odd[7] = even[7] + even[4] + even[8]
            odd[8] = even[8] + even[0] + even[5] + even[7] + even[9]
            odd[9] = even[9] + even[6] + even[8]

    # Get count of all possible numbers of length "n" starting
    # with digit 0, 1, 2, ..., 9
    totalCount = 0
    if(useOdd == 1):
        for i in range(10):
            totalCount += even[i]

    else:
        for i in range(10):
            totalCount += odd[i]

    return totalCount


# Driver program to test above function
if __name__ == "__main__":
    keypad = [['1', '2', '3'],
              ['4', '5', '6'],
              ['7', '8', '9'],
              ['*', '0', '#']]

    print("Count for numbers of length ", 1, ": ", getCount(keypad, 1))
    print("Count for numbers of length ", 2, ": ", getCount(keypad, 2))
    print("Count for numbers of length ", 3, ": ", getCount(keypad, 3))
    print("Count for numbers of length ", 4, ": ", getCount(keypad, 4))
    print("Count for numbers of length ", 5, ": ", getCount(keypad, 5))

# A Dynamic Programming based C program to count number of
# possible numbers of given length

# Return count of all possible numbers of length n
# in a given numeric keyboard


def getCount(keypad, n):
    if (keypad == None or n <= 0):
        return 0
    if (n == 1):
        return 10

    # left, up, right, down move from current location
    row = [0, 0, -1, 0, 1]
    col = [0, -1, 0, 1, 0]

    # taking n+1 for simplicity - count[i][j] will store
    # number count starting with digit i and length j
    # count[10][n+1]
    count = [[0]*(n + 1)]*10
    i = 0
    j = 0
    k = 0
    move = 0
    ro = 0
    co = 0
    num = 0
    nextNum = 0
    totalCount = 0

    # count numbers starting with
    # digit i and of lengths 0 and 1
    for i in range(10):
        count[i][0] = 0
        count[i][1] = 1

    # Bottom up - Get number
    # count of length 2, 3, 4, ... , n
    for k in range(2, n + 1):
        for i in range(4):  # Loop on keypad row
            for j in range(3):   # Loop on keypad column

                # Process for 0 to 9 digits
                if (keypad[i][j] != '*' and keypad[i][j] != '#'):

                    # Here we are counting the numbers starting with
                    # digit keypad[i][j] and of length k keypad[i][j]
                    # will become 1st digit, and we need to look for
                    # (k-1) more digits
                    num = ord(keypad[i][j]) - 48
                    count[num][k] = 0

                    # move left, up, right, down from current location
                    # and if new location is valid, then get number
                    # count of length (k-1) from that new digit and
                    # add in count we found so far
                    for move in range(5):
                        ro = i + row[move]
                        co = j + col[move]
                        if (ro >= 0 and ro <= 3 and co >= 0 and co <= 2 and
                                keypad[ro][co] != '*' and keypad[ro][co] != '#'):
                            nextNum = ord(keypad[ro][co]) - 48
                            count[num][k] += count[nextNum][k - 1]

    # Get count of all possible numbers of length "n" starting
    # with digit 0, 1, 2, ..., 9
    totalCount = 0
    for i in range(10):
        totalCount += count[i][n]
    return totalCount


# Driver code
if __name__ == "__main__":
    keypad = [['1', '2', '3'],
              ['4', '5', '6'],
              ['7', '8', '9'],
              ['*', '0', '#']]

    print("Count for numbers of length", 1, ":", getCount(keypad, 1))
    print("Count for numbers of length", 2, ":", getCount(keypad, 2))
    print("Count for numbers of length", 3, ":", getCount(keypad, 3))
    print("Count for numbers of length", 4, ":", getCount(keypad, 4))
    print("Count for numbers of length", 5, ":", getCount(keypad, 5))
