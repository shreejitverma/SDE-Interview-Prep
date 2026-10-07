/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^(T/M + 1)) where N = candidates count, T = target, M = min(candidates)
// Space: O(T/M) auxiliary (recursion stack depth)

pub struct Solution;

impl Solution {
    pub fn combination_sum(mut candidates: Vec<i32>, target: i32) -> Vec<Vec<i32>> {
        candidates.sort_unstable();
        let mut result = Vec::new();
        let mut path = Vec::new();
        Self::backtrack(&candidates, target, 0, &mut path, &mut result);
        result
    }

    fn backtrack(
        candidates: &[i32],
        remain: i32,
        start: usize,
        path: &mut Vec<i32>,
        result: &mut Vec<Vec<i32>>,
    ) {
        if remain == 0 {
            result.push(path.clone());
            return;
        }

        for i in start..candidates.len() {
            if candidates[i] > remain {
                break;
            }
            path.push(candidates[i]);
            Self::backtrack(candidates, remain - candidates[i], i, path, result);
            path.pop();
        }
    }
}
