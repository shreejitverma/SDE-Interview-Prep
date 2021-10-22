// URL: https://www.geeksforgeeks.org/merge-sort/

package sorts;
import java.util.Random;

public class MergeSort {
	private static void doMerge(int[] input, int low, int mid, int high) {
		
		// Get sizes of the arrays to be sorted in this recursive call
		int firstSize = mid - low + 1;
		int secondSize = high - mid;
		// Create arrays temporarily for the sorting
		int[] tempL = new int[firstSize];
		int[] tempR = new int[secondSize];
		// Copy values in both the temp arrays
		for (int idx = 0; idx < firstSize; idx++)
			tempL[idx] = input[low + idx];
		for (int idx = 0; idx < secondSize; idx++)
			tempR[idx] = input[mid + 1 + idx];
		// Initialize indexes back to zero to check the values from both
		// arrays.
		int idxL = 0, idxR = 0;
		int k = low;
		// Continue the array comparison till any one of them reaches the end.
		while (idxR < secondSize && idxL < firstSize) {
			if (tempL[idxL] <= tempR[idxR]) {
				input[k] = tempL[idxL];
				idxL++;
			}
			else {
				input[k] = tempR[idxR];
				idxR++;
			}
			k++;
		}
		// Copy the remaining the elements from both the arrays into the 
		// original array.
		while (idxR < secondSize) {
			input[k++] = tempR[idxR++];
		}
		while (idxL < firstSize) {
			input[k++] = tempL[idxL++];
		}
	}

	// Merge Sort method
	// Divide the array in two equal parts
	private static void doMergeSort(int[] input, int low, int high) {
		if (low < high) {
			int mid = low + (high - low) / 2;
			doMergeSort(input, low, mid);
			doMergeSort(input, mid + 1, high);
			doMerge(input, low, mid, high);
		}
	}

	public static void printArray(String text, int[] input) {
		System.out.print(text + ": ");
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}

	// Generate random initialized array
	public static int[] generateArray(int size, int maxValue) {
		int[] input = RandomInput.generateArray(size, 0, maxValue);
		return input;
	}

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
	
	public static void main(String[] args) {
		int[] input = generateArray(20, 20);
		printArray("Input", input);
		doMergeSort(input, 0, input.length-1);
		printArray("Output", input);
	}
}