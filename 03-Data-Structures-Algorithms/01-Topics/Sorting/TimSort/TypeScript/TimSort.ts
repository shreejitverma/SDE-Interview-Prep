/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: TimSort
 * Time Complexity: O(N log N) worst & average, O(N) best case
 * Space Complexity: O(N) auxiliary
 */

const RUN = 32;

function insertionSort(arr: number[], left: number, right: number): void {
    for (let i = left + 1; i <= right; i++) {
        const temp = arr[i];
        let j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

function merge(arr: number[], l: number, m: number, r: number): void {
    const left = arr.slice(l, m + 1);
    const right = arr.slice(m + 1, r + 1);
    let i = 0;
    let j = 0;
    let k = l;

    while (i < left.length && j < right.length) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }
    while (i < left.length) arr[k++] = left[i++];
    while (j < right.length) arr[k++] = right[j++];
}

export function timSort(arr: number[]): void {
    const n = arr.length;

    for (let i = 0; i < n; i += RUN) {
        insertionSort(arr, i, Math.min(i + RUN - 1, n - 1));
    }

    for (let size = RUN; size < n; size *= 2) {
        for (let left = 0; left < n; left += 2 * size) {
            const mid = left + size - 1;
            const right = Math.min(left + 2 * size - 1, n - 1);
            if (mid < right) {
                merge(arr, left, mid, right);
            }
        }
    }
}
