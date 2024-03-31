/* Auto-complete feature using trie;
 * URL: https://www.geeksforgeeks.org/auto-complete-feature-using-trie/
 * URL: https://www.geeksforgeeks.org/implement-a-phone-directory/
 * Can be used a implementation of a phone directory.
 */
package trie;
import trie.Trie;
import java.util.Scanner;
import java.io.File;
import java.util.Vector;
import java.io.FileNotFoundException;
import java.util.Iterator;

public class AutoComplete {
	private static final int ENG_ALPHABET_SIZE = 26;
	
	/* Return a list of strings that can be completed by the
	 * user. Just like auto-complete in a search engine.
	 * First search for the word if it is present (does not matter
	 * if the exact word is present in the dictionary or not).
	 * If present, then start searching for the remaining words
	 * from that node.
	 * Example: dictionary: {"your", "yourself", "yo"}
	 * searchedWord: "you"
	 * Returned suggestions: {"your", "yourself"}
	 */
	public static Vector<String> getAutoCompleteString(Trie dictionary, String searchedWord) {
		// Store the stirngs in a vector
		Vector <String> listOfWords = new Vector<String>();
		TrieNode temp = dictionary.root;
		String word = "";
		boolean isPresent = false;
		int idx = -1;
		if (searchedWord != null && searchedWord.length() != 0) {
			for (idx = 0; idx < searchedWord.length(); idx++) {
				int alphabet = searchedWord.charAt(idx) - 'a';
				if (temp.alphabets[alphabet] != null) {
					temp = temp.alphabets[alphabet];
				}
			}
			// Does not matter if it is word present in the dictionary or not.
			// This helps in getting the word or it's prefix in the dictionary.
			// No need to check if it is the end of word or not.
			if (idx == searchedWord.length()) {
				isPresent = true;
				TrieNode start = temp;
				addStringsForAutoComplete(searchedWord, start, listOfWords);
			}
		}
		return listOfWords;
	}

	// Recursive method which traverses all the valid nodes
	public static void addStringsForAutoComplete(String word, TrieNode start, Vector<String> listOfWords) {
		if (start == null) {
			return;
		}
		// If the current word is present, then it is also one of the suggestions.
		if (start.isEndOfWord == true) {
			listOfWords.add(word);
		}

		// Recursively search from this node
		for (int idx = 0; idx < ENG_ALPHABET_SIZE; idx++) {
			if (start.alphabets[idx] != null) {
				char alphabetToAppend = (char)('a' + idx);
				addStringsForAutoComplete(word + alphabetToAppend, start.alphabets[idx], listOfWords);
			}
		}

	}

	/* Read words that are to be stored in the dictionary from a file.
	 * Create a trie out of thos words.
	 * Start recommending/auto-complete.
	 */
	public static void main(String[] args) throws FileNotFoundException {
		File filePtr = new File("most_common_english_words.txt");
		Scanner sc = new Scanner(filePtr);
		Trie dictionary = new Trie();

		// Read file of english words line by line
		while (sc.hasNextLine()) {
			String word = sc.nextLine();
			dictionary.insert(word);
		}

		sc = new Scanner(System.in);
		while (true) {
			System.out.println("Enter the string for auto-complete: ");
			String prefix = sc.next();
			Vector<String> listOfWords = getAutoCompleteString(dictionary, prefix);

			// Print the list of words one by one using an iterator
			Iterator it = listOfWords.iterator();
		while (it.hasNext()) {
			System.out.println(it.next());
		}

		}
	}	
}