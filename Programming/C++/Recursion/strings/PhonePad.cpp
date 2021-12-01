#include <iostream>
#include <vector>
#include <string>
using namespace std;

static void pad(string p, string up)
{
    if (up.empty())
    {
        cout << p << endl;
        return;
    }
    int digit = up[0] - '0'; // this will convert '2' into 2
    for (int i = (digit - 1) * 3; i < digit * 3; i++)
    {
        char ch = (char)('a' + i);
        pad(p + ch, up.substr(1));
    }
}

vector<string> padRet(string p, string up)
{
    if (up.empty())
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    int digit = up[0] - '0'; // this will convert '2' into 2

    vector<string> list;

    for (int i = (digit - 1) * 3; i < digit * 3; i++)
    {
        char ch = (char)('a' + i);
        vector<string> ansFromBelowCalls;
        ansFromBelowCalls = padRet(p + ch, up.substr(1));
        int n = ansFromBelowCalls.size();
        for (int i = 0; i < n; ++i)
        {
            list.push_back(ansFromBelowCalls[i]);
        }
    }
    return list;
}

int padCount(string p, string up)
{
    if (up.empty())
    {
        return 1;
    }
    int count = 0;
    int digit = up[0] - '0'; // this will convert '2' into 2
    for (int i = (digit - 1) * 3; i < digit * 3; i++)
    {
        char ch = (char)('a' + i);
        count = count + padCount(p + ch, up.substr(1));
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
    vector<string> ans;
    ans = padRet("", "12");
    display(ans);
    cout << padRet("", "12").size() << endl;
    cout << padCount("", "12") << endl;
}