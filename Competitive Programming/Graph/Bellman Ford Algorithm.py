'''https://www.geeksforgeeks.org/bellman-ford-algorithm-dp-23/
http://www.youtube.com/watch?v=Ttezuzs39nk
http://courses.csail.mit.edu/6.006/spring11/lectures/lec15.pdf
Algorithm 
Following are the detailed steps.
Input: Graph and a source vertex src 
Output: Shortest distance to all vertices from src. 
If there is a negative weight cycle, then shortest distances are not calculated, negative weight cycle is reported.
1) This step initializes distances from the source to all vertices as infinite and distance to the source itself as 0. 
Create an array dist[] of size |V| with all values as infinite except dist[src] where src is source vertex.
2) This step calculates shortest distances. 
Do following |V|-1 times where |V| is the number of vertices in given graph. 
…..a) Do following for each edge u-v 
………………If dist[v] > dist[u] + weight of edge uv, then update dist[v] 
………………….dist[v] = dist[u] + weight of edge uv
3) This step reports if there is a negative weight cycle in graph. Do following for each edge u-v 
……If dist[v] > dist[u] + weight of edge uv, then “Graph contains negative weight cycle” 
The idea of step 3 is, step 2 guarantees the shortest distances if the graph doesn’t contain a negative weight cycle. 
If we iterate through all edges one more time and get a shorter path for any vertex, then there is a negative weight cycle
How does this work? Like other Dynamic Programming Problems, the algorithm calculates shortest paths in a bottom-up manner. 
It first calculates the shortest distances which have at-most one edge in the path. 
Then, it calculates the shortest paths with at-most 2 edges, and so on. 
After the i-th iteration of the outer loop, the shortest paths with at most i edges are calculated. 
There can be maximum |V| – 1 edges in any simple path, that is why the outer loop runs |v| – 1 times. 
The idea is, assuming that there is no negative weight cycle, 
if we have calculated shortest paths with at most i edges, 
then an iteration over all edges guarantees to give shortest path with at-most (i+1) edges '''


# Python3 program for Bellman-Ford's single source
# shortest path algorithm.

# Class to represent a graph
class Graph:

    def __init__(self, vertices):
        self.V = vertices  # No. of vertices
        self.graph = []

    # function to add an edge to graph
    def addEdge(self, u, v, w):
        self.graph.append([u, v, w])

    # utility function used to print the solution
    def printArr(self, dist):
        print("Vertex Distance from Source")
        for i in range(self.V):
            print("{0}\t\t{1}".format(i, dist[i]))

    # The main function that finds shortest distances from src to
    # all other vertices using Bellman-Ford algorithm. The function
    # also detects negative weight cycle
    def BellmanFord(self, src):

        # Step 1: Initialize distances from src to all other vertices
        # as INFINITE
        dist = [float("Inf")] * self.V
        dist[src] = 0

        # Step 2: Relax all edges |V| - 1 times. A simple shortest
        # path from src to any other vertex can have at-most |V| - 1
        # edges
        for _ in range(self.V - 1):
            # Update dist value and parent index of the adjacent vertices of
            # the picked vertex. Consider only those vertices which are still in
            # queue
            for u, v, w in self.graph:
                if dist[u] != float("Inf") and dist[u] + w < dist[v]:
                    dist[v] = dist[u] + w

        # Step 3: check for negative-weight cycles. The above step
        # guarantees shortest distances if graph doesn't contain
        # negative weight cycle. If we get a shorter path, then there
        # is a cycle.

        for u, v, w in self.graph:
            if dist[u] != float("Inf") and dist[u] + w < dist[v]:
                print("Graph contains negative weight cycle")
                return

        # print all distance
        self.printArr(dist)


g = Graph(5)
g.addEdge(0, 1, -1)
g.addEdge(0, 2, 4)
g.addEdge(1, 2, 3)
g.addEdge(1, 3, 2)
g.addEdge(1, 4, 2)
g.addEdge(3, 2, 5)
g.addEdge(3, 1, 1)
g.addEdge(4, 3, -3)

# Print the solution
g.BellmanFord(0)
