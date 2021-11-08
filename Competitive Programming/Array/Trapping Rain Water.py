'''
https://practice.geeksforgeeks.org/problems/trapping-rain-water-1587115621/1
https://www.geeksforgeeks.org/trapping-rain-water/
Trapping Rain Water 
Medium Accuracy: 49.62% Submissions: 99632 Points: 4
Given an array arr[] of N non-negative integers representing the height of blocks. 
If width of each block is 1, compute how much water can be trapped between the blocks during the rainy season. 
 

Example 1:

Input:
N = 6
arr[] = {3,0,0,2,0,4}
Output:
10
Explanation: 

Example 2:

Input:
N = 4
arr[] = {7,4,0,9}
Output:
10
Explanation:
Water trapped by above 
block of height 4 is 3 units and above 
block of height 0 is 7 units. So, the 
total unit of water trapped is 10 units.
Example 3:

Input:
N = 3
arr[] = {6,9,9}
Output:
0
Explanation:
No water will be trapped.

Your Task:
You don't need to read input or print anything. The task is to complete the function trappingWater() which takes arr[] and N as input parameters and returns the total amount of water that can be trapped.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
3 < N < 106
0 < Ai < 108


Method 2: This is an efficient solution to the above problem.

Approach: In the previous solution, to find the highest bar on the left and right, 
array traversal is needed which reduces the efficiency of the solution. 
To make this efficient one must pre-compute the highest bar on the left and right of every bar in linear time. 
Then use these pre-computed values to find the amount of water in every array element.
Algorithm: 
Create two arrays left and right of size n. create a variable max_ = INT_MIN.
Run one loop from start to end. In each iteration update max_ as max_ = max(max_, arr[i]) and also assign left[i] = max_
Update max_ = INT_MIN.
Run another loop from end to start. In each iteration update max_ as max_ = max(max_, arr[i]) 
and also assign right[i] = max_
Traverse the array from start to end.
The amount of water that will be stored in this column is min(a,b) – array[i],
(where a = left[i] and b = right[i]) add this value to total amount of water stored
Print the total amount of water stored.
'''
# Python program to find
# maximum amount of water that can
# be trapped within given set of bars.
# Space Complexity : O(1)


def findWater(arr, n):

    # initialize output
    result = 0

    # maximum element on left and right
    left_max = 0
    right_max = 0

    # indices to traverse the array
    lo = 0
    hi = n-1

    while(lo <= hi):

        if(arr[lo] < arr[hi]):

            if(arr[lo] > left_max):

                # update max in left
                left_max = arr[lo]
            else:

                # water on curr element = max - curr
                result += left_max - arr[lo]
            lo += 1

        else:

            if(arr[hi] > right_max):
                # update right maximum
                right_max = arr[hi]
            else:
                result += right_max - arr[hi]
            hi -= 1

    return result

# Driver program


arr = [0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1]
n = len(arr)

print("Maximum water that can be accumulated is ",
      findWater(arr, n))

'''Output: 

Maximum water that can be accumulated is 6
Complexity Analysis: 
Time Complexity: O(n). 
Only one traversal of the array is needed, So time Complexity is O(n).
Space Complexity: O(n). 
Two extra arrays are needed each of size n.
Space Optimization for the above Solution: 

Instead of maintaining two arrays of size n for storing the left 
and a right max of each element, maintain two variables to store the maximum till that point. 
Since water trapped at any element = min(max_left, max_right) – arr[i]. 
Calculate water trapped on smaller elements out of A[lo] and A[hi] first and move the pointers till lo doesn’t cross hi.

Implementation:'''


# Pythpn3 implementation of the approach

# Function to return the maximum
# water that can be stored
def maxWater(arr, n):
    size = n - 1

    # Let the first element be stored as
    # previous, we shall loop from index 1
    prev = arr[0]

    # To store previous wall's index
    prev_index = 0
    water = 0

    # To store the water until a larger wall
    # is found, if there are no larger walls
    # then delete temp value from water
    temp = 0
    for i in range(1, size + 1):

        # If the current wall is taller than
        # the previous wall then make current
        # wall as the previous wall and its
        # index as previous wall's index
        # for the subsequent loops
        if (arr[i] >= prev):
            prev = arr[i]
            prev_index = i

            # Because larger or same height wall is found
            temp = 0
        else:

            # Since current wall is shorter than
            # the previous, we subtract previous
            # wall's height from the current wall's
            # height and add it to the water
            water += prev - arr[i]

            # Store the same value in temp as well
            # If we dont find any larger wall then
            # we will subtract temp from water
            temp += prev - arr[i]

    # If the last wall was larger than or equal
    # to the previous wall then prev_index would
    # be equal to size of the array (last element)
    # If we didn't find a wall greater than or equal
    # to the previous wall from the left then
    # prev_index must be less than the index
    # of the last element
    if (prev_index < size):

        # Temp would've stored the water collected
        # from previous largest wall till the end
        # of array if no larger wall was found then
        # it has excess water and remove that
        # from 'water' var
        water -= temp

        # We start from the end of the array, so previous
        # should be assigned to the last element
        prev = arr[size]

        # Loop from the end of array up to the 'previous index'
        # which would contain the "largest wall from the left"
        for i in range(size, prev_index - 1, -1):

            # Right end wall will be definitely smaller
            # than the 'previous index' wall
            if (arr[i] >= prev):
                prev = arr[i]
            else:
                water += prev - arr[i]

    # Return the maximum water
    return water


# Driver code
arr = [0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1]
n = len(arr)
print(maxWater(arr, n))

'''Method 5 (Two Pointer Approach)

Approach: At every index, The amount of rainwater stored is 
the difference between current index height and a minimum of left max height and right max-height
Algorithm :
Take two pointers l and r. Initialize l to the starting index 0 and r to the last index n-1
Since l is the first element, left_max would be 0, and right max for r would be 0
While l <= r , iterate the array . We have two possible conditions
Condition1 : left_max <= right max
Consider Element at index l
Since we have traversed all elements to the left of l, left_max is known 
For the right max of l, We can say that the right max would  always be >= current r_max here
So, min(left_max,right_max) would always equal to left_max in this case
Increment l
Condition2 : left_max >  right max
Consider Element at index r
Since we have traversed all elements to the right of r, right_max is known
For the left max of l, We can say that the left max would  always be >= current l_max here
So, min(left_max,right_max) would always equal to right_max in this case
Decrement r
Implementation:'''
# Python3 implementation of the approach

# Function to return the maximum
# water that can be stored


def maxWater(arr, n):
    # indices to traverse the array
    left = 0
    right = n-1

    # To store Left max and right max
    # for two pointers left and right
    l_max = 0
    r_max = 0

    # To store the total amount
    # of rain water trapped
    result = 0
    while (left <= right):

        # We need check for minimum of left
        # and right max for each element
        if r_max <= l_max:

            # Add the difference between
            # current value and right max at index r
            result += max(0, r_max-arr[right])

            # Update right max
            r_max = max(r_max, arr[right])

            # Update right pointer
            right -= 1
        else:

            # Add the difference between
            # current value and left max at index l
            result += max(0, l_max-arr[left])

            # Update left max
            l_max = max(l_max, arr[left])

            # Update left pointer
            left += 1
    return result


# Driver code
arr = [0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1]
n = len(arr)
print(maxWater(arr, n))
