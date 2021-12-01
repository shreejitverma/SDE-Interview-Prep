#include <iostream>
using namespace std;

void message4()
{
    cout << "Hello World" << endl;
}
void message3()
{
    cout << "Hello World" << endl;
    message4();
}
void message2()
{
    cout << "Hello World" << endl;
    message3();
}
void message1()
{
    cout << "Hello World" << endl;
    message2();
}

void message()
{
    cout << "Hello World" << endl;
    message1();
}
int main()
{
    // write a function that prints hello world
    message();
    return 0;
}
