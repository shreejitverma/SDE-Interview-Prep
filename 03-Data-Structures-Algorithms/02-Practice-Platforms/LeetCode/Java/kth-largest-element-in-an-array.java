import java.util.PriorityQueue;
import java.util.Random;

/*
 * Problem: LeetCode 215 - Kth Largest Element in an Array
 * Difficulty: Medium
 * Concepts: QuickSelect, Min-Heap, Divide and Conquer
 *
 * Time Complexity: O(n) average (QuickSelect), O(n log k) (Min-Heap)
 * Space Complexity: O(1) iterative (QuickSelect), O(k) (Min-Heap)
 */

class Solution {
    private final Random rand = new Random();

    public int findKthLargest(int[] nums, int k) {
        int targetIdx = nums.length - k;
        int left = 0;
        int right = nums.length - 1;

        while (left <= right) {
            int pivotIdx = left + rand.nextInt(right - left + 1);
            int pivotVal = nums[pivotIdx];

            int lt = left;
            int gt = right;
            int i = left;

            while (i <= gt) {
                if (nums[i] < pivotVal) {
                    swap(nums, i++, lt++);
                } else if (nums[i] > pivotVal) {
                    swap(nums, i, gt--);
                } else {
                    i++;
                }
            }

            if (targetIdx >= lt && targetIdx <= gt) {
                return nums[targetIdx];
            } else if (targetIdx < lt) {
                right = lt - 1;
            } else {
                left = gt + 1;
            }
        }

        return nums[targetIdx];
    }

    private void swap(int[] nums, int i, int j) {
        int temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }
}

class SolutionMinHeap {
    public int findKthLargest(int[] nums, int k) {
        PriorityQueue<Integer> minHeap = new PriorityQueue<>();
        for (int num : nums) {
            minHeap.offer(num);
            if (minHeap.size() > k) {
                minHeap.poll();
            }
        }
        return minHeap.peek();
    }
}
