/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

function containsDuplicate(nums: number[]): boolean {
    const lookup = new Set<number>();
    for (const num of nums) {
        if (lookup.has(num)) {
            return true;
        }
        lookup.add(num);
    }
    return false;
}
