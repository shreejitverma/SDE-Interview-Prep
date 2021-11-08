/*
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
The following figure shows the above optimal substructure property in the all-pairs shortest path problem.
*/
class Solution
{
public:
    void shortest_distance(vector<vector<int> > &matrix)
    {
        // Code here
        int n = matrix.size();
        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    if (matrix[i][k] == -1 || matrix[k][j] == -1)
                        continue;
                    if (matrix[i][j] == -1)
                        matrix[i][j] = matrix[i][k] + matrix[k][j];
                    else
                        matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);
                }
            }
        }
    }
};
// C++ Program for Floyd Warshall Algorithm
#include <bits/stdc++.h>
using namespace std;

// Number of vertices in the graph
#define V 4

/* Define Infinite as a large enough
value.This value will be used for
vertices not connected to each other */
#define INF 99999

// A function to print the solution matrix
void printSolution(int dist[][V]);

// Solves the all-pairs shortest path
// problem using Floyd Warshall algorithm
void floydWarshall(int graph[][V])
{
    /* dist[][] will be the output matrix
    that will finally have the shortest
    distances between every pair of vertices */
    int dist[V][V], i, j, k;

    /* Initialize the solution matrix same
    as input graph matrix. Or we can say
    the initial values of shortest distances
    are based on shortest paths considering
    no intermediate vertex. */
    for (i = 0; i < V; i++)
        for (j = 0; j < V; j++)
            dist[i][j] = graph[i][j];

    /* Add all vertices one by one to
    the set of intermediate vertices.
    ---> Before start of an iteration,
    we have shortest distances between all
    pairs of vertices such that the
    shortest distances consider only the
    vertices in set {0, 1, 2, .. k-1} as
    intermediate vertices.
    ----> After the end of an iteration,
    vertex no. k is added to the set of
    intermediate vertices and the set becomes {0, 1, 2, ..
    k} */
    for (k = 0; k < V; k++)
    {
        // Pick all vertices as source one by one
        for (i = 0; i < V; i++)
        {
            // Pick all vertices as destination for the
            // above picked source
            for (j = 0; j < V; j++)
            {
                // If vertex k is on the shortest path from
                // i to j, then update the value of
                // dist[i][j]
                if (dist[i][j] > (dist[i][k] + dist[k][j]) && (dist[k][j] != INF && dist[i][k] != INF))
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    // Print the shortest distance matrix
    printSolution(dist);
}

/* A utility function to print solution */
void printSolution(int dist[][V])
{
    cout << "The following matrix shows the shortest "
            "distances"
            " between every pair of vertices \n";
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (dist[i][j] == INF)
                cout << "INF"
                     << "     ";
            else
                cout << dist[i][j] << "     ";
        }
        cout << endl;
    }
}

// Driver code
int main()
{
    /* Let us create the following weighted graph
            10
    (0)------->(3)
        |     /|\
    5 |     |
        |     | 1
    \|/     |
    (1)------->(2)
            3     */
    int graph[V][V] = {{0, 5, INF, 10},
                       {INF, 0, 3, INF},
                       {INF, INF, 0, 1},
                       {INF, INF, INF, 0}};

    // Print the solution
    floydWarshall(graph);
    return 0;
}