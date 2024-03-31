/* Given an integer array, apply quick sort technique
 * to sort the given array.
 * 1. First step of quick sort: take a random pivot and divide the array
 * 2. Put the pivot at the correct place 
 *    divide the left and right side of teh array
 *	  continue with the same algorithm until end.
 */

package sorts;
import java.util.Scanner;

public class QuickSort {
	public static int divideArray(int[] array, int leftIdx, int rightIdx) {
		int pivotIndex = rightIdx;
		int leftPtr = leftIdx - 1;
		for (int runningPtr = leftIdx; runningPtr < rightIdx; runningPtr++) {
			if (array[runningPtr] <= array[pivotIndex]) {
				leftPtr++;
				int temp = array[leftPtr];
				array[leftPtr] = array[runningPtr];
				array[runningPtr] = temp;
			}
		}
		leftPtr++;
		int temp = array[leftPtr];
		array[leftPtr] = array[pivotIndex];
		array[pivotIndex] = temp;
		return leftPtr;
	}

	public static void quickSort(int[] array, int leftIdx, int rightIdx) {
		if (leftIdx < rightIdx) {
			int pivot = divideArray(array, leftIdx, rightIdx);
			quickSort(array, leftIdx, pivot-1);
			quickSort(array, pivot+1, rightIdx);
		}
	}

	public static void doQuickSort(int[] array) {
		quickSort(array, 0, array.length-1);
	}

	public static void takeInput(Scanner sc, int len, int[] array) {
		int idx = 0;
		while (len > 0) {
			len -= 1;
			array[idx++] = sc.nextInt();
		}
	}

	public static void printArray(int[] array) {
		System.out.println();
		for (int idx = 0; idx < array.length; idx++) {
			System.out.print(array[idx] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the length of the array: ");
		int len = sc.nextInt();
		int idx = 0;
		int[] array = new int[len];
		takeInput(sc, len, array);
		doQuickSort(array);
		printArray(array);
	}	
}