/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Knuth-Morris-Pratt (KMP) Algorithm & Longest Prefix Suffix (LPS)
 * Standard: Modern C++20
 * Description: Linear-time O(N + M) exact string pattern matching using the
 *              Longest Proper Prefix which is also a Suffix (LPS) array with std::string_view.
 * 
 * Complexity:
 * - Preprocessing LPS: O(M) Time, O(M) Space
 * - Pattern Search: O(N) Time
 * - Total: O(N + M) Time, O(M) Space
 */

#include <iostream>
#include <vector>
#include <string_view>
#include <cassert>

class KMP {
public:
    // Computes the Longest Proper Prefix which is also a Suffix (LPS) array
    [[nodiscard]] static std::vector<size_t> compute_lps(std::string_view pattern) {
        size_t m = pattern.length();
        std::vector<size_t> lps(m, 0);
        size_t len = 0; // Length of the previous longest prefix suffix
        size_t i = 1;

        while (i < m) {
            if (pattern[i] == pattern[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) {
                    len = lps[len - 1]; // Fallback to smaller matching prefix
                } else {
                    lps[i] = 0;
                    i++;
                }
            }
        }
        return lps;
    }

    // Searches for all 0-indexed starting occurrences of pattern in text
    [[nodiscard]] static std::vector<size_t> search_all(std::string_view text, std::string_view pattern) {
        if (pattern.empty() || text.length() < pattern.length()) {
            return {};
        }

        std::vector<size_t> lps = compute_lps(pattern);
        std::vector<size_t> matches;

        size_t i = 0; // Index in text
        size_t j = 0; // Index in pattern
        size_t n = text.length();
        size_t m = pattern.length();

        while (i < n) {
            if (text[i] == pattern[j]) {
                i++;
                j++;
            }

            if (j == m) {
                matches.push_back(i - j); // Match found at starting index i - j
                j = lps[j - 1];
            } else if (i < n && text[i] != pattern[j]) {
                if (j != 0) {
                    j = lps[j - 1];
                } else {
                    i++;
                }
            }
        }

        return matches;
    }
};

int main() {
    std::string_view text = "AABAACAADAABAABA";
    std::string_view pattern = "AABA";

    auto occurrences = KMP::search_all(text, pattern);
    // "AABA" occurs at indices 0, 9, 12
    std::vector<size_t> expected = {0, 9, 12};
    assert(occurrences == expected);

    // No match test
    assert(KMP::search_all("ABCDEFG", "XYZ").empty());

    // Single character test
    assert(KMP::search_all("AAAA", "AA") == (std::vector<size_t>{0, 1, 2}));

    std::cout << "All KMP Pattern Search tests passed successfully.\n";
    return 0;
}
