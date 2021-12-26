'''https://practice.geeksforgeeks.org/problems/sort-a-stack/1
Sort a stack 
Easy Accuracy: 50.51% Submissions: 57234 Points: 2
Given a stack, the task is to sort it such that the top of the stack has the greatest element.

Example 1:

Input:
Stack: 3 2 1
Output: 3 2 1
Example 2:

Input:
Stack: 11 2 32 3 41
Output: 41 32 11 3 2
Your Task: 
You don't have to read input or print anything. Your task is to complete the function sort() which sorts the elements present in the given stack. (The sorted stack is printed by the driver's code by popping the elements of the stack.)

Expected Time Complexity: O(N*N)
Expected Auxilliary Space: O(N) recursive.

Constraints:
1<=N<=100

Note:The Input/Ouput format and Example given are used for system's internal purpose, and should be used by a user for Expected Output only. As it is a function problem, hence a user should not read any input from stdin/console. The task is to complete the function specified, and not to write the full code.'''
# Python program to sort a stack using recursion

# Recursive method to insert element in sorted way


def sortedInsert(s, element):

    # Base case: Either stack is empty or newly inserted
    # item is greater than top (more than all existing)
    if len(s) == 0 or element > s[-1]:
        s.append(element)
        return
    else:

        # Remove the top item and recur
        temp = s.pop()
        sortedInsert(s, element)

        # Put back the top item removed earlier
        s.append(temp)

# Method to sort stack


def sortStack(s):

    # If stack is not empty
    if len(s) != 0:

        # Remove the top item
        temp = s.pop()

        # Sort remaining stack
        sortStack(s)

        # Push the top item back in sorted stack
        sortedInsert(s, temp)

# Printing contents of stack


def printStack(s):
    for i in s[::-1]:
        print(i, end=" ")
    print()


# Driver Code
if __name__ == '__main__':
    s = []
    s.append(30)
    s.append(-5)
    s.append(18)
    s.append(14)
    s.append(-3)

    print("Stack elements before sorting: ")
    printStack(s)

    sortStack(s)

    print("\nStack elements after sorting: ")
    printStack(s)
