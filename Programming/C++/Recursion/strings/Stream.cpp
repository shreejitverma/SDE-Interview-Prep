#include <iostream>
#include <vector>
#include <string>
using namespace std;
void skip(string p, string up)
{
    if (up.empty())
    {
        cout << p << endl;
        return;
    }
    char ch = up[0];

    if (ch == 'a')
    {
        skip(p, up.substr(1));
    }
    else
    {
        skip(p + ch, up.substr(1));
    }
}

string skip(string up)
{
    if (up.empty())
    {
        return "";
    }

    char ch = up[0];

    if (ch == 'a')
    {
        return skip(up.substr(1));
    }
    else
    {
        return ch + skip(up.substr(1));
    }
}

string skipApple(string up)
{
    if (up.empty())
    {
        return "";
    }
    if (up.find("apple") == 0)
    {
        return skipApple(up.substr(5));
    }
    else
    {
        return up[0] + skipApple(up.substr(1));
    }
}

string skipAppNotApple(string up)
{
    if (up.empty())
    {
        return "";
    }
    if (!up.find("app") && up.find("apple"))
    {
        return skipAppNotApple(up.substr(3));
    }
    else
    {
        return up[0] + skipAppNotApple(up.substr(1));
    }
}

int main()
{
    cout << skipApple("bacapplecdah") << endl;
    cout << skipAppNotApple("bacapplcdah") << endl;
}
