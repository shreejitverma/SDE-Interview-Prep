/*
https://www.geeksforgeeks.org/recursively-print-all-sentences-that-can-be-formed-from-list-of-word-lists/

Given a list of word lists How to print all sentences possible taking one word from a list at a time via recursion? 
Example: 

Input: {{"you", "we"},
        {"have", "are"},
        {"sleep", "eat", "drink"}}

Output:
  you have sleep
  you have eat
  you have drink
  you are sleep
  you are eat
  you are drink
  we have sleep
  we have eat
  we have drink
  we are sleep
  we are eat
  we are drink 




We strongly recommend minimizing your browser and try this yourself first. 
The idea is based on a simple depth-first traversal. We start from every word of the first list as the first word of an output sentence, then recur for the remaining lists.
Below is the C++ implementation of the above idea. In the below implementation, the input list of the list is considered as a 2D array. If we take a closer look, we can observe that the code is very close to the DFS of graph.
*/

// C++ program to print all possible sentences from a list of word list
#include <iostream>
#include <string>

#define R 3
#define C 3
using namespace std;

// A recursive function to print all possible sentences that can be formed
// from a list of word list
void printUtil(string arr[R][C], int m, int n, string output[R])
{
    // Add current word to output array
    output[m] = arr[m][n];

    // If this is last word of current output sentence, then print
    // the output sentence
    if (m == R - 1)
    {
        for (int i = 0; i < R; i++)
            cout << output[i] << " ";
        cout << endl;
        return;
    }

    // Recur for next row
    for (int i = 0; i < C; i++)
        if (arr[m + 1][i] != "")
            printUtil(arr, m + 1, i, output);
}

// A wrapper over printUtil()
void print(string arr[R][C])
{
    // Create an array to store sentence
    string output[R];

    // Consider all words for first row as starting points and
    // print all sentences
    for (int i = 0; i < C; i++)
        if (arr[0][i] != "")
            printUtil(arr, 0, i, output);
}

// Driver program to test above functions
int main()
{
    string arr[R][C] = {{"you", "we"},
                        {"have", "are"},
                        {"sleep", "eat", "drink"}};

    print(arr);

    return 0;
}