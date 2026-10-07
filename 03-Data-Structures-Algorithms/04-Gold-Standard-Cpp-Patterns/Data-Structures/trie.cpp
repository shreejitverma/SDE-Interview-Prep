/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Gold-Standard Trie (String Prefix Trie & Bitwise 0-1 Trie)
 * Standard: Modern C++20
 * Description: High-performance string prefix trie supporting prefix counts, autocomplete matching,
 *              and a 0-1 bitwise Trie for maximum XOR pair queries in O(32) time.
 * 
 * Complexity:
 * - String Trie:
 *   - Insert: O(L) Time, where L is word length
 *   - Search / StartsWith: O(L) Time
 *   - Space: O(Sigma * Total_Characters)
 * - Bitwise Trie:
 *   - Insert: O(32) Time
 *   - Find Max XOR: O(32) Time
 *   - Space: O(32 * N)
 */

#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include <cassert>
#include <algorithm>

// --- Part 1: String Prefix Trie ---
class PrefixTrie {
public:
    struct Node {
        std::vector<std::unique_ptr<Node>> children;
        int word_count = 0;   // Number of words ending at this node
        int prefix_count = 0; // Number of words passing through this node

        Node() : children(26) {}
    };

    PrefixTrie() : root_(std::make_unique<Node>()) {}

    void insert(std::string_view word) {
        Node* curr = root_.get();
        curr->prefix_count++;
        for (char ch : word) {
            size_t idx = static_cast<size_t>(ch - 'a');
            assert(idx < 26 && "Only lowercase English letters supported");
            if (!curr->children[idx]) {
                curr->children[idx] = std::make_unique<Node>();
            }
            curr = curr->children[idx].get();
            curr->prefix_count++;
        }
        curr->word_count++;
    }

    [[nodiscard]] bool contains(std::string_view word) const {
        const Node* node = find_node(word);
        return node != nullptr && node->word_count > 0;
    }

    [[nodiscard]] bool starts_with(std::string_view prefix) const {
        return find_node(prefix) != nullptr;
    }

    [[nodiscard]] int count_words_with_prefix(std::string_view prefix) const {
        const Node* node = find_node(prefix);
        return node ? node->prefix_count : 0;
    }

private:
    std::unique_ptr<Node> root_;

    [[nodiscard]] const Node* find_node(std::string_view prefix) const {
        const Node* curr = root_.get();
        for (char ch : prefix) {
            size_t idx = static_cast<size_t>(ch - 'a');
            if (idx >= 26 || !curr->children[idx]) {
                return nullptr;
            }
            curr = curr->children[idx].get();
        }
        return curr;
    }
};

// --- Part 2: Bitwise 0-1 Trie (Maximum XOR Pair) ---
class BitwiseXORTrie {
public:
    struct Node {
        std::unique_ptr<Node> children[2];
    };

    BitwiseXORTrie() : root_(std::make_unique<Node>()) {}

    void insert(uint32_t num) {
        Node* curr = root_.get();
        for (int i = 31; i >= 0; --i) {
            uint32_t bit = (num >> i) & 1U;
            if (!curr->children[bit]) {
                curr->children[bit] = std::make_unique<Node>();
            }
            curr = curr->children[bit].get();
        }
    }

    // Finds the maximum XOR value obtainable between num and any previously inserted number
    [[nodiscard]] uint32_t find_max_xor(uint32_t num) const {
        const Node* curr = root_.get();
        uint32_t max_xor = 0;

        for (int i = 31; i >= 0; --i) {
            uint32_t bit = (num >> i) & 1U;
            uint32_t desired = 1U - bit; // Greedily look for opposite bit
            if (curr->children[desired]) {
                max_xor |= (1U << i);
                curr = curr->children[desired].get();
            } else if (curr->children[bit]) {
                curr = curr->children[bit].get();
            } else {
                break;
            }
        }
        return max_xor;
    }

private:
    std::unique_ptr<Node> root_;
};

int main() {
    // 1. Test String Prefix Trie
    PrefixTrie trie;
    trie.insert("apple");
    trie.insert("app");
    trie.insert("application");
    trie.insert("banana");

    assert(trie.contains("apple"));
    assert(trie.contains("app"));
    assert(!trie.contains("appl"));
    assert(trie.starts_with("app"));
    assert(trie.count_words_with_prefix("app") == 3);
    assert(trie.count_words_with_prefix("ban") == 1);
    assert(trie.count_words_with_prefix("cat") == 0);

    // 2. Test Bitwise 0-1 Trie
    BitwiseXORTrie xor_trie;
    std::vector<uint32_t> nums = {3, 10, 5, 25, 2, 8};
    for (uint32_t x : nums) {
        xor_trie.insert(x);
    }

    uint32_t max_xor = 0;
    for (uint32_t x : nums) {
        max_xor = std::max(max_xor, xor_trie.find_max_xor(x));
    }
    // 5 XOR 25 = 28
    assert(max_xor == 28);

    std::cout << "All Prefix Trie and Bitwise Trie tests passed successfully.\n";
    return 0;
}
