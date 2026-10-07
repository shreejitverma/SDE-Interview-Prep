/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

function topKFrequent(nums: number[], k: number): number[] {
    const counts = new Map<number, number>();
    for (const num of nums) {
        counts.set(num, (counts.get(num) || 0) + 1);
    }

    const buckets: number[][] = Array.from({ length: nums.length + 1 }, () => []);
    for (const [num, freq] of counts.entries()) {
        buckets[freq].push(num);
    }

    const result: number[] = [];
    for (let i = buckets.length - 1; i >= 0 && result.length < k; i--) {
        for (const num of buckets[i]) {
            result.push(num);
            if (result.length === k) {
                return result;
            }
        }
    }

    return result;
}
