'''https: // www.geeksforgeeks.org/given-an-array-of-of-size-n-finds-all-the-elements-that-appear-more-than-nk-times/
Given an array of size n, find all elements in array that appear more than n/k times. For example, if the input arrays is {3, 1, 2, 2, 1, 2, 3, 3} and k is 4, then the output should be [2, 3]. Note that size of array is 8 (or n = 8), so we need to find all elements that appear more than 2 ( or 8/4) times. There are two elements that appear more than two times, 2 and 3. 

A simple method is to pick all elements one by one. For every picked element, count its occurrences by traversing the array, if count becomes more than n/k, then print the element. Time Complexity of this method would be O(n2).
A better solution is to use sorting. First, sort all elements using a O(nLogn) algorithm. Once the array is sorted, we can find all required elements in a linear scan of array. So overall time complexity of this method is O(nLogn) + O(n) which is O(nLogn).
Following is an interesting O(nk) solution:

We can solve the above problem in O(nk) time using O(k-1) extra space. Note that there can never be more than k-1 elements in output(Why?). There are mainly three steps in this algorithm.


1) Create a temporary array of size(k-1) to store elements and their counts(The output elements are going to be among these k-1 elements). Following is structure of temporary array elements.

struct eleCount {
    int element;
    int count;
};
struct eleCount temp[];
This step takes O(k) time.
2) Traverse through the input array and update temp[](add/remove an element or increase/decrease count) for every traversed element. The array temp[] stores potential(k-1) candidates at every step. This step takes O(nk) time.

3) Iterate through final(k-1) potential candidates(stored in temp[]). or every element, check if it actually has count more than n/k. This step takes O(nk) time.

The main step is step 2, how to maintain(k-1) potential candidates at every point? The steps used in step 2 are like famous game: Tetris. We treat each number as a piece in Tetris, which falls down in our temporary array temp[]. Our task is to try to keep the same number stacked on the same column(count in temporary array is incremented).

Consider k=4, n=9
Given array: 3 1 2 2 2 1 4 3 3

i=0
         3 _ _
temp[] has one element, 3 with count 1

i=1
         3 1 _
temp[] has two elements, 3 and 1 with
counts 1 and 1 respectively

i=2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 1, 1 and 1 respectively.

i=3
         - - 2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 1, 1 and 2 respectively.

i=4
         - - 2
         - - 2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 1, 1 and 3 respectively.

i=5
         - - 2
         - 1 2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 1, 2 and 3 respectively.
Now the question arises, what to do when temp[] is full and we see a new element – we remove the bottom row from stacks of elements, i.e., we decrease count of every element by 1 in temp[]. We ignore the current element.

i=6
         - - 2
         - 1 2
temp[] has two elements, 1 and 2 with
counts as 1 and 2 respectively.

i=7
           - 2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 1, 1 and 2 respectively.

i=8
         3 - 2
         3 1 2
temp[] has three elements, 3, 1 and 2 with
counts as 2, 1 and 2 respectively.
Finally, we have at most k-1 numbers in temp[]. The elements in temp are {3, 1, 2}. Note that the counts in temp[] are useless now, the counts were needed only in step 2. Now we need to check whether the actual counts of elements in temp[] are more than n/k(9/4) or not. The elements 3 and 2 have counts more than 9/4. So we print 3 and 2.

Note that the algorithm doesn’t miss any output element. There can be two possibilities, many occurrences are together or spread across the array. If occurrences are together, then count will be high and won’t become 0. If occurrences are spread, then the element would come again in temp[]. Following is the implementation of the above algorithm.
'''

# A Python3 program to print elements with
# count more than n/k

# Prints elements with more than n/k
# occurrences in arrof size n. If
# there are no such elements, then
# it prints nothing.


from collections import Counter


def moreThanNdK(arr, n, k):

    # k must be greater than 1
    # to get some output
    if (k < 2):
        return

    # Step 1: Create a temporary array
    # (contains element and count) of
    # size k-1. Initialize count of all
    # elements as 0
    temp = [[0 for i in range(2)]
            for i in range(k)]

    for i in range(k - 1):
        temp[i][0] = 0

    # Step 2: Process all elements
    # of input array
    for i in range(n):
        j = 0

        # If arr[i] is already present in
        # the element count array, then
        # increment its count
        while j < k - 1:
            if (temp[j][1] == arr[i]):
                temp[j][0] += 1
                break

            j += 1

        # If arr[i] is not present in temp
        if (j == k - 1):
            l = 0

            # If there is position available
            # in temp[], then place arr[i]
            # in the first available position
            # and set count as 1*/
            while l < k - 1:
                if (temp[l][0] == 0):
                    temp[l][1] = arr[i]
                    temp[l][0] = 1
                    break

                l += 1

            # If all the position in the
            # tempare filled, then decrease
            # count of every element by 1
            if (l == k - 1):
                while l < k:
                    temp[l][0] -= 1
                    l += 1

    # Step 3: Check actual counts
    # of potential candidates in temp[]
    for i in range(k - 1):

        # Calculate actual count of elements
        ac = 0  # Actual count
        for j in range(n):
            if (arr[j] == temp[i][1]):
                ac += 1

        # If actual count is more
        # than n/k, then print
        if (ac > n // k):
            print("Number:",
                  temp[i][1],
                  " Count:", ac)


# Driver code
if __name__ == '__main__':

    print("First Test")
    arr1 = [4, 5, 6, 7, 8, 4, 4]
    size = len(arr1)
    k = 3
    moreThanNdK(arr1, size, k)

    print("Second Test")
    arr2 = [4, 2, 2, 7]
    size = len(arr2)
    k = 3
    moreThanNdK(arr2, size, k)

    print("Third Test")
    arr3 = [2, 7, 2]
    size = len(arr3)
    k = 2
    moreThanNdK(arr3, size, k)

    print("Fourth Test")
    arr4 = [2, 3, 3, 2]
    size = len(arr4)
    k = 3
    moreThanNdK(arr4, size, k)
'''Time Complexity: O(nk) 
Auxiliary Space: O(k)
Generally asked variations of this problem are, find all elements that appear n/3 times or n/4 times in O(n) time complexity and O(1) extra space.

Another Approach:

Hashing can also be an efficient solution. With a good hash function, we can solve the above problem in O(n) time on average. Extra space required hashing would be higher than O(k). Also, hashing cannot be used to solve the above variations with O(1) extra space.

Below is the implementation of the above idea:'''

# Python3 code to find elements whose
# frequency yis more than n/k


def morethanNbyK(arr, n, k):
    x = n // k

    # unordered_map initialization
    freq = {}

    for i in range(n):
        if arr[i] in freq:
            freq[arr[i]] += 1
        else:
            freq[arr[i]] = 1

    # Traversing the map
    for i in freq:

        # Checking if value of a key-value pair
        # is greater than x (where x=n/k)
        if (freq[i] > x):

            # Print the key of whose value
            # is greater than x
            print(i)


# Driver code
arr = [1, 1, 2, 2, 3, 5, 4, 2, 2, 3, 1, 1, 1]
n = len(arr)
k = 4

morethanNbyK(arr, n, k)


# Another One

# Python3 implementation

# Function to find the number of array
# elements with frequency more than n/k times

def printElements(arr, n, k):

    # Calculating n/k
    x = n//k

    # Counting frequency of every
    # element using Counter
    mp = Counter(arr)

    # Traverse the map and print all
    # the elements with occurrence
    # more than n/k times
    for it in mp:
        if mp[it] > x:
            print(it)


# Driver code
arr = [1, 1, 2, 2, 3, 5, 4, 2, 2, 3, 1, 1, 1]
n = len(arr)
k = 4

printElements(arr, n, k)
