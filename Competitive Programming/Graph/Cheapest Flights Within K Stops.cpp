/*
https://leetcode.com/problems/cheapest-flights-within-k-stops/description/

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
src != dst
*/

// Time:  O((|E| + |V|) * log|V|) = O(|E| * log|V|)
// Space: O(|E| + |V|) = O(|E|)

class Solution
{
public:
    int findCheapestPrice(int n, vector<vector<int> > &flights, int src, int dst, int K)
    {
        using P = pair<int, int>;
        unordered_map<int, vector<P> > adj;
        for (const auto &flight : flights)
        {
            int u, v, w;
            tie(u, v, w) = make_tuple(flight[0], flight[1], flight[2]);
            adj[u].emplace_back(v, w);
        }

        unordered_map<int, unordered_map<int, int> > best;
        best[src][K + 1] = 0;
        using T = tuple<int, int, int>;
        priority_queue<T, vector<T>, greater<T> > min_heap;
        min_heap.emplace(0, src, K + 1);
        while (!min_heap.empty())
        {
            int result, u, k;
            tie(result, u, k) = min_heap.top();
            min_heap.pop();
            if (k < 0 ||
                (best.count(u) && best[u].count(k) && best[u][k] < result))
            {
                continue;
            }
            if (u == dst)
            {
                return result;
            }
            for (const auto &kvp : adj[u])
            {
                int v, w;
                tie(v, w) = kvp;
                if (!best.count(v) ||
                    !best[v].count(k - 1) ||
                    result + w < best[v][k - 1])
                {
                    best[v][k - 1] = result + w;
                    min_heap.emplace(result + w, v, k - 1);
                }
            }
        }
        return -1;
    }
};