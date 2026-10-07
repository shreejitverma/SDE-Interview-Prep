/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  add_num: O(log N), find_median: O(1)
// Space: O(N)

use std::cmp::Reverse;
use std::collections::BinaryHeap;

pub struct MedianFinder {
    max_heap: BinaryHeap<i32>,
    min_heap: BinaryHeap<Reverse<i32>>,
}

impl MedianFinder {
    pub fn new() -> Self {
        MedianFinder {
            max_heap: BinaryHeap::new(),
            min_heap: BinaryHeap::new(),
        }
    }

    pub fn add_num(&mut self, num: i32) {
        if self.max_heap.is_empty() || num <= *self.max_heap.peek().unwrap() {
            self.max_heap.push(num);
        } else {
            self.min_heap.push(Reverse(num));
        }

        if self.max_heap.len() > self.min_heap.len() + 1 {
            let val = self.max_heap.pop().unwrap();
            self.min_heap.push(Reverse(val));
        } else if self.min_heap.len() > self.max_heap.len() {
            let Reverse(val) = self.min_heap.pop().unwrap();
            self.max_heap.push(val);
        }
    }

    pub fn find_median(&self) -> f64 {
        if self.max_heap.len() > self.min_heap.len() {
            *self.max_heap.peek().unwrap() as f64
        } else {
            let top_max = *self.max_heap.peek().unwrap() as f64;
            let Reverse(top_min) = *self.min_heap.peek().unwrap();
            (top_max + top_min as f64) / 2.0
        }
    }
}
