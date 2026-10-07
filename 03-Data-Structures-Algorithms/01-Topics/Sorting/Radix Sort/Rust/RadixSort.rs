/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: Radix Sort
 * Time Complexity: O(d * (N + b)) where d is digits, b is base (10)
 * Space Complexity: O(N + b)
 */

pub fn radix_sort(arr: &mut [u32]) {
    if arr.len() <= 1 {
        return;
    }

    let max_val = *arr.iter().max().unwrap();
    let mut exp = 1u32;

    while max_val / exp > 0 {
        count_sort_by_digit(arr, exp);
        if let Some(next_exp) = exp.checked_mul(10) {
            exp = next_exp;
        } else {
            break;
        }
    }
}

fn count_sort_by_digit(arr: &mut [u32], exp: u32) {
    let n = arr.len();
    let mut output = vec![0u32; n];
    let mut count = [0usize; 10];

    for &val in arr.iter() {
        let digit = ((val / exp) % 10) as usize;
        count[digit] += 1;
    }

    for i in 1..10 {
        count[i] += count[i - 1];
    }

    for &val in arr.iter().rev() {
        let digit = ((val / exp) % 10) as usize;
        count[digit] -= 1;
        output[count[digit]] = val;
    }

    arr.copy_from_slice(&output);
}
