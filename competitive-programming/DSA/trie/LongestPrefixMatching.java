/* Reference URL: https://www.geeksforgeeks.org/longest-prefix-matching-a-trie-based-solution-in-java/
 * Another one: https://leetcode.com/articles/implement-trie-prefix-tree/
 * Use trie to get the longest prefix of a given word.
 * Dictionary taken to be a file named "most_common_english_words.txt" present
 * in the current directory for simplicity instead of having an input for that.
 */
package trie;
import trie.Trie;
import java.io.File;
import java.util.Scanner;
import java.io.FileNotFoundException;

public class LongestPrefixMatching {

	public static String getLongestPrefix(String word, Trie dictionary) {
		String longestPrefix = "";
		String tempString = "";
		TrieNode temp = dictionary.root;
		int idx;

		for (idx = 0; idx < word.length(); idx++) {
			int alphabet = word.charAt(idx) - 'a';
			if (temp.alphabets[alphabet] != null) {
				tempString = tempString + word.charAt(idx);
				temp = temp.alphabets[alphabet];
				if (temp.isEndOfWord == true) {
					longestPrefix = tempString;
				}
			}
			else {
				break;
			}
		}
		// Check 2 conditions
		// 1. If the word is not present in the dictionary at all
		// 2. If the word was not present as a prefix.
		if (longestPrefix.length() == 0 || idx != word.length())
			longestPrefix = "No such prefix in the dictionary";
		return longestPrefix;
	}

	public static void main(String[] args) throws FileNotFoundException {
		File file = new File("most_common_english_words.txt");
		Scanner sc = new Scanner(file);
		Trie dictionary = new Trie();

		//read the file and create a trie
		while (sc.hasNextLine()) {
			String word = sc.nextLine();
			dictionary.insert(word);
		}
		sc = new Scanner(System.in);

		while (true) {
			System.out.println("Enter the word to search longest prefix: ");
			String word = sc.next();

			System.out.println(getLongestPrefix(word, dictionary));
		}
	}
}