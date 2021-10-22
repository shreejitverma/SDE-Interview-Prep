/* URL: https://practice.geeksforgeeks.org/problems/sieve-of-eratosthenes/0/?track=SP-Misc
 *
 */

package problem_solving;
import java.util.Scanner;

public class SieveOfErastosthenes {
	private static void doSieve(int n) {
		int[] sieve = new int[n+1];
		System.out.println("Prime numbers upto n: ");
		for (int idx = 2; idx <= n; idx++) {
			int multiple = idx + idx;
			while (multiple <= n) {
				if (sieve[multiple] != 1) {
					sieve[multiple] = 1;
				}
				multiple += idx;
			}
		}

		for (int idx = 2; idx <= n; idx++)
			if (sieve[idx] != 1)
				System.out.print(idx + " ");
		System.out.println();
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		int testCases = sc.nextInt();
		while (testCases > 0) {
			testCases -= 1;
			int value = sc.nextInt();
			doSieve(value);
		}
	}
}