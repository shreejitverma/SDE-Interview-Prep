/*https://www.hackerrank.com/challenges/journey-to-the-moon/problem
The member states of the UN are planning to send  people to the moon. They want them to be from different countries. You will be given a list of pairs of astronaut ID's. Each pair is made of astronauts from the same country. Determine how many pairs of astronauts from different countries they can choose from.
Example
 

There are  astronauts numbered  through . Astronauts grouped by country are  and . There are  pairs to choose from:  and .
Function Description
Complete the journeyToMoon function in the editor below.
journeyToMoon has the following parameter(s):
int n: the number of astronauts
int astronaut[p][2]: each element  is a  element array that represents the ID's of two astronauts from the same country
Returns 
- int: the number of valid pairs
Input Format
The first line contains two integers  and , the number of astronauts and the number of pairs. 
Each of the next  lines contains  space-separated integers denoting astronaut ID's of two who share the same nationality.
Constraints


Sample Input 0
5 3
0 1
2 3
0 4
Sample Output 0
6
Explanation 0
Persons numbered  belong to one country, and those numbered  belong to another. The UN has  ways of choosing a pair:

Sample Input 1
4 1
0 2
Sample Output 1
5
Explanation 1
Persons numbered  belong to the same country, but persons  and  don't share countries with anyone else. The UN has  ways of choosing a pair:
*/
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <set>
#include <queue>
#include <iostream>
#include <sstream>
#include <stack>
#include <deque>
#include <cmath>
#include <memory.h>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <utility>
using namespace std;

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define RFOR(i, b, a) for (int i = (b)-1; i >= (a); --i)
#define REP(i, N) FOR(i, 0, N)
#define RREP(i, N) RFOR(i, N, 0)

#define ALL(V) V.begin(), V.end()
#define SZ(V) (int)V.size()
#define PB push_back
#define MP make_pair
#define Pi 3.14159265358979

typedef long long Int;
typedef unsigned long long UInt;
typedef vector<int> VI;
typedef pair<int, int> PII;

VI a[1 << 17];
bool was[1 << 17];

int n, m;

int dfs(int cur)
{
    if (was[cur])
        return 0;

    was[cur] = true;
    int res = 1;

    REP(i, SZ(a[cur]))
    {
        int nx = a[cur][i];

        res += dfs(nx);
    }

    return res;
}

int main()
{
    //	freopen("in.txt", "r", stdin);
    //	freopen("out.txt", "w", stdout);

    scanf("%d%d", &n, &m);

    REP(i, m)
    {
        int x, y;
        scanf("%d%d", &x, &y);

        a[x].push_back(y);
        a[y].push_back(x);
    }

    VI all;

    REP(i, n)
    {
        if (!was[i])
        {
            all.push_back(dfs(i));
        }
    }

    Int res = 0;
    Int sum = 0;
    REP(i, SZ(all))
    {
        res += sum * all[i];

        sum += all[i];
    }

    cout << res << endl;

    return 0;
}
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdlib>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// This is a standard Find-Union Set problem.
int fa[100001];
int getfather(int x)
{
    if (x == fa[x])
        return x;
    fa[x] = getfather(fa[x]);
    return fa[x];
}
int main()
{
    int n, p;
    scanf("%d %d", &n, &p);
    for (int i = 0; i < n; i++)
        fa[i] = i;
    for (int i = 0; i < p; i++)
    {
        int a, b;
        scanf("%d %d", &a, &b);
        if (getfather(a) != getfather(b)) // Merge two trees at the root!
            fa[getfather(a)] = getfather(b);
    }
    int count[100001] = {0};
    vector<int> c;
    for (int i = 0; i < n; i++)
        count[getfather(i)]++;
    for (int i = 0; i < n; i++)
        if (count[i] != 0)
            c.push_back(count[i]);
    int l = c.size();
    /* The following is my previous approach. It got TLE at extreme conditions.
    long long sum = 0;
    for (int i=0;i<l;i++)
        for (int j=i+1;j<l;j++)
            sum += c[i]*c[j];
    printf("%lld\n",sum);
    */
    // Here We need pairwise product sum. For example, if we have a, b, c, d, we want ab+ac+ad+bc+bd+cd
    // Naive approach is too slow. Here we can use ((a+b+c+d)^2 - (a^2+b^2+c^2+d^2)) / 2 to calculate. It's way faster.
    long long sumsquare = 0, squaresum = 0;
    for (int i = 0; i < l; i++)
    {
        sumsquare += c[i];
        squaresum += c[i] * c[i];
    }
    sumsquare = sumsquare * sumsquare;
    long long ans = (sumsquare - squaresum) / 2;
    printf("%lld\n", ans);
}
