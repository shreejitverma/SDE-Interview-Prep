/* https://www.geeksforgeeks.org/find-the-minimum-distance-between-two-numbers/
 *
 */

package arrays;
import java.util.Random;

public class MinDistBetweenNumbers extends Stub {
	/* Logic is to keep two pointers both reading the input and 
	 * updating itself as any of the two numbers is found.
	 * As the numbers are found, compare the difference of their
	 * positions, and store the minimum distance between them.
	 */
	public static int getMinDistBetweenNumbers(int[] input, int x, int y) {
		int posOne = -1;
		int posTwo = -1;
		int idx = 0;
		int dist = 0;
		int minDist = Integer.MAX_VALUE;

		while (idx < input.length) {
			if (input[idx] == x) {
				posOne = idx;
			}
			if (input[idx] == y) {
				posTwo = idx;
			}
			if ((posTwo != -1 && posOne != -1 ) && ((posTwo - posOne) < minDist)) {
				minDist = (posTwo - posOne) > 0 ? (posTwo - posOne) : (posOne - posTwo);
			}
			idx += 1;
		}
		return minDist;
	}

	/* Generate a random input array.
	 * Generate two random values to be searched 
	 * in the array.
	 * Return the minimum distance between them.
	 * The numbers may not be present in the array
	 * since it is randomly generated.
	 */
	public static void main(String[] args) {
		int[] input = generateArray(20, 10);
		Random rm = new Random();
		int x = rm.nextInt(10);
		int y = rm.nextInt(10);
		printStringArray("------------------------------");
		printArray("Input", input);
		int dist = getMinDistBetweenNumbers(input, x, y);
		if (dist == Integer.MAX_VALUE) {
			printStringArray("Number not found");
		}
		else {
			printStringArray("Minimum distance between x=" + x + " and y=" + y + " is " + dist);
		}
		printStringArray("------------------------------");
	}
}