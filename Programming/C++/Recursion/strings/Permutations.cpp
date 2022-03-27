
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
void generateParenthesis(int n, string s, int left, int right, vector<string> &result)
{
    if (left == n && right == n)
    {
        result.push_back(s);
        return;
    }

    if (left < n)
    {
        generateParenthesis(n, s + "(", left + 1, right, result);
    }
    if (left > right)
    {
        generateParenthesis(n, s + ")", left, right + 1, result);
    }
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
    int left = 0;  //
    int right = 0; //
    string s = "";
    vector<string> result;
    generateParenthesis(3, s, left, right, result);
    display(result);
}
