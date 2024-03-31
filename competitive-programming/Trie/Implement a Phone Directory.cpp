/*
https://practice.geeksforgeeks.org/problems/phone-directory4628/1
Phone directory 
Hard Accuracy: 36.85% Submissions: 2994 Points: 8
Given a list of contacts contact[] of length n where each contact is a string which exist in a phone directory and a query string s. The task is to implement a search query for the phone directory. Run a search query for each prefix p of the query string s (i.e. from  index 1 to |s|) that prints all the distinct contacts which have the same prefix as p in lexicographical increasing order. Please refer the explanation part for better understanding.
Note: If there is no match between query and contacts, print "0".

Example 1:

Input: 
n = 3
contact[] = {"geeikistest", "geeksforgeeks", 
"geeksfortest"}
s = "geeips"
Output:
geeikistest geeksforgeeks geeksfortest
geeikistest geeksforgeeks geeksfortest
geeikistest geeksforgeeks geeksfortest
geeikistest
0
0
Explaination: By running the search query on 
contact list for "g" we get: "geeikistest", 
"geeksforgeeks" and "geeksfortest".
By running the search query on contact list 
for "ge" we get: "geeikistest" "geeksforgeeks"
and "geeksfortest".
By running the search query on contact list 
for "gee" we get: "geeikistest" "geeksforgeeks"
and "geeksfortest".
By running the search query on contact list 
for "geei" we get: "geeikistest".
No results found for "geeip", so print "0". 
No results found for "geeips", so print "0".
Your Task:
Youd do not need to read input or print anything. Your task is to complete the function displayContacts() which takes n, contact[ ] and s as input parameters and returns a list of list of strings for required prefixes. If some prefix has no matching contact return "0" on that list.

Expected Time Complexity: O(|s| * n * max|contact[i]|)
Expected Auxiliary Space: O(n * max|contact[i]|)

Constraints:
1 ≤ n ≤ 50
1 ≤ |contact[i]| ≤ 50
1 ≤ |s| ≤ 6 
*/

class Solution
{
public:
    class Node
    {
    public:
        bool wordEnd;
        unordered_map<char, Node *> mp;
        Node()
        {
            wordEnd = 0;
        }
    };
    void addWord(string &word, Node *root)
    {
        Node *curr = root;
        int i = 0;
        while (i < word.size())
        {
            if (curr->mp.find(word[i]) == curr->mp.end())
            {
                curr->mp[word[i]] = new Node();
            }
            curr = curr->mp[word[i]];
            i++;
        }
        curr->wordEnd = 1;
    }
    void dfs(Node *curr, vector<string> &res, string &form)
    {

        if (curr->wordEnd)
            res.push_back(form);
        char c;
        for (int i = 0; i < 26; i++)
        {
            c = char('a' + i);
            if (curr->mp.find(c) == curr->mp.end())
                continue;
            form.push_back(c);
            dfs(curr->mp[c], res, form);
            form.pop_back();
        }
    }
    vector<vector<string> > displayContacts(int n, string contact[], string s)
    {
        Node *root = new Node();
        for (int i = 0; i < n; i++)
        {
            addWord(contact[i], root);
        }
        vector<vector<string> > res;
        string prefix;
        Node *curr = root;
        for (auto c : s)
        {
            prefix.push_back(c);
            if (curr->mp.find(c) == curr->mp.end())
                break;
            else
                curr = curr->mp[c];
            vector<string> temp;
            dfs(curr, temp, prefix);
            if (temp.empty())
                break;
            else
                res.push_back(temp);
        }
        while (res.size() < s.size())
        {
            res.push_back({"0"});
        }
        delete (root);
        return res;
    }
};

/*
Phone Directory can be efficiently implemented using Trie Data Structure. We insert all the contacts into Trie.

Generally search query on a Trie is to determine whether the string is present or not in the trie, 
but in this case we are asked to find all the strings with each prefix of ‘str’. 
This is equivalent to doing a DFS traversal on a graph. 
From a Trie node, visit adjacent Trie nodes and do this recursively until there are no more adjacent. 
This recursive function will take 2 arguments one as Trie Node which points to the current Trie Node being visited 
and other as the string which stores the string found so far with prefix as ‘str’.
Each Trie Node stores a boolean variable ‘isLast’ which is true if the node represents end of a contact(word).

// This function displays all words with given
// prefix.  "node" represents last node when 
// path from root follows characters of "prefix".
displayContacts (TreiNode node, string prefix)
    If (node.isLast is true)
        display prefix

        // finding adjacent nodes
    for each character ‘i’ in lower case Alphabets 
        if (node.child[i] != NULL)
            displayContacts(node.child[i], prefix+i)
User will enter the string character by character and we need to display suggestions with the prefix formed 
after every entered character.
So one approach to find the prefix starting with the string formed is to check if the prefix exists in the Trie, 
if yes then call the displayContacts() function. In this approach after every entered character 
we check if the string exists in the Trie.
Instead of checking again and again, we can maintain a pointer prevNode‘ that points to the TrieNode 
which corresponds to the last entered character by the user, now we need to check the child node for the ‘prevNode’ 
when user enters another character to check if it exists in the Trie. If the new prefix is not in the Trie, 
then all the string which are formed by entering characters after ‘prefix’ can’t be found in Trie too. 
So we break the loop that is being used to generate prefixes one by one 
and print “No Result Found” for all remaining characters.
*/
// C++ Program to Implement a Phone
// Directory Using Trie Data Structure
#include <bits/stdc++.h>
using namespace std;

struct TrieNode
{
    // Each Trie Node contains a Map 'child'
    // where each alphabet points to a Trie
    // Node.
    // We can also use a fixed size array of
    // size 256.
    unordered_map<char, TrieNode *> child;

    // 'isLast' is true if the node represents
    // end of a contact
    bool isLast;

    // Default Constructor
    TrieNode()
    {
        // Initialize all the Trie nodes with NULL
        for (char i = 'a'; i <= 'z'; i++)
            child[i] = NULL;

        isLast = false;
    }
};

// Making root NULL for ease so that it doesn't
// have to be passed to all functions.
TrieNode *root = NULL;

// Insert a Contact into the Trie
void insert(string s)
{
    int len = s.length();

    // 'itr' is used to iterate the Trie Nodes
    TrieNode *itr = root;
    for (int i = 0; i < len; i++)
    {
        // Check if the s[i] is already present in
        // Trie
        TrieNode *nextNode = itr->child[s[i]];
        if (nextNode == NULL)
        {
            // If not found then create a new TrieNode
            nextNode = new TrieNode();

            // Insert into the Map
            itr->child[s[i]] = nextNode;
        }

        // Move the iterator('itr') ,to point to next
        // Trie Node
        itr = nextNode;

        // If its the last character of the string 's'
        // then mark 'isLast' as true
        if (i == len - 1)
            itr->isLast = true;
    }
}

// This function simply displays all dictionary words
// going through current node. String 'prefix'
// represents string corresponding to the path from
// root to curNode.
void displayContactsUtil(TrieNode *curNode, string prefix)
{
    // Check if the string 'prefix' ends at this Node
    // If yes then display the string found so far
    if (curNode->isLast)
        cout << prefix << endl;

    // Find all the adjacent Nodes to the current
    // Node and then call the function recursively
    // This is similar to performing DFS on a graph
    for (char i = 'a'; i <= 'z'; i++)
    {
        TrieNode *nextNode = curNode->child[i];
        if (nextNode != NULL)
            displayContactsUtil(nextNode, prefix + (char)i);
    }
}

// Display suggestions after every character enter by
// the user for a given query string 'str'
void displayContacts(string str)
{
    TrieNode *prevNode = root;

    string prefix = "";
    int len = str.length();

    // Display the contact List for string formed
    // after entering every character
    int i;
    for (i = 0; i < len; i++)
    {
        // 'prefix' stores the string formed so far
        prefix += (char)str[i];

        // Get the last character entered
        char lastChar = prefix[i];

        // Find the Node corresponding to the last
        // character of 'prefix' which is pointed by
        // prevNode of the Trie
        TrieNode *curNode = prevNode->child[lastChar];

        // If nothing found, then break the loop as
        // no more prefixes are going to be present.
        if (curNode == NULL)
        {
            cout << "nNo Results Found for "
                    " << prefix
                 << "" n ";
                i++;
            break;
        }

        // If present in trie then display all
        // the contacts with given prefix.
        cout << "nSuggestions based on "
                " << prefix
             << "" are n ";
            displayContactsUtil(curNode, prefix);

        // Change prevNode for next prefix
        prevNode = curNode;
    }

    // Once search fails for a prefix, we print
    // "Not Results Found" for all remaining
    // characters of current query string "str".
    for (; i < len; i++)
    {
        prefix += (char)str[i];
        cout << "nNo Results Found for "
                " << prefix
             << "" n ";
    }
}

// Insert all the Contacts into the Trie
void insertIntoTrie(string contacts[], int n)
{
    // Initialize root Node
    root = new TrieNode();

    // Insert each contact into the trie
    for (int i = 0; i < n; i++)
        insert(contacts[i]);
}

// Driver program to test above functions
int main()
{
    // Contact list of the User
    string contacts[] = {"gforgeeks", "geeksquiz"};

    // Size of the Contact List
    int n = sizeof(contacts) / sizeof(string);

    // Insert all the Contacts into Trie
    insertIntoTrie(contacts, n);

    string query = "gekk";

    // Note that the user will enter 'g' then 'e', so
    // first display all the strings with prefix as 'g'
    // and then all the strings with prefix as 'ge'
    displayContacts(query);

    return 0;
}