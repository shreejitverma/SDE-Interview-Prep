/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: QuickSort
 * Time Complexity: O(N log N) average, O(N^2) worst case
 * Space Complexity: O(log N) stack
 */

pub fn quick_sort<T: Ord>(arr: &mut [T]) {
    let len = arr.len();
    if len <= 1 {
        return;
    }
    let pivot_idx = partition(arr);
    quick_sort(&mut arr[0..pivot_idx]);
    quick_sort(&mut arr[pivot_idx + 1..len]);
}

fn partition<T: Ord>(arr: &mut [T]) -> usize {
    let len = arr.len();
    let pivot_idx = len - 1;
    let mut i = 0;

    for j in 0..len - 1 {
        if arr[j] <= arr[pivot_idx] {
            arr.swap(i, j);
            i += 1;
        }
    }
    arr.swap(i, pivot_idx);
    i
}
