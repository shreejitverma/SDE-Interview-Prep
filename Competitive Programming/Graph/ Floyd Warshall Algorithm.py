'''
https://practice.geeksforgeeks.org/problems/implementing-floyd-warshall2042/1
https://www.geeksforgeeks.org/floyd-warshall-algorithm-dp-16/
Floyd Warshall 
Medium Accuracy: 44.25% Submissions: 17604 Points: 4
The problem is to find shortest distances between every pair of vertices 
in a given edge weighted directed Graph. 
The Graph is represented as adjancency matrix, and the matrix denotes the weight of the edegs (if it exists) else -1. 
Do it in-place.
 

Example 1:

Input: matrix = {{0,25},{-1,0}}
Output: {{0,25},{-1,0}}
Explanation: The shortest distance between
every pair is already given(if it exists).
Example 2:

Input: matrix = {{0,1,43},{1,0,6},{-1,-1,0}}
Output: {{0,1,7},{1,0,6},{-1,-1,0}}
Explanation: We can reach 3 from 1 as 1->2->3
and the cost will be 1+6=7 which is less than 
43.
 

Your Task:
You don't need to read, return or print anything. 
Your task is to complete the function shortest_distance() 
which takes the matrix as input parameter and modify the distances for every pair in-place.
 

Expected Time Complexity: O(n3)
Expected Space Compelxity: O(1)
 

Constraints:
1 <= n <= 100

The Floyd Warshall Algorithm is for solving the All Pairs Shortest Path problem. The problem is to find shortest distances between every pair of vertices in a given edge weighted directed Graph. 
Example: 

Input:
       graph[][] = { {0,   5,  INF, 10},
                    {INF,  0,  3,  INF},
                    {INF, INF, 0,   1},
                    {INF, INF, INF, 0} }
which represents the following graph
             10
       (0)------->(3)
        |         /|\
      5 |          |
        |          | 1
       \|/         |
       (1)------->(2)
            3       
Note that the value of graph[i][j] is 0 if i is equal to j 
And graph[i][j] is INF (infinite) if there is no edge from vertex i to j.

Output:
Shortest distance matrix
      0      5      8      9
    INF      0      3      4
    INF    INF      0      1
    INF    INF    INF      0

Floyd Warshall Algorithm 
We initialize the solution matrix same as the input graph matrix as a first step. 
Then we update the solution matrix by considering all vertices as an intermediate vertex. 
The idea is to one by one pick all vertices and updates all shortest paths which include the picked vertex 
as an intermediate vertex in the shortest path. When we pick vertex number k as an intermediate vertex, 
we already have considered vertices {0, 1, 2, .. k-1} as intermediate vertices. 
For every pair (i, j) of the source and destination vertices respectively, there are two possible cases. 
1) k is not an intermediate vertex in shortest path from i to j. We keep the value of dist[i][j] as it is. 
2) k is an intermediate vertex in shortest path from i to j. 
We update the value of dist[i][j] as dist[i][k] + dist[k][j] if dist[i][j] > dist[i][k] + dist[k][j]
The following figure shows the above optimal substructure property in the all-pairs shortest path problem.'''


class Solution:
    def shortest_distance(self, matrix):
        # Code here
        V = len(matrix)

    for i in range(V):
        for j in range(V):
            if matrix[i][j] == -1:
                matrix[i][j] = float("inf")

    for k in range(V):
        for i in range(V):
            for j in range(V):
                matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j])

    for i in range(V):
        for j in range(V):
            if matrix[i][j] == float("inf"):
                matrix[i][j] = -1
# Python Program for Floyd Warshall Algorithm


# Number of vertices in the graph
V = 4

# Define infinity as the large
# enough value. This value will be
# used for vertices not connected to each other
INF = 99999

# Solves all pair shortest path
# via Floyd Warshall Algorithm


def floydWarshall(graph):
    """ dist[][] will be the output
       matrix that will finally
        have the shortest distances
        between every pair of vertices """
    """ initializing the solution matrix
    same as input graph matrix
    OR we can say that the initial
    values of shortest distances
    are based on shortest paths considering no
    intermediate vertices """

    dist = list(map(lambda i: list(map(lambda j: j, i)), graph))

    """ Add all vertices one by one
    to the set of intermediate
     vertices.
     ---> Before start of an iteration,
     we have shortest distances
     between all pairs of vertices
     such that the shortest
     distances consider only the
     vertices in the set
    {0, 1, 2, .. k-1} as intermediate vertices.
      ----> After the end of a
      iteration, vertex no. k is
     added to the set of intermediate
     vertices and the
    set becomes {0, 1, 2, .. k}
    """
    for k in range(V):

        # pick all vertices as source one by one
        for i in range(V):

            # Pick all vertices as destination for the
            # above picked source
            for j in range(V):

                # If vertex k is on the shortest path from
                # i to j, then update the value of dist[i][j]
                dist[i][j] = min(dist[i][j],
                                 dist[i][k] + dist[k][j]
                                 )
    printSolution(dist)


# A utility function to print the solution
def printSolution(dist):
    print "Following matrix shows the shortest distances\
 between every pair of vertices"
    for i in range(V):
        for j in range(V):
            if(dist[i][j] == INF):
                print "%7s" % ("INF"),
            else:
                print "%7d\t" % (dist[i][j]),
            if j == V-1:
                print ""


# Driver program to test the above program
# Let us create the following weighted graph
"""
            10
       (0)------->(3)
        |         /|\
      5 |          |
        |          | 1
       \|/         |
       (1)------->(2)
            3           """
graph = [[0, 5, INF, 10],
         [INF, 0, 3, INF],
         [INF, INF, 0,   1],
         [INF, INF, INF, 0]
         ]
# Print the solution
floydWarshall(graph)
