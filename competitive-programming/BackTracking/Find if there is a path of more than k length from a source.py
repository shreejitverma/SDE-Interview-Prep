'''https://www.geeksforgeeks.org/find-if-there-is-a-path-of-more-than-k-length-from-a-source/
Given a graph, a source vertex in the graph and a number k, find if there is a simple path (without any cycle) starting from given source and ending at any other vertex such that the distance from source to that vertex is atleast ‘k’ length.
 

Example:
Input  : Source s = 0, k = 58
Output : True
There exists a simple path 0 -> 7 -> 1
-> 2 -> 8 -> 6 -> 5 -> 3 -> 4
Which has a total distance of 60 km which
is more than 58.

Input  : Source s = 0, k = 62
Output : False

In the above graph, the longest simple
path has distance 61 (0 -> 7 -> 1-> 2
 -> 3 -> 4 -> 5-> 6 -> 8, so output 
should be false for any input greater 
than 61.
We strongly recommend you to minimize your browser and try this yourself first.
One important thing to note is, simply doing BFS or DFS and picking the longest edge at every step would not work. The reason is, a shorter edge can produce longer path due to higher weight edges connected through it.
The idea is to use Backtracking. We start from given source, explore all paths from current vertex. We keep track of current distance from source. If distance becomes more than k, we return true. If a path doesn’t produces more than k distance, we backtrack.
How do we make sure that the path is simple and we don’t loop in a cycle? The idea is to keep track of current path vertices in an array. Whenever we add a vertex to path, we check if it already exists or not in current path. If it exists, we ignore the edge.
Below is implementation of above idea. '''

# Program to find if there is a simple path with
# weight more than k

# This class represents a dipathted graph using
# adjacency list representation


class Graph:
    # Allocates memory for adjacency list
    def __init__(self, V):
        self.V = V
        self.adj = [[] for i in range(V)]

    # Returns true if graph has path more than k length
    def pathMoreThanK(self, src, k):
        # Create a path array with nothing included
        # in path
        path = [False]*self.V

        # Add source vertex to path
        path[src] = 1

        return self.pathMoreThanKUtil(src, k, path)

    # Prints shortest paths from src to all other vertices
    def pathMoreThanKUtil(self, src, k, path):
        # If k is 0 or negative, return true
        if (k <= 0):
            return True

        # Get all adjacent vertices of source vertex src and
        # recursively explore all paths from src.
        i = 0
        while i != len(self.adj[src]):
            # Get adjacent vertex and weight of edge
            v = self.adj[src][i][0]
            w = self.adj[src][i][1]
            i += 1

            # If vertex v is already there in path, then
            # there is a cycle (we ignore this edge)
            if (path[v] == True):
                continue

            # If weight of is more than k, return true
            if (w >= k):
                return True

            # Else add this vertex to path
            path[v] = True

            # If this adjacent can provide a path longer
            # than k, return true.
            if (self.pathMoreThanKUtil(v, k-w, path)):
                return True

            # Backtrack
            path[v] = False

        # If no adjacent could produce longer path, return
        # false
        return False

    # Utility function to an edge (u, v) of weight w
    def addEdge(self, u, v, w):
        self.adj[u].append([v, w])
        self.adj[v].append([u, w])


# Driver program to test methods of graph class
if __name__ == '__main__':

    # create the graph given in above fugure
    V = 9
    g = Graph(V)

    #  making above shown graph
    g.addEdge(0, 1, 4)
    g.addEdge(0, 7, 8)
    g.addEdge(1, 2, 8)
    g.addEdge(1, 7, 11)
    g.addEdge(2, 3, 7)
    g.addEdge(2, 8, 2)
    g.addEdge(2, 5, 4)
    g.addEdge(3, 4, 9)
    g.addEdge(3, 5, 14)
    g.addEdge(4, 5, 10)
    g.addEdge(5, 6, 2)
    g.addEdge(6, 7, 1)
    g.addEdge(6, 8, 6)
    g.addEdge(7, 8, 7)

    src = 0
    k = 62
    if g.pathMoreThanK(src, k):
        print("Yes")
    else:
        print("No")

    k = 60
    if g.pathMoreThanK(src, k):
        print("Yes")
    else:
        print("No")

'''
Output: 

No
Yes
Exercise: 
Modify the above solution to find weight of longest path from a given source.
Time Complexity: O(n!) 
Explanation: 
From the source node, we one-by-one visit all the paths and check if the total weight is greater than k for each path. So, the worst case will be when the number of possible paths is maximum. This is the case when every node is connected to every other node. 
Beginning from the source node we have n-1 adjacent nodes. The time needed for a path to connect any two nodes is 2. One for joining the source and the next adjacent vertex. One for breaking the connection between the source and the old adjacent vertex. 
After selecting a node out of n-1 adjacent nodes, we are left with n-2 adjacent nodes (as the source node is already included in the path) and so on at every step of selecting a node our problem reduces by 1 node.
We can write this in the form of a recurrence relation as: F(n) = n*(2+F(n-1)) 
This expands to: 2n + 2n*(n-1) + 2n*(n-1)*(n-2) + ……. + 2n(n-1)(n-2)(n-3)…..1 
As n times 2n(n-1)(n-2)(n-3)….1 is greater than the given expression so we can safely say time complexity is: n*2*n! 
Here in the question the first node is defined so time complexity becomes 
F(n-1) = 2(n-1)*(n-1)! = 2*n*(n-1)! – 2*1*(n-1)! = 2*n!-2*(n-1)! = O(n!)
This article is contributed by Shivam Gupta. The explanation for time complexity is contributed by Pranav Nambiar. Please write comments if you find anything incorrect, or you want to share more information about the topic discussed above
 '''
