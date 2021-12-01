
#include <iostream>
#include <vector>
#include <string>
using namespace std;
vector<string> addAll(vector<string> first, vector<string> second);

void subseq(string p, string up)
{
    if (up.empty())
    {
        cout << p << endl;
        return;
    }
    char ch = up[0];
    subseq(p + ch, up.substr(1));
    subseq(p, up.substr(1));
}

vector<string> subseqRet(string p, string up)
{
    if (up.empty())
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    char ch = up[0];
    vector<string> left = subseqRet(p + ch, up.substr(1));
    vector<string> right = subseqRet(p, up.substr(1));

    ;
    left = addAll(left, right);
    return left;
}

void subseqAscii(string p, string up)
{
    if (up.empty())
    {
        cout << p << endl;
        return;
    }
    char ch = up[0];
    subseqAscii(p + ch, up.substr(1));
    subseqAscii(p, up.substr(1));
    subseqAscii(p + to_string(ch + 0), up.substr(1));
}

vector<string> subseqAsciiRet(string p, string up)
{
    if (up.empty())
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    char ch = up[0];
    vector<string> first = subseqAsciiRet(p + ch, up.substr(1));
    vector<string> second = subseqAsciiRet(p, up.substr(1));
    vector<string> third = subseqAsciiRet(p + to_string(ch + 0), up.substr(1));

    first = addAll(first, second);
    first = addAll(first, third);
    return first;
}
void display(vector<string> ans)
{
    for (int i = 0; i < ans.size(); i++)
    {
        cout << ans[i] << " ";
    }
    cout << endl;
}
vector<string> addAll(vector<string> first, vector<string> second)
{
    for (int i = 0; i < second.size(); i++)
    {
        first.push_back(second[i]);
    }
    return first;
}
int main()
{
    subseqAscii("", "abc");
    vector<string> ans = subseqAsciiRet("", "abc");
    display(ans);
}
