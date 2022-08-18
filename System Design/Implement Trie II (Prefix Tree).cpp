/*
https://leetcode.com/problems/implement-trie-ii-prefix-tree/
1804. Implement Trie II (Prefix Tree)
Medium

101

7

Add to List

Share
A trie (pronounced as "try") or prefix tree is a tree data structure used to efficiently store and retrieve keys in a dataset of strings. There are various applications of this data structure, such as autocomplete and spellchecker.

Implement the Trie class:

Trie() Initializes the trie object.
void insert(String word) Inserts the string word into the trie.
int countWordsEqualTo(String word) Returns the number of instances of the string word in the trie.
int countWordsStartingWith(String prefix) Returns the number of strings in the trie that have the string prefix as a prefix.
void erase(String word) Erases the string word from the trie.
 

Example 1:

Input
["Trie", "insert", "insert", "countWordsEqualTo", "countWordsStartingWith", "erase", "countWordsEqualTo", "countWordsStartingWith", "erase", "countWordsStartingWith"]
[[], ["apple"], ["apple"], ["apple"], ["app"], ["apple"], ["apple"], ["app"], ["apple"], ["app"]]
Output
[null, null, null, 2, 2, null, 1, 1, null, 0]

Explanation
Trie trie = new Trie();
trie.insert("apple");               // Inserts "apple".
trie.insert("apple");               // Inserts another "apple".
trie.countWordsEqualTo("apple");    // There are two instances of "apple" so return 2.
trie.countWordsStartingWith("app"); // "app" is a prefix of "apple" so return 2.
trie.erase("apple");                // Erases one "apple".
trie.countWordsEqualTo("apple");    // Now there is only one instance of "apple" so return 1.
trie.countWordsStartingWith("app"); // return 1
trie.erase("apple");                // Erases "apple". Now the trie is empty.
trie.countWordsStartingWith("app"); // return 0
 

Constraints:

1 <= word.length, prefix.length <= 2000
word and prefix consist only of lowercase English letters.
At most 3 * 104 calls in total will be made to insert, countWordsEqualTo, countWordsStartingWith, and erase.
It is guaranteed that for any function call to erase, the string word will exist in the trie.
*/

// Time:  ctor:         O(1)
//        insert:       O(n)
//        count_word:   O(n)
//        count_prefix: O(n)
//        erase:        O(n)
// Space: O(t), t is the number of nodes in trie

class TrieNode
{
public:
    TrieNode() : children(26) {}
    vector<unique_ptr<TrieNode> > children;
    int pcnt = 0;
    int cnt = 0;
};

class Trie
{
public:
    Trie()
    {
        trie_ = make_unique<TrieNode>();
    }

    void insert(string word)
    {
        auto curr = trie_.get();
        ++curr->pcnt;
        for (const auto &c : word)
        {
            if (curr->children[c - 'a'] == nullptr)
            {
                curr->children[c - 'a'] = make_unique<TrieNode>();
            }
            curr = curr->children[c - 'a'].get();
            ++curr->pcnt;
        }
        ++curr->cnt;
    }

    int countWordsEqualTo(string word)
    {
        auto curr = trie_.get();
        for (const auto &c : word)
        {
            if (curr->children[c - 'a'] == nullptr)
            {
                return 0;
            }
            curr = curr->children[c - 'a'].get();
        }
        return curr->cnt;
    }

    int countWordsStartingWith(string prefix)
    {
        auto curr = trie_.get();
        for (const auto &c : prefix)
        {
            if (curr->children[c - 'a'] == nullptr)
            {
                return 0;
            }
            curr = curr->children[c - 'a'].get();
        }
        return curr->pcnt;
    }

    void erase(string word)
    {
        int cnt = countWordsEqualTo(word);
        if (!cnt)
        {
            return;
        }
        auto curr = trie_.get();
        --curr->pcnt;
        for (const auto &c : word)
        {
            if (curr->children[c - 'a']->pcnt == 1)
            {
                curr->children[c - 'a'].reset(); // delete all unused nodes
                return;
            }
            curr = curr->children[c - 'a'].get();
            --curr->pcnt;
        }
        --curr->cnt;
    }

private:
    unique_ptr<TrieNode> trie_;
};