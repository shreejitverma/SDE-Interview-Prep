/* Class to implement heap sort using already 
 * created Heap.java class.
 */
package heaps;
import heaps.Heap;
import java.util.Random;
import java.util.Scanner;

public class HeapSort {
	
	/* Heap Sort
	 * Create a heap first.
	 * Then, replace the first element with the last, if sorting in ascending order
	 * and the heap is a max-heap.
	 * Decrease the size of the array by one over which run the heapify again.
	 * Keep doing this until you reach the beginning of the array input.
	 */
	private static void doHeapSort(int[] input) {		
		Heap hp = new Heap(input);
		hp.printHeap();
		hp.createHeap();
		int size = input.length - 1;

		for (int idx = 0; idx < input.length; idx++) {
			int temp = hp.array[0];
			hp.array[0] = hp.array[size];
			hp.array[size] = temp;
			size--;
			hp.capacity--;
			hp.heapify(0);
		}
		print(input);

	}

	private static void print(int[] input) {
		System.out.println();
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}
	private static int[] getArray(int size, int maxValue) {
		int[] input = new int[size];
		Random rm = new Random();
		for (int idx = 0; idx < size; idx++) {
			input[idx] = rm.nextInt(maxValue + 1);
		}
		return input;
	}

	public static void main(String[] args) {
		System.out.println("Enter the size of the array: ");
		Scanner sc = new Scanner(System.in);
		int size = sc.nextInt();
		System.out.println("Enter the maximum value for the array: ");
		int maxValue = sc.nextInt();
		int[] input = getArray(size, maxValue);
		doHeapSort(input);
	}
}