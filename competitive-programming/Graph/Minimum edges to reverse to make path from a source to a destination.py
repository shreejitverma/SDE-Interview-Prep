'''https://www.geeksforgeeks.org/minimum-edges-reverse-make-path-source-destination/

Given a directed graph and a source node and destination node, we need to find how many edges we need to reverse in order to make at least 1 path from source node to destination node.

Examples:  

In case you wish to attend live classes with experts, please refer DSA Live Classes for Working Professionals and Competitive Programming Live for Students.




In above graph there were two paths from node 0 to node 6,
0 -> 1 -> 2 -> 3 -> 6
0 -> 1 -> 5 -> 4 -> 6
But for first path only two edges need to be reversed, so answer will be 2 only.

This problem can be solved assuming a different version of the given graph. In this version we make a reverse edge corresponding to every edge and we assign that a weight 1 and assign a weight 0 to original edge. After this modification above graph looks something like below, 
 



Now we can see that we have modified the graph in such a way that, if we move towards original edge, no cost is incurred, but if we move toward reverse edge 1 cost is added. So if we apply Dijkstra’s shortest path on this modified graph from given source, then that will give us minimum cost to reach from source to destination i.e. minimum edge reversal from source to destination. 

Below is the code based on above concept. '''
# Python3 Program to find minimum edge reversal to get
# atleast one path from source to destination

# method adds a directed edge from u to v with weight w


def addEdge(u, v, w):
    global adj
    adj[u].append((v, w))

# Prints shortest paths from src to all other vertices


def shortestPath(src):

    # Create a set to store vertices that are being
    # prerocessed
    setds = {}

    # Create a vector for distances and initialize all
    # distances as infinite (INF)
    dist = [10**18 for i in range(V)]

    # Insert source itself in Set and initialize its
    global adj
    setds[(0, src)] = 1
    dist[src] = 0

    # /* Looping till all shortest distance are finalized

    # then setds will become empty */
    while (len(setds) > 0):

        # The first vertex in Set is the minimum distance
        # vertex, extract it from set.
        tmp = list(setds.keys())[0]
        del setds[tmp]

        # vertex label is stored in second of pair (it
        # has to be done this way to keep the vertices
        # sorted distance (distance must be first item
        # in pair)
        u = tmp[1]

        # 'i' is used to get all adjacent vertices of a vertex
        # list< pair<int, int> >::iterator i;
        for i in adj[u]:

            # Get vertex label and weight of current adjacent
            # of u.
            v = i[0]
            weight = i[1]

            # If there is shorter path to v through u.
            if (dist[v] > dist[u] + weight):

                # /* If distance of v is not INF then it must be in
                #     our set, so removing it and inserting again
                #     with updated less distance.
                #     Note : We extract only those vertices from Set
                #     for which distance is finalized. So for them,
                #     we would never reach here. */
                if (dist[v] != 10**18):
                    del setds[(dist[v], v)]

                # Updating distance of v
                dist[v] = dist[u] + weight
                setds[(dist[v], v)] = 1

    return dist

# /* method adds reverse edge of each original edge
# in the graph. It gives reverse edge a weight = 1
# and all original edges a weight of 0. Now, the
# length of the shortest path will give us the answer.
# If shortest path is p: it means we used p reverse
# edges in the shortest path. */


def modelGraphWithEdgeWeight(edge, E, V):
    global adj
    for i in range(E):

        # original edge : weight 0
        addEdge(edge[i][0], edge[i][1], 0)

        # reverse edge : weight 1
        addEdge(edge[i][1], edge[i][0], 1)

# Method returns minimum number of edges to be
# reversed to reach from src to dest


def getMinEdgeReversal(edge, E, V, src, dest):

    # get modified graph with edge weight
    modelGraphWithEdgeWeight(edge, E, V)

    # get shortes path vector
    dist = shortestPath(src)

    # If distance of destination is still INF,
    # not possible
    if (dist[dest] == 10**18):
        return -1
    else:
        return dist[dest]


# Driver code
if __name__ == '__main__':
    V = 7
    edge = [[0, 1], [2, 1], [2, 3], [5, 1], [4, 5], [6, 4], [6, 3]]
    E, adj = len(edge), [[] for i in range(V + 1)]
    minEdgeToReverse = getMinEdgeReversal(edge, E, V, 0, 6)
    if (minEdgeToReverse != -1):
        print(minEdgeToReverse)
    else:
        print("Not possible")
