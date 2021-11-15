'''https://practice.geeksforgeeks.org/problems/detect-cycle-in-an-undirected-graph/1
Detect cycle in an undirected graph 
Medium Accuracy: 35.66% Submissions: 100k+ Points: 4
Given an undirected graph with V vertices and E edges, check whether it contains any cycle or not. 

Example 1:

Input:   

Output: 1
Explanation: 1->2->3->4->1 is a cycle.
Example 2:

Input: 

Output: 0
Explanation: No cycle in the graph.
 

Your Task:
You don't need to read or print anything. Your task is to complete the function isCycle() which takes V denoting the number of vertices and adjacency list as input parameters and returns a boolean value denoting if the undirected graph contains any cycle or not, return 1 if a cycle is present else return 0.

NOTE: The adjacency list denotes the edges of the graph where edges[i][0] and edges[i][1] represent an edge.

 

Expected Time Complexity: O(V + E)
Expected Space Complexity: O(V)


 

Constraints:
1 ≤ V, E ≤ 105'''


class Solution:

    # Function to detect cycle in an undirected graph.
    def isCycle(self, V, adj):
    vis = [0]*V

    def dfs(cur, parent):
        vis[cur] = 1

        for i in adj[cur]:
            if not vis[i]:
                if dfs(i, cur):
                    return True
            elif i != parent:
                return True

        return False

    for i in range(V):
        if not vis[i]:
            if dfs(i, -1):
                return True

    return False


# {
#  Driver Code Starts
if __name__ == '__main__':

    T = int(input())
    for i in range(T):
        V, E = map(int, input().split())
        adj = [[] for i in range(V)]
        for _ in range(E):
            u, v = map(int, input().split())
            adj[u].append(v)
            adj[v].append(u)
        obj = Solution()
        ans = obj.isCycle(V, adj)
        if(ans):
            print("1")
        else:
            print("0")

# } Driver Code Ends
