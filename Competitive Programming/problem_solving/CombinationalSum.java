/* URL: https://www.geeksforgeeks.org/combinational-sum/
 *
 */


package problem_solving;
import java.util.Scanner;

public class CombinationalSum {
	/* Recursively, call the values with the currentSum as the changing value
	 */
	private static void findCombSum(int[] array, int idx, int currSum, int finalSum, String answer) {
		if (array == null || array.length == 0 || idx == array.length || currSum > finalSum)
			return;

		if (currSum == finalSum) {
			System.out.println(answer);
			return;
		}
		for (int j = idx; j < array.length; j++) {
			if (currSum + array[j] > finalSum)
				continue;
			findCombSum(array, j, currSum + array[j], finalSum, answer + "," + array[j]);
		}
	}


	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of the integer array: ");
		int size = sc.nextInt();
		int[] input = new int[size];
		int idx = 0;
		System.out.println("Enter the values of the integer array: ");
		while (size > 0) {
			size -= 1;
			input[idx] = sc.nextInt();
			idx++;
		}

		System.out.println("Enter the final sum to be ascertained: ");
		int finalSum = sc.nextInt();

		findCombSum(input, 0, 0, finalSum, "");

	}
}