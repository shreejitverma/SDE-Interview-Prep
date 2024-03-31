/*
https://leetcode.com/problems/number-of-operations-to-make-network-connected/
1319. Number of Operations to Make Network Connected
Medium

1515

25

Add to List

Share
There are n computers numbered from 0 to n-1 connected by ethernet cables connections forming a network where connections[i] = [a, b] represents a connection between computers a and b. Any computer can reach any other computer directly or indirectly through the network.

Given an initial computer network connections. You can extract certain cables between two directly connected computers, and place them between any pair of disconnected computers to make them directly connected. Return the minimum number of times you need to do this in order to make all the computers connected. If it's not possible, return -1. 

 

Example 1:



Input: n = 4, connections = [[0,1],[0,2],[1,2]]
Output: 1
Explanation: Remove cable between computer 1 and 2 and place between computers 1 and 3.
Example 2:



Input: n = 6, connections = [[0,1],[0,2],[0,3],[1,2],[1,3]]
Output: 2
Example 3:

Input: n = 6, connections = [[0,1],[0,2],[0,3],[1,2]]
Output: -1
Explanation: There are not enough cables.
Example 4:

Input: n = 5, connections = [[0,1],[0,2],[3,4],[2,3]]
Output: 0
 

Constraints:

1 <= n <= 10^5
1 <= connections.length <= min(n*(n-1)/2, 10^5)
connections[i].length == 2
0 <= connections[i][0], connections[i][1] < n
connections[i][0] != connections[i][1]
There are no repeated connections.
No two computers are connected by more than one cable.
*/

// Time:  O(|E| + |V|)
// Space: O(|V|)

class Solution
{
public:
    int makeConnected(int n, vector<vector<int> > &connections)
    {
        if (connections.size() < n - 1)
        {
            return -1;
        }
        UnionFind union_find(n);
        for (const auto &c : connections)
        {
            union_find.union_set(c[0], c[1]);
        }
        return union_find.size() - 1;
    }

private:
    class UnionFind
    {
    public:
        UnionFind(const int n) : set_(n), size_(n)
        {
            iota(set_.begin(), set_.end(), 0);
        }

        int find_set(const int x)
        {
            if (set_[x] != x)
            {
                set_[x] = find_set(set_[x]); // Path compression.
            }
            return set_[x];
        }

        bool union_set(const int x, const int y)
        {
            int x_root = find_set(x), y_root = find_set(y);
            if (x_root == y_root)
            {
                return false;
            }
            set_[min(x_root, y_root)] = max(x_root, y_root);
            --size_;
            return true;
        }

        int size() const
        {
            return size_;
        }

    private:
        vector<int> set_;
        int size_;
    };
};

// Time:  O(|E| + |V|)
// Space: O(|V|)
class Solution2
{
public:
    int makeConnected(int n, vector<vector<int> > &connections)
    {
        if (connections.size() < n - 1)
        {
            return -1;
        }
        unordered_map<int, vector<int> > G;
        for (const auto &c : connections)
        {
            G[c[0]].emplace_back(c[1]);
            G[c[1]].emplace_back(c[0]);
        }
        int result = 0;
        unordered_set<int> lookup;
        for (int i = 0; i < n; ++i)
        {
            result += dfs(G, i, &lookup);
        }
        return result - 1;
    }

private:
    int dfs(const unordered_map<int, vector<int> > &G,
            int i, unordered_set<int> *lookup)
    {
        if (lookup->count(i))
        {
            return 0;
        }
        lookup->emplace(i);
        if (G.count(i))
        {
            for (const auto &j : G.at(i))
            {
                dfs(G, j, lookup);
            }
        }
        return 1;
    }
};