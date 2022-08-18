'''https://leetcode.com/problems/kth-ancestor-of-a-tree-node/
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
There will be at most 5 * 104 queries.'''

# Time:  ctor: O(n * logh)
#        get:  O(logh)
# Space: O(n * logh)

# binary jump solution (frequently used in competitive programming)
# Template:
# https://github.com/kamyu104/FacebookHackerCup-2019/blob/master/Final%20Round/little_boat_on_the_sea.py


class TreeAncestor(object):

    def __init__(self, n, parent):
        """
        :type n: int
        :type parent: List[int]
        """
        par = [[p] if p != -1 else [] for p in parent]
        q = [par[i] for i, p in enumerate(parent) if p != -1]
        i = 0
        while q:
            new_q = []
            for p in q:
                if not (i < len(par[p[i]])):
                    continue
                p.append(par[p[i]][i])
                new_q.append(p)
            q = new_q
            i += 1
        self.__parent = par

    def getKthAncestor(self, node, k):
        """
        :type node: int
        :type k: int
        :rtype: int
        """
        par, i, pow_i_of_2 = self.__parent, 0, 1
        while pow_i_of_2 <= k:
            if k & pow_i_of_2:
                if not (i < len(par[node])):
                    return -1
                node = par[node][i]
            i += 1
            pow_i_of_2 *= 2
        return node

# Your TreeAncestor object will be instantiated and called as such:
# obj = TreeAncestor(n, parent)
# param_1 = obj.getKthAncestor(node,k)
