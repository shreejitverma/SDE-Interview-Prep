/*
https://www.geeksforgeeks.org/find-the-k-th-permutation-sequence-of-first-n-natural-numbers/
Given two integers N and K, find the Kth permutation sequence of numbers from 1 to N without using STL function.
Note: Assume that the inputs are such that Kth permutation of N number is always possible.

Examples: 

Naive Approach:
To solve the problem mentioned above the simple approach is to find all permutation sequences and output the kth out of them. But this method is not so efficient and takes more time, hence it can be optimized.



Efficient Approach:
To optimize the above method mentioned above, observe that the value of k can be directly used to find the number at each index of the sequence.  

The first position of an n length sequence is occupied by each of the numbers from 1 to n exactly n! / n that is (n-1)! number of times and in ascending order. So the first position of the kth sequence will be occupied by the number present at index = k / (n-1)! (according to 1-based indexing).
The currently found number can not occur again so it is removed from the original n numbers and now the problem reduces to finding the ( k % (n-1)! )th permutation sequence of the remaining n-1 numbers.
This process can be repeated until we have only one number left which will be placed in the first position of the last 1-length sequence.
The factorial values involved here can be very large as compared to k. So, the trick used to avoid the full computation of such large factorials is that as soon as the product n * (n-1) * … becomes greater than k, we no longer need to find the actual factorial value because: 
k / n_actual_factorial_value = 0 
and k / n_partial_factorial_value = 0 
when partial_factorial_value > k 
 

Below is the implementation of the above approach: 
*/

//  C++ program to Find the kth Permutation
// Sequence of first n natural numbers

#include <bits/stdc++.h>
using namespace std;

// Function to find the index of number
// at first position of
// kth sequence of set of size n
int findFirstNumIndex(int &k, int n)
{

    if (n == 1)
        return 0;
    n--;

    int first_num_index;
    // n_actual_fact = n!
    int n_partial_fact = n;

    while (k >= n_partial_fact && n > 1)
    {
        n_partial_fact = n_partial_fact * (n - 1);
        n--;
    }

    // First position of the
    // kth sequence will be
    // occupied by the number present
    // at index = k / (n-1)!
    first_num_index = k / n_partial_fact;

    k = k % n_partial_fact;

    return first_num_index;
}

// Function to find the
// kth permutation of n numbers
string findKthPermutation(int n, int k)
{
    // Store final answer
    string ans = "";

    set<int> s;

    // Insert all natural number
    // upto n in set
    for (int i = 1; i <= n; i++)
        s.insert(i);

    set<int>::iterator itr;

    // Mark the first position
    itr = s.begin();

    // subtract 1 to get 0 based indexing
    k = k - 1;

    for (int i = 0; i < n; i++)
    {

        int index = findFirstNumIndex(k, n - i);

        advance(itr, index);

        // itr now points to the
        // number at index in set s
        ans += (to_string(*itr));

        // remove current number from the set
        s.erase(itr);

        itr = s.begin();
    }
    return ans;
}

// Driver code
int main()
{

    int n = 3, k = 4;

    string kth_perm_seq = findKthPermutation(n, k);

    cout << kth_perm_seq << endl;

    return 0;
}