/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V + E)

use std::collections::VecDeque;

pub struct Solution;

impl Solution {
    pub fn can_finish(num_courses: i32, prerequisites: Vec<Vec<i32>>) -> bool {
        let n = num_courses as usize;
        let mut adj = vec![Vec::new(); n];
        let mut in_degree = vec![0i32; n];

        for pre in prerequisites {
            let course = pre[0] as usize;
            let prereq = pre[1] as usize;
            adj[prereq].push(course);
            in_degree[course] += 1;
        }

        let mut queue = VecDeque::new();
        for i in 0..n {
            if in_degree[i] == 0 {
                queue.push_back(i);
            }
        }

        let mut visited_count = 0;
        while let Some(curr) = queue.pop_front() {
            visited_count += 1;

            for &neighbor in &adj[curr] {
                in_degree[neighbor] -= 1;
                if in_degree[neighbor] == 0 {
                    queue.push_back(neighbor);
                }
            }
        }

        visited_count == n
    }
}
