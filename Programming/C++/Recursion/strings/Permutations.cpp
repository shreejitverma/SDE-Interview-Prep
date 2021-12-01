
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void permutations(string p, string up)
{
    if (up.empty())
    {
        cout << p << endl;
        return;
    }
    char ch = up[0];
    for (int i = 0; i <= p.length(); i++)
    {
        string f = p.substr(0, i);
        string s = p.substr(i, p.length());
        permutations(f + ch + s, up.substr(1));
    }
}

vector<string> permutationsList(string p, string up)
{
    if (up.empty())
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    char ch = up[0];

    // local to this call
    vector<string> ans;

    for (int i = 0; i <= p.length(); i++)
    {
        string f = p.substr(0, i);
        string s = p.substr(i, p.length());

        vector<string> ansFromBelowCalls = permutationsList(f + ch + s, up.substr(1));
        int n = ansFromBelowCalls.size();
        for (int i = 0; i < n; ++i)
        {
            ans.push_back(ansFromBelowCalls[i]);
        }
    }
    return ans;
}

int permutationsCount(string p, string up)
{
    if (up.empty())
    {
        return 1;
    }
    int count = 0;
    char ch = up[0];
    for (int i = 0; i <= p.length(); i++)
    {
        string f = p.substr(0, i);
        string s = p.substr(i, p.length());
        count = count + permutationsCount(f + ch + s, up.substr(1));
    }
    return count;
}
void display(vector<string> ans)
{
    for (int i = 0; i < ans.size(); i++)
    {
        cout << ans[i] << " ";
    }
    cout << endl;
}
int main()
{
    permutations("", "abc");

    vector<string> ans = permutationsList("", "abc");
    display(ans);
    cout << endl;
    cout << permutationsCount("", "abcd") << "\n";
}
