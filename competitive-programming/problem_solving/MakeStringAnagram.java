/* URL: https://practice.geeksforgeeks.org/problems/anagram-of-string/1/?ref=self
 *
 */

package problem_solving;
import java.util.Scanner;

public class MakeStringAnagram {
	// Return the minimum number of characters to be deleted to make 
	// the string anagram.
	private static void makeAnagram(String first, String second) {
		int flag[] = new int[26];
		int flagS[] = new int[26];

		for (int i = 0; i < first.length(); i++) {
			flag[first.charAt(i)-'a'] += 1;
		}
		for (int i = 0; i < second.length(); i++) {
			flagS[second.charAt(i)-'a'] += 1;
		}
		int result = 0;
		for (int i = 0; i < 26; i++) {
			result += Math.abs(flag[i] - flagS[i]);
		}
		System.out.println("Minimum deletions for making the two strings anagram: " + result);
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		String first = sc.next();
		String second = sc.next();
		makeAnagram(first, second);
	}
}