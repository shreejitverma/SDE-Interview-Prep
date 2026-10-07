/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Counting Sort
 * Time Complexity: O(N + K)
 * Space Complexity: O(N + K)
 */

pub fn counting_sort(arr: &[i32]) -> Vec<i32> {
    if arr.is_empty() {
        return Vec::new();
    }

    let min_val = *arr.iter().min().unwrap();
    let max_val = *arr.iter().max().unwrap();
    let range = (max_val - min_val + 1) as usize;

    let mut count = vec![0usize; range];
    for &v in arr {
        count[(v - min_val) as usize] += 1;
    }

    for i in 1..range {
        count[i] += count[i - 1];
    }

    let mut output = vec![0i32; arr.len()];
    for &v in arr.iter().rev() {
        let idx = (v - min_val) as usize;
        count[idx] -= 1;
        output[count[idx]] = v;
    }

    output
}
