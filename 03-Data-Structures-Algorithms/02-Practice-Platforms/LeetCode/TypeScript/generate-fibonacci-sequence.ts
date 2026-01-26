/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

// dp
function* fibGenerator(): Generator<number, any, number> {
    let a = 0, b = 1;
    for (; true; [a, b] = [b, a+b]) {
        yield a;
    }
};
