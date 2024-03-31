/*
https://practice.geeksforgeeks.org/problems/strongly-connected-components-kosarajus-algo/1
Strongly Connected Components (Kosaraju's Algo) 
Medium Accuracy: 49.73% Submissions: 33164 Points: 4
Given a Directed Graph with V vertices (Numbered from 0 to V-1) and E edges, Find the number of strongly connected components in the graph.
 

Example 1:

Input:

Output:
3
Explanation:

We can clearly see that there are 3 Strongly
Connected Components in the Graph
Example 2:

Input:

Output:
1
Explanation:
All of the nodes are connected to each other.
So, there's only one SCC.
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function kosaraju() which takes the number of vertices V and adjacency list of the graph as inputs and returns an integer denoting the number of strongly connected components in the given graph.
 

Expected Time Complexity: O(V+E).
Expected Auxiliary Space: O(V).
 

Constraints:
1 ≤ V ≤ 5000
0 ≤ E ≤ (V*(V-1))
0 ≤ u, v ≤ N-1
Sum of E over all testcases will not exceed 25*106
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends

class Solution
{
public:
    //Function to find number of strongly connected components in the graph.
    vector<int> order;

    void dfs(int src, vector<int> &vis, vector<int> g[])
    {
        vis[src] = 1;
        for (auto x : g[src])
        {
            if (!vis[x])
            {
                dfs(x, vis, g);
            }
        }
        order.push_back(src);
    }

    void rdfs(int src, vector<int> &vis1, vector<int> rev[])
    {
        vis1[src] = 1;
        for (auto x : rev[src])
        {
            if (!vis1[x])
            {
                rdfs(x, vis1, rev);
            }
        }
    }

    int kosaraju(int V, vector<int> adj[])
    {
        //code here
        order.clear();
        vector<int> rev[V];
        for (int y = 0; y < V; y++)
        {
            for (auto x : adj[y])
            {
                rev[x].push_back(y);
            }
        }
        vector<int> vis(V, 0);
        for (int i = 0; i < V; i++)
        {
            if (!vis[i])
            {
                dfs(i, vis, adj);
            }
        }

        vector<int> vis1(V, 0);
        int count = 0;
        for (int i = V - 1; i >= 0; i--)
        {
            if (!vis1[order[i]])
            {
                rdfs(order[i], vis1, rev);
                count++;
            }
        }
        return count;
    }
};

// { Driver Code Starts.

int main()
{

    int t;
    cin >> t;
    while (t--)
    {
        int V, E;
        cin >> V >> E;

        vector<int> adj[V];

        for (int i = 0; i < E; i++)
        {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
        }

        Solution obj;
        cout << obj.kosaraju(V, adj) << "\n";
    }

    return 0;
}

// } Driver Code Ends