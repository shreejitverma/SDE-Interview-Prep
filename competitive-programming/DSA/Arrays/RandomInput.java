/* Generate Random numbers for inputs to 
 * programs.
 */
package utilities;
import java.util.Random;

public class RandomInput {
	
	// returns a value [rangeLow, rangeHigh)
	private static int getRandomValue(int rangeLow, int rangeHigh) {
		Random rm = new Random();
		return rm.nextInt(rangeHigh) + rangeLow;
	}

	public static int[] generateArray(int size, int rangeLow, int rangeHigh) {
		int[] input = new int[size];
		for (int idx = 0; idx < size; idx++) {
			input[idx] = getRandomValue(rangeLow, rangeHigh);
		}
		return input;
	}
}