/* Shuffle a given input array randomly such that
 * any combination is equally likely.
 * URL: geeksforgeeks
 */
package arrays;
import utilities.RandomInput;
// Need this for generating a number between a range
import java.util.Random;

public class ShuffleArray extends Stub {
	/* Algorithm used: 
	 * Generate a random number between 0 to length-1
	 * Replace the value with the last element
	 * Generate a random number between 0 and length-2
	 * Replace the value with the second last element now
	 * Repeat until you reach the begining of the array.
	 */ 
	private static void shuffleNow(int[] input) {
		int idx = 0;
		int last = input.length;
		Random rm = new Random();
		while (last > 0) {
			// generate a value [0, length-of-input) exclusive
			int val = rm.nextInt(last);
			swap(input, val, last - 1);
			last--;
		}
		printArray("Output", input);
	}

	private static void swap(int[] input, int x, int y) {
		int temp = input[x];
		input[x] = input[y];
		input[y] = temp;
	}

	private static void testCases() {
		// Method reuse to generate a random array as input
		printStringArray("------------------------");
		int[] input = generateArray(20, 20);
		printArray("Input", input);
		shuffleNow(input);
		input = generateArray(10, 20);
		printArray("Input", input);
		shuffleNow(input);
		input = generateArray(10, 20);
		printArray("Input", input);
		shuffleNow(input);
		printStringArray("------------------------");
	}

	public static void main(String[] args) {
		testCases();
	}
}