'''https://www.geeksforgeeks.org/rotate-a-matrix-by-90-degree-in-clockwise-direction-without-using-any-extra-space/
Given a square matrix, turn it by 90 degrees in a clockwise direction without using any extra space.

Examples: 

Input:
1 2 3 
4 5 6
7 8 9  
Output:
7 4 1 
8 5 2
9 6 3

Input:
1 2
3 4
Output:
3 1
4 2 

Method 2:

Approach: The approach is based on the pattern made by indices after rotating the matrix. Consider the following illustration to have a clear insight into it.

Consider a 3 x 3 matrix having indices (i, j) as follows. 



00 01 02 
10 11 12 
20 21 22

After rotating the matrix by 90 degrees in clockwise direction, indices transform into
20 10 00  current_row_index = 0, i = 2, 1, 0 
21 11 01 current_row_index = 1, i = 2, 1, 0 
22 12 02  current_row_index = 2, i = 2, 1, 0

Observation: In any row, for every decreasing row index i, there exists a constant column index j, such that j = current_row_index. 

This pattern can be printed using 2 nested loops.
(This pattern of writing indices is achieved by writing the exact indices of the desired elements of 
where they actually existed in the original array.)  

Below is the implementation of the above approach:'''

# Python3 implementation of above approach
N = 4

# Function to rotate the matrix 90 degree clockwise


def rotate90Clockwise(arr):
    global N

    # printing the matrix on the basis of
    # observations made on indices.
    for j in range(N):
        for i in range(N - 1, -1, -1):
            print(arr[i][j], end=" ")
        print()


# Driver code
arr = [[1, 2, 3, 4],
       [5, 6, 7, 8],
       [9, 10, 11, 12],
       [13, 14, 15, 16]]
rotate90Clockwise(arr)
