/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: TimSort
 * Time Complexity: O(N log N) worst & average, O(N) best case
 * Space Complexity: O(N) auxiliary
 */

const RUN: usize = 32;

fn insertion_sort<T: Ord + Clone>(arr: &mut [T], left: usize, right: usize) {
    for i in (left + 1)..=right {
        let temp = arr[i].clone();
        let mut j = i;
        while j > left && arr[j - 1] > temp {
            arr[j] = arr[j - 1].clone();
            j -= 1;
        }
        arr[j] = temp;
    }
}

fn merge<T: Ord + Clone>(arr: &mut [T], l: usize, m: usize, r: usize) {
    let left = arr[l..=m].to_vec();
    let right = arr[(m + 1)..=r].to_vec();

    let (mut i, mut j, mut k) = (0, 0, l);

    while i < left.len() && j < right.len() {
        if left[i] <= right[j] {
            arr[k] = left[i].clone();
            i += 1;
        } else {
            arr[k] = right[j].clone();
            j += 1;
        }
        k += 1;
    }

    while i < left.len() {
        arr[k] = left[i].clone();
        i += 1;
        k += 1;
    }

    while j < right.len() {
        arr[k] = right[j].clone();
        j += 1;
        k += 1;
    }
}

pub fn tim_sort<T: Ord + Clone>(arr: &mut [T]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }

    let mut i = 0;
    while i < n {
        let right = (i + RUN - 1).min(n - 1);
        insertion_sort(arr, i, right);
        i += RUN;
    }

    let mut size = RUN;
    while size < n {
        let mut left = 0;
        while left < n {
            let mid = left + size - 1;
            let right = (left + 2 * size - 1).min(n - 1);
            if mid < right {
                merge(arr, left, mid, right);
            }
            left += 2 * size;
        }
        size *= 2;
    }
}
