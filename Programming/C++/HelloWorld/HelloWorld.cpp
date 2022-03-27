#include <iostream>
#include <vector>
using namespace std;
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

int main()
{

    int left = 0;  //
    int right = 0; //
    string s = "";
    vector<string> result;
    generateParenthesis(3, s, left, right, result);
    // cout<<result.size();
    for (int i = 0; i < result.size(); i++)
    {
        cout << result[i] << endl;
    }
    return 0;
}
