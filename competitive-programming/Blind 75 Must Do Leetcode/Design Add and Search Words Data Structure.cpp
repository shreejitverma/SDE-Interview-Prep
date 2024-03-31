/*
https://leetcode.com/problems/design-add-and-search-words-data-structure/
211. Design Add and Search Words Data Structure
Medium

3595

150

Add to List

Share
Design a data structure that supports adding new words and finding if a string matches any previously added string.

Implement the WordDictionary class:

WordDictionary() Initializes the object.
void addWord(word) Adds word to the data structure, it can be matched later.
bool search(word) Returns true if there is any string in the data structure that matches word or false otherwise. word may contain dots '.' where dots can be matched with any letter.
 

Example:

Input
["WordDictionary","addWord","addWord","addWord","search","search","search","search"]
[[],["bad"],["dad"],["mad"],["pad"],["bad"],[".ad"],["b.."]]
Output
[null,null,null,null,false,true,true,true]

Explanation
WordDictionary wordDictionary = new WordDictionary();
wordDictionary.addWord("bad");
wordDictionary.addWord("dad");
wordDictionary.addWord("mad");
wordDictionary.search("pad"); // return False
wordDictionary.search("bad"); // return True
wordDictionary.search(".ad"); // return True
wordDictionary.search("b.."); // return True
 

Constraints:

1 <= word.length <= 500
word in addWord consists lower-case English letters.
word in search consist of  '.' or lower-case English letters.
At most 50000 calls will be made to addWord and search.
*/

//Runtime: 132 ms, faster than 50.37% of C++ online submissions for Add and Search Word - Data structure design.
//Memory Usage: 62.8 MB, less than 35.19% of C++ online submissions for Add and Search Word - Data structure design.

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

struct TrieNodeDepth
{
    TrieNode *node = NULL;
    int depth = 0;

    TrieNodeDepth()
    {
    }

    TrieNodeDepth(TrieNode *n, int d)
    {
        node = n;
        depth = d;
    }
};

class Trie
{
private:
    TrieNode *root;
    int R = 26;

    TrieNode *searchPrefix(string word, TrieNode *node = NULL)
    {
        //the default value of argument should be a const, not a variable
        //, so we set node = NULL in function's argument list
        //, and then set it to root later
        if (node == NULL)
            node = root;
        //fail to search all branches when c == '.'
        /**
        for(int i = 0; i < word.size(); i++){
            char c = word[i];
            if(c == '.'){
                for(int j = 0; j < R; j++){
                    if(node->containsKey('a'+j)){
                        TrieNode* subnode = searchPrefix(word.substr(j, word.size()), node->get('a'+j));
                        cout << "subnode" << i << " " << (char)('a'+j) << " " << (subnode != NULL) << endl;
                        if(subnode != NULL){
                            cout << "subnode is end: " << subnode->getEnd() << endl;
                            return subnode;
                        }
                    }
                }
            }else if(node->containsKey(c)){
                node = node->get(c);
            }else{
                return NULL;
            }
        }
        **/
        //should be initialized as NULL, if later we find better node, we update it
        TrieNode *ans = NULL;
        stack<TrieNodeDepth> stk;
        stk.push(TrieNodeDepth(node, 0));
        while (!stk.empty())
        {
            TrieNodeDepth tnd = stk.top();
            stk.pop();
            TrieNode *node = tnd.node;
            int depth = tnd.depth;
            if (depth == word.size())
            {
                if (node != NULL && node->getEnd())
                {
                    // cout << "return end node" << endl;
                    ans = node;
                    //this is the best kind of node, break so that ans won't be updated anymore
                    break;
                }
                else if (node != NULL)
                {
                    // cout << "not null node" << endl;
                    ans = node;
                    //continue to see if there is better node
                    continue; //don't need to push children anymore
                }
            }
            char c = word[depth];
            // cout << depth << " " << c << endl;
            if (c == '.')
            {
                //push all branches into stack
                for (int j = 0; j < R; j++)
                {
                    char tmpc = 'a' + j;
                    if (node->containsKey(tmpc))
                    {
                        stk.push(TrieNodeDepth(node->get(tmpc), depth + 1));
                    }
                }
            }
            else if (node->containsKey(c))
            {
                stk.push(TrieNodeDepth(node->get(c), depth + 1));
            }
            //cannot early return, because there could be better nodes still in the stack
            // else{
            //     return NULL;
            // }
        }
        return ans;
    }

public:
    Trie()
    {
        root = new TrieNode();
    }

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

    bool search(string word)
    {
        TrieNode *node = searchPrefix(word);
        // cout << "search: " << (node != NULL) << endl;
        // if(node != NULL) cout << "search, is end: " << (node->getEnd()) << endl;
        return node != NULL && node->getEnd();
    }

    bool startsWith(string prefix)
    {
        TrieNode *node = searchPrefix(prefix);
        return node != NULL;
    }
};

class WordDictionary
{
public:
    Trie *trie;

    /** Initialize your data structure here. */
    WordDictionary()
    {
        trie = new Trie();
    }

    /** Adds a word into the data structure. */
    void addWord(string word)
    {
        trie->insert(word);
    }

    /** Returns if the word is in the data structure. A word could contain the dot character '.' to represent any one letter. */
    bool search(string word)
    {
        return trie->search(word);
    }
};

/**
 * Your WordDictionary object will be instantiated and called as such:
 * WordDictionary* obj = new WordDictionary();
 * obj->addWord(word);
 * bool param_2 = obj->search(word);
 */

//solution from discussion
//https://leetcode.com/problems/add-and-search-word-data-structure-design/discuss/59554/My-simple-and-clean-Java-code
//Runtime: 192 ms, faster than 22.70% of C++ online submissions for Add and Search Word - Data structure design.
//Memory Usage: 111.5 MB, less than 6.70% of C++ online submissions for Add and Search Word - Data structure design.

class TrieNode
{
public:
    vector<TrieNode *> links;
    string item;

    TrieNode()
    {
        links = vector<TrieNode *>(26);
        item = "";
    }
};

class WordDictionary
{
public:
    TrieNode *root = new TrieNode();

    /** Initialize your data structure here. */
    WordDictionary()
    {
    }

    /** Adds a word into the data structure. */
    void addWord(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (node->links[c - 'a'] == NULL)
            {
                node->links[c - 'a'] = new TrieNode();
            }
            node = node->links[c - 'a'];
        }
        //mark it as end
        node->item = word;
    }

    bool match(string word, int depth, TrieNode *node)
    {
        //node->item != "" means the node is end
        if (depth == word.size())
            return (node->item != "");
        char c = word[depth];
        // cout << depth << " " << c << endl;
        if (c != '.')
        {
            //recursive search
            return (node->links[c - 'a'] != NULL) && match(word, depth + 1, node->links[c - 'a']);
        }
        else
        {
            //search for all its children
            for (int i = 0; i < node->links.size(); i++)
            {
                if (node->links[i] != NULL)
                {
                    if (match(word, depth + 1, node->links[i]))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /** Returns if the word is in the data structure. A word could contain the dot character '.' to represent any one letter. */
    bool search(string word)
    {
        return match(word, 0, root);
    }
};