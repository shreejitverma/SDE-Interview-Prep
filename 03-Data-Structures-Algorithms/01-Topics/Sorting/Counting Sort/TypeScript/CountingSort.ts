/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Counting Sort
 * Time Complexity: O(N + K)
 * Space Complexity: O(N + K)
 */

export function countingSort(arr: number[]): number[] {
    if (arr.length === 0) {
        return arr;
    }

    let minVal = arr[0];
    let maxVal = arr[0];
    for (const v of arr) {
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }

    const k = maxVal - minVal + 1;
    const count = new Array(k).fill(0);
    for (const v of arr) {
        count[v - minVal]++;
    }

    for (let i = 1; i < k; i++) {
        count[i] += count[i - 1];
    }

    const output = new Array(arr.length);
    for (let i = arr.length - 1; i >= 0; i--) {
        output[count[arr[i] - minVal] - 1] = arr[i];
        count[arr[i] - minVal]--;
    }

    return output;
}
