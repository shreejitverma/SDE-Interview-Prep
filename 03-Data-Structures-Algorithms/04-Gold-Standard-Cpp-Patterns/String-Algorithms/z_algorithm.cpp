/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Z-Algorithm for Linear-Time String Matching
 * Standard: Modern C++20
 * Description: Computes the Z-array in O(N) linear time, where Z[i] is the length of the
 *              longest substring starting from S[i] that matches the prefix of S.
 *              Used for exact pattern search, periodic string analysis, and string compression.
 * 
 * Complexity:
 * - Z-Array Construction: O(N) linear time
 * - Pattern Matching: O(N + M) linear time where N is text length, M is pattern length
 * - Space: O(N) auxiliary vector for Z-array
 */

#include <iostream>
#include <vector>
#include <string_view>
#include <string>
#include <cassert>

class ZAlgorithm {
public:
    // Computes the Z-array for a given string view in strictly O(N) time
    [[nodiscard]] static std::vector<size_t> compute_z(std::string_view s) {
        const size_t n = s.size();
        std::vector<size_t> z(n, 0);
        if (n == 0) return z;

        z[0] = n;
        size_t l = 0;
        size_t r = 0;

        for (size_t i = 1; i < n; ++i) {
            if (i < r) {
                // Inside current Z-box [l, r)
                z[i] = std::min(r - i, z[i - l]);
            }
            // Attempt to expand beyond r
            while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
                ++z[i];
            }
            // Update Z-box boundaries if we extended further right
            if (i + z[i] > r) {
                l = i;
                r = i + z[i];
            }
        }
        return z;
    }

    // Finds all 0-indexed starting occurrences of pattern in text in O(N + M) time
    [[nodiscard]] static std::vector<size_t> search(std::string_view text, std::string_view pattern) {
        if (pattern.empty() || text.size() < pattern.size()) {
            return {};
        }

        // Delimiter character guaranteed not to appear in typical alphabet
        // Concatenate pattern + '$' + text
        std::string combined;
        combined.reserve(pattern.size() + 1 + text.size());
        combined.append(pattern);
        combined.push_back('\0'); // Using null char as unique delimiter
        combined.append(text);

        const auto z = compute_z(combined);
        const size_t m = pattern.size();
        const size_t offset = m + 1;

        std::vector<size_t> matches;
        for (size_t i = offset; i < combined.size(); ++i) {
            if (z[i] >= m) {
                matches.push_back(i - offset);
            }
        }
        return matches;
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // 1. Z-array basic test
    // String: a a b z a a b z a
    // Z:      9 1 0 0 5 1 0 0 1
    std::string_view s = "aabzaabza";
    auto z = ZAlgorithm::compute_z(s);
    assert(z[0] == 9);
    assert(z[1] == 1);
    assert(z[2] == 0);
    assert(z[3] == 0);
    assert(z[4] == 5);
    assert(z[5] == 1);

    // 2. Pattern Matching test
    std::string_view text = "abracadabracoabraca";
    std::string_view pattern = "abraca";
    auto matches = ZAlgorithm::search(text, pattern);
    // Occurrences at index 0 and index 13
    assert(matches.size() == 2);
    assert(matches[0] == 0);
    assert(matches[1] == 13);

    // 3. Overlapping matches test
    std::string_view text_overlap = "aaaaaa";
    std::string_view pat_overlap = "aaa";
    auto matches_overlap = ZAlgorithm::search(text_overlap, pat_overlap);
    // Occurrences at index 0, 1, 2, 3
    assert(matches_overlap.size() == 4);
    assert(matches_overlap[0] == 0);
    assert(matches_overlap[1] == 1);
    assert(matches_overlap[2] == 2);
    assert(matches_overlap[3] == 3);

    // 4. No match test
    auto matches_none = ZAlgorithm::search("abcdefg", "xyz");
    assert(matches_none.empty());

    // 5. Pattern equals text
    auto matches_exact = ZAlgorithm::search("hello", "hello");
    assert(matches_exact.size() == 1 && matches_exact[0] == 0);

    std::cout << "All Z-Algorithm C++20 tests passed successfully." << std::endl;
    return 0;
}
