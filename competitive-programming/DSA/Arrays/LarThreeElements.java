/* Find the largest three elements in the array.
 * One solution: Sort the array and get the last
 * three elements in the array.
 * This solution: Does it in single pass.
 */

package arrays;

public class LarThreeElements extends Stub {
	public static void getLarThreeElement(int[] input) {
		int largest       = Integer.MIN_VALUE;
		int secondLargest = Integer.MIN_VALUE;
		int thirdLargest  = Integer.MIN_VALUE;

		if (input != null && input.length != 0) {
			for (int idx = 0; idx < input.length; idx++) {
				if (input[idx] > largest) {
					thirdLargest = secondLargest;
					secondLargest = largest;
					largest = input[idx];
				}
				else if (input[idx] > secondLargest) {
					thirdLargest = secondLargest;
					secondLargest = input[idx];
				}
				else if (input[idx] > thirdLargest) {
					thirdLargest = input[idx];
				}
			}
			print("Largest", largest);
			print("2nd Largest", secondLargest);
			print("3rd Largest", thirdLargest);
			if (secondLargest == Integer.MIN_VALUE || thirdLargest == Integer.MIN_VALUE) {
				printStringArray("Input does not have either second or third largest value");
			}
		}
	}

	public static void main(String[] args) {
		int[] input = generateArray(10, 20);
		printArray("Input", input);
		getLarThreeElement(input);
	}
}