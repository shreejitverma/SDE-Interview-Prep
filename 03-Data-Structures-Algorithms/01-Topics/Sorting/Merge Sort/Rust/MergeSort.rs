/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Merge Sort
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

pub fn merge_sort<T: Ord + Clone>(arr: &[T]) -> Vec<T> {
    let len = arr.len();
    if len <= 1 {
        return arr.to_vec();
    }
    let mid = len / 2;
    let left = merge_sort(&arr[0..mid]);
    let right = merge_sort(&arr[mid..len]);
    merge(&left, &right)
}

fn merge<T: Ord + Clone>(left: &[T], right: &[T]) -> Vec<T> {
    let mut result = Vec::with_capacity(left.len() + right.len());
    let (mut i, mut j) = (0, 0);

    while i < left.len() && j < right.len() {
        if left[i] <= right[j] {
            result.push(left[i].clone());
            i += 1;
        } else {
            result.push(right[j].clone());
            j += 1;
        }
    }
    while i < left.len() {
        result.push(left[i].clone());
        i += 1;
    }
    while j < right.len() {
        result.push(right[j].clone());
        j += 1;
    }
    result
}
