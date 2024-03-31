/* URL: https://www.hackerrank.com/challenges/abbr/problem?h_l=interview&playlist_slugs%5B%5D=interview-preparation-kit&playlist_slugs%5B%5D=dynamic-programming
 */

package problem_solving;
import java.util.Scanner;

public class Abbreviation {
	private static boolean matchStrings(String a, String b, int i, int j) {
		if (a != null && b == null) {
			return true;
		}
		if (a == null && b == null) {
			return true;
		}
		if (a == null && b != null) {
			return false;
		}
		if (i == a.length() && j == b.length()) {
			return true;
		}

		if (i == a.length() && j != b.length()) {
			return false;
		}
		if (i != a.length() && j == b.length()) {
			return true;
		}
		if (a.charAt(i) == b.charAt(j))
			return matchStrings(a, b, i + 1, j + 1);
		else
			return matchStrings(a, b, i + 1, j);
	}

	private static String convertLowerToUpper(String a) {

		String newString = "";
		for (int i = 0; i < a.length(); i++) {
			if (a.charAt(i) >= 97 && a.charAt(i) <= 122) {
				newString = newString + (char)(a.charAt(i) - 32);
			}
			else {
				newString = newString + a.charAt(i);
			}
		}
		return newString;
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		String a = sc.next();
		String b = sc.next();

		a = convertLowerToUpper(a);
		b = convertLowerToUpper(b);

		//System.out.println(a);
		//System.out.println(b);
		if (matchStrings(a, b, 0, 0) == true) {
			System.out.println("True");
		}
		else {
			System.out.println("False");
		}
	}
}