'''https://practice.geeksforgeeks.org/problems/steps-by-knight5927/1
Steps by Knight
Medium Accuracy: 50.11% Submissions: 33425 Points: 4
Given a square chessboard, the initial position of Knight and position of a target. Find out the minimum steps a Knight will take to reach the target position.

Note:
The initial and the target position co-ordinates of Knight have been given accoring to 1-base indexing.



Example 1:

Input:
N=6
knightPos[ ] = {4, 5}
targetPos[ ] = {1, 1}
Output:
3
Explanation:

Knight takes 3 step to reach from
(4, 5) to (1, 1):
(4, 5) -> (5, 3) -> (3, 2) -> (1, 1).




Your Task:
You don't need to read input or print anything. Your task is to complete the function minStepToReachTarget() which takes the inital position of Knight (KnightPos), the target position of Knight (TargetPos) and the size of the chess board (N) as an input parameters and returns the minimum number of steps required by the knight to reach from its current position to the given target position.



Expected Time Complexity: O(N2).
Expected Auxiliary Space: O(N2).



Constraints:
1 <= N <= 1000
1 <= Knight_pos(X, Y), Targer_pos(X, Y) <= N'''

from collections import deque


class Solution:

	# Function to find out minimum steps Knight needs to reach target position.
	def isInside(self, x, y, N):
        return (x >= 0 and x < N
        and y >= 0 and y < N)

    def minStepToReachTarget(self, KnightPos, TargetPos, N):
        # Code here
        # all possible movments for the knight
        dx = [2, 2, -2, -2, 1, 1, -1, -1]
        dy = [1, -1, 1, -1, 2, -2, 2, -2]
        KnightPos[0] -= 1
        KnightPos[1] -= 1
        TargetPos[0] -= 1
        TargetPos[1] -= 1        
        
        visited = []
        for i in range(N):
            temp = []
            for j in range(N):
                temp.append(False)
            visited.append(temp)
        
        Q = deque()
        Q.append(KnightPos + [0])
        
        while(Q):
            cur = Q.popleft()
            
            if(cur[0:2] == TargetPos):
                return cur[2]
        
            for k in range(8):
                x = cur[0]+dx[k]
                y = cur[1]+dy[k]
                
                if(self.isInside(x, y, N) and not visited[x][y]):
                    visited[x][y] = True
                    Q.append([x, y, cur[2] + 1])
        

# { 
#  Driver Code Starts

if __name__ == '__main__':
	T=int(input())
	for i in range(T):
		N = int(input())
		KnightPos = list(map(int, input().split()))
		TargetPos = list(map(int, input().split()))
		obj = Solution()
		ans = obj.minStepToReachTarget(KnightPos, TargetPos, N)
		print(ans)

# } Driver Code Ends
