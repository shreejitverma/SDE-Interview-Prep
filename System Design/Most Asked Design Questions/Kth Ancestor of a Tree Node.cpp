/*
https://leetcode.com/problems/kth-ancestor-of-a-tree-node/
1483. Kth Ancestor of a Tree Node
Hard

761

73

Add to List

Share
You are given a tree with n nodes numbered from 0 to n - 1 in the form of a parent array parent where parent[i] is the parent of ith node. The root of the tree is node 0. Find the kth ancestor of a given node.

The kth ancestor of a tree node is the kth node in the path from that node to the root node.

Implement the TreeAncestor class:

TreeAncestor(int n, int[] parent) Initializes the object with the number of nodes in the tree and the parent array.
int getKthAncestor(int node, int k) return the kth ancestor of the given node node. If there is no such ancestor, return -1.
 

Example 1:


Input
["TreeAncestor", "getKthAncestor", "getKthAncestor", "getKthAncestor"]
[[7, [-1, 0, 0, 1, 1, 2, 2]], [3, 1], [5, 2], [6, 3]]
Output
[null, 1, 0, -1]

Explanation
TreeAncestor treeAncestor = new TreeAncestor(7, [-1, 0, 0, 1, 1, 2, 2]);
treeAncestor.getKthAncestor(3, 1); // returns 1 which is the parent of 3
treeAncestor.getKthAncestor(5, 2); // returns 0 which is the grandparent of 5
treeAncestor.getKthAncestor(6, 3); // returns -1 because there is no such ancestor
 

Constraints:

1 <= k <= n <= 5 * 104
parent.length == n
parent[0] == -1
0 <= parent[i] < n for all 0 < i < n
0 <= node < n
There will be at most 5 * 104 queries.
*/

// Time:  ctor: O(n * logh)
//        get:  O(logh)
// Space: O(n * logh)

// binary jump solution (frequently used in competitive programming)
class TreeAncestor
{
public:
    TreeAncestor(int n, vector<int> &parent)
    {
        vector<int> q;
        for (const auto &p : parent)
        {
            parent_.emplace_back(vector<int>(p != -1, p));
            if (p != -1)
            {
                q.emplace_back(parent_.size() - 1);
            }
        }
        for (int i = 0; !q.empty(); ++i)
        {
            vector<int> new_q;
            for (const auto &curr : q)
            {
                if (!(i < parent_[parent_[curr][i]].size()))
                {
                    continue;
                }
                parent_[curr].emplace_back(parent_[parent_[curr][i]][i]);
                new_q.emplace_back(curr);
            }
            q = move(new_q);
        }
    }

    int getKthAncestor(int node, int k)
    {
        for (; k; k -= k & ~(k - 1))
        {
            int i = __builtin_ctz(k & ~(k - 1));
            if (!(i < parent_[node].size()))
            {
                return -1;
            }
            node = parent_[node][i];
        }
        return node;
    }

private:
    vector<vector<int> > parent_;
};
/**
 * Your TreeAncestor object will be instantiated and called as such:
 * TreeAncestor* obj = new TreeAncestor(n, parent);
 * int param_1 = obj->getKthAncestor(node,k);
 */