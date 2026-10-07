/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(1) auxiliary

function merge(intervals: number[][]): number[][] {
    if (intervals.length <= 1) return intervals;

    intervals.sort((a, b) => a[0] - b[0]);
    const result: number[][] = [intervals[0]];

    for (let i = 1; i < intervals.length; i++) {
        const curr = intervals[i];
        const last = result[result.length - 1];

        if (curr[0] <= last[1]) {
            last[1] = Math.max(last[1], curr[1]);
        } else {
            result.push(curr);
        }
    }

    return result;
}
