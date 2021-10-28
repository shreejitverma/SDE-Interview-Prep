/*
https://leetcode.com/problems/clone-graph/
133. Clone Graph
Medium

4012

1973

Add to List

Share
Given a reference of a node in a connected undirected graph.

Return a deep copy (clone) of the graph.

Each node in the graph contains a value (int) and a list (List[Node]) of its neighbors.

class Node {
    public int val;
    public List<Node> neighbors;
}
 

Test case format:

For simplicity, each node's value is the same as the node's index (1-indexed). For example, the first node with val == 1, the second node with val == 2, and so on. The graph is represented in the test case using an adjacency list.

An adjacency list is a collection of unordered lists used to represent a finite graph. Each list describes the set of neighbors of a node in the graph.

The given node will always be the first node with val = 1. You must return the copy of the given node as a reference to the cloned graph.

 

Example 1:


Input: adjList = [[2,4],[1,3],[2,4],[1,3]]
Output: [[2,4],[1,3],[2,4],[1,3]]
Explanation: There are 4 nodes in the graph.
1st node (val = 1)'s neighbors are 2nd node (val = 2) and 4th node (val = 4).
2nd node (val = 2)'s neighbors are 1st node (val = 1) and 3rd node (val = 3).
3rd node (val = 3)'s neighbors are 2nd node (val = 2) and 4th node (val = 4).
4th node (val = 4)'s neighbors are 1st node (val = 1) and 3rd node (val = 3).
Example 2:


Input: adjList = [[]]
Output: [[]]
Explanation: Note that the input contains one empty list. The graph consists of only one node with val = 1 and it does not have any neighbors.
Example 3:

Input: adjList = []
Output: []
Explanation: This an empty graph, it does not have any nodes.
Example 4:


Input: adjList = [[2],[1]]
Output: [[2],[1]]
 

Constraints:

The number of nodes in the graph is in the range [0, 100].
1 <= Node.val <= 100
Node.val is unique for each node.
There are no repeated edges and no self-loops in the graph.
The Graph is connected and all nodes can be visited starting from the given node.
*/

//BFS
//Runtime: 12 ms, faster than 64.56% of C++ online submissions for Clone Graph.
//Memory Usage: 8.9 MB, less than 98.02% of C++ online submissions for Clone Graph.
/*
// Definition for a Node.
class Node {
public:
    int val;
    vector<Node*> neighbors;
    
    Node() {
        val = 0;
        neighbors = vector<Node*>();
    }
    
    Node(int _val) {
        val = _val;
        neighbors = vector<Node*>();
    }
    
    Node(int _val, vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
};
*/

class Solution
{
public:
    Node *cloneGraph(Node *node)
    {
        if (!node)
            return node;

        queue<Node *> q;
        unordered_map<int, vector<int> > adjList;

        q.push(node);
        //mark "node" as existing in graph
        //also mark it as visited!
        adjList[node->val] = {};

        while (!q.empty())
        {
            Node *cur = q.front();
            q.pop();
            // cout << "visit " << cur->val << endl;

            for (Node *nei : cur->neighbors)
            {
                //here we actually build the graph
                adjList[cur->val].push_back(nei->val);
                if (adjList.find(nei->val) != adjList.end())
                    continue;
                //mark nei as visited!
                adjList[nei->val] = {};
                q.push(nei);
            }
        }

        int count = adjList.size();

        // cout << "count: " << count << endl;

        //dummy node
        vector<Node *> newnodes = {new Node(0)};

        for (int i = 1; i <= count; ++i)
        {
            newnodes.push_back(new Node(i));
        }

        for (int i = 1; i <= count; ++i)
        {
            for (const int &nei : adjList[i])
            {
                // cout << i << " -> " << nei << endl;
                newnodes[i]->neighbors.push_back(newnodes[nei]);
            }
        }

        return newnodes[1];
    }
};

//BFS, one pass
//https://leetcode.com/problems/clone-graph/discuss/42313/C%2B%2B-BFSDFS
//Runtime: 8 ms, faster than 96.13% of C++ online submissions for Clone Graph.
//Memory Usage: 8.5 MB, less than 98.02% of C++ online submissions for Clone Graph.
class Solution
{
public:
    Node *cloneGraph(Node *node)
    {
        if (!node)
            return node;

        unordered_map<Node *, Node *> old2new;

        old2new[node] = new Node(node->val);

        queue<Node *> q;

        //visit the old graph
        q.push(node);

        while (!q.empty())
        {
            Node *cur = q.front();
            q.pop();

            for (Node *nei : cur->neighbors)
            {
                if (old2new.find(nei) != old2new.end())
                {
                    //create the edge
                    old2new[cur]->neighbors.push_back(old2new[nei]);
                    continue;
                }
                old2new[nei] = new Node(nei->val);
                //visit the old graph
                q.push(nei);
                //create the edge
                old2new[cur]->neighbors.push_back(old2new[nei]);
            }
        }

        return old2new[node];
    }
};

//DFS
//https://leetcode.com/problems/clone-graph/discuss/42313/C%2B%2B-BFSDFS
//Runtime: 8 ms, faster than 96.13% of C++ online submissions for Clone Graph.
//Memory Usage: 9 MB, less than 98.02% of C++ online submissions for Clone Graph.
class Solution
{
public:
    unordered_map<Node *, Node *> old2new;

    Node *cloneGraph(Node *node)
    {
        if (!node)
            return node;

        //this node is already visited
        if (old2new.find(node) != old2new.end())
            return old2new[node];

        old2new[node] = new Node(node->val);

        for (Node *nei : node->neighbors)
        {
            //build new node for "nei" and then connect them together
            old2new[node]->neighbors.push_back(cloneGraph(nei));
        }

        return old2new[node];
    }
};