'''
https://www.geeksforgeeks.org/find-count-number-given-string-present-2d-character-array/
Given a 2-Dimensional character array and a string, we need to find the given string in 2-dimensional character array such that individual characters can be present left to right, right to left, top to down or down to top.

Examples: 

In case you wish to attend live classes with experts, please refer DSA Live Classes for Working Professionals and Competitive Programming Live for Students.
Input : a ={
            {D,D,D,G,D,D},
            {B,B,D,E,B,S},
            {B,S,K,E,B,K},
            {D,D,D,D,D,E},
            {D,D,D,D,D,E},
            {D,D,D,D,D,G}
           }
        str= "GEEKS"
Output :2

Input : a = {
            {B,B,M,B,B,B},
            {C,B,A,B,B,B},
            {I,B,G,B,B,B},
            {G,B,I,B,B,B},
            {A,B,C,B,B,B},
            {M,C,I,G,A,M}
            }
        str= "MAGIC"

Output :3

We have discussed simpler problem to find if a word exists or not in a matrix.
To count all occurrences, we follow simple brute force approach. Traverse through each character of the matrix and taking each character as start of the string to be found, try to search in all the possible directions. Whenever, a word is found, increase the count, and after traversing the matrix what ever will be the value of count will be number of times string exists in character matrix.

Algorithm : 
1- Traverse matrix character by character and take one character as string start 
2- For each character find the string in all the four directions recursively 
3- If a string found, we increase the count 
4- When we are done with one character as start, we repeat the same process for the next character 
5- Calculate the sum of count for each character 
6- Final count will be the answer'''
# Python code for finding count
# of string in a given 2D
# character array.

# utility function to search
# complete string from any
# given index of 2d array


def internalSearch(ii, needle, row, col, hay,
                   row_max, col_max):

    found = 0
    if (row >= 0 and row <= row_max and
        col >= 0 and col <= col_max and
            needle[ii] == hay[row][col]):
        match = needle[ii]
        ii += 1
        hay[row][col] = 0
        if (ii == len(needle)):
            found = 1
        else:

            # through Backtrack searching
            # in every directions
            found += internalSearch(ii, needle, row,
                                    col + 1, hay, row_max, col_max)
            found += internalSearch(ii, needle, row,
                                    col - 1, hay, row_max, col_max)
            found += internalSearch(ii, needle, row + 1,
                                    col, hay, row_max, col_max)
            found += internalSearch(ii, needle, row - 1,
                                    col, hay, row_max, col_max)
        hay[row][col] = match
    return found

# Function to search the string in 2d array


def searchString(needle, row, col, strr,
                 row_count, col_count):
    found = 0
    for r in range(row_count):
        for c in range(col_count):
            found += internalSearch(0, needle, r, c,
                                    strr, row_count - 1, col_count - 1)

    return found

# Driver code


needle = "MAGIC"
inputt = ["BBABBM", "CBMBBA", "IBABBG",
          "GOZBBI", "ABBBBC", "MCIGAM"]

strr = [0] * len(inputt)

for i in range(len(inputt)):
    strr[i] = list(inputt[i])

print("count: ", searchString(needle, 0, 0, strr,
                              len(strr), len(strr[0])))
