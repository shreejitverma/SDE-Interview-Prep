/* Re-arrange a given array in maximum-minimum
 * form.
 * Input:  1,2,3,4,5,6,7
 * Output: 7,1,6,2,5,3,4
 */

package arrays;
import utilities.Utilities;

public class MaximumMinimumForm extends Stub {
	// 1. Sort the array.
	// 2. Take aux array.
	// 3. Re-arrange the array.
	public static void doMaxMinForm(int[] input) {
		Utilities.sortArray(input);
		printArray("Sorted", input);
		int[] aux = new int[input.length];
		int last = input.length - 1;
		int first = 0, idx = 0;
		while (first <= last) {
			if (idx % 2 == 0) {
				aux[idx] = input[last];
				last--;
			}
			else {
				aux[idx] = input[first];
				first++;	
			}
			idx++;
		}
		printArray("Output", aux);
	}

	public static void main(String[] args) {
		// Use method from Stub.java to generate array.
		int[] input = generateArray(11, 10);
		printArray("Input", input);
		doMaxMinForm(input);
	}
}
