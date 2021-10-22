package arrays;
import utilities.RandomInput;

public class ReplaceElementWithMultiplicationOfPreviousAndNext {

	// Need to store the previous element for doing in a single pass.	
	private static void replace(int[] input) {
		if (input != null && input.length > 2) {
			int tempL = input[0];
			int tempR = input[input.length-1];
			input[0] = input[0] * input[1];
			for (int idx = 1; idx < input.length-1; idx++) {
				tempR = input[idx];
				input[idx] = tempL * input[idx+1];
				tempL = tempR;
			}
			input[input.length-1] = tempL * input[input.length-1];
		}
	}

	public static void printArray(String text, int[] array) {
		System.out.print(text + ": ");
		for (int idx = 0; idx < array.length; idx++) {
			System.out.print(array[idx] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		int[] array = RandomInput.generateArray(10, 0, 10);
		printArray("Input", array);
		replace(array);
		printArray("Output", array);
	}
}