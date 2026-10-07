/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Build O(N), Query O(log N), Update O(log N)
// Space Complexity: O(N)

pub struct SegmentTree {
    tree: Vec<i64>,
    n: usize,
}

impl SegmentTree {
    pub fn new(arr: &[i64]) -> Self {
        let n = arr.len();
        let mut st = SegmentTree {
            tree: vec![0; 4 * n],
            n,
        };
        if n > 0 {
            st.build(arr, 1, 0, n - 1);
        }
        st
    }

    fn build(&mut self, arr: &[i64], node: usize, start: usize, end: usize) {
        if start == end {
            self.tree[node] = arr[start];
            return;
        }
        let mid = (start + end) / 2;
        self.build(arr, 2 * node, start, mid);
        self.build(arr, 2 * node + 1, mid + 1, end);
        self.tree[node] = self.tree[2 * node] + self.tree[2 * node + 1];
    }

    pub fn update(&mut self, idx: usize, val: i64) {
        if self.n > 0 {
            self.update_node(1, 0, self.n - 1, idx, val);
        }
    }

    fn update_node(&mut self, node: usize, start: usize, end: usize, idx: usize, val: i64) {
        if start == end {
            self.tree[node] = val;
            return;
        }
        let mid = (start + end) / 2;
        if idx <= mid {
            self.update_node(2 * node, start, mid, idx, val);
        } else {
            self.update_node(2 * node + 1, mid + 1, end, idx, val);
        }
        self.tree[node] = self.tree[2 * node] + self.tree[2 * node + 1];
    }

    pub fn query(&self, left: usize, right: usize) -> i64 {
        if self.n == 0 || left > right || right >= self.n {
            return 0;
        }
        self.query_node(1, 0, self.n - 1, left, right)
    }

    fn query_node(&self, node: usize, start: usize, end: usize, l: usize, r: usize) -> i64 {
        if r < start || end < l {
            return 0;
        }
        if l <= start && end <= r {
            return self.tree[node];
        }
        let mid = (start + end) / 2;
        let left_sum = self.query_node(2 * node, start, mid, l, r);
        let right_sum = self.query_node(2 * node + 1, mid + 1, end, l, r);
        left_sum + right_sum
    }
}

fn main() {
    let arr = [1, 3, 5, 7, 9, 11];
    let mut st = SegmentTree::new(&arr);

    println!("Sum in range [1, 3]: {}", st.query(1, 3)); // 3 + 5 + 7 = 15
    st.update(1, 10);
    println!("Sum in range [1, 3] after update: {}", st.query(1, 3)); // 10 + 5 + 7 = 22
}
