'''https://practice.geeksforgeeks.org/problems/unique-rows-in-boolean-matrix/1
https://www.geeksforgeeks.org/print-unique-rows/
Unique rows in boolean matrix 
Easy Accuracy: 49.41% Submissions: 12314 Points: 2
Given a binary matrix your task is to find all unique rows of the given matrix.

Example 1:

Input:
row = 3, col = 4 
M[][] = {{1 1 0 1},{1 0 0 1},{1 1 0 1}}
Output: 1 1 0 1 $1 0 0 1 $
Explanation: Above the matrix of size 3x4
looks like
1 1 0 1
1 0 0 1
1 1 0 1
The two unique rows are 1 1 0 1 and
1 0 0 1 .
Your Task:
You only need to implement the given function uniqueRow(). The function takes three arguments the first argument is a matrix M and the next two arguments are row and col denoting the rows and columns of the matrix. The function should return the list of the unique row of the martrix. Do not read input, instead use the arguments given in the function.

Note: The drivers code print the rows "$" separated.

Expected Time Complexity: O(row * col)
Expected Auxiliary Space: O(row * col)

Constraints:
1<=row,col<=40
0<=M[][]<=1

Method 1: This method explains the simple approach towards solving the above problem. 



Approach: A simple approach would be to check each row with all processed rows. 
Print the first row. Now, starting from the second row, for each row, compare the row with already processed rows.
 If the row matches with any of the processed rows, skip it else print it.

Algorithm: 

Traverse the matrix row-wise
For each row check if there is any similar row less than the current index.
If any two rows are similar then do not print the row.
Else print the row.
Implementation: '''


def uniqueRow(row, col, matrix):
    # complete the function
    # complete the function
    ans = []
    hash_set = set()
    i = 0
    while i < (row*col):
        row_st = ''
        for j in range(i, i+col):
            row_st += matrix[j]

        if row_st not in hash_set:
            rows = []
            hash_set.add(row_st)
            for j in range(i, i+col):
                rows.append(matrix[j])
            ans.append(rows)
        i += col
    return ans


# Given a binary matrix of M X N of
# integers, you need to return only
# unique rows of binary array
ROW = 4
COL = 5

# The main function that prints
# all unique rows in a given matrix.


def findUniqueRows(M):

    # Traverse through the matrix
    for i in range(ROW):
        flag = 0

        # Check if there is similar column
        # is already printed, i.e if i and
        # jth column match.
        for j in range(i):
            flag = 1

            for k in range(COL):
                if (M[i][k] != M[j][k]):
                    flag = 0

            if (flag == 1):
                break

        # If no row is similar
        if (flag == 0):

            # Print the row
            for j in range(COL):
                print(M[i][j], end=" ")

            print()


# Driver Code
if __name__ == '__main__':

    M = [[0, 1, 0, 0, 1],
         [1, 0, 1, 1, 0],
         [0, 1, 0, 0, 1],
         [1, 0, 1, 0, 0]]

    findUniqueRows(M)
