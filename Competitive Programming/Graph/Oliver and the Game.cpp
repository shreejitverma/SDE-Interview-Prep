/*
https://www.hackerearth.com/practice/algorithms/graphs/topological-sort/practice-problems/algorithm/oliver-and-the-game-3/
Problem
Oliver and Bob are best friends. They have spent their entire childhood in the beautiful city of Byteland. The people of Byteland live happily along with the King.
The city has a unique architecture with total N houses. The King's Mansion is a very big and beautiful bungalow having address = 1. Rest of the houses in Byteland have some unique address, (say A), are connected by roads and there is always a unique path between any two houses in the city. Note that the King's Mansion is also included in these houses.

Oliver and Bob have decided to play Hide and Seek taking the entire city as their arena. In the given scenario of the game, it's Oliver's turn to hide and Bob is supposed to find him.
Oliver can hide in any of the houses in the city including the King's Mansion. As Bob is a very lazy person, for finding Oliver, he either goes towards the King's Mansion (he stops when he reaches there), or he moves away from the Mansion in any possible path till the last house on that path.

Oliver runs and hides in some house (say X) and Bob is starting the game from his house (say Y). If Bob reaches house X, then he surely finds Oliver.

Given Q queries, you need to tell Bob if it is possible for him to find Oliver or not.

The queries can be of the following two types:
0 X Y : Bob moves towards the King's Mansion.
1 X Y : Bob moves away from the King's Mansion

INPUT :
The first line of the input contains a single integer N, total number of houses in the city. Next N-1 lines contain two space separated integers A and B denoting a road between the houses at address A and B.
Next line contains a single integer Q denoting the number of queries.
Following Q lines contain three space separated integers representing each query as explained above.

OUTPUT :
Print "YES" or "NO" for each query depending on the answer to that query.

CONSTRAINTS :
1 ≤ N ≤ 10^5
1 ≤ A,B ≤ N
1 ≤ Q ≤ 5*10^5
1 ≤ X,Y ≤ N

NOTE :
Large Input size. Use printf scanf or other fast I/O methods.

Sample Input
9
1 2
1 3
2 6
2 7
6 9
7 8
3 4
3 5
5
0 2 8
1 2 8
1 6 5
0 6 5
1 9 1
Sample Output
YES
NO
NO
NO
YES
Time Limit: 1
Memory Limit: 256
Source Limit:
Explanation
Query 1 Bob goes from 8 towards 1 meeting 2 in the path. Query 2 Bob goes from 8 away from 1 and never meets 2. Query 3 Bob goes from 5 away from 1 and never meets 6. Query 4 Bob goes from 5 towards 1 and never meets 6. Query 5 Bob goes from 1 away from 1 and meets finds Oliver at 9. he can take the following two paths 1 -> 2 -> 6 -> 9 OR 1 -> 2 -> 7 -> 8 9 appears in atleast one of them
*/

/*
// Sample code to perform I/O:

cin >> name;                            // Reading input from STDIN
cout << "Hi, " << name << ".\n";        // Writing output to STDOUT

// Warning: Printing unwanted or ill-formatted data to output will cause the test cases to fail
*/

// Write your code here
#include <bits/stdc++.h>

using namespace std;

#define MOD 1000000007
#define ll long long int
#define ld long double
#define pb push_back
#define mkp make_pair

vector<int> v[100001];
int tim;
int ta[100001], td[100001];

void dfs(int x)
{
    tim++;
    ta[x] = tim;
    for (int i = 0; i < v[x].size(); ++i)
    {
        if (!ta[v[x][i]])
        {
            dfs(v[x][i]);
            tim++;
        }
    }
    tim++;
    td[x] = tim;
}

int main()
{
    int n, m, i, j, k, a, b, c, x, y;
    cin >> n;
    for (i = 1; i < n; ++i)
    {
        scanf("%d %d", &x, &y);
        v[x].pb(y);
        v[y].pb(x);
    }
    dfs(1);
    cin >> m;
    while (m--)
    {
        scanf("%d %d %d", &a, &b, &c);
        if (a == 1)
            swap(c, b);
        if (ta[b] <= ta[c] && td[c] <= td[b])
            printf("YES\n");
        else
            printf("NO\n");
    }

    return 0;
}
// Author's Solution
#include <bits/stdc++.h>
using namespace std;

int vertex;
vector<vector<int> > tree; //used for representing the tree
vector<bool> visited;

vector<int> starttime; // starttime[i] notes the time at which DFS enters node i
vector<int> endtime;   // endtime[i] notes the time at which DFS exits node i
int timer = 0;         // a global variable that stores the timer at that instant

void makeTree() // takes the input and creates a directed graph representing the tree
{
    scanf("%d", &vertex);
    tree.resize(vertex + 1);

    for (int i = 1; i < vertex; i++)
    {
        int x, y;
        scanf("%d%d", &x, &y);

        tree[x].push_back(y);
    }
}

void measureTime(int v) // Performs Depth First Search
{
    visited[v] = 1;

    starttime[v] = timer++;

    for (int i = 0; i < tree[v].size(); i++) // calling measureTime() for adjacent nodes of node v and performing DFS
    {
        if (visited[tree[v][i]] == 0)
            measureTime(tree[v][i]);
    }
    endtime[v] = timer++;
}

int check(int x, int y)
{
    if (starttime[x] > starttime[y] && endtime[x] < endtime[y]) // checks weather node x lies in the subtree of node y or not
        return 1;
    return 0;
}

int main()
{
    makeTree();

    visited.resize(vertex + 1, 0);
    starttime.resize(vertex + 1, 0);
    endtime.resize(vertex + 1, 0);

    measureTime(1);

    int q;
    scanf("%d", &q);

    while (q--)
    {
        int type, x, y;
        scanf("%d%d%d", &type, &x, &y);

        if (!check(x, y) && !check(y, x))
        {
            printf("NO\n");
            continue;
        }

        if (type == 0)
        {
            if (check(y, x) == 1)
                printf("YES\n");
            else
                printf("NO\n");
        }
        else
        {
            if (check(x, y) == 1)
                printf("YES\n");
            else
                printf("NO\n");
        }
    }

    return 0;
}
// Tester's Solution
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <iomanip>
#include <list>
#include <math.h>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#define LL long long
#define ULL unsigned long long
#define F first
#define S second
#define pb push_back
#define FOR(i, lb, ub) for (i = lb; i <= ub; i++)
#define RFOR(i, ub, lb) for (i = ub; i >= lb; i--)
#define FORS(it, v) for (it = v.begin(); it != v.end(); it++)
using namespace std;
vector<int> G[100005];
bool visited[100005];
int in[100005], out[100005], ts;
int cnt;
void dfs(int x)
{
    cnt++;
    visited[x] = true;
    in[x] = ++ts;
    for (int i = 0; i < G[x].size(); ++i)
    {
        if (!visited[G[x][i]])
            dfs(G[x][i]);
    }
    out[x] = ++ts;
}
bool is_subtree(int x, int y) //if y lies in subtree of x
{
    if (in[x] <= in[y] && out[x] >= out[y])
        return true;
    return false;
}
int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int t, i, j;
    int n;
    cin >> n;
    assert(n >= 1 && n <= 100000);
    FOR(i, 0, n - 2)
    {
        int a, b;
        cin >> a >> b;
        assert(a >= 1 && a <= n);
        assert(b >= 1 && b <= n);
        G[a].pb(b);
        G[b].pb(a);
    }
    dfs(1);
    assert(cnt == n);
    int q;
    cin >> q;
    assert(q >= 1 && q <= 500000);
    while (q--)
    {
        int a, x, y;
        cin >> a >> x >> y;
        assert(a == 0 || a == 1);
        assert(x >= 1 && x <= n);
        assert(y >= 1 && y <= n);
        if ((!a && is_subtree(x, y)) || (a && is_subtree(y, x)))
            cout << "YES\n";
        else
            cout << "NO\n";
    }

    return 0;
}