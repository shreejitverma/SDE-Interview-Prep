/* URL: https://practice.geeksforgeeks.org/problems/gcd-of-array/0/?track=SP-Misc
 *
 */

package problem_solving;
import java.util.Scanner;

public class GCDOfArray {
	private static void print() {

	}
	
	private static int findGCD(int a, int b) {
		if (a == 0)
			return b;
		return findGCD(b % a, a);
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		int input = sc.nextInt();
		int idx = 0;
		int array[] = new int[input];
		while (input > 0) {
			int val = sc.nextInt();
			array[idx] = val;
			idx++;
			input--;
		}
		if (array == null || array.length == 0)
			break;
		int gcd = array[0];
		for (int i = 1; i < array.length; i++) {
			gcd = findGCD(gcd, array[i]);
		}
		System.out.println("GCD of the array: " + gcd);
	}
}