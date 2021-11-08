/*
https://practice.geeksforgeeks.org/problems/negative-weight-cycle3504/1
https://www.geeksforgeeks.org/detect-negative-cycle-graph-bellman-ford/
Negative weight cycle 
Medium Accuracy: 50.77% Submissions: 14632 Points: 4
Given a weighted directed graph with n nodes and m edges. Nodes are labeled from 0 to n-1, the task is to check if it contains a negative weight cycle or not.
Note: edges[i] is defined as u, v and weight.
 

Example 1:

Input: n = 3, edges = {{0,1,-1},{1,2,-2},
{2,0,-3}}
Output: 1
Explanation: The graph contains negative weight
cycle as 0->1->2->0 with weight -1,-2,-3,-1.
Example 2:

Input: n = 3, edges = {{0,1,-1},{1,2,-2},
{2,0,3}}
Output: 0
Explanation: The graph does not contain any
negative weight cycle.
 

Your Task:
You don't need to read or print anyhting. Your task is to complete the function isNegativeWeightCycle() which takes n and edges as input paramater and returns 1 if graph contains negative weight cycle otherwise returns 0.
 

Expected Time Complexity: O(n*m)
Expected Space Compelxity: O(n)
 

Constraints:
1 <= n <= 100
1 <= m <= n*(n-1), where m is the total number of Edges in the directed graph.

*/
class Solution
{

public:
    int isNegativeWeightCycle(int n, vector<vector<int> > edges)
    {
        // Code here
        vector<int> dis(n, INT_MAX);
        dis[0] = 0;
        for (int i = 1; i <= n - 1; i++)
        {
            for (auto &it : edges)
            {
                if (dis[it[0]] == INT_MAX)
                    continue;
                if (dis[it[0]] + it[2] < dis[it[1]])
                {
                    dis[it[1]] = dis[it[0]] + it[2];
                }
            }
        }
        for (auto &it : edges)
        {
            if (dis[it[0]] + it[2] < dis[it[1]])
            {
                return 1;
            }
        }
        return 0;
    }
};

// A C++ program for Bellman-Ford's single source
// shortest path algorithm.
#include <bits/stdc++.h>
using namespace std;

// a structure to represent a weighted edge in graph
struct Edge
{
    int src, dest, weight;
};

// a structure to represent a connected, directed and
// weighted graph
struct Graph
{
    // V-> Number of vertices, E-> Number of edges
    int V, E;

    // graph is represented as an array of edges.
    struct Edge *edge;
};

// Creates a graph with V vertices and E edges
struct Graph *createGraph(int V, int E)
{
    struct Graph *graph = new Graph;
    graph->V = V;
    graph->E = E;
    graph->edge = new Edge[graph->E];
    return graph;
}

// The main function that finds shortest distances
// from src to all other vertices using Bellman-
// Ford algorithm. The function also detects
// negative weight cycle
bool isNegCycleBellmanFord(struct Graph *graph,
                           int src, int dist[])
{
    int V = graph->V;
    int E = graph->E;

    // Step 1: Initialize distances from src
    // to all other vertices as INFINITE
    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX;
    dist[src] = 0;

    // Step 2: Relax all edges |V| - 1 times.
    // A simple shortest path from src to any
    // other vertex can have at-most |V| - 1
    // edges
    for (int i = 1; i <= V - 1; i++)
    {
        for (int j = 0; j < E; j++)
        {
            int u = graph->edge[j].src;
            int v = graph->edge[j].dest;
            int weight = graph->edge[j].weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
                dist[v] = dist[u] + weight;
        }
    }

    // Step 3: check for negative-weight cycles.
    // The above step guarantees shortest distances
    // if graph doesn't contain negative weight cycle.
    // If we get a shorter path, then there
    // is a cycle.
    for (int i = 0; i < E; i++)
    {
        int u = graph->edge[i].src;
        int v = graph->edge[i].dest;
        int weight = graph->edge[i].weight;
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
            return true;
    }

    return false;
}

// Returns true if given graph has negative weight
// cycle.
bool isNegCycleDisconnected(struct Graph *graph)
{

    int V = graph->V;
    // To keep track of visited vertices to avoid
    // recomputations.
    bool visited[V];
    memset(visited, 0, sizeof(visited));

    // This array is filled by Bellman-Ford
    int dist[V];

    // Call Bellman-Ford for all those vertices
    // that are not visited
    for (int i = 0; i < V; i++)
    {
        if (visited[i] == false)
        {
            // If cycle found
            if (isNegCycleBellmanFord(graph, i, dist))
                return true;

            // Mark all vertices that are visited
            // in above call.
            for (int i = 0; i < V; i++)
                if (dist[i] != INT_MAX)
                    visited[i] = true;
        }
    }

    return false;
}

// Driver program to test above functions
int main()
{
    /* Let us create the graph given in above example */
    int V = 5; // Number of vertices in graph
    int E = 8; // Number of edges in graph
    struct Graph *graph = createGraph(V, E);

    // add edge 0-1 (or A-B in above figure)
    graph->edge[0].src = 0;
    graph->edge[0].dest = 1;
    graph->edge[0].weight = -1;

    // add edge 0-2 (or A-C in above figure)
    graph->edge[1].src = 0;
    graph->edge[1].dest = 2;
    graph->edge[1].weight = 4;

    // add edge 1-2 (or B-C in above figure)
    graph->edge[2].src = 1;
    graph->edge[2].dest = 2;
    graph->edge[2].weight = 3;

    // add edge 1-3 (or B-D in above figure)
    graph->edge[3].src = 1;
    graph->edge[3].dest = 3;
    graph->edge[3].weight = 2;

    // add edge 1-4 (or A-E in above figure)
    graph->edge[4].src = 1;
    graph->edge[4].dest = 4;
    graph->edge[4].weight = 2;

    // add edge 3-2 (or D-C in above figure)
    graph->edge[5].src = 3;
    graph->edge[5].dest = 2;
    graph->edge[5].weight = 5;

    // add edge 3-1 (or D-B in above figure)
    graph->edge[6].src = 3;
    graph->edge[6].dest = 1;
    graph->edge[6].weight = 1;

    // add edge 4-3 (or E-D in above figure)
    graph->edge[7].src = 4;
    graph->edge[7].dest = 3;
    graph->edge[7].weight = -3;

    if (isNegCycleDisconnected(graph))
        cout << "Yes";
    else
        cout << "No";

    return 0;
}