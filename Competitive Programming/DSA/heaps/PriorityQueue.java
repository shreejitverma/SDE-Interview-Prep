// Priority Queue: https://www.geeksforgeeks.org/priority-queue-using-heap/
// Add a new value with the priority
// Compare with the current priority of the node and 
// insert if it is less than the current priority.
// Low value of priority means low priority of the node.
	
package heaps;
import heaps.PriorityHeap;
import java.util.Scanner;
import java.util.Random;

// define a data structure for priority queue
public class PriorityQueue {
	int size = -1;
	int[] input;

	private static int[] getArray(int size, int maxValue) {
		Random rm = new Random();
		int[] input = new int[size];

		for (int idx = 0; idx < size; idx++) {
			input[idx] = rm.nextInt(maxValue+1);
		}
		return input;
	}

	public static void print(String text, int[] input) {
		System.out.print(text + " : ");
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}
	/* Sorting of the input will be done based on the priority of the 
	 * input values which is stored in the priority array.
	 * The ith index of priority denotes the priority of the ith value
	 * in the input array.
	 */
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of the array: ");
		int size = sc.nextInt();
		System.out.println("Enter the max random value generated for the array: ");
		int maxValue = sc.nextInt();
		int[] input = getArray(size, maxValue);
		// priority of the values in the input array.
		int[] priority = getArray(size, maxValue);
		print("Input   ", input);
		print("Priority", priority);

		PriorityHeap pHeap = new PriorityHeap(input, priority);
		pHeap.createHeap();
		pHeap.print();
		
		while (true) {
			System.out.println("Enter 1/2/3 to enqueue/dequeue/print the priority queue");
			int val = sc.nextInt();
			switch (val) {
				case 1:
					System.out.println("Enter the value to enqueue: ");
					val = sc.nextInt();
					int status = pHeap.enQueue(val);
					if (status == Integer.MIN_VALUE) {
						System.out.println("Overflow condition");
					}
					break;
				case 2:
					val = pHeap.deQueue();
					System.out.println("Dequeued!" + val);
					break;
				case 3:
					pHeap.print();
					break;
				default:

					break;
			}
		}

		
	}
}