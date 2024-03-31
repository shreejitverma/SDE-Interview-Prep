'''https://practice.geeksforgeeks.org/problems/find-the-string-in-grid0111/1

Find the string in grid
Medium Accuracy: 46.55% Submissions: 9799 Points: 4
Given a 2D grid of n*m of characters and a word, find all occurrences of given word in grid. A word can be matched in all 8 directions at any point. Word is said be found in a direction if all characters match in this direction (not in zig-zag form). The 8 directions are, horizontally left, horizontally right, vertically up, vertically down and 4 diagonal directions.


Example 1:

Input: grid = {{a,b,c},{d,r,f},{g,h,i}},
word = "abc"
Output: {{0,0}}
Expalnation: From (0,0) one can find "abc"
in horizontally right direction.
Example 2:

Input: grid = {{a,b,a,b},{a,b,e,b},{e,b,e,b}}
,word = "abe"
Output: {{0,0},{0,2},{1,0}}
Explanation: From (0,0) one can find "abe" in
right-down diagonal. From (0,2) one can
find "abe" in left-down diagonal. From
(1,0) one can find "abe" in Horizontally right
direction.


Your Task:
You don't need to read or print anyhting, Your task is to complete the function searchWord() which takes grid and word as input parameters and returns a list containg the positions from where the word originates in any direction. If there is no such position then returns an empty list.

Note: The returning list should be lexicographically smallest. If the word can be found in multiple directions strating from the same coordinates, the list should contain the coordinates only once.


Expected Time Complexity: O(n*m*k) where k is constant
Exected Space Complexity: O(1)


Constraints:
1 <= n <= m <= 100
1 <= |word| <= 10'''


class Solution:
	def search(self, grid, words, i, j, n, m):
       dx = [-1, -1, -1, 0, 0, 1, 1, 1]
       dy = [-1, 0, 1, -1, 1, -1, 0, 1]
       if (grid[i][j] != word[0]):
            return False;
       for dir in range(8):
           x = i + dx[dir]
           y = j + dy[dir]
           for index in range(1,len(word)):
               if (x < 0 or y < 0 or x >= n or y >= m or grid[x][y] != word[index]):
                    break
               x += dx[dir]
               y += dy[dir]
           else:
                return True;
       return False;
    def searchWord(self, grid, word):
       ans = []
       n = len(grid)
       m = len(grid[0])
       for i in range(n):
           for j in range(m):
               a = self.search(grid, word, i, j, n, m)
               if a:
                   ans.append((i, j))
       return ans;
# { 
#  Driver Code Starts
# Initial Template for Python 3

if __name__ == '__main__':
	T=int(input())
	for i in range(T):
		n, m = input().split()
		n = int(n); m = int(m);
		grid = []
		for _ in range(n):
			cur  = input()
			temp = []
			for __ in cur:
				temp.append(__)
			grid.append(temp)
		word = input()
		obj = Solution()
		ans = obj.searchWord(grid, word)
		for _ in ans:
			for __ in _:
				print(__, end = " ")
			print()
		if len(ans)==0:
		    print(-1)

# } Driver Code Ends
# Python3 program to search a word in a 2D grid
# User function Template for python3
class GFG:

    def __init__(self):
        self.R = None
        self.C = None
        self.dir = [[-1, 0], [1, 0], [1, 1],
                    [1, -1], [-1, -1], [-1, 1],
                    [0, 1], [0, -1]]

    # This function searches in all 8-direction
    # from point(row, col) in grid[][]
    def search2D(self, grid, row, col, word):

        # If first character of word doesn't match
        # with the given starting point in grid.
        if grid[row][col] != word[0]:
            return False

        # Search word in all 8 directions
        # starting from (row, col)
        for x, y in self.dir:

            # Initialize starting point
            # for current direction
            rd, cd = row + x, col + y
            flag = True

            # First character is already checked,
            # match remaining characters
            for k in range(1, len(word)):

                # If out of bound or not matched, break
                if (0 <= rd < self.R and
                    0 <= cd < self.C and
                        word[k] == grid[rd][cd]):

                    # Moving in particular direction
                    rd += x
                    cd += y
                else:
                    flag = False
                    break

            # If all character matched, then
            # value of flag must be false
            if flag:
                return True
        return False

    # Searches given word in a given matrix
    # in all 8 directions
    def patternSearch(self, grid, word):

        # Rows and columns in given grid
        self.R = len(grid)
        self.C = len(grid[0])

        # Consider every point as starting point
        # and search given word
        for row in range(self.R):
            for col in range(self.C):
                if self.search2D(grid, row, col, word):
                    print("pattern found at " +
                          str(row) + ', ' + str(col))


# Driver Code
if __name__ == '__main__':
    grid = ["GEEKSFORGEEKS",
            "GEEKSQUIZGEEK",
            "IDEQAPRACTICE"]
    gfg = GFG()
    gfg.patternSearch(grid, 'GEEKS')
    print('')
    gfg.patternSearch(grid, 'EEE')
