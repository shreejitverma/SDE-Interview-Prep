/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

#include <string>
#include <vector>
#include <memory>
#include <array>

using namespace std;

class WordDictionary {
public:
    struct TrieNode {
        bool isEnd = false;
        array<unique_ptr<TrieNode>, 26> children{};
    };

    WordDictionary() : root_(make_unique<TrieNode>()) {}

    void addWord(const string& word) {
        auto* curr = root_.get();
        for (char ch : word) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                curr->children[idx] = make_unique<TrieNode>();
            }
            curr = curr->children[idx].get();
        }
        curr->isEnd = true;
    }

    bool search(const string& word) const {
        return searchHelper(word, 0, root_.get());
    }

private:
    bool searchHelper(const string& word, int index, const TrieNode* curr) const {
        if (!curr) return false;
        if (index == static_cast<int>(word.length())) {
            return curr->isEnd;
        }

        char ch = word[index];
        if (ch == '.') {
            for (const auto& child : curr->children) {
                if (child && searchHelper(word, index + 1, child.get())) {
                    return true;
                }
            }
            return false;
        } else {
            int idx = ch - 'a';
            return curr->children[idx] && searchHelper(word, index + 1, curr->children[idx].get());
        }
    }

    unique_ptr<TrieNode> root_;
};
