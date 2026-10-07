/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Radix Sort
 * Time Complexity: O(d * (N + b)) where d is digits, b is base (10)
 * Space Complexity: O(N + b)
 */

export function radixSort(arr: number[]): number[] {
    if (arr.length <= 1) return arr;

    const maxVal = Math.max(...arr);

    for (let exp = 1; Math.floor(maxVal / exp) > 0; exp *= 10) {
        countSortByDigit(arr, exp);
    }
    return arr;
}

function countSortByDigit(arr: number[], exp: number): void {
    const n = arr.length;
    const output = new Array(n);
    const count = new Array(10).fill(0);

    for (let i = 0; i < n; i++) {
        const digit = Math.floor(arr[i] / exp) % 10;
        count[digit]++;
    }

    for (let i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (let i = n - 1; i >= 0; i--) {
        const digit = Math.floor(arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    for (let i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}
