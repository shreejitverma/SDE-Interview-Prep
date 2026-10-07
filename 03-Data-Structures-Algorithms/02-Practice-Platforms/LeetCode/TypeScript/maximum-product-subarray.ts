/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

function maxProduct(nums: number[]): number {
    let maxProd = nums[0];
    let minProd = nums[0];
    let result = nums[0];
    for (let i = 1; i < nums.length; i++) {
        const x = nums[i];
        if (x < 0) {
            const temp = maxProd;
            maxProd = minProd;
            minProd = temp;
        }
        maxProd = Math.max(x, maxProd * x);
        minProd = Math.min(x, minProd * x);
        result = Math.max(result, maxProd);
    }
    return result;
}
