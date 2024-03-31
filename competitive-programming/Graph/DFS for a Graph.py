'''https://www.geeksforgeeks.org/depth-first-search-or-dfs-for-a-graph/

Depth First Traversal (or Search) for a graph is similar to Depth First Traversal of a tree. 
The only catch here is, unlike trees, graphs may contain cycles, a node may be visited twice. 
To avoid processing a node more than once, use a boolean visited array. 

Prerequisites: See this post for all applications of Depth First Traversal.
Following are implementations of simple Depth First Traversal. 
The C++ implementation uses an adjacency list representation of graphs. 
STL‘s list container is used to store lists of adjacent nodes.
Solution:

Approach: Depth-first search is an algorithm for traversing or searching tree or graph data structures. 
The algorithm starts at the root node (selecting some arbitrary node as the root node in the case of a graph) 
and explores as far as possible along each branch before backtracking. 
So the basic idea is to start from the root or any arbitrary node and mark the node 
and move to the adjacent unmarked node and continue this loop until there is no unmarked adjacent node. 
Then backtrack and check for other unmarked nodes and traverse them. 
Finally, print the nodes in the path.

Algorithm: 
Create a recursive function that takes the index of the node and a visited array.
Mark the current node as visited and print the node.
Traverse all the adjacent and unmarked nodes and call the recursive function with the index of the adjacent node.
Implementation:'''

# Python3 program to print DFS traversal
# from a given given graph
from collections import defaultdict

# This class represents a directed graph using
# adjacency list representation


class Graph:

    # Constructor
    def __init__(self):

        # default dictionary to store graph
        self.graph = defaultdict(list)

    # function to add an edge to graph
    def addEdge(self, u, v):
        self.graph[u].append(v)

    # A function used by DFS
    def DFSUtil(self, v, visited):

        # Mark the current node as visited
        # and print it
        visited.add(v)
        print(v, end=' ')

        # Recur for all the vertices
        # adjacent to this vertex
        for neighbour in self.graph[v]:
            if neighbour not in visited:
                self.DFSUtil(neighbour, visited)

    # The function to do DFS traversal. It uses
    # recursive DFSUtil()
    def DFS(self, v):

        # Create a set to store visited vertices
        visited = set()

        # Call the recursive helper function
        # to print DFS traversal
        self.DFSUtil(v, visited)

# Driver code


# Create a graph given
# in the above diagram
g = Graph()
g.addEdge(0, 1)
g.addEdge(0, 2)
g.addEdge(1, 2)
g.addEdge(2, 0)
g.addEdge(2, 3)
g.addEdge(3, 3)

print("Following is DFS from (starting from vertex 2)")
g.DFS(2)


'''
Handling Disconnected Graph 

Solution: This will happen by handling a corner case. 
The above code traverses only the vertices reachable from a given source vertex. 
All the vertices may not be reachable from a given vertex, as in a Disconnected graph. 
To do a complete DFS traversal of such graphs, run DFS from all unvisited nodes after a DFS. 
The recursive function remains the same.
Algorithm: 
Create a recursive function that takes the index of the node and a visited array.
Mark the current node as visited and print the node.
Traverse all the adjacent and unmarked nodes and call the recursive function with the index of the adjacent node.
Run a loop from 0 to the number of vertices and check if the node is unvisited in the previous DFS, 
call the recursive function with the current node.'''

'''Python program to print DFS traversal for complete graph'''

# this class represents a directed graph using adjacency list representation


class Graph:
    # Constructor
    def __init__(self):
        # default dictionary to store graph
        self.graph = defaultdict(list)

    # Function to add an edge to graph
    def addEdge(self, u, v):
        self.graph[u].append(v)
    # A function used by DFS

    def DFSUtil(self, v, visited):
        # Mark the current node as visited and print it
        visited.add(v)
        print(v)

        # recur for all the vertices adjacent to this vertex
        for neighbour in self.graph[v]:
            if neighbour not in visited:
                self.DFSUtil(neighbour, visited)
        # The function to do DFS traversal. It uses recursive DFSUtil

    def DFS(self):
        # create a set to store all visited vertices
        visited = set()
        # call the recursive helper function to print DFS traversal starting from all
        # vertices one by one
        for vertex in self.graph:
            if vertex not in visited:
                self.DFSUtil(vertex, visited)
# Driver code
# create a graph given in the above diagram


g = Graph()
g.addEdge(0, 1)
g.addEdge(0, 2)
g.addEdge(1, 2)
g.addEdge(2, 0)
g.addEdge(2, 3)
g.addEdge(3, 3)
g.DFS()
