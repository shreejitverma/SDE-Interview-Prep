'''https://www.geeksforgeeks.org/detect-negative-cycle-graph-bellman-ford/We are given a directed graph. We need to compute whether the graph has a negative cycle or not. A negative cycle is one in which the overall sum of the cycle becomes negative.

Negative weights are found in various applications of graphs. For example, instead of paying cost for a path, we may get some advantage if we follow the path.



Examples: 

Input : 4 4
        0 1 1
        1 2 -1
        2 3 -1
        3 0 -1

Output : Yes
The graph contains a negative cycle.

The idea is to use Bellman-Ford Algorithm. 

Below is an algorithm to find if there is a negative weight cycle reachable from the given source.
1) Initialize distances from the source to all vertices as infinite and distance to the source itself as 0. Create an array dist[] of size |V| with all values as infinite except dist[src] where src is the source vertex.
2) This step calculates the shortest distances. Do the following |V|-1 times where |V| is the number of vertices in the given graph. 
     a) Do the following for each edge u-v.
     b) If dist[v] > dist[u] + weight of edge uv, then update dist[v]. 
     c) dist[v] = dist[u] + weight of edge uv.
3) This step reports if there is a negative weight cycle in the graph. Do the following for each edge u-v 
     a) If dist[v] > dist[u] + weight of edge uv, then the “Graph has a negative weight cycle” 

The idea of step 3 is, step 2 guarantees the shortest distances if the graph doesn’t contain a negative weight cycle. If we iterate through all edges one more time and get a shorter path for any vertex, then there is a negative weight cycle.'''

# A Python3 program to check if a graph contains negative
# weight cycle using Bellman-Ford algorithm. This program
# works only if all vertices are reachable from a source
# vertex 0.

# a structure to represent a weighted edge in graph


class Edge:

    def __init__(self):
        self.src = 0
        self.dest = 0
        self.weight = 0

# a structure to represent a connected, directed and
# weighted graph


class Graph:

    def __init__(self):

        # V. Number of vertices, E. Number of edges
        self.V = 0
        self.E = 0

        # graph is represented as an array of edges.
        self.edge = None

# Creates a graph with V vertices and E edges


def createGraph(V, E):

    graph = Graph()
    graph.V = V
    graph.E = E
    graph.edge = [Edge() for i in range(graph.E)]
    return graph

# The main function that finds shortest distances
# from src to all other vertices using Bellman-
# Ford algorithm.  The function also detects
# negative weight cycle


def isNegCycleBellmanFord(graph, src):

    V = graph.V
    E = graph.E
    dist = [1000000 for i in range(V)]
    dist[src] = 0

    # Step 2: Relax all edges |V| - 1 times.
    # A simple shortest path from src to any
    # other vertex can have at-most |V| - 1
    # edges
    for i in range(1, V):
        for j in range(E):

            u = graph.edge[j].src
            v = graph.edge[j].dest
            weight = graph.edge[j].weight
            if (dist[u] != 1000000 and dist[u] + weight < dist[v]):
                dist[v] = dist[u] + weight

    # Step 3: check for negative-weight cycles.
    # The above step guarantees shortest distances
    # if graph doesn't contain negative weight cycle.
    # If we get a shorter path, then there
    # is a cycle.
    for i in range(E):

        u = graph.edge[i].src
        v = graph.edge[i].dest
        weight = graph.edge[i].weight
        if (dist[u] != 1000000 and dist[u] + weight < dist[v]):
            return True

    return False


# Driver program to test above functions
if __name__ == '__main__':

    # Let us create the graph given in above example
    V = 5  # Number of vertices in graph
    E = 8  # Number of edges in graph
    graph = createGraph(V, E)

    # add edge 0-1 (or A-B in above figure)
    graph.edge[0].src = 0
    graph.edge[0].dest = 1
    graph.edge[0].weight = -1

    # add edge 0-2 (or A-C in above figure)
    graph.edge[1].src = 0
    graph.edge[1].dest = 2
    graph.edge[1].weight = 4

    # add edge 1-2 (or B-C in above figure)
    graph.edge[2].src = 1
    graph.edge[2].dest = 2
    graph.edge[2].weight = 3

    # add edge 1-3 (or B-D in above figure)
    graph.edge[3].src = 1
    graph.edge[3].dest = 3
    graph.edge[3].weight = 2

    # add edge 1-4 (or A-E in above figure)
    graph.edge[4].src = 1
    graph.edge[4].dest = 4
    graph.edge[4].weight = 2

    # add edge 3-2 (or D-C in above figure)
    graph.edge[5].src = 3
    graph.edge[5].dest = 2
    graph.edge[5].weight = 5

    # add edge 3-1 (or D-B in above figure)
    graph.edge[6].src = 3
    graph.edge[6].dest = 1
    graph.edge[6].weight = 1

    # add edge 4-3 (or E-D in above figure)
    graph.edge[7].src = 4
    graph.edge[7].dest = 3
    graph.edge[7].weight = -3

    if (isNegCycleBellmanFord(graph, 0)):
        print("Yes")
    else:
        print("No")

'''How does it work? 
As discussed, the Bellman-Ford algorithm, for a given source, first calculates the shortest distances which have at most one edge in the path. Then, it calculates the shortest paths with at-most 2 edges, and so on. After the i-th iteration of the outer loop, the shortest paths with at most i edges are calculated. There can be a maximum |V| – 1 edge on any simple path, that is why the outer loop runs |v| – 1 time. If there is a negative weight cycle, then one more iteration would give a short route.
 

Detect a negative cycle in a Graph using Bellman Ford Algorithm

How to handle a disconnected graph (If the cycle is not reachable from the source)? 
The above algorithm and program might not work if the given graph is disconnected. It works when all vertices are reachable from source vertex 0.
To handle disconnected graphs, we can repeat the process for vertices for which distance is infinite.'''

# A Python3 program for Bellman-Ford's single source
# shortest path algorithm.

# The main function that finds shortest distances
# from src to all other vertices using Bellman-
# Ford algorithm. The function also detects
# negative weight cycle


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
