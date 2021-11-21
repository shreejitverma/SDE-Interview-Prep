'''https://practice.geeksforgeeks.org/problems/implement-two-stacks-in-an-array/1

Implement two stacks in an array 
Easy Accuracy: 49.76% Submissions: 64563 Points: 2
Your task is to implement  2 stacks in one array efficiently.

 

Example 1:

Input:
push1(2)
push1(3)
push2(4)
pop1()
pop2()
pop2()

Output:
3 4 -1

Explanation:
push1(2) the stack1 will be {2}
push1(3) the stack1 will be {2,3}
push2(4) the stack2 will be {4}
pop1()   the poped element will be 3 
from stack1 and stack1 will be {2}
pop2()   the poped element will be 4 
from stack2 and now stack2 is empty
pop2()   the stack2 is now empty hence -1 .
 

Your Task:
You don't need to read input or print anything. You are required to complete the 4 methods push1, push2 which takes one argument an integer 'x' to be pushed into stack one and two and pop1, pop2 which returns the integer poped out from stack one and two. If no integer is present in the array return -1.

 

Expected Time Complexity: O(1) for all the four methods.
Expected Auxiliary Space: O(1) for all the four methods.

 

Constraints:
1 <= Number of queries <= 100
1 <= value in the stack <= 100
The sum of elements in both the stacks < size of the given array'''
# User function Template for python3


# Function to push an integer into the stack1.
import sys
import io
import atexit


def push1(a, x):
    a.insert(0, x)

# Function to push an integer into the stack2.


def push2(a, x):
    a.append(x)

# Function to remove an element from top of the stack1.


def pop1(a):
    if a[0] == -1:
        return -1
    return a.pop(0)

# Function to remove an element from top of the stack2.


def pop2(a):
    if a[-1] == -1:
        return -1
    return a.pop(-1)


# {
#  Driver Code Starts
# Initial Template for Python 3


# Contributed by : Nagendra Jha

_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())


top2 = 101
top1 = -1

if __name__ == '__main__':
    test_cases = int(input())
    for cases in range(test_cases):
        n = int(input())
        arr = list(map(int, input().strip().split()))
        a = [-1 for i in range(101)]  # array to be used for the 2 stacks.
        i = 0  # curr index
        while i < len(arr):
            if arr[i] == 1:
                if arr[i+1] == 1:
                    push1(a, arr[i+2])
                    i += 1
                else:
                    print(pop1(a), end=" ")
                i += 1
            else:
                if arr[i+1] == 1:
                    push2(a, arr[i+2])
                    i += 1
                else:
                    print(pop2(a), end=" ")
                i += 1
            i += 1
        top2 = 101
        top1 = -1
        print(' ')

# } Driver Code Ends
