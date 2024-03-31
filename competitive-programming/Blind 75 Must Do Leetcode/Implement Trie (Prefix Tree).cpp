/*

https://leetcode.com/problems/implement-trie-prefix-tree/
208. Implement Trie (Prefix Tree)
Medium

5780

82

Add to List

Share
A trie (pronounced as "try") or prefix tree is a tree data structure used to efficiently store and retrieve keys in a dataset of strings. There are various applications of this data structure, such as autocomplete and spellchecker.

Implement the Trie class:

Trie() Initializes the trie object.
void insert(String word) Inserts the string word into the trie.
boolean search(String word) Returns true if the string word is in the trie (i.e., was inserted before), and false otherwise.
boolean startsWith(String prefix) Returns true if there is a previously inserted string word that has the prefix prefix, and false otherwise.
 

Example 1:

Input
["Trie", "insert", "search", "search", "startsWith", "insert", "search"]
[[], ["apple"], ["apple"], ["app"], ["app"], ["app"], ["app"]]
Output
[null, null, true, false, true, null, true]

Explanation
Trie trie = new Trie();
trie.insert("apple");
trie.search("apple");   // return True
trie.search("app");     // return False
trie.startsWith("app"); // return True
trie.insert("app");
trie.search("app");     // return True
 

Constraints:

1 <= word.length, prefix.length <= 2000
word and prefix consist only of lowercase English letters.
At most 3 * 104 calls in total will be made to insert, search, and startsWith.
*/

//Runtime: 548 ms, faster than 5.04% of C++ online submissions for Implement Trie (Prefix Tree).
//Memory Usage: 23.9 MB, less than 100.00% of C++ online submissions for Implement Trie (Prefix Tree).

class Trie
{
public:
    set<string> words;
    /** Initialize your data structure here. */
    Trie()
    {
    }

    /** Inserts a word into the trie. */
    void insert(string word)
    {
        words.insert(word);
    }

    /** Returns if the word is in the trie. */
    bool search(string word)
    {
        return words.find(word) != words.end();
    }

    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix)
    {
        for (set<string>::iterator it = words.begin(); it != words.end(); it++)
        {
            if ((*it).rfind(prefix, 0) == 0)
                return true;
        }
        return false;
    }
};

/**
 * Your Trie object will be instantiated and called as such:
 * Trie* obj = new Trie();
 * obj->insert(word);
 * bool param_2 = obj->search(word);
 * bool param_3 = obj->startsWith(prefix);
 */

//Runtime: 92 ms, faster than 59.89% of C++ online submissions for Implement Trie (Prefix Tree).
//Memory Usage: 48 MB, less than 30.86% of C++ online submissions for Implement Trie (Prefix Tree).
class TrieNode
{
private:
    vector<TrieNode *> links;
    int R = 26;
    bool isEnd = false;

public:
    TrieNode()
    {
        links = vector<TrieNode *>(R);
    }

    bool containsKey(char c)
    {
        return links[c - 'a'] != NULL;
    }

    TrieNode *get(char c)
    {
        return links[c - 'a'];
    }

    void put(char c, TrieNode *node)
    {
        links[c - 'a'] = node;
    }

    void setEnd()
    {
        isEnd = true;
    }

    bool getEnd()
    {
        return isEnd;
    }
};

class Trie
{
private:
    TrieNode *root;

    // search a prefix or whole key in trie and
    // returns the node where search ends
    TrieNode *searchPrefix(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (node->containsKey(c))
            {
                node = node->get(c);
            }
            else
            {
                return NULL;
            }
        }
        return node;
    }

public:
    /** Initialize your data structure here. */
    Trie()
    {
        root = new TrieNode();
    }

    /** Inserts a word into the trie. */
    //time: O(m), space: O(m), m is the key length.
    void insert(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (!node->containsKey(c))
            {
                node->put(c, new TrieNode());
            }
            node = node->get(c);
        }
        node->setEnd();
    }

    /** Returns if the word is in the trie. */
    //time: O(m), space: O(1)
    bool search(string word)
    {
        TrieNode *node = searchPrefix(word);
        return node != NULL && node->getEnd();
    }

    /** Returns if there is any word in the trie that starts with the given prefix. */
    //time: O(m), space: O(1)
    bool startsWith(string prefix)
    {
        TrieNode *node = searchPrefix(prefix);
        return node != NULL;
    }
};

//Runtime: 124 ms, faster than 45.63% of C++ online submissions for Implement Trie (Prefix Tree).
//Memory Usage: 48.1 MB, less than 16.67% of C++ online submissions for Implement Trie (Prefix Tree).
class TrieNode
{
public:
    bool isEnd;
    vector<TrieNode *> children;
    char c;

    TrieNode(char c = '\0')
    {
        isEnd = false;
        this->c = c;
        children = vector<TrieNode *>(26, nullptr);
    }
};

class Trie
{
public:
    TrieNode *root;

    /** Initialize your data structure here. */
    Trie()
    {
        root = new TrieNode();
    }

    /** Inserts a word into the trie. */
    void insert(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (node->children[c - 'a'] == nullptr)
            {
                node->children[c - 'a'] = new TrieNode(c);
            }
            node = node->children[c - 'a'];
        }
        node->isEnd = true;
    }

    /** Returns if the word is in the trie. */
    bool search(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (node->children[c - 'a'] == nullptr)
                return false;
            node = node->children[c - 'a'];
        }
        return node->isEnd;
    }

    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix)
    {
        TrieNode *node = root;
        for (char c : prefix)
        {
            if (node->children[c - 'a'] == nullptr)
                return false;
            node = node->children[c - 'a'];
        }
        return true;
    }
};

/**
 * Your Trie object will be instantiated and called as such:
 * Trie* obj = new Trie();
 * obj->insert(word);
 * bool param_2 = obj->search(word);
 * bool param_3 = obj->startsWith(prefix);
 */