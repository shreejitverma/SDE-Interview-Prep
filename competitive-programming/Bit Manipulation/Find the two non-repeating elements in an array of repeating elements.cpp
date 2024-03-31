/*Given an array A containing 2*N+2 positive numbers, out of which 2*N numbers exist in pairs whereas the other two number occur exactly once and are distinct. Find the other two numbers.


Example 1:

Input: 
N = 2
arr[] = {1, 2, 3, 2, 1, 4}
Output:
3 4 
Explanation:
3 and 4 occur exactly once.
Example 2:

Input:
N = 1
arr[] = {2, 1, 3, 2}
Output:
1 3
Explanation:
1 3 occur exactly once.

Your Task:
You do not need to read or print anything. Your task is to complete the function singleNumber() which takes the array as input parameter and returns a list of two numbers which occur exactly once in the array. The list must be in ascending order.


Expected Time Complexity: O(N)
Expected Space Complexity: O(1)


Constraints:
1 <= length of array <= 106 
1 <= Elements in array <= 5 * 106
// C++ program for above approach
*/
#include <bits/stdc++.h>
using namespace std;

/* This function sets the values of
*x and *y to non-repeating elements
in an array arr[] of size n*/
void get2NonRepeatingNos(int arr[], int n, int *x, int *y)
{
    /* Will hold Xor of all elements */
    int Xor = arr[0];

    /* Will have only single set bit of Xor */
    int set_bit_no;
    int i;
    *x = 0;
    *y = 0;

    /* Get the Xor of all elements */
    for (i = 1; i < n; i++)
        Xor ^= arr[i];

    /* Get the rightmost set bit in set_bit_no */
    set_bit_no = Xor & ~(Xor - 1);

    /* Now divide elements in two sets by
    comparing rightmost set bit of Xor with bit
    at same position in each element. */
    for (i = 0; i < n; i++)
    {

        /*Xor of first set */
        if (arr[i] & set_bit_no)
            *x = *x ^ arr[i];
        /*Xor of second set*/
        else
        {
            *y = *y ^ arr[i];
        }
    }
}

/* Driver code */
int main()
{
    int arr[] = {2, 3, 7, 9, 11, 2, 3, 11};
    int n = sizeof(arr) / sizeof(*arr);
    int *x = new int[(sizeof(int))];
    int *y = new int[(sizeof(int))];
    get2NonRepeatingNos(arr, n, x, y);
    cout << "The non-repeating elements are " << *x
         << " and " << *y;
}