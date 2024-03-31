'''https://www.geeksforgeeks.org/the-knights-tour-problem-backtracking-1/
Backtracking is an algorithmic paradigm that tries different solutions until finds a solution that “works”. Problems that are typically solved using the backtracking technique have the following property in common. These problems can only be solved by trying every possible configuration and each configuration is tried only once. A Naive solution for these problems is to try all configurations and output a configuration that follows given problem constraints. Backtracking works incrementally and is an optimization over the Naive solution where all possible configurations are generated and tried.
For example, consider the following Knight’s Tour problem. 

Problem Statement:
Given a N*N board with the Knight placed on the first block of an empty board. Moving according to the rules of chess knight must visit each square exactly once. Print the order of each cell in which they are visited.

Example:



Input : 
N = 8
Output:
0  59  38  33  30  17   8  63
37  34  31  60   9  62  29  16
58   1  36  39  32  27  18   7
35  48  41  26  61  10  15  28
42  57   2  49  40  23   6  19
47  50  45  54  25  20  11  14
56  43  52   3  22  13  24   5
51  46  55  44  53   4  21  12
The path followed by Knight to cover all the cells
Following is a chessboard with 8 x 8 cells. Numbers in cells indicate the move number of Knight. 

knight-tour-problem

Let us first discuss the Naive algorithm for this problem and then the Backtracking algorithm.

Naive Algorithm for Knight’s tour 
The Naive Algorithm is to generate all tours one by one and check if the generated tour satisfies the constraints. 

while there are untried tours
{ 
   generate the next tour 
   if this tour covers all squares 
   { 
      print this path;
   }
}
Backtracking works in an incremental way to attack problems. Typically, we start from an empty solution vector and one by one add items (Meaning of item varies from problem to problem. In the context of Knight’s tour problem, an item is a Knight’s move). When we add an item, we check if adding the current item violates the problem constraint, if it does then we remove the item and try other alternatives. If none of the alternatives works out then we go to the previous stage and remove the item added in the previous stage. If we reach the initial stage back then we say that no solution exists. If adding an item doesn’t violate constraints then we recursively add items one by one. If the solution vector becomes complete then we print the solution.

Backtracking Algorithm for Knight’s tour 

Following is the Backtracking algorithm for Knight’s tour problem. 

If all squares are visited 
    print the solution
Else
   a) Add one of the next moves to solution vector and recursively 
   check if this move leads to a solution. (A Knight can make maximum 
   eight moves. We choose one of the 8 moves in this step).
   b) If the move chosen in the above step doesn't lead to a solution
   then remove this move from the solution vector and try other 
   alternative moves.
   c) If none of the alternatives work then return false (Returning false 
   will remove the previously added item in recursion and if false is 
   returned by the initial call of recursion then "no solution exists" )
Following are implementations for Knight’s tour problem. It prints one of the possible solutions in 2D matrix form. Basically, the output is a 2D 8*8 matrix with numbers from 0 to 63 and these numbers show steps made by Knight. '''

# Python3 program to solve Knight Tour problem using Backtracking

# Chessboard Size
n = 8


def isSafe(x, y, board):
    '''
        A utility function to check if i,j are valid indexes
        for N*N chessboard
    '''
    if(x >= 0 and y >= 0 and x < n and y < n and board[x][y] == -1):
        return True
    return False


def printSolution(n, board):
    '''
        A utility function to print Chessboard matrix
    '''
    for i in range(n):
        for j in range(n):
            print(board[i][j], end=' ')
        print()


def solveKT(n):
    '''
        This function solves the Knight Tour problem using
        Backtracking. This function mainly uses solveKTUtil()
        to solve the problem. It returns false if no complete
        tour is possible, otherwise return true and prints the
        tour.
        Please note that there may be more than one solutions,
        this function prints one of the feasible solutions.
    '''

    # Initialization of Board matrix
    board = [[-1 for i in range(n)]for i in range(n)]

    # move_x and move_y define next move of Knight.
    # move_x is for next value of x coordinate
    # move_y is for next value of y coordinate
    move_x = [2, 1, -1, -2, -2, -1, 1, 2]
    move_y = [1, 2, 2, 1, -1, -2, -2, -1]

    # Since the Knight is initially at the first block
    board[0][0] = 0

    # Step counter for knight's position
    pos = 1

    # Checking if solution exists or not
    if(not solveKTUtil(n, board, 0, 0, move_x, move_y, pos)):
        print("Solution does not exist")
    else:
        printSolution(n, board)


def solveKTUtil(n, board, curr_x, curr_y, move_x, move_y, pos):
    '''
        A recursive utility function to solve Knight Tour
        problem
    '''

    if(pos == n**2):
        return True

    # Try all next moves from the current coordinate x, y
    for i in range(8):
        new_x = curr_x + move_x[i]
        new_y = curr_y + move_y[i]
        if(isSafe(new_x, new_y, board)):
            board[new_x][new_y] = pos
            if(solveKTUtil(n, board, new_x, new_y, move_x, move_y, pos+1)):
                return True

            # Backtracking
            board[new_x][new_y] = -1
    return False


# Driver Code
if __name__ == "__main__":

    # Function Call
    solveKT(n)
