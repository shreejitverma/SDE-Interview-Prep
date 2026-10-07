/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) or O(N) auxiliary

function eraseOverlapIntervals(intervals: number[][]): number {
    if (intervals.length === 0) {
        return 0;
    }

    // Sort by end time ascending
    intervals.sort((a, b) => a[1] - b[1]);

    let removals = 0;
    let prevEnd = intervals[0][1];

    for (let i = 1; i < intervals.length; i++) {
        if (intervals[i][0] < prevEnd) {
            removals++;
        } else {
            prevEnd = intervals[i][1];
        }
    }

    return removals;
}
