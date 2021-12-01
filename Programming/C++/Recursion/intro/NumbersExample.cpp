#include <iostream>
using namespace std;
void print1(int n);
void print2(int n);
void print3(int n);
void print4(int n);
void print5(int n);

void print1(int n)
{
    cout << n << endl;
    print2(2);
}

void print2(int n)
{
    cout << n << endl;
    print3(3);
}

void print3(int n)
{
    cout << n << endl;
    print4(4);
}

void print4(int n)
{
    cout << n << endl;
    print5(5);
}

// func body here changes
void print5(int n)
{
    cout << n << endl;
}

int main()
{
    print1(1);
    return 0;
}
