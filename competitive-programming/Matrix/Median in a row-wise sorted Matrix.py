'''https://practice.geeksforgeeks.org/problems/median-in-a-row-wise-sorted-matrix1527/1
Median in a row-wise sorted Matrix 
Medium Accuracy: 53.5% Submissions: 20209 Points: 4
Given a row wise sorted matrix of size RxC where R and C are always odd, find the median of the matrix.

Example 1:

Input:
R = 3, C = 3
M = [[1, 3, 5], 
     [2, 6, 9], 
     [3, 6, 9]]

Output: 5

Explanation:
Sorting matrix elements gives us 
{1,2,3,3,5,6,6,9,9}. Hence, 5 is median. 
 

Example 2:

Input:
R = 3, C = 1
M = [[1], [2], [3]]
Output: 2

Your Task:  
You don't need to read input or print anything. Your task is to complete the function median() which takes the integers R and C along with the 2D matrix as input parameters and returns the median of the matrix.

Expected Time Complexity: O(32 * R * log(C))
Expected Auxiliary Space: O(1)


Constraints:
1<= R,C <=150
1<= matrix[i][j] <=2000'''


class Solution:
    def median(self, matrix, r, c):

        def count_ele(arr, ele):
            start = 0
            end = len(arr) - 1

            while start <= end:
                mid = (start + end) // 2
                if arr[mid] <= ele:
                    start = mid + 1
                else:
                    end = mid - 1
            return start

        start = 1
        end = 2000

        while start <= end:
            mid = (start + end) // 2
            cu = 0
            for i in matrix:
                cu += count_ele(i, mid)

            if cu <= (r*c) // 2:
                start = mid + 1
            else:
                end = mid - 1
        return start

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':
    ob = Solution()
    t = int(input())
    for _ in range(t):
        r, c = map(int, input().strip().split())
        matrix = [[0 for j in range(c)] for i in range(r)]
        line1 = [int(x) for x in input().strip().split()]
        k = 0
        for i in range(r):
            for j in range(c):
                matrix[i][j] = line1[k]
                k += 1
        ans = ob.median(matrix, r, c)
        print(ans)
# } Driver Code Ends
