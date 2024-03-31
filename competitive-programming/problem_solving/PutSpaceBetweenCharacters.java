/* URL: https://www.geeksforgeeks.org/put-spaces-words-starting-capital-letters/
 *
 */
package problem_solving;

import java.util.Scanner;

public class PutSpaceBetweenCharacters {
	/* recursive solution to the question.
	 *
	 */
	private static void putSpaceBetweenChars(String characters, int idx) {
		if (characters == null || characters.length() == 0 || idx >= characters.length()) {
			return;
		}
		if (characters.charAt(idx) >= 65 && characters.charAt(idx) <= 90) {
			System.out.print(" " + (char)(characters.charAt(idx) + 32));
		}
		else {
			System.out.print(characters.charAt(idx));
		}
		putSpaceBetweenChars(characters, idx + 1);
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the character string: ");
		String characters = sc.next();
		putSpaceBetweenChars(characters, 0);
	}
}