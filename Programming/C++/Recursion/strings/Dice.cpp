#include <iostream>
#include <vector>
#include <string>
using namespace std;

void dice(string p, int target)
{
    if (target == 0)
    {
        cout << p << endl;
        return;
    }

    for (int i = 1; i <= 6 && i <= target; i++)
    {
        dice(p + to_string(i), target - i);
    }
}

vector<string> diceRet(string p, int target)
{
    if (target == 0)
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    vector<string> list;

    for (int i = 1; i <= 6 && i <= target; i++)
    {
        // vector<string> v = list;
        vector<string> ansFromBelowCalls;
        ansFromBelowCalls = diceRet(p + to_string(i), target - i);
        int n = ansFromBelowCalls.size();
        for (int i = 0; i < n; ++i)
        {
            list.push_back(ansFromBelowCalls[i]);
        }
    }
    return list;
}

void diceFace(string p, int target, int face)
{
    if (target == 0)
    {
        cout << p << endl;
        return;
    }

    for (int i = 1; i <= face && i <= target; i++)
    {
        diceFace(p + to_string(i), target - i, face);
    }
}

vector<string> diceFaceRet(string p, int target, int face)
{
    if (target == 0)
    {
        vector<string> list;
        list.push_back(p);
        return list;
    }
    vector<string> list;
    for (int i = 1; i <= face && i <= target; i++)
    {
        vector<string> ansFromBelowCalls = diceFaceRet(p + to_string(i), target - i, face);
        int n = ansFromBelowCalls.size();
        for (int i = 0; i < n; ++i)
        {
            list.push_back(ansFromBelowCalls[i]);
        }
    }
    return list;
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
    dice("", 4);

    vector<string> ans;
    vector<string> anotherone;
    ans = diceRet("", 4);
    display(ans);
    diceFace("", 4, 6);
    anotherone = diceFaceRet("", 4, 6);
    display(anotherone);
    return 0;
}
