'''
https://www.geeksforgeeks.org/vertex-cover-problem-set-1-introduction-approximate-algorithm-2/
A vertex cover of an undirected graph is a subset of its vertices such that for every edge (u, v) of the graph, either ‘u’ or ‘v’ is in the vertex cover. Although the name is Vertex Cover, the set covers all edges of the given graph. Given an undirected graph, the vertex cover problem is to find minimum size vertex cover. 
The following are some examples. 

Vertex Cover Problem is a known NP Complete problem, i.e., there is no polynomial-time solution for this unless P = NP. There are approximate polynomial-time algorithms to solve the problem though. Following is a simple approximate algorithm adapted from CLRS book.



Naive Approach:

Consider all the subset of vertices one by one and find out whether it covers all edges of the graph. For eg. in a graph consisting only 3 vertices the set consisting of the combination of vertices are:{0,1,2,{0,1},{0,2},{1,2},{0,1,2}} . Using each element of this set check whether these vertices cover all  all the edges of the graph. Hence update the optimal answer. And hence print the subset having minimum number of vertices which also covers all the edges of the graph.

Approximate Algorithm for Vertex Cover: 
 

1) Initialize the result as {}
2) Consider a set of all edges in given graph.  Let the set be E.
3) Do following while E is not empty
...a) Pick an arbitrary edge (u, v) from set E and add 'u' and 'v' to result
...b) Remove all edges from E which are either incident on u or v.
4) Return result 
Below diagram to show the execution of the above approximate algorithm: 
 

vertexCover

How well the above algorithm perform? 
It can be proved that the above approximate algorithm never finds a vertex cover whose size is more than twice the size of the minimum possible vertex cover (Refer this for proof)
Implementation: 
The following are C++ and Java implementations of the above approximate algorithm. 
 
 Output: 

0 1 3 4 5 6
The Time Complexity of the above algorithm is O(V + E).
Exact Algorithms: 
Although the problem is NP complete, it can be solved in polynomial time for the following types of graphs. 
1) Bipartite Graph 
2) Tree Graph
The problem to check whether there is a vertex cover of size smaller than or equal to a given number k can also be solved in polynomial time if k is bounded by O(LogV) (Refer this)'''
# Python3 program to print Vertex Cover
# of a given undirected graph
from collections import defaultdict

# This class represents a directed graph
# using adjacency list representation


class Graph:

    def __init__(self, vertices):

        # No. of vertices
        self.V = vertices

        # Default dictionary to store graph
        self.graph = defaultdict(list)

    # Function to add an edge to graph
    def addEdge(self, u, v):
        self.graph[u].append(v)

    # The function to print vertex cover
    def printVertexCover(self):

        # Initialize all vertices as not visited.
        visited = [False] * (self.V)

        # Consider all edges one by one
        for u in range(self.V):

            # An edge is only picked when
            # both visited[u] and visited[v]
            # are false
            if not visited[u]:

                # Go through all adjacents of u and
                # pick the first not yet visited
                # vertex (We are basically picking
                # an edge (u, v) from remaining edges.
                for v in self.graph[u]:
                    if not visited[v]:

                        # Add the vertices (u, v) to the
                        # result set. We make the vertex
                        # u and v visited so that all
                        # edges from/to them would
                        # be ignored
                        visited[v] = True
                        visited[u] = True
                        break

        # Print the vertex cover
        for j in range(self.V):
            if visited[j]:
                print(j, end=' ')

        print()

# Driver code


# Create a graph given in
# the above diagram
g = Graph(7)
g.addEdge(0, 1)
g.addEdge(0, 2)
g.addEdge(1, 3)
g.addEdge(3, 4)
g.addEdge(4, 5)
g.addEdge(5, 6)

g.printVertexCover()
