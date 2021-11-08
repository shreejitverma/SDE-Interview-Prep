'''https://practice.geeksforgeeks.org/problems/next-permutation5226/1
https://www.geeksforgeeks.org/find-next-greater-number-set-digits/
Next Permutation 
Medium Accuracy: 54.7% Submissions: 24590 Points: 4
Implement the next permutation, which rearranges the list of numbers into Lexicographically next greater permutation of list of numbers. If such arrangement is not possible, it must be rearranged to the lowest possible order i.e. sorted in an ascending order. You are given an list of numbers arr[ ] of size N.

Example 1:

Input: N = 6
arr = {1, 2, 3, 6, 5, 4}
Output: {1, 2, 4, 3, 5, 6}
Explaination: The next permutation of the 
given array is {1, 2, 4, 3, 5, 6}.
Example 2:

Input: N = 3
arr = {3, 2, 1}
Output: {1, 2, 3}
Explaination: As arr[] is the last 
permutation. So, the next permutation 
is the lowest one.
Your Task:
You do not need to read input or print anything. Your task is to complete the function nextPermutation() which takes N and arr[ ] as input parameters and returns a list of numbers containing the next permutation.

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)

Constraints:
1 ≤ N ≤ 10000


Following are few observations about the next greater number. 
1) If all digits sorted in descending order, then output is always “Not Possible”. For example, 4321. 
2) If all digits are sorted in ascending order, then we need to swap last two digits. For example, 1234. 
3) For other cases, we need to process the number from rightmost side (why? 
because we need to find the smallest of all greater numbers)



You can now try developing an algorithm yourself. 
Following is the algorithm for finding the next greater number. 
I) Traverse the given number from rightmost digit, keep traversing till 
you find a digit which is smaller than the previously traversed digit. 
For example, if the input number is “534976”, we stop at 4 because 4 is smaller than next digit 9. 
If we do not find such a digit, then output is “Not Possible”.

II) Now search the right side of above found digit ‘d’ for the smallest digit greater than ‘d’. 
For “534976″, the right side of 4 contains “976”. The smallest digit greater than 4 is 6.

III) Swap the above found two digits, we get 536974 in above example.

IV) Now sort all digits from position next to ‘d’ to the end of number. 
The number that we get after sorting is the output. For above example, we sort digits in bold 536974. 
We get “536479” which is the next greater number for input 534976.'''
# Python program to find the smallest number which
# is greater than a given no. has same set of
# digits as given number

# Given number as int array, this function finds the
# greatest number and returns the number as integer


def findNext(number, n):

    # Start from the right most digit and find the first
    # digit that is smaller than the digit next to it
    for i in range(n-1, 0, -1):
        if number[i] > number[i-1]:
            break

    # If no such digit found,then all numbers are in
    # descending order, no greater number is possible
    if i == 1 and number[i] <= number[i-1]:
        print("Next number not possible")
        return

    # Find the smallest digit on the right side of
    # (i-1)'th digit that is greater than number[i-1]
    x = number[i-1]
    smallest = i
    for j in range(i+1, n):
        if number[j] > x and number[j] < number[smallest]:
            smallest = j

    # Swapping the above found smallest digit with (i-1)'th
    number[smallest], number[i-1] = number[i-1], number[smallest]

    # X is the final number, in integer datatype
    x = 0
    # Converting list upto i-1 into number
    for j in range(i):
        x = x * 10 + number[j]

    # Sort the digits after i-1 in ascending order
    number = sorted(number[i:])
    # converting the remaining sorted digits into number
    for j in range(n-i):
        x = x * 10 + number[j]

    print("Next number with set of digits is", x)


# Driver Program to test above function
digits = "534976"

# converting into integer array,
# number becomes [5,3,4,9,7,6]
number = map(int, digits)
findNext(number, len(digits))


'''The above implementation can be optimized in following ways. 
1) We can use binary search in step II instead of linear search. 
2) In step IV, instead of doing simple sort, we can apply some clever technique to do it in linear time. 
Hint: We know that all digits are linearly sorted in reverse order except one digit which was swapped.
With above optimizations, we can say that the time complexity of this method is O(n). 

Optimised Approach : 

1. Here instead of sorting the digits after (i-1) index, we are reversing the digits as mentioned 
in the above optimisation point.
2. As they will be in decreasing order so to find the smallest element possible from the right part 
we just reverse them thus reducing time complexity.

Below is the implementation of the above approach:'''


# A python program to find the next greatest number
def nextPermutation(arr):

    # find the length of the array
    n = len(arr)

    # start from the right most digit and find the first
    # digit that is smaller than the digit next to it.
    k = n - 2
    while k >= 0:
        if arr[k] < arr[k + 1]:
            break
        k -= 1

    # reverse the list if the digit that is smaller than the
    # digit next to it is not found.
    if k < 0:
        arr = arr[::-1]
    else:

        # find the first greatest element than arr[k] from the
        # end of the list
        for l in range(n - 1, k, -1):
            if arr[l] > arr[k]:
                break

        # swap the elements at arr[k] and arr[l
        arr[l], arr[k] = arr[k], arr[l]

        # reverse the list from k + 1 to the end to find the
        # most nearest greater number to the given input number
        arr[k + 1:] = reversed(arr[k + 1:])

    return arr


# Driver code
arr = [5, 3, 4, 9, 7, 6]
print(*nextPermutation(arr))
