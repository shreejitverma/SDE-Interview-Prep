/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^2)
// Space: O(1) auxiliary

function rotate(matrix: number[][]): void {
    const n = matrix.length;

    // Step 1: Transpose matrix
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            const temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }

    // Step 2: Reverse each row horizontally
    for (let i = 0; i < n; i++) {
        let left = 0;
        let right = n - 1;
        while (left < right) {
            const temp = matrix[i][left];
            matrix[i][left] = matrix[i][right];
            matrix[i][right] = temp;
            left++;
            right--;
        }
    }
}
