/* Dutch National Flag Problem
 * Given 'n' objects colored red, white or blue, 
 * sort them so that objects of the same color are
 * adjacent with the colors in order red, white and blue.
 * Similar to partitioning the array of 0s, 1s, and 2s.
 */

package sorts;
import java.util.Random;
import java.util.Scanner;
import sorts.RandomInput;

public class Partitioning {
	
	private static void swap(int[] array, int x, int y) {
		int temp = array[x];
		array[x] = array[y];
		array[y] = temp;
	}
	// Divide the array into three parts
	// a[1...low] zeros
	// a[low+1...mid] ones
	// a[mid+1...hih] twos
	// Time complexity: O(n)
	// Space complexity: O(1)
	public static void doPartitioning(int[] array) {
		int low = 0, mid = 0, high = array.length-1;
		while (mid <= high) {
			// Invariant: a[0-low-1] = 0s etc
			int value = array[mid];
			switch(value) {
				case 0:
					swap(array, low, mid);
					low++; mid++;
					break;

				case 1:
					mid++;
					break;

				case 2:
					swap(array, mid, high);
					high--;
					break;
			}
		}
	}

	public static void printArray(int[] array) {
		System.out.println();
		for (int idx = 0; idx < array.length; idx++) {
			System.out.print(array[idx] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		int[] input = RandomInput.generateArray(10, 0, 3);
		printArray(input);
		doPartitioning(input);
		printArray(input);
	}
}