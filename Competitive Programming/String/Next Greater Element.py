'''https://www.geeksforgeeks.org/next-greater-element/

Given an array, print the Next Greater Element (NGE) for every element. 
The Next greater Element for an element x is the first greater element on the right side of x in the array. 
Elements for which no greater element exist, consider the next greater element as -1. 

Examples: 
For an array, the rightmost element always has the next greater element as -1.
For an array that is sorted in decreasing order, all elements have the next greater element as -1.
For the input array [4, 5, 2, 25], the next greater elements for each element are as follows.
Element       NGE
   4      -->   5
   5      -->   25
   2      -->   25
   25     -->   -1
d) For the input array [13, 7, 6, 12}, the next greater elements for each element are as follows.  



Method 1 (Simple) 
Use two loops: The outer loop picks all the elements one by one. 
The inner loop looks for the first greater element for the element picked by the outer loop. 
If a greater element is found then that element is printed as next, otherwise, -1 is printed.

Below is the implementation of the above approach:

'''
# Function to print element and NGE pair for all elements of list


def printNGE(arr):

    for i in range(0, len(arr), 1):

        next = -1
        for j in range(i+1, len(arr), 1):
            if arr[i] < arr[j]:
                next = arr[j]
                break

        print(str(arr[i]) + " -- " + str(next))


# Driver program to test above function
arr = [11, 13, 21, 3]
printNGE(arr)


'''Time Complexity: O(N2) 
Auxiliary Space: O(1)
 

Method 2 (Using Stack) 

Push the first element to stack.
Pick rest of the elements one by one and follow the following steps in loop. 
Mark the current element as next.
If stack is not empty, compare top element of stack with next.
If next is greater than the top element, Pop element from stack. 
next is the next greater element for the popped element.
Keep popping from the stack while the popped element is smaller than next. 
next becomes the next greater element for all such popped elements.
Finally, push the next in the stack.
After the loop in step 2 is over, pop all the elements from the stack and print -1 as the next element for them.'''

# Python program to print next greater element using stack

# Stack Functions to be used by printNGE()


def createStack():
    stack = []
    return stack


def isEmpty(stack):
    return len(stack) == 0


def push(stack, x):
    stack.append(x)


def pop(stack):
    if isEmpty(stack):
        print("Error : stack underflow")
    else:
        return stack.pop()


'''prints element and NGE pair for all elements of
   arr[] '''


def printNGE(arr):
    s = createStack()
    element = 0
    next = 0

    # push the first element to stack
    push(s, arr[0])

    # iterate for rest of the elements
    for i in range(1, len(arr), 1):
        next = arr[i]

        if isEmpty(s) == False:

            # if stack is not empty, then pop an element from stack
            element = pop(s)

            '''If the popped element is smaller than next, then
                a) print the pair
                b) keep popping while elements are smaller and
                   stack is not empty '''
            while element < next:
                print(str(element) + " -- " + str(next))
                if isEmpty(s) == True:
                    break
                element = pop(s)

            '''If element is greater than next, then push
               the element back '''
            if element > next:
                push(s, element)

        '''push next to stack so that we can find
           next greater for it '''
        push(s, next)

    '''After iterating over the loop, the remaining
       elements in stack do not have the next greater
       element, so print -1 for them '''

    while isEmpty(s) == False:
        element = pop(s)
        next = -1
        print(str(element) + " -- " + str(next))


# Driver code
arr = [11, 13, 21, 3]
printNGE(arr)

'''Method 3:

1. This is same as above method but the elements are pushed and popped only once into the stack. 
The array is changed in place. 
The array elements are pushed into the stack until it finds a greatest element in the right of array. 
In other words the elements are popped from stack when top of the stack value is smaller in the current array element.

2. Once all the elements are processed in the array but stack is not empty. 
The left out elements in the stack doesn’t encounter any greatest element . 
So pop the element from stack and change it’s index value as -1 in the array.'''

# Python3 code


class Solution:
    def nextLargerElement(self, arr, n):
        # code here
        s = []
        for i in range(len(arr)):
            while s and s[-1].get("value") < arr[i]:
                d = s.pop()
                arr[d.get("ind")] = arr[i]
            s.append({"value": arr[i], "ind": i})
        while s:
            d = s.pop()
            arr[d.get("ind")] = -1
        return arr


if __name__ == "__main__":
    print(Solution().nextLargerElement([6, 8, 0, 1, 3], 5))
