/* Remove a character from a string recursively/iteratively.
 * Understand the usages of StringBUilder and String Buffer,
 * and String class.
 */

package strings;

public class RemoveCharacter {
	public static void removeCharacterIter(String text, char ch) {
		String newString = "";
		for (int idx = 0; idx < text.length(); idx++) {
			if (text.charAt(idx) != ch) {
				newString += text.charAt(idx);
			}
		}
		System.out.println("Removed character: " + newString);
	}
	
	public static String removeCharacterRecur(String text, char ch, int idx) {
		if (text.length() == 0)	return "";
		if (text.charAt(idx) != ch) {
			return text.charAt(idx) + removeCharacterRecur(text.substring(idx+1), ch, idx);
		}
		else {
			return removeCharacterRecur(text.substring(idx+1), ch, idx);	
		}
	}

	private static void testCases() {
		//removeCharacterRecur()
		//removeCharacterIter("Siddhartha", 'd');
		System.out.println(removeCharacterRecur("Siddhartha", 'd', 0));
	}

	public static void main(String[] args) {
		testCases();
	}
}