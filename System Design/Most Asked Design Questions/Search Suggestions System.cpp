/*
https://leetcode.com/problems/search-suggestions-system/
1268. Search Suggestions System
Medium

1661

110

Add to List

Share
Given an array of strings products and a string searchWord. We want to design a system that suggests at most three product names from products after each character of searchWord is typed. Suggested products should have common prefix with the searchWord. If there are more than three products with a common prefix return the three lexicographically minimums products.

Return list of lists of the suggested products after each character of searchWord is typed. 

 

Example 1:

Input: products = ["mobile","mouse","moneypot","monitor","mousepad"], searchWord = "mouse"
Output: [
["mobile","moneypot","monitor"],
["mobile","moneypot","monitor"],
["mouse","mousepad"],
["mouse","mousepad"],
["mouse","mousepad"]
]
Explanation: products sorted lexicographically = ["mobile","moneypot","monitor","mouse","mousepad"]
After typing m and mo all products match and we show user ["mobile","moneypot","monitor"]
After typing mou, mous and mouse the system suggests ["mouse","mousepad"]
Example 2:

Input: products = ["havana"], searchWord = "havana"
Output: [["havana"],["havana"],["havana"],["havana"],["havana"],["havana"]]
Example 3:

Input: products = ["bags","baggage","banner","box","cloths"], searchWord = "bags"
Output: [["baggage","bags","banner"],["baggage","bags","banner"],["baggage","bags"],["bags"]]
Example 4:

Input: products = ["havana"], searchWord = "tatiana"
Output: [[],[],[],[],[],[],[]]
 

Constraints:

1 <= products.length <= 1000
There are no repeated elements in products.
1 <= Σ products[i].length <= 2 * 10^4
All characters of products[i] are lower-case English letters.
1 <= searchWord.length <= 1000
All characters of searchWord are lower-case English letters.
*/

// Time:  ctor: O(n * l), n is the number of products
//                      , l is the average length of product name
//        suggest: O(l^2)
// Space: O(t), t is the number of nodes in trie

class Solution
{
public:
    vector<vector<string> > suggestedProducts(vector<string> &products, string searchWord)
    {
        TrieNode trie;
        for (int i = 0; i < products.size(); ++i)
        {
            trie.insert(products, i);
        }
        auto curr = &trie;
        vector<vector<string> > result(searchWord.length());
        for (int i = 0; i < searchWord.length(); ++i)
        { // Time: O(l)
            if (!curr->leaves.count(searchWord[i]))
            {
                break;
            }
            curr = curr->leaves[searchWord[i]];
            for (const auto &j : curr->infos)
            {
                result[i].emplace_back(products[j]);
            }
        }
        return result;
    }

    class TrieNode
    {
    public:
        static const int TOP_COUNT = 3;

        ~TrieNode()
        {
            for (auto &kv : leaves)
            {
                if (kv.second)
                {
                    delete kv.second;
                }
            }
        }

        // Time:  O(l)
        void insert(const vector<string> &words, int i)
        {
            auto *curr = this;
            for (const auto &c : words[i])
            {
                if (!curr->leaves.count(c))
                {
                    curr->leaves[c] = new TrieNode;
                }
                curr = curr->leaves[c];
                curr->add_info(words, i);
            }
        }

        // Time:  O(l)
        void add_info(const vector<string> &words, int i)
        {
            infos.emplace_back(i);
            sort(infos.begin(), infos.end(),
                 [&words](const auto &a, const auto &b)
                 {
                     return words[a] < words[b];
                 });
            if (infos.size() > TOP_COUNT)
            {
                infos.pop_back();
            }
        }

        vector<int> infos;
        unordered_map<char, TrieNode *> leaves;
    };
};

// Time:  ctor: O(n * l * log(n * l)), n is the number of products
//                                   , l is the average length of product name
//        suggest: O(l^2)
// Space: O(t), t is the number of nodes in trie
class Solution2
{
public:
    vector<vector<string> > suggestedProducts(vector<string> &products, string searchWord)
    {
        sort(products.begin(), products.end()); // Time: O(n * l * log(n * l))
        TrieNode trie;
        for (int i = 0; i < products.size(); ++i)
        {
            trie.insert(products, i);
        }
        auto curr = &trie;
        vector<vector<string> > result(searchWord.length());
        for (int i = 0; i < searchWord.length(); ++i)
        { // Time: O(l)
            if (!curr->leaves.count(searchWord[i]))
            {
                break;
            }
            curr = curr->leaves[searchWord[i]];
            for (const auto &j : curr->infos)
            {
                result[i].emplace_back(products[j]);
            }
        }
        return result;
    }

    class TrieNode
    {
    public:
        static const int TOP_COUNT = 3;

        ~TrieNode()
        {
            for (auto &kv : leaves)
            {
                if (kv.second)
                {
                    delete kv.second;
                }
            }
        }

        // Time:  O(l)
        void insert(const vector<string> &words, int i)
        {
            auto *curr = this;
            for (const auto &c : words[i])
            {
                if (!curr->leaves.count(c))
                {
                    curr->leaves[c] = new TrieNode;
                }
                curr = curr->leaves[c];
                curr->add_info(words, i);
            }
        }

        // Time:  O(1)
        void add_info(const vector<string> &words, int i)
        {
            if (infos.size() == TOP_COUNT)
            {
                return;
            }
            infos.emplace_back(i);
        }

        vector<int> infos;
        unordered_map<char, TrieNode *> leaves;
    };
};

// Time:  ctor: O(n * l * log(n * l)), n is the number of products
//                                   , l is the average length of product name
//        suggest: O(l^2 * n)
// Space: O(n * l)
class Solution3
{
public:
    vector<vector<string> > suggestedProducts(vector<string> &products, string searchWord)
    {
        sort(products.begin(), products.end()); // Time: O(n * l * log(n * l))
        vector<vector<string> > result;
        string prefix;
        for (int i = 0; i < searchWord.length(); ++i)
        { // Time: O(l)
            prefix += searchWord[i];
            int start = distance(products.cbegin(),
                                 lower_bound(products.cbegin(),
                                             products.cend(),
                                             prefix)); // Time: O(log(n * l))
            vector<string> new_products;
            for (int j = start; j < products.size(); ++j)
            { // Time: O(n * l)
                if (!(i < products[j].length() && products[j][i] == searchWord[i]))
                {
                    break;
                }
                new_products.emplace_back(products[j]);
            }
            products = move(new_products);
            result.emplace_back();
            for (int j = 0; j < min(int(products.size()), 3); ++j)
            {
                result.back().emplace_back(products[j]);
            };
        }
        return result;
    }
};