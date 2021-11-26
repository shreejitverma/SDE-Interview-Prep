// C++ program for the above approach
#include <bits/stdc++.h>
using namespace std;

// Recursive function to use implicit stack
// to insert an element at the bottom of stack
stack<int> recur(stack<int> S, int N)
{
    // If stack is empty
    if (S.empty())
        S.push(N);

    else
    {

        // Stores the top element
        int X = S.top();

        // Pop the top element
        S.pop();

        // Recurse with remaining elements
        S = recur(S, N);

        // Push the previous
        // top element again
        S.push(X);
    }
    return S;
}

// Function to insert an element
// at the bottom of stack
void insertToBottom(
    stack<int> S, int N)
{

    // Recursively insert
    // N at the bottom of S
    S = recur(S, N);

    // Print the stack S
    while (!S.empty())
    {
        cout << S.top() << " ";
        S.pop();
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

    return 0;
}
// Iterative version``
// C++ program for the above approach
#include <bits/stdc++.h>
using namespace std;

// Function to insert an element
// at the bottom of a given stack
void insertToBottom(stack<int> S, int N)
{
    // Temporary stack
    stack<int> temp;

    // Iterate until S becomes empty
    while (!S.empty())
    {

        // Push the top element of S
        // into the stack temp
        temp.push(S.top());

        // Pop the top element of S
        S.pop();
    }

    // Push N into the stack S
    S.push(N);

    // Iterate until temp becomes empty
    while (!temp.empty())
    {

        // Push the top element of
        // temp into the stack S
        S.push(temp.top());

        // Pop the top element of temp
        temp.pop();
    }

    // Print the elements of S
    while (!S.empty())
    {
        cout << S.top() << " ";
        S.pop();
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

    return 0;
}