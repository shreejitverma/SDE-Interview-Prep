/* Class which creates a heap.
 * Keep a max-heap.
 * Assuming that the values in the arrays are integers.
 * URL: https://www.geeksforgeeks.org/binary-heap/
 */
package heaps;

public class Heap {

	int[] array;
	int capacity = 0;

	// Initialize array size here
	Heap(int[] input) {
		capacity = input.length;
		array = input;
	}

	public int getMax() {
		int max = -1;
		if (capacity >= 1) {
			max = array[0];
		}
		return max;
	}

	// method to swap the values
	private void swap(int x, int y) {
		int temp = array[x];
		array[x] = array[y];
		array[y] = temp;
	}

	/* Takes O(log n) time
	 */
	public void heapify(int parentIdx) {
		int leftChildIdx = 2 * parentIdx + 1;
		int rightChildIdx = 2 * parentIdx + 2;
		int largestIdx = parentIdx;

		if (leftChildIdx < capacity && array[leftChildIdx] > array[parentIdx]) {
			largestIdx = leftChildIdx;
		}
		if (rightChildIdx < capacity && array[rightChildIdx] > array[largestIdx]) {
			largestIdx = rightChildIdx;
		}
		

		if (largestIdx != parentIdx) {
			swap(largestIdx, parentIdx);
			heapify(largestIdx);
		}
	}

	public void printHeap() {
		System.out.println();
		for (int idx = 0; idx < capacity; idx++) {
			System.out.print(array[idx] + " ");
		}
		System.out.println();
	}

	public void createHeap() {
		int size = capacity;
		for (int idx = capacity / 2; idx >= 0; idx--) {
			heapify(idx);
		}
	}
}