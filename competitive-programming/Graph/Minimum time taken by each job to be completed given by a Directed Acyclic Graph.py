'''https://www.geeksforgeeks.org/minimum-time-taken-by-each-job-to-be-completed-given-by-a-directed-acyclic-graph/
Given a Directed Acyclic Graph having V vertices and E edges, where each edge {U, V} represents the Jobs U and V such that Job V can only be started only after completion of Job U. The task is to determine the minimum time taken by each job to be completed where each Job takes unit time to get completed.
Approach: The job can be started only if all the jobs that are prerequisites of the job that are done. Therefore, the idea is to use Topological Sort for the given network. Below are the steps:

Finish the jobs that are not dependent on any other job.
Create an array inDegree[] to store the count of the dependent node for each node in the given network.
Initialize a queue and push all the vertex whose inDegree[] is 0.
Initialize the timer to 1 and store the current queue size(say size) and do the following:
Pop the node from the queue until the size is 0 and update the finishing time of this node to the timer.
While popping the node(say node U) from the queue decrement the inDegree of every node connected to it.
If inDegree of any node is 0 in the above step then insert that node in the queue.
Increment the timer after all the above steps.
Print the finishing time of all the nodes after we traverse every node in the above step.
Below is the implementation of the above approach::

Output: 
1 1 2 2 2 3 4 5 2 6
 

Time Complexity: O(V+E), where V is the number of nodes and E is the number of edges. 
Auxiliary Space: O(V)
'''

# Python3 program for the above approach
from collections import defaultdict

# Class to represent a graph


class Graph:

    def __init__(self, vertices, edges):

        # Dictionary containing adjacency List
        self.graph = defaultdict(list)

        # No. of vertices
        self.n = vertices

        # No. of edges
        self.m = edges

    # Function to add an edge to graph
    def addEdge(self, u, v):
        self.graph[u].append(v)

    # Function to find the minimum time
    # needed by each node to get the task
    def printOrder(self, n, m):

        # Create a vector to store indegrees of all
        # vertices. Initialize all indegrees as 0.
        indegree = [0] * (self.n + 1)

        # Traverse adjacency lists to fill indegrees
        # of vertices. This step takes O(V + E) time
        for i in self.graph:
            for j in self.graph[i]:
                indegree[j] += 1

        # Array to store the time in which
        # the job i can be done
        job = [0] * (self.n + 1)

        # Create an queue and enqueue all
        # vertices with indegree 0
        q = []

        # Update the time of the jobs
        # who don't require any job to
        # be completed before this job
        for i in range(1, self.n + 1):
            if indegree[i] == 0:
                q.append(i)
                job[i] = 1

        # Iterate until queue is empty
        while q:

            # Get front element of queue
            cur = q.pop(0)

            for adj in self.graph[cur]:

                # Decrease in-degree of
                # the current node
                indegree[adj] -= 1

                # Push its adjacent elements
                if (indegree[adj] == 0):
                    job[adj] = 1 + job[cur]
                    q.append(adj)

        # Print the time to complete
        # the job
        for i in range(1, n + 1):
            print(job[i], end=" ")

        print()

# Driver Code


# Given Nodes N and edges M
n = 10
m = 13

g = Graph(n, m)

# Given Directed Edges of graph
g.addEdge(1, 3)
g.addEdge(1, 4)
g.addEdge(1, 5)
g.addEdge(2, 3)
g.addEdge(2, 8)
g.addEdge(2, 9)
g.addEdge(3, 6)
g.addEdge(4, 6)
g.addEdge(4, 8)
g.addEdge(5, 8)
g.addEdge(6, 7)
g.addEdge(7, 8)
g.addEdge(8, 10)

# Function Call
g.printOrder(n, m)
