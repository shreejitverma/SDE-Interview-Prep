'''https://practice.geeksforgeeks.org/problems/negative-weight-cycle3504/1
https://www.geeksforgeeks.org/detect-negative-cycle-graph-bellman-ford/
Negative weight cycle 
Medium Accuracy: 50.77% Submissions: 14632 Points: 4
Given a weighted directed graph with n nodes and m edges. Nodes are labeled from 0 to n-1, the task is to check if it contains a negative weight cycle or not.
Note: edges[i] is defined as u, v and weight.
 

Example 1:

Input: n = 3, edges = {{0,1,-1},{1,2,-2},
{2,0,-3}}
Output: 1
Explanation: The graph contains negative weight
cycle as 0->1->2->0 with weight -1,-2,-3,-1.
Example 2:

Input: n = 3, edges = {{0,1,-1},{1,2,-2},
{2,0,3}}
Output: 0
Explanation: The graph does not contain any
negative weight cycle.
 

Your Task:
You don't need to read or print anyhting. Your task is to complete the function isNegativeWeightCycle() which takes n and edges as input paramater and returns 1 if graph contains negative weight cycle otherwise returns 0.
 

Expected Time Complexity: O(n*m)
Expected Space Compelxity: O(n)
 

Constraints:
1 <= n <= 100
1 <= m <= n*(n-1), where m is the total number of Edges in the directed graph.
'''

# A Python3 program for Bellman-Ford's single source
# shortest path algorithm.

# The main function that finds shortest distances
# from src to all other vertices using Bellman-
# Ford algorithm. The function also detects
# negative weight cycle


class Solution:
    def isNegativeWeightCycle(self, v, e):
        # Code here
        dist = [1e6] * v
    dist[0] = 0
    for i in range(n-1):
        for j in range(len(e)):

            src = e[j][0]
            dest = e[j][1]
            wt = e[j][2]

            if dist[src] + wt < dist[dest]:
                dist[dest] = dist[src] + wt

    for i in range(len(e)):
        src = e[i][0]
        dest = e[i][1]
        wt = e[i][2]

        if dist[src] != 1e6 and dist[src] + wt < dist[dest]:
            return 1

    return 0


def isNegCycleBellmanFord(src, dist):
    global graph, V, E

    # Step 1: Initialize distances from src
    # to all other vertices as INFINITE
    for i in range(V):
        dist[i] = 10**18
    dist[src] = 0

    # Step 2: Relax all edges |V| - 1 times.
    # A simple shortest path from src to any
    # other vertex can have at-most |V| - 1
    # edges
    for i in range(1, V):
        for j in range(E):
            u = graph[j][0]
            v = graph[j][1]
            weight = graph[j][2]
            if (dist[u] != 10**18 and dist[u] + weight < dist[v]):
                dist[v] = dist[u] + weight

    # Step 3: check for negative-weight cycles.
    # The above step guarantees shortest distances
    # if graph doesn't contain negative weight cycle.
    # If we get a shorter path, then there
    # is a cycle.
    for i in range(E):
        u = graph[i][0]
        v = graph[i][1]
        weight = graph[i][2]
        if (dist[u] != 10**18 and dist[u] + weight < dist[v]):
            return True

    return False
# Returns true if given graph has negative weight
# cycle.


def isNegCycleDisconnected():
    global V, E, graph

    # To keep track of visited vertices to avoid
    # recomputations.
    visited = [0]*V
    # memset(visited, 0, sizeof(visited))

    # This array is filled by Bellman-Ford
    dist = [0]*V

    # Call Bellman-Ford for all those vertices
    # that are not visited
    for i in range(V):
        if (visited[i] == 0):

            # If cycle found
            if (isNegCycleBellmanFord(i, dist)):
                return True

            # Mark all vertices that are visited
            # in above call.
            for i in range(V):
                if (dist[i] != 10**18):
                    visited[i] = True
    return False


# Driver code
if __name__ == '__main__':

    # /* Let us create the graph given in above example */
    V = 5  # Number of vertices in graph
    E = 8  # Number of edges in graph
    graph = [[0, 0, 0] for i in range(8)]

    # add edge 0-1 (or A-B in above figure)
    graph[0][0] = 0
    graph[0][1] = 1
    graph[0][2] = -1

    # add edge 0-2 (or A-C in above figure)
    graph[1][0] = 0
    graph[1][1] = 2
    graph[1][2] = 4

    # add edge 1-2 (or B-C in above figure)
    graph[2][0] = 1
    graph[2][1] = 2
    graph[2][2] = 3

    # add edge 1-3 (or B-D in above figure)
    graph[3][0] = 1
    graph[3][1] = 3
    graph[3][2] = 2

    # add edge 1-4 (or A-E in above figure)
    graph[4][0] = 1
    graph[4][1] = 4
    graph[4][2] = 2

    # add edge 3-2 (or D-C in above figure)
    graph[5][0] = 3
    graph[5][1] = 2
    graph[5][2] = 5

    # add edge 3-1 (or D-B in above figure)
    graph[6][0] = 3
    graph[6][1] = 1
    graph[6][2] = 1

    # add edge 4-3 (or E-D in above figure)
    graph[7][0] = 4
    graph[7][1] = 3
    graph[7][2] = -3

    if (isNegCycleDisconnected()):
        print("Yes")
    else:
        print("No")
