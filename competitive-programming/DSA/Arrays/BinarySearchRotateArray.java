package arrays;
import java.util.Scanner;
import utilities.Utilities;
import arrays.Pivot;
import arrays.BinarySearch;

public class BinarySearchRotateArray {
	
	public static void doBSInRotatedArray(int[] array, int low, int high, int value) {
		int pivotIndex = Pivot.findPivot(array, 0, high);
		Utilities.print("Pivot position", ":", pivotIndex);
		int positionLeft = BinarySearch.doBinarySearch(array, 0, pivotIndex, value);
		int positionRight = BinarySearch.doBinarySearch(array, pivotIndex+1, high, value);
		if ( positionLeft != -1) {
			Utilities.print("Position of value", ":", positionLeft);
		}
		else if (positionRight != -1) {
			Utilities.print("Position of value", ":", positionRight);	
		}
		else
			Utilities.print("Not found", ".");
	}


	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		Utilities.print("Enter the length of the array", ":");
		int len = sc.nextInt();
		int[] array = new int[len];
		int idx = 0;
		Utilities.print("Enter the values of the array", ":");
		while (len > 0) {
			len -= 1;
			array[idx++] = sc.nextInt();
		}
		Utilities.print("Enter the value to search", ":");
		int value = sc.nextInt();
		doBSInRotatedArray(array, 0, array.length-1, value);
	}
}