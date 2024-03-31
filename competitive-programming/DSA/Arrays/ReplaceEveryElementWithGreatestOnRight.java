/* Replace every element with the next greatest element on the right of the array.
 */
package arrays;
import utilities.RandomInput;

public class ReplaceEveryElementWithGreatestOnRight implements Stub {
	// Possible in single pass of the array
	private static void replace(int[] array) {
		if (array != null && array.length != 0) {
			int largest = array[array.length - 1];
			for (int idx = array.length-2; idx >= 0; idx--) {
				int temp = array[idx];
				array[idx] = largest;
				if (temp > largest) {
					largest = temp;
				}
			}
			// The rightmost element will always have the value -1
			array[array.length-1] = -1;
		}
	}

	public static void main(String[] args) {
		// generate random numbers in range [-10, 10)
		int[] input = generateArray(10, 10);
		printArray("Input", input);
		replace(input);
		printArray("Output", input);
	}
}