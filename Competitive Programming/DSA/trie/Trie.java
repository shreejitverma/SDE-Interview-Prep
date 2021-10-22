/* Store the words to search in a dictionary.
 * Using a trie.
 * Awesome explanation here: https://leetcode.com/articles/implement-trie-prefix-tree/
 */

package trie;
class TrieNode {
	private static final int ENG_ALPHABET_SIZE = 26;
	// Store the pointer to every alphabet possible
	TrieNode[] alphabets;
	boolean isEndOfWord = false;

	TrieNode() {
		alphabets = new TrieNode[ENG_ALPHABET_SIZE];
	}
}

public class Trie {
	TrieNode root;
	
	public Trie() {
		root = new TrieNode();
	}

	public boolean search(String word) {
		boolean found = false;
		TrieNode tempPtr = root;
		int alphabet = 0;
		for (int idx = 0; idx < word.length(); idx++) {
			if (word.charAt(idx) >= 97) {
				alphabet = word.charAt(idx) - 'a';
			}
			else {
				alphabet = word.charAt(idx) - 'A';
			}
			if (tempPtr.alphabets[alphabet] != null) {
				tempPtr = tempPtr.alphabets[alphabet];
			}
			else {
				return false;
			}
		}
		if (tempPtr != null && tempPtr.isEndOfWord)
			found = true;
		return found;
	}

	public void insert(String word) {
		TrieNode tempPtr = root;
		for (int idx = 0; idx < word.length(); idx++) {
			int alphabet = word.charAt(idx) - 'a';
			if (tempPtr.alphabets[alphabet] == null) {
				tempPtr.alphabets[alphabet] = new TrieNode();
			}
			tempPtr = tempPtr.alphabets[alphabet];
		}
		tempPtr.isEndOfWord = true;
	}

}