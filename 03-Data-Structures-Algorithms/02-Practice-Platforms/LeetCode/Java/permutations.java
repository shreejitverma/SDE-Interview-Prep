/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * N!)
// Space: O(N) auxiliary (recursion stack depth)

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class Solution {
    public List<List<Integer>> permute(int[] nums) {
        List<List<Integer>> result = new ArrayList<>();
        List<Integer> current = new ArrayList<>();
        for (int num : nums) {
            current.add(num);
        }
        backtrack(current, 0, result);
        return result;
    }

    private void backtrack(List<Integer> current, int first, List<List<Integer>> result) {
        if (first == current.size()) {
            result.add(new ArrayList<>(current));
            return;
        }

        for (int i = first; i < current.size(); i++) {
            Collections.swap(current, first, i);
            backtrack(current, first + 1, result);
            Collections.swap(current, first, i);
        }
    }
}
