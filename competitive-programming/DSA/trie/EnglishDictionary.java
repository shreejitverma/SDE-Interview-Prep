/* Implement a dictionary using a trie datastructure.
 * Also, learn the using of a file reader in java.
 */
package trie;
import java.util.Scanner;
import trie.Trie;
import java.io.File;
import java.io.FileNotFoundException;

public class EnglishDictionary {

	public static void main(String[] args) throws FileNotFoundException {
		File fr = new File("most_common_english_words.txt");
		Scanner sc = new Scanner(fr);
		Trie dictionary = new Trie();

		// Read the words one by one and insert in the trie.
		while (sc.hasNextLine()) {
			String word = sc.nextLine();
			dictionary.insert(word);
			System.out.println(word);
		}
		sc = new Scanner(System.in);
		while (true) {
			System.out.println("Enter the word to search for: ");

			String word = sc.next();
			System.out.println("Found: " + dictionary.search(word));
		}
	}	
}