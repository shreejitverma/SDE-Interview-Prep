'''https://www.geeksforgeeks.org/longest-possible-route-in-a-matrix-with-hurdles/
Longest Possible Route in a Matrix with Hurdles
Difficulty Level : Medium
Last Updated : 18 Aug, 2021
Given an M x N matrix, with a few hurdles arbitrarily placed, calculate the length of the longest possible route possible from source to a destination within the matrix. We are allowed to move to only adjacent cells which are not hurdles. The route cannot contain any diagonal moves and a location once visited in a particular path cannot be visited again.
For example, the longest path with no hurdles from source to destination is highlighted below. The length of the path is 24.

The idea is to use Backtracking. We start from the source cell of the matrix, move forward in all four allowed directions, and recursively checks if they lead to the solution or not. If the destination is found, we update the value of the longest path else if none of the above solutions work we return false from our function.
Below is the C++ implementation of the above idea –'''

# Check if it is possible to go to position (x, y) from
# the current position. The function returns false if the cell
# is invalid, has a value 0, or it is already visited.


def isSafe(mat, visited, x, y):
    return 0 <= x < len(mat) and 0 <= y < len(mat[0]) and \
           not (mat[x][y] == 0 or visited[x][y])


# Find the longest possible route in a matrix `mat` from the source cell (i, j)
# to destination cell `dest`.
# `max_dist` —> keep track of the length of the longest path from source to destination
# `dist` —> length of the path from the source cell to the current cell (i, j)
def findLongestPath(mat, visited, i, j, dest, max_dist=0, dist=0):

    # if the destination is not possible from the current cell
    if mat[i][j] == 0:
        return 0

    # if the destination is found, update `max_dist`
    if (i, j) == dest:
        return max(dist, max_dist)

    # set (i, j) cell as visited
    visited[i][j] = 1

    # go to the bottom cell
    if isSafe(mat, visited, i + 1, j):
        max_dist = findLongestPath(
            mat, visited, i + 1, j, dest, max_dist, dist + 1)

    # go to the right cell
    if isSafe(mat, visited, i, j + 1):
        max_dist = findLongestPath(
            mat, visited, i, j + 1, dest, max_dist, dist + 1)

    # go to the top cell
    if isSafe(mat, visited, i - 1, j):
        max_dist = findLongestPath(
            mat, visited, i - 1, j, dest, max_dist, dist + 1)

    # go to the left cell
    if isSafe(mat, visited, i, j - 1):
        max_dist = findLongestPath(
            mat, visited, i, j - 1, dest, max_dist, dist + 1)

    # backtrack: remove (i, j) from the visited matrix
    visited[i][j] = 0

    return max_dist


# Wrapper over findLongestPath() function
def findLongestPathLength(mat, src, dest):

    # get source cell (i, j)
    i, j = src

    # get destination cell (x, y)
    x, y = dest

    # base case
    if not mat or len(mat) == 0 or mat[i][j] == 0 or mat[x][y] == 0:
        return 0

    # `M × N` matrix
    (M, N) = (len(mat), len(mat[0]))

    # construct an `M × N` matrix to keep track of visited cells
    visited = [[0 for x in range(N)] for y in range(M)]

    # (i, j) are the source cell coordinates, and (x, y) are the
    # destination cell coordinates
    return findLongestPath(mat, visited, i, j, dest)


if __name__ == '__main__':

    # input matrix
    mat = [
        [1, 0, 1, 1, 1, 1, 0, 1, 1, 1],
        [1, 0, 1, 0, 1, 1, 1, 0, 1, 1],
        [1, 1, 1, 0, 1, 1, 0, 1, 0, 1],
        [0, 0, 0, 0, 1, 0, 0, 1, 0, 0],
        [1, 0, 0, 0, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 0],
        [1, 0, 0, 0, 1, 0, 0, 1, 0, 1],
        [1, 0, 1, 1, 1, 1, 0, 0, 1, 1],
        [1, 1, 0, 0, 1, 0, 0, 0, 0, 1],
        [1, 0, 1, 1, 1, 1, 0, 1, 0, 0]
    ]

    src = (0, 0)
    dest = (5, 7)

    print("The maximum length path is", findLongestPathLength(mat, src, dest)
