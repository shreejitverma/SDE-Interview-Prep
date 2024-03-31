'''https://practice.geeksforgeeks.org/problems/strongly-connected-components-kosarajus-algo/1
Strongly Connected Components (Kosaraju's Algo) 
Medium Accuracy: 49.73% Submissions: 33164 Points: 4
Given a Directed Graph with V vertices (Numbered from 0 to V-1) and E edges, Find the number of strongly connected components in the graph.
 

Example 1:

Input:

Output:
3
Explanation:

We can clearly see that there are 3 Strongly
Connected Components in the Graph
Example 2:

Input:

Output:
1
Explanation:
All of the nodes are connected to each other.
So, there's only one SCC.
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function kosaraju() which takes the number of vertices V and adjacency list of the graph as inputs and returns an integer denoting the number of strongly connected components in the given graph.
 

Expected Time Complexity: O(V+E).
Expected Auxiliary Space: O(V).
 

Constraints:
1 ≤ V ≤ 5000
0 ≤ E ≤ (V*(V-1))
0 ≤ u, v ≤ N-1
Sum of E over all testcases will not exceed 25*106'''
# User function Template for python3


from collections import OrderedDict
import sys


class Solution:
    def dfs(self, curr, graph, visited):

        visited[curr] = True

        for nbr in graph[curr]:

            if visited[nbr]:
                continue

            self.dfs(nbr, graph, visited)

    def transpose(self, graph):

        res = [[] for i in range(len(graph))]

        for u in range(len(graph)):
            for v in graph[u]:

                res[v].append(u)

        return res

    def dfs1(self, curr, graph, visited, stack):

        visited[curr] = True

        for nbr in graph[curr]:

            if visited[nbr]:
                continue

            self.dfs1(nbr, graph, visited, stack)

        stack.append(curr)

    # Function to find number of strongly connected components in the graph.

    def kosaraju(self, V, adj):
        # code here
        visited = [False for i in range(V)]
        stack = []

        for u in range(V):

            if not visited[u]:
                self.dfs1(u, adj, visited, stack)

        # stack is filled here

        graph = self.transpose(adj)
        for i in range(V):
            visited[i] = False

        count = 0

        while stack:

            node = stack.pop()

            if not visited[node]:
                self.dfs(node, graph, visited)
                count += 1

        return count
# {
#  Driver Code Starts
# Initial Template for Python 3


sys.setrecursionlimit(10**6)
if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        V, E = list(map(int, input().strip().split()))
        adj = [[] for i in range(V)]
        for i in range(E):
            a, b = map(int, input().strip().split())
            adj[a].append(b)
        ob = Solution()

        print(ob.kosaraju(V, adj))
# } Driver Code Ends
