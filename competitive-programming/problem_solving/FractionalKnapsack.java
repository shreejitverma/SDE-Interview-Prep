/* URL: https://practice.geeksforgeeks.org/problems/fractional-knapsack/0/?ref=self
 *
 */

package problem_solving;
import java.util.Scanner;
import java.util.Vector;
import java.util.Iterator;
import java.util.Collections;
import java.util.Comparator;

class Item {
	int value;
	int weight;
	Item(int val, int wt) {
		value = val;
		weight = wt;
	}
}

public class FractionalKnapsack {
	
	/* Sort the items based on value.
	 *
	 */
	private static void sortItem(Vector<Item> items) {
		Collections.sort(items, new Comparator<Item>() {
			@Override
			public int compare(Item first, Item second) {
				if (first.value > second.value)	return -1;
				return 1;
			}
		});
	}


	private static int fractionalKnapsackMaximumProfit(Vector<Item> items, int maxWeight) {
		// Sort items based on value, following the greedy approach.
		sortItem(items);
		int maximumValue = 0;

		Iterator<Item> it = items.iterator();
		while (maxWeight > 0 && it.hasNext()) {
			Item i = it.next();
			int wt = i.weight;
			int v = i.value;
			System.out.println(wt + " " + v);
			if (maxWeight - wt >= 0) {
				maxWeight -= wt;
				maximumValue += v;
				System.out.println(v);
			}
			else {
				break;
			}
		}

		if (it.hasNext()) {
			Item i = it.next();
			maximumValue += i.value / i.weight * maxWeight;
		}
		return maximumValue;
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the number of test cases: ");
		int input = sc.nextInt();
		Vector<Item> item = new Vector<Item>();

		while (input > 0) {
			input -= 1;
			System.out.println("Enter the total number of items: ");
			int N = sc.nextInt();
			System.out.println("Enter the maximum weight: ");
			int W = sc.nextInt();
			int n = N;
			int idx = 0;
			while (2*n > 0) {
				n -= 1;
				int val = sc.nextInt();
				int wt = sc.nextInt();
				item.add(new Item(val, wt));
				idx++;

			}

			System.out.println("Maximum value carried in knapsack: " + fractionalKnapsackMaximumProfit(item, W));
		}
	}
}