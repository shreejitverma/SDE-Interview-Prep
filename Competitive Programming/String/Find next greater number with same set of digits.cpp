/*
https://practice.geeksforgeeks.org/problems/next-permutation5226/1
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
We get “536479” which is the next greater number for input 534976.
*/

// C++ program to find the smallest number which greater than a given number
// and has same set of digits as given number
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;

// Utility function to swap two digits
void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Given a number as a char array number[], this function finds the
// next greater number.  It modifies the same array to store the result
void findNext(char number[], int n)
{
    int i, j;

    // I) Start from the right most digit and find the first digit that is
    // smaller than the digit next to it.
    for (i = n - 1; i > 0; i--)
        if (number[i] > number[i - 1])
            break;

    // If no such digit is found, then all digits are in descending order
    // means there cannot be a greater number with same set of digits
    if (i == 0)
    {
        cout << "Next number is not possible";
        return;
    }

    // II) Find the smallest digit on right side of (i-1)'th digit that is
    // greater than number[i-1]
    int x = number[i - 1], smallest = i;
    for (j = i + 1; j < n; j++)
        if (number[j] > x && number[j] < number[smallest])
            smallest = j;

    // III) Swap the above found smallest digit with number[i-1]
    swap(&number[smallest], &number[i - 1]);

    // IV) Sort the digits after (i-1) in ascending order
    sort(number + i, number + n);

    cout << "Next number with same set of digits is " << number;

    return;
}

// Driver program to test above function
int main()
{
    char digits[] = "534976";
    int n = strlen(digits);
    findNext(digits, n);
    return 0;
}
/*
The above implementation can be optimized in following ways. 
1) We can use binary search in step II instead of linear search. 
2) In step IV, instead of doing simple sort, we can apply some clever technique to do it in linear time. 
Hint: We know that all digits are linearly sorted in reverse order except one digit which was swapped.
With above optimizations, we can say that the time complexity of this method is O(n). 

Optimised Approach : 

1. Here instead of sorting the digits after (i-1) index, we are reversing the digits as mentioned 
in the above optimisation point.
2. As they will be in decreasing order so to find the smallest element possible from the right part 
we just reverse them thus reducing time complexity.

Below is the implementation of the above approach:
*/
#include <bits/stdc++.h>
using namespace std;

vector<int> nextPermutation(int n, vector<int> arr)
{
    // If number of digits is 1 then just return the vector
    if (n == 1)
        return arr;

    // Start from the right most digit and find the first
    // digit that is
    // smaller than the digit next to it.
    int i = 0;
    for (i = n - 1; i > 0; i--)
    {
        if (arr[i] > arr[i - 1])
            break;
    }

    // If there is a possibility of a next greater element
    if (i != 0)
    {
        // Find the smallest digit on right side of (i-1)'th
        // digit that is
        // greater than number[i-1]
        for (int j = n - 1; j >= i; j--)
        {
            if (arr[i - 1] < arr[j])
            {
                // Swap the found smallest digit i.e. arr[j]
                // with arr[i-1]
                swap(arr[i - 1], arr[j]);
                break;
            }
        }
    }

    // Reverse the digits after (i-1) because the digits
    // after (i-1) are in decreasing order and thus we will
    // get the smallest element possible from these digits
    reverse(arr.begin() + i, arr.end());

    // If i is 0 that means elements are in decreasing order
    // Therefore, no greater element possible then we just
    // return the lowest possible
    // order/element formed from these digits by just
    // reversing the vector

    return arr;
}

int main()
{
    int n = 6;
    vector<int> v{5, 3, 4, 9, 7, 6};
    vector<int> res;
    res = nextPermutation(n, v);
    for (int i = 0; i < res.size(); i++)
    {
        cout << res[i] << " ";
    }
}