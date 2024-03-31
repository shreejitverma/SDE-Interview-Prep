package arrays;
import java.util.Scanner;
import java.util.Arrays;

class BinarySearch {
	public static int doBinarySearch(int[] input, int low, int high, int value) {
		if (low <= high) {
			int mid = low + (high - low) / 2;
			if (input[mid] == value) {
				return mid;
			}
			else if (input[mid] < value) {
				return doBinarySearch(input, mid+1, high, value);
			}
			else {
				return doBinarySearch(input, low, mid-1, value);
			}
		}
		return -1;
	}
	
	public static void print(String text, int[] input) {
		System.out.println(text);
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
	}

	public static void printValue(String text, int value) {
		System.out.println(text + ": " + value);
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		int len = sc.nextInt();
		int idx = 0;
		int input[] = new int[len];
		while (len > 0) {
			input[idx++] = sc.nextInt();
			len--;
		}
		System.out.print(" Enter value to search: ");
		int value = sc.nextInt();
		print("Array: ", input);
		Arrays.sort(input);
		print("Sorted Array: ", input);
		printValue("\nBinary Search for ", value);
		printValue("", doBinarySearch(input, 0, input.length-1, value));
	}
}