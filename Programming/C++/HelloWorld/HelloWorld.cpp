// #include <iostream>

// int main()
// {
//     int a = 5;
//     int b = 6;
//     int sum = a + b;
//     std::cout << "Hello World" << sum << std::endl;
// }
#include <iostream>
#include <stack>
// C++ program for the above approach
// #include <bits/stdc++.h>
using namespace std;

// Function to insert an element
// at the bottom of a given stack
void insertToBottom(stack<int> &S, int N)
{

    if (S.empty())
        S.push(N);
    else
    {
        /* All items are held in Function Call Stack until we
           reach end of the stack. When the stack becomes
           empty, the st.size() becomes 0, the
           above if part is executed and the item is inserted
           at the bottom */

        int a = S.top();
        S.pop();
        insertToBottom(S, N);

        //push all the items held in Function Call Sack
        //once the item is inserted at the bottom
        S.push(a);
    }
}

// Driver Code
int main()
{
    // Input
    stack<int> S;
    S.push(5);
    S.push(4);
    S.push(3);
    S.push(2);
    S.push(1);

    int N = 7;

    insertToBottom(S, N);
    insertToBottom(S, 9);
    // Print the elements of S
    while (!S.empty())
    {
        cout << S.top() << " ";
        S.pop();
    }
    return 0;
}