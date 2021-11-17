'''https://leetcode.com/problems/number-of-operations-to-make-network-connected/
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
No two computers are connected by more than one cable.'''

# Time:  O(|E| + |V|)
# Space: O(|V|)

import collections


class UnionFind(object):
    def __init__(self, n):
        self.set = range(n)
        self.count = n

    def find_set(self, x):
        if self.set[x] != x:
            self.set[x] = self.find_set(self.set[x])  # path compression.
        return self.set[x]

    def union_set(self, x, y):
        x_root, y_root = map(self.find_set, (x, y))
        if x_root == y_root:
            return False
        self.set[max(x_root, y_root)] = min(x_root, y_root)
        self.count -= 1
        return True


class Solution(object):
    def makeConnected(self, n, connections):
        """
        :type n: int
        :type connections: List[List[int]]
        :rtype: int
        """
        if len(connections) < n-1:
            return -1
        union_find = UnionFind(n)
        for i, j in connections:
            union_find.union_set(i, j)
        return union_find.count - 1


# Time:  O(|E| + |V|)
# Space: O(|V|)


class Solution2(object):
    def makeConnected(self, n, connections):
        """
        :type n: int
        :type connections: List[List[int]]
        :rtype: int
        """
        def dfs(i, lookup):
            if i in lookup:
                return 0
            lookup.add(i)
            if i in G:
                for j in G[i]:
                    dfs(j, lookup)
            return 1

        if len(connections) < n-1:
            return -1
        G = collections.defaultdict(list)
        for i, j in connections:
            G[i].append(j)
            G[j].append(i)
        lookup = set()
        return sum(dfs(i, lookup) for i in range(n)) - 1
