package heaps;

public class PriorityHeap {
	int[] input;
	// current size of the heap.
	int size;
	// maximum size that can be stored in the heap
	int capacity;
	int[] priority;

	public PriorityHeap(int[] in, int[] pr) {
		input = in;
		priority = pr;
		size = input.length;
		capacity = size;
	}

	public int getMax() {
		int val = Integer.MIN_VALUE;
		if (size != 0) {
			val = input[0];
			int p = priority[0];
			input[0] = input[size-1];
			priority[0] = priority[size-1];
			size--;
			heapify(0);
		}
		return val;
	}

	// Swap the values and the priority for the heap.
	private void swap(int x, int y) {
		int temp = input[x];
		int p = priority[x];
		input[x] = input[y];
		priority[x] = priority[y];
		input[y] = temp;
		priority[y] = p;
	}

	public int enQueue(int val) {
		int ret = Integer.MIN_VALUE;
		if (size != capacity) {
			
		}
		return ret;
	}

	public int deQueue() {
		int ret = Integer.MIN_VALUE;
		if (size != 0) {

			ret = getMax();
		}
		return ret;
	}
	public void heapify(int parentIdx) {
		int leftChildIdx = 2 * parentIdx + 1;
		int rightChildIdx = 2 * parentIdx + 2;
		int largestIdx = parentIdx;

		if(leftChildIdx < size && priority[leftChildIdx] > priority[parentIdx]) {
			largestIdx = leftChildIdx;
		}
		if (rightChildIdx < size && priority[rightChildIdx] > priority[largestIdx]) {
			largestIdx = rightChildIdx;
		}

		if (largestIdx != parentIdx) {
			swap(largestIdx, parentIdx);
			heapify(largestIdx);
		}
	}

	public void print() {
		System.out.println();
		for (int idx = 0; idx < size; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();

		System.out.println();
		for (int idx = 0; idx < size; idx++) {
			System.out.print(priority[idx] + " ");
		}
		System.out.println();

	}

	public void createHeap() {

		for (int idx = size / 2; idx >= 0; idx--) {
			heapify(idx);
		}
	}

}