package arrays;
import java.util.Scanner;
import utilities.Utilities;

class Pivot {
	public static int findPivot(int[] input, int low, int high) {
		if (low < high) {
			int mid = low + (high - low) / 2;
			if (input[mid] > input[mid+1]) {
				return mid;
			}
			else if (input[mid] < input[mid+1] && input[mid] < input[low]) {
				return findPivot(input, low, mid);
			}
			else if (input[mid] < input[mid+1] && input[mid] > input[low]) {
				return findPivot(input, mid, high);
			}
		}
		return -1;
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		// Using from utilities package
		Utilities.print("Enter the length of array", ":");
		int len = sc.nextInt();
		int idx = 0;
		int[] array = new int[len];
		while (len > 0) {
			len -= 1;
			array[idx++] = sc.nextInt();
		}
		Utilities.print("Pivot in array", ":", findPivot(array, 0, array.length - 1));
	}
}