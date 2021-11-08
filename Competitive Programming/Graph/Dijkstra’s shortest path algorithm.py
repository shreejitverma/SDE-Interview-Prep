'''https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

Like Prim’s MST, we generate a SPT (shortest path tree) with a given source as a root. 
We maintain two sets, one set contains vertices included in the shortest-path tree, 
other set includes vertices not yet included in the shortest-path tree. 
At every step of the algorithm, we find a vertex that is in the other set (set of not yet included) 
and has a minimum distance from the source.
Below are the detailed steps used in Dijkstra’s algorithm to find the shortest path 
from a single source vertex to all other vertices in the given graph. 

Algorithm 
1) Create a set sptSet (shortest path tree set) that keeps track of vertices included 
in the shortest-path tree, i.e., whose minimum distance from the source is calculated and finalized. 
Initially, this set is empty. 
2) Assign a distance value to all vertices in the input graph. 
Initialize all distance values as INFINITE. 
Assign distance value as 0 for the source vertex so that it is picked first. 
3) While sptSet doesn’t include all vertices 
….a) Pick a vertex u which is not there in sptSet and has a minimum distance value. 
….b) Include u to sptSet. 
….c) Update distance value of all adjacent vertices of u. 
To update the distance values, iterate through all adjacent vertices. 
For every adjacent vertex v, if the sum of distance value of u (from source) 
and weight of edge u-v, is less than the distance value of v, then update the distance value of v. 



We use a boolean array sptSet[] to represent the set of vertices included in SPT. If a value sptSet[v] is true, 
then vertex v is included in SPT, otherwise not. 
Array dist[] is used to store the shortest distance values of all vertices.
'''
# Python program for Dijkstra's single
# source shortest path algorithm. The program is
# for adjacency matrix representation of the graph

# Library for INT_MAX
import sys


class Graph():

    def __init__(self, vertices):
        self.V = vertices
        self.graph = [[0 for column in range(vertices)]
                      for row in range(vertices)]

    def printSolution(self, dist):
        print "Vertex \tDistance from Source"
        for node in range(self.V):
            print node, "\t", dist[node]

    # A utility function to find the vertex with
    # minimum distance value, from the set of vertices
    # not yet included in shortest path tree
    def minDistance(self, dist, sptSet):

        # Initialize minimum distance for next node
        min = sys.maxint

        # Search not nearest vertex not in the
        # shortest path tree
        for u in range(self.V):
            if dist[u] < min and sptSet[u] == False:
                min = dist[u]
                min_index = u

        return min_index

    # Function that implements Dijkstra's single source
    # shortest path algorithm for a graph represented
    # using adjacency matrix representation
    def dijkstra(self, src):

        dist = [sys.maxint] * self.V
        dist[src] = 0
        sptSet = [False] * self.V

        for cout in range(self.V):

            # Pick the minimum distance vertex from
            # the set of vertices not yet processed.
            # x is always equal to src in first iteration
            x = self.minDistance(dist, sptSet)

            # Put the minimum distance vertex in the
            # shortest path tree
            sptSet[x] = True

            # Update dist value of the adjacent vertices
            # of the picked vertex only if the current
            # distance is greater than new distance and
            # the vertex in not in the shortest path tree
            for y in range(self.V):
                if self.graph[x][y] > 0 and sptSet[y] == False and \
                        dist[y] > dist[x] + self.graph[x][y]:
                    dist[y] = dist[x] + self.graph[x][y]

        self.printSolution(dist)


# Driver program
g = Graph(9)
g.graph = [[0, 4, 0, 0, 0, 0, 0, 8, 0],
           [4, 0, 8, 0, 0, 0, 0, 11, 0],
           [0, 8, 0, 7, 0, 4, 0, 0, 2],
           [0, 0, 7, 0, 9, 14, 0, 0, 0],
           [0, 0, 0, 9, 0, 10, 0, 0, 0],
           [0, 0, 4, 14, 10, 0, 2, 0, 0],
           [0, 0, 0, 0, 0, 2, 0, 1, 6],
           [8, 11, 0, 0, 0, 0, 1, 0, 7],
           [0, 0, 2, 0, 0, 0, 6, 7, 0]
           ]

g.dijkstra(0)
