'''https://www.geeksforgeeks.org/fractional-knapsack-problem/
Given weights and values of n items, we need to put these items in a knapsack of capacity W 
to get the maximum total value in the knapsack.

In the 0-1 Knapsack problem, we are not allowed to break items. We either take the whole item or don’t take it. 

Input: 
Items as (value, weight) pairs 
arr[] = {{60, 10}, {100, 20}, {120, 30}} 
Knapsack Capacity, W = 50; 



Output: 
Maximum possible value = 240 
by taking items of weight 10 and 20 kg and 2/3 fraction 
of 30 kg. Hence total price will be 60+100+(2/3)(120) = 240

In Fractional Knapsack, we can break items for maximizing the total value of knapsack. 
This problem in which we can break an item is also called the fractional knapsack problem. 

Input : 
Same as above

Output :
Maximum possible value = 240
By taking full items of 10 kg, 20 kg and 
2/3rd of last item of 30 kg
A brute-force solution would be to try all possible subset with all different fraction but that will be too much time taking. 

An efficient solution is to use Greedy approach. 
The basic idea of the greedy approach is to calculate the ratio value/weight for each item 
and sort the item on basis of this ratio. Then take the item with the highest ratio 
and add them until we can’t add the next item as a whole and at the end add the 
next item as much as we can. Which will always be the optimal solution to this problem.
A simple code with our own comparison function can be written as follows, 
please see sort function more closely, the third argument to sort function is our comparison function which sorts 
the item according to value/weight ratio in non-decreasing order. 
After sorting we need to loop over these items and add them in our knapsack satisfying above-mentioned criteria.

Below is the implementation of the above idea:
'''
# Python3 program to solve fractional
# Knapsack Problem


class ItemValue:

    """Item Value DataClass"""

    def __init__(self, wt, val, ind):
        self.wt = wt
        self.val = val
        self.ind = ind
        self.cost = val // wt

    def __lt__(self, other):
        return self.cost < other.cost

# Greedy Approach


class FractionalKnapSack:

    """Time Complexity O(n log n)"""
    @staticmethod
    def getMaxValue(wt, val, capacity):
        """function to get maximum value """
        iVal = []
        for i in range(len(wt)):
            iVal.append(ItemValue(wt[i], val[i], i))

        # sorting items by value
        iVal.sort(reverse=True)

        totalValue = 0
        for i in iVal:
            curWt = int(i.wt)
            curVal = int(i.val)
            if capacity - curWt >= 0:
                capacity -= curWt
                totalValue += curVal
            else:
                fraction = capacity / curWt
                totalValue += curVal * fraction
                capacity = int(capacity - (curWt * fraction))
                break
        return totalValue


# Driver Code
if __name__ == "__main__":
    wt = [10, 40, 20, 30]
    val = [60, 40, 100, 120]
    capacity = 50

    # Function call
    maxValue = FractionalKnapSack.getMaxValue(wt, val, capacity)
    print("Maximum value in Knapsack =", maxValue)
