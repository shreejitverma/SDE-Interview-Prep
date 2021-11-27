'''https://practice.geeksforgeeks.org/problems/m-coloring-problem-1587115620/1
M-Coloring Problem 
Medium Accuracy: 47.46% Submissions: 21791 Points: 4
Given an undirected graph and an integer M. The task is to determine if the graph can be colored with at most M colors such that no two adjacent vertices of the graph are colored with the same color. Here coloring of a graph means the assignment of colors to all vertices. Print 1 if it is possible to colour vertices and 0 otherwise.

Example 1:

Input:
N = 4
M = 3
E = 5
Edges[] = {(0,1),(1,2),(2,3),(3,0),(0,2)}
Output: 1
Explanation: It is possible to colour the
given graph using 3 colours.
Example 2:

Input:
N = 3
M = 2
E = 3
Edges[] = {(0,1),(1,2),(0,2)}
Output: 0
Your Task:
Your task is to complete the function graphColoring() which takes the 2d-array graph[], the number of colours and the number of nodes as inputs and returns true if answer exists otherwise false. 1 is printed if the returned value is true, 0 otherwise. The printing is done by the driver's code.
Note: In Example there are Edges not the graph.Graph will be like, if there is an edge between vertex X and vertex Y graph[] will contain 1 at graph[X-1][Y-1], else 0. In 2d-array graph[ ], nodes are 0-based indexed, i.e. from 0 to N-1.Function will be contain 2-D graph not the edges.

Expected Time Complexity: O(MN).
Expected Auxiliary Space: O(N).

Constraints:
1 ≤ N ≤ 20
1 ≤ E ≤ (N*(N-1))/2
1 ≤ M ≤ N'''


def isSafe(graph, v, colour, c, V):

    for i in range(V):
        # checking if the connected nodes to v have same colour as c.
        if (graph[v][i] == 1 and colour[i] == c):
            return False

    # returning true if no connected node has same colour.
    return True


def graphColourUtil(graph, m, colour, v, V):

    # if all vertices have been assigned colour then we return true.
    if v == V:
        return True

    for c in range(1, m+1):

        # checking if this colour can be given to a particular node.
        if (isSafe(graph, v, colour, c, V) == True):

            # assigning colour to the node.
            colour[v] = c

            # calling function recursively and checking if other nodes
            # can be assigned other colours.
            if (graphColourUtil(graph, m, colour, v+1, V) == True):
                # returning true if the current allocation was successful.
                return True
            # if not true, we backtrack and remove the colour for that node.
            colour[v] = 0

    # if no colour can be assigned, we return false.
    return False


# Function to determine if graph can be coloured with at most M colours such
# that no two adjacent vertices of graph are coloured with same colour.
def graphColoring(graph, k, V):
    colour = [0] * V

    # checking if colours can be assigned.
    if (graphColourUtil(graph, k, colour, 0, V) == False):
        return False
    return True


# Function to determine if graph can be coloured with at most M colours such
# that no two adjacent vertices of graph are coloured with same colour.
# def graphColoring(graph, k, V):
#     n=len(graph)
#     color=[0]*V


#     def solve(node,graph,color,n,m):
#         if node==n:
#             return True
#         for i in range(1,m+1):
#             if isSafe(node,graph,color,n,i):
#                 color[node]=1
#                 if solve(node+1,graph,color,n,m):
#                     return True
#                 color[node]=0
#     def isSafe(node,graph,color,n,col):
#         for k in range(n):
#             if k!=node and (graph[k][node]==1 or graph[node][k]==1) and color[k]==col:
#                 return False
#         return True
#     if solve(0,graph,color,V,k):
#         return True
#     return False
#     #your code here

# {
#  Driver Code Starts
# Initial Template for Python 3
if __name__ == "__main__":
    t = int(input())
    while(t > 0):
        V = int(input())
        k = int(input())
        m = int(input())
        list = [int(x) for x in input().strip().split()]
        graph = [[0 for i in range(V)] for j in range(V)]
        cnt = 0
        for i in range(m):
            graph[list[cnt]-1][list[cnt+1]-1] = 1
            graph[list[cnt+1]-1][list[cnt]-1] = 1
            cnt += 2
        if(graphColoring(graph, k, V) == True):
            print(1)
        else:
            print(0)

        t = t-1

# } Driver Code Ends
