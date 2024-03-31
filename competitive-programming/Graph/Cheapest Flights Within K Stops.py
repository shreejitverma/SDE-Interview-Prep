'''https://leetcode.com/problems/cheapest-flights-within-k-stops/description/

Cheapest Flights Within K Stops
Medium

4105

166

Add to List

Share
There are n cities connected by some number of flights. You are given an array flights where flights[i] = [fromi, toi, pricei] indicates that there is a flight from city fromi to city toi with cost pricei.

You are also given three integers src, dst, and k, return the cheapest price from src to dst with at most k stops. If there is no such route, return -1.

 

Example 1:


Input: n = 3, flights = [[0,1,100],[1,2,100],[0,2,500]], src = 0, dst = 2, k = 1
Output: 200
Explanation: The graph is shown.
The cheapest price from city 0 to city 2 with at most 1 stop costs 200, as marked red in the picture.
Example 2:


Input: n = 3, flights = [[0,1,100],[1,2,100],[0,2,500]], src = 0, dst = 2, k = 0
Output: 500
Explanation: The graph is shown.
The cheapest price from city 0 to city 2 with at most 0 stop costs 500, as marked blue in the picture.
 

Constraints:

1 <= n <= 100
0 <= flights.length <= (n * (n - 1) / 2)
flights[i].length == 3
0 <= fromi, toi < n
fromi != toi
1 <= pricei <= 104
There will not be any multiple flights between two cities.
0 <= src, dst, k < n
src != dst'''
import collections
import heapq


class Solution(object):
    def findCheapestPrice(self, n, flights, src, dst, K):
        """
        :type n: int
        :type flights: List[List[int]]
        :type src: int
        :type dst: int
        :type K: int
        :rtype: int
        """
        adj = collections.defaultdict(list)
        for u, v, w in flights:
            adj[u].append((v, w))
        best = collections.defaultdict(
            lambda: collections.defaultdict(lambda: float("inf")))
        best[src][K+1] = 0
        min_heap = [(0, src, K+1)]
        while min_heap:
            result, u, k = heapq.heappop(min_heap)
            if k < 0 or best[u][k] < result:
                continue
            if u == dst:
                return result
            for v, w in adj[u]:
                if result+w < best[v][k-1]:
                    best[v][k-1] = result+w
                    heapq.heappush(min_heap, (result+w, v, k-1))
        return -1
