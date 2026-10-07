/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

import java.util.HashSet;
import java.util.Set;

class Solution {
    public boolean containsDuplicate(int[] nums) {
        Set<Integer> lookup = new HashSet<>();
        for (int num : nums) {
            if (!lookup.add(num)) {
                return true;
            }
        }
        return false;
    }
}
