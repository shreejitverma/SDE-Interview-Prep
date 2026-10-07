/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 74 - Search a 2D Matrix
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(log(M * N))
 * - Space: O(1)
 */

function searchMatrix(matrix: number[][], target: number): boolean {
    if (matrix.length === 0 || matrix[0].length === 0) {
        return false;
    }

    const m = matrix.length;
    const n = matrix[0].length;
    let left = 0;
    let right = m * n - 1;

    while (left <= right) {
        const mid = Math.floor(left + (right - left) / 2);
        const row = Math.floor(mid / n);
        const col = mid % n;
        const val = matrix[row][col];

        if (val === target) {
            return true;
        } else if (val < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return false;
}
