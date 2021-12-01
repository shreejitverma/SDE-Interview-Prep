#include <iostream>
using namespace std;
int main()
{
    char ch = 'a';
    for (int i = 0; i < 26; i++)
    {
        cout << (char)(ch + i) << " ";
    }
    cout << endl;
    return 0;
}