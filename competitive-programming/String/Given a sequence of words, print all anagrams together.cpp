/*
https://practice.geeksforgeeks.org/problems/print-anagrams-together/1
https://www.geeksforgeeks.org/given-a-sequence-of-words-print-all-anagrams-together/
Print Anagrams Together 
Medium Accuracy: 56.1% Submissions: 20996 Points: 4
Given an array of strings, return all groups of strings that are anagrams. 
The groups must be created in order of their appearance in the original array. 
Look at the sample case for clarification.


Example 1:

Input:
N = 5
words[] = {act,god,cat,dog,tac}
Output: 
god dog
act cat tac
Explanation:
There are 2 groups of
anagrams "god", "dog" make group 1.
"act", "cat", "tac" make group 2.
Example 2:

Input:
N = 3
words[] = {no,on,is}
Output: 
no on
is
Explanation:
There are 2 groups of
anagrams "no", "on" make group 1.
"is" makes group 2. 

Your Task:
The task is to complete the function Anagrams() that takes a list of strings as input 
and returns a list of groups such that each group consists of all the strings that are anagrams.


Expected Time Complexity: O(N*|S|*log|S|), where |S| is the length of the strings.
Expected Auxiliary Space: O(N*|S|), where |S| is the length of the strings.


Constraints:
1<=N<=100


A simple method is to create a Hash Table. 
Calculate the hash value of each word in such a way that all anagrams have the same hash value. 
Populate the Hash Table with these hash values. Finally, print those words together with same hash values. 
A simple hashing mechanism can be modulo sum of all characters. 
With modulo sum, two non-anagram words may have same hash value. 
This can be handled by matching individual characters.

Following is another method to print all anagrams together. 
Take two auxiliary arrays, index array and word array. 
Populate the word array with the given sequence of words. 
Sort each individual word of the word array. 
Finally, sort the word array and keep track of the corresponding indices. 
After sorting, all the anagrams cluster together. 
Use the index array to print the strings from the original array of strings.



Let us understand the steps with following input Sequence of Words: 

"cat", "dog", "tac", "god", "act"
1) Create two auxiliary arrays index[] and words[]. 
Copy all given words to words[] and store the original indexes in index[] 

index[]:  0   1   2   3   4
words[]: cat dog tac god act
2) Sort individual words in words[]. Index array doesn’t change.

index[]:   0    1    2    3    4
words[]:  act  dgo  act  dgo  act
3) Sort the words array. Compare individual words using strcmp() to sort

index:     0    2    4    1    3
words[]:  act  act  act  dgo  dgo
4) All anagrams come together. But words are changed in words array. 
To print the original words, take index from the index array and use it in the original array. 
We get "cat tac act dog god"
Following are the implementations of the above algorithm. 
In the following program, an array of structure “Word” is used to store both index and word arrays. 
DupArray is another structure that stores array of structure “Word”. 
*/
// { Driver Code Starts
//Initial Template for C++
#include <bits/stdc++.h>
#include <unordered_map>
using namespace std;

// } Driver Code Ends
//User function Template for C++

class Solution
{
public:
    vector<vector<string> > Anagrams(vector<string> &strs)
    {
        //code here
        vector<vector<string> > ans;

        int n = strs.size();

        if (n == 0)
            return ans;

        unordered_map<string, vector<string> > m;

        for (auto i : strs)
        {
            string s = i;
            sort(s.begin(), s.end());
            m[s].push_back(i);
        }

        for (auto i : m)
            ans.push_back(i.second);

        return ans;
    }
};

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n;
        cin >> n;
        vector<string> string_list(n);
        for (int i = 0; i < n; ++i)
            cin >> string_list[i];
        Solution ob;
        vector<vector<string> > result = ob.Anagrams(string_list);
        sort(result.begin(), result.end());
        for (int i = 0; i < result.size(); i++)
        {
            for (int j = 0; j < result[i].size(); j++)
            {
                cout << result[i][j] << " ";
            }
            cout << "\n";
        }
    }

    return 0;
}
// } Driver Code Ends
// A C++ program to print all anagrams together
#include <bits/stdc++.h>
#include <string.h>
using namespace std;

// structure for each word of duplicate array
class Word
{
public:
    char *str; // to store word itself
    int index; // index of the word in the original array
};

// structure to represent duplicate array.
class DupArray
{
public:
    Word *array; // Array of words
    int size;    // Size of array
};

// Create a DupArray object that contains an array of Words
DupArray *createDupArray(char *str[], int size)
{
    // Allocate memory for dupArray and all members of it
    DupArray *dupArray = new DupArray();
    dupArray->size = size;
    dupArray->array = new Word[(dupArray->size * sizeof(Word))];

    // One by one copy words from the given wordArray to dupArray
    int i;
    for (i = 0; i < size; ++i)
    {
        dupArray->array[i].index = i;
        dupArray->array[i].str = new char[(strlen(str[i]) + 1)];
        strcpy(dupArray->array[i].str, str[i]);
    }

    return dupArray;
}

// Compare two characters. Used in qsort() for
// sorting an array of characters (Word)
int compChar(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}

// Compare two words. Used in qsort()
// for sorting an array of words
int compStr(const void *a, const void *b)
{
    Word *a1 = (Word *)a;
    Word *b1 = (Word *)b;
    return strcmp(a1->str, b1->str);
}

// Given a list of words in wordArr[],
void printAnagramsTogether(char *wordArr[], int size)
{
    // Step 1: Create a copy of all words present in given wordArr.
    // The copy will also have original indexes of words
    DupArray *dupArray = createDupArray(wordArr, size);

    // Step 2: Iterate through all words in dupArray and sort
    // individual words.
    int i;
    for (i = 0; i < size; ++i)
        qsort(dupArray->array[i].str,
              strlen(dupArray->array[i].str), sizeof(char), compChar);

    // Step 3: Now sort the array of words in dupArray
    qsort(dupArray->array, size, sizeof(dupArray->array[0]), compStr);

    // Step 4: Now all words in dupArray are together, but these
    // words are changed. Use the index member of word struct to
    // get the corresponding original word
    for (i = 0; i < size; ++i)
        cout << wordArr[dupArray->array[i].index] << " ";
}

// Driver program to test above functions
int main()
{
    char *wordArr[] = {"cat", "dog", "tac", "god", "act"};
    int size = sizeof(wordArr) / sizeof(wordArr[0]);
    printAnagramsTogether(wordArr, size);
    return 0;
}