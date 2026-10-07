/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(L) per insert / search / startsWith operation where L is string length
// Space: O(N * L) where N is number of words inserted

#include <string>
#include <array>
#include <memory>

class Trie {
private:
    struct TrieNode {
        std::array<std::unique_ptr<TrieNode>, 26> children{};
        bool is_end = false;
    };

    std::unique_ptr<TrieNode> root;

    const TrieNode* findPrefix(const std::string& prefix) const {
        const TrieNode* curr = root.get();
        for (char ch : prefix) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                return nullptr;
            }
            curr = curr->children[idx].get();
        }
        return curr;
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}

    void insert(const std::string& word) {
        TrieNode* curr = root.get();
        for (char ch : word) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                curr->children[idx] = std::make_unique<TrieNode>();
            }
            curr = curr->children[idx].get();
        }
        curr->is_end = true;
    }

    bool search(const std::string& word) const {
        const TrieNode* node = findPrefix(word);
        return node != nullptr && node->is_end;
    }

    bool startsWith(const std::string& prefix) const {
        return findPrefix(prefix) != nullptr;
    }
};
