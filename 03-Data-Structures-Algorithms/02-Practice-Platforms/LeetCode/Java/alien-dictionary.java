/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 269 - Alien Dictionary
 * Difficulty: Hard
 * Language: Java
 *
 * Performance Analysis:
 * - Time Complexity: O(C) where C is the total number of characters across all words in the input.
 *   Graph construction compares adjacent words; BFS topological sort processes at most 26 vertices.
 * - Space Complexity: O(1) auxiliary space, bounded by the English alphabet size (|V| <= 26, |E| <= 26^2).
 */

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;

public class Solution {

    public String alienOrder(String[] words) {
        if (words == null || words.length == 0) {
            return "";
        }

        // 1. Initialize graph structures for alphabet of size 26
        List<Integer>[] adj = new ArrayList[26];
        for (int i = 0; i < 26; ++i) {
            adj[i] = new ArrayList<>();
        }
        int[] inDegree = new int[26];
        boolean[] present = new boolean[26];
        int uniqueCharCount = 0;

        for (String w : words) {
            for (int i = 0; i < w.length(); ++i) {
                int c = w.charAt(i) - 'a';
                if (!present[c]) {
                    present[c] = true;
                    uniqueCharCount++;
                }
            }
        }

        // 2. Build directed edges by comparing adjacent words
        for (int i = 0; i < words.length - 1; ++i) {
            String w1 = words[i];
            String w2 = words[i + 1];

            // Invalid prefix case: e.g. ["abc", "ab"]
            if (w1.length() > w2.length() && w1.startsWith(w2)) {
                return "";
            }

            int minLen = Math.min(w1.length(), w2.length());
            for (int j = 0; j < minLen; ++j) {
                int c1 = w1.charAt(j) - 'a';
                int c2 = w2.charAt(j) - 'a';
                if (c1 != c2) {
                    adj[c1].add(c2);
                    inDegree[c2]++;
                    break; // Only the first differing character determines precedence
                }
            }
        }

        // 3. Kahn's Algorithm (BFS Topological Sort)
        Queue<Integer> queue = new ArrayDeque<>();
        for (int i = 0; i < 26; ++i) {
            if (present[i] && inDegree[i] == 0) {
                queue.offer(i);
            }
        }

        StringBuilder sb = new StringBuilder();
        while (!queue.isEmpty()) {
            int u = queue.poll();
            sb.append((char) ('a' + u));

            for (int v : adj[u]) {
                inDegree[v]--;
                if (inDegree[v] == 0) {
                    queue.offer(v);
                }
            }
        }

        // 4. Cycle check: if topological order length does not match unique chars, cycle exists
        if (sb.length() < uniqueCharCount) {
            return "";
        }

        return sb.toString();
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        String[] words1 = {"wrt", "wrf", "er", "ett", "rftt"};
        System.out.println("Order 1: " + sol.alienOrder(words1)); // "wertf"

        String[] words2 = {"z", "x"};
        System.out.println("Order 2: " + sol.alienOrder(words2)); // "zx"

        String[] words3 = {"z", "x", "z"};
        System.out.println("Order 3: " + sol.alienOrder(words3)); // "" (cycle)
    }
}
