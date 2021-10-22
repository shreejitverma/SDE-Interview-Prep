/** Utilities file which contains common methods used 
 *  over and over again in coding.
 * Method overloading in action.
 */
package utilities;
import java.util.Scanner;
import java.util.Arrays;

public class Utilities {
	
	public static void printMatrix(int[][] matrix, int row, int col) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				System.out.print(matrix[i][j] + " ");
			}
			print("\n", "");
		}
	}
	public static void print(int value) {
		System.out.print(value + " ");
	}

	public static void print(String text, String delimeter, int[] array) {
		System.out.print(text + delimeter + " ");

		for (int idx = 0; idx < array.length; idx++) {
			System.out.print(array[idx] + " ");
		}
		System.out.println();
	}

	public static void print(String text, String delimeter, int value) {
		System.out.println(text + delimeter + " " + value);
	}

	public static void print(String text, String delimeter) {
		System.out.println(text + delimeter);
	}

	private static Scanner getScanner() {
		return new Scanner(System.in);
	}

	public static String takeInput() {
		Scanner sc = getScanner();
		return sc.next();
	}
	
	public static int takeInputIntegerValue() {
		Scanner sc = getScanner();
		return sc.nextInt();
	}

	public static void sortArray(int[] array) {
		Arrays.sort(array);
	}

	// No need to return the value since the values are pushed
	// in the original array.
	public static void takeInputArray(int[] array, int len) {
		Scanner sc = getScanner();
		int idx = 0;
		while (len > 0) {
			len -= 1;
			array[idx++] = sc.nextInt();
		}
	}
}