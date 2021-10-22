package arrays;
import utilities.RandomInput;

public class Stub {
	
	public static void printArray(String text, int[] input) {
		System.out.print(text + ": ");
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}

	public static void print(String text, int value) {
		System.out.println(text + ": " + value);
	}
	public static void printStringArray(String text) {
		System.out.println(text);
	}

	public static int[] generateArray(int size, int maxValue) {
		int[] input = RandomInput.generateArray(size, 0, maxValue);
		return input;
	}
}