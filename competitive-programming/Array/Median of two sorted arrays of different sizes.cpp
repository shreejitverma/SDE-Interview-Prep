/*
https://www.geeksforgeeks.org/median-of-two-sorted-arrays-of-different-sizes/
Given two sorted arrays, a[] and b[], the task is to find the median of these sorted arrays, in O(log n + log m) time complexity, when n is the number of elements in the first array, and m is the number of elements in the second array. 
This is an extension of median of two sorted arrays of equal size problem. Here we handle arrays of unequal size also.

Example: 
Input: ar1[] = {-5, 3, 6, 12, 15}
        ar2[] = {-12, -10, -6, -3, 4, 10}
Output : The median is 3.
Explanation : The merged array is :
        ar3[] = {-12, -10, -6, -5 , -3,
                 3, 4, 6, 10, 12, 15},
       So the median of the merged array is 3

Input: ar1[] = {2, 3, 5, 8}
        ar2[] = {10, 12, 14, 16, 18, 20}
Output : The median is 11.
Explanation : The merged array is :
        ar3[] = {2, 3, 5, 8, 10, 12, 14, 16, 18, 20}
        if the number of the elements are even, 
        so there are two middle elements,
        take the average between the two :
        (10 + 12) / 2 = 11.
Method 1: This method uses a linear and simpler approach. 



Approach: The given arrays are sorted, so merge the sorted arrays in an efficient way and keep the count of elements inserted in the output array or printed form. So when the elements in the output array are half the original size of the given array print the element as a median element. There are two cases: 

Case 1: m+n is odd, the median is at (m+n)/2 th index in the array obtained after merging both the arrays.
Case 2: m+n is even, the median will be average of elements at index ((m+n)/2 – 1) and (m+n)/2 in the array obtained after merging both the arrays
Algorithm: 

Given two arrays are sorted. So they can be merged in O(m+n) time. Create a variable count to have a count of elements in the output array.
If the value of (m+n) is odd then there is only one median else the median is the average of elements at index (m+n)/2 and ((m+n)/2 – 1).
To merge the both arrays, keep two indices i and j initially assigned to 0. Compare the ith index of 1st array and jth index of second, increase the index of the smallest element and increase the count.
Check if the count reached (m+n) / 2 if (m+n) is odd and store the element, if even store the average of (m+n)/2 th and (m+n)/2 -1 th element and print it.
Implementation: 
*/
// A Simple Merge based O(n) solution to find
// median of two sorted arrays
#include <bits/stdc++.h>
using namespace std;

/* This function returns median of ar1[] and ar2[].
Assumption in this function:
Both ar1[] and ar2[] are sorted arrays */
int getMedian(int ar1[], int ar2[], int n, int m)
{
    int i = 0; /* Current index of input array ar1[] */
    int j = 0; /* Current index of input array ar2[] */
    int count;
    int m1 = -1, m2 = -1;

    // Since there are (n+m) elements,
    // There are following two cases
    // if n+m is odd then the middle
    //index is median i.e. (m+n)/2
    if ((m + n) % 2 == 1)
    {
        for (count = 0; count <= (n + m) / 2; count++)
        {
            if (i != n && j != m)
            {
                m1 = (ar1[i] > ar2[j]) ? ar2[j++] : ar1[i++];
            }
            else if (i < n)
            {
                m1 = ar1[i++];
            }
            // for case when j<m,
            else
            {
                m1 = ar2[j++];
            }
        }
        return m1;
    }

    // median will be average of elements
    // at index ((m+n)/2 - 1) and (m+n)/2
    // in the array obtained after merging ar1 and ar2
    else
    {
        for (count = 0; count <= (n + m) / 2; count++)
        {
            m2 = m1;
            if (i != n && j != m)
            {
                m1 = (ar1[i] > ar2[j]) ? ar2[j++] : ar1[i++];
            }
            else if (i < n)
            {
                m1 = ar1[i++];
            }
            // for case when j<m,
            else
            {
                m1 = ar2[j++];
            }
        }
        return (m1 + m2) / 2;
    }
}

/* Driver code */
int main()
{
    int ar1[] = {900};
    int ar2[] = {5, 8, 10, 20};

    int n1 = sizeof(ar1) / sizeof(ar1[0]);
    int n2 = sizeof(ar2) / sizeof(ar2[0]);
    cout << getMedian(ar1, ar2, n1, n2);
    return 0;
}

/*
Output
10
 

Complexity Analysis: 

 



Time Complexity: O(m + n). 
To merge both the arrays O(m+n) time is needed.
Space Complexity: O(1). 
No extra space is required.
 

Efficient solution: 

 

Solution 4 : Binary Search



 

Approach: The given two arrays are sorted, so we can utilize the ability of Binary Search to divide the array and find the median.

 

Median means the point at which the whole array is divide into two parts. Hence since the two arrays are not merged so to get the median we require merging which is costly. Hence instead of merging we will use below given algorithm to efficiently find median.

 

Algorithm:

 

1. Lets assume that there are two arrays A and B with array A having the minimum number of elements.
   If this is not the case than swap A and B to make A having small size.
2. The edge cases like one array is empty or both are empty will be handled.
3. let n be the size of A and m be the size of B.
   Now think of an idea that if we have to find the median than we have to divide the whole merged array into two parts
   namely left and right parts.
   Now since we are given the size of left part (i.e (n+m+1)/2), Now look at below given example.
   
       A-> 1,2,3,4,5     n = 5
       B-> 1,2,3,4,5,6   m = 6
   
    Here merged array will look like :- 1,1,2,2,3,3,4,4,5,5,6 and median then is 3
    
    Now we can see our left part which is underlined. We divide A and B into two parts such that the 
    sum of left part of both A and B will result in left part of merged array.
    
    A-> 1,2,3,4,5     // pointers l =0 and r = n-1 hence mid = (l+r)/2;
       B -> 1,2,3,4,5,6

    we can see that left part of A is given as n/2 and since total length of left part in merged array
    is (m+n+1)/2, so left part of B = (m+n+1)/2-n/2;
    
    Now we just have to confirm if our left and right partitions in A and B are correct or not.
    
4. Now we have 4 variables indicating four values two from array A and two from array B.
    leftA -> Rightmost element in left part of A = 2
    leftb -> Rightmost element in left part of B = 4
    rightA -> Leftmost element in right part of A = 3
    rightB -> Leftmost element in right part of B = 5
    
    Hence to confirm that partition is correct we have to check the following conditions.
    leftA<=rightB and leftB<=rightA  // This is the case when the sum of two parts of A and B results in left part of merged array
    
    if our partition not works that means we have to  find other mid point in A and then left part in B
    This is seen when
     
    leftA > rightB    //means we have to dec size of A's partition
    so do r = mid-1;
    else
        do l =mid+1;
    
    Hence repeat the above steps with new partitions till we get the answers.
5. If leftA<=rightB and leftB<=rightA
    then we get correct partition and our answer depends on the total size of merged array (i.e. m+n)
    
    If (m+n)%2==0
     ans is max(leftA,leftB)+min(rightA,rightB)/2; // max of left part is nearest to median and min of right part is nearest to medain
    else
     ans is max(leftA,leftB);
 
Output
Median of the two arrays are
3
Time Complexity: O(min(log m, log n)) : Since binary search is being applied on the smaller of the 2 arrays
Auxiliary Space: O(1)
Hence the above algorithm can be coded as
*/

#include <bits/stdc++.h>
using namespace std;

// Method to find median
double Median(vector<int> &A, vector<int> &B)
{
    int n = A.size();
    int m = B.size();
    if (n > m)
        return Median(B, A); // Swapping to make A smaller

    int start = 0;
    int end = n;
    int realmidinmergedarray = (n + m + 1) / 2;

    while (start <= end)
    {
        int mid = (start + end) / 2;
        int leftAsize = mid;
        int leftBsize = realmidinmergedarray - mid;
        int leftA = (leftAsize > 0)
                        ? A[leftAsize - 1]
                        : INT_MIN; // checking overflow of indices
        int leftB = (leftBsize > 0) ? B[leftBsize - 1] : INT_MIN;
        int rightA = (leftAsize < n) ? A[leftAsize] : INT_MAX;
        int rightB = (leftBsize < m) ? B[leftBsize] : INT_MAX;

        // if correct partition is done
        if (leftA <= rightB and leftB <= rightA)
        {
            if ((m + n) % 2 == 0)
                return (max(leftA, leftB) + min(rightA, rightB)) / 2.0;
            return max(leftA, leftB);
        }
        else if (leftA > rightB)
        {
            end = mid - 1;
        }
        else
            start = mid + 1;
    }
    return 0.0;
}

// Driver code
int main()
{
    vector<int> arr1 = {-5, 3, 6, 12, 15};
    vector<int> arr2 = {-12, -10, -6, -3, 4, 10};
    cout << "Median of the two arrays are" << endl;
    cout << Median(arr1, arr2);
    return 0;
}