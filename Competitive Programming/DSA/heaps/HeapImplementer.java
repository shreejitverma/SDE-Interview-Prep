/* Class which creates a integer array and converts it
 * into a max-heap.
 * Tests the methods of Heap.class present in the same directory.
 */
package heaps;
import heaps.Heap;
import java.util.Scanner;
import java.util.Random;

public class HeapImplementer {
	
	private static int[] getRandomArray(int capacity, int maxRandValue) {
		int[] input = new int[capacity];
		Random rm = new Random();
		for (int idx = 0; idx < capacity; idx++) {
			input[idx] = rm.nextInt(maxRandValue);
		}
		return input;
	}
	public static void main(String[] args) {
		System.out.println("================================================================");
		Scanner sc = new Scanner(System.in);
		System.out.println("Heap Value generation is done randomly using Random.class ");
		System.out.println(" Enter the size of the heap: ");
		int capacity = sc.nextInt();
		System.out.println(" Enter the maximum random value you want in the heap: ");
		int maxRandValue = sc.nextInt();
		int[] input = getRandomArray(capacity, maxRandValue);
		Heap hp = new Heap(input);

		hp.printHeap();
		hp.createHeap();
		hp.printHeap();

		/*
		while (true) {
			System.out.println("Enter 1/2/3 for insert/remove(getMaximum)/print the heap");	
			int val = sc.nextInt();
			switch (val) {
				case 1:
					
					break;

				case 2:
					break;

				case 3:
					break;

				default:
					break;
			}
		}
		*/
		System.out.println("================================================================");
	}
}