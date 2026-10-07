/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

function hammingWeight(n: number): number {
    let count = 0;
    while (n !== 0) {
        n = (n & (n - 1)) >>> 0;
        count++;
    }
    return count;
}
