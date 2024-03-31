'''
https://www.geeksforgeeks.org/convert-sentence-equivalent-mobile-numeric-keypad-sequence/
https://youtu.be/hE4Nn7o4GFw
Given a sentence in the form of a string, convert it into its equivalent mobile numeric keypad sequence. 
 

Mobile-keypad

In case you wish to attend live classes with experts, please refer DSA Live Classes for Working Professionals and Competitive Programming Live for Students.
Examples : 
 



Input : GEEKSFORGEEKS
Output : 4333355777733366677743333557777
For obtaining a number, we need to press a
number corresponding to that character for 
number of times equal to position of the 
character. For example, for character C, 
we press number 2 three times and accordingly.

Input : HELLO WORLD
Output : 4433555555666096667775553
 

Follow the steps given below to convert a sentence into its equivalent mobile numeric keypad sequence. 
 

For each character, store the sequence which should be obtained at its respective position in an array, i.e. for Z, store 9999. For Y, store 999. For K, store 55 and so on.
For each character, subtract ASCII value of ‘A’ and obtain the position in the array pointed 
by that character and add the sequence stored in that array to a string.
If the character is a space, store 0
Print the overall sequence.
Below is the implementation of above method : '''
# Python3 implementation to convert
# a sentence into its equivalent
# mobile numeric keypad sequence

# Function which computes the
# sequence


def printSequence(arr, input):

    # length of input string
    n = len(input)
    output = ""

    for i in range(n):

        # checking for space
        if(input[i] == ' '):
            output = output + "0"
        else:
            # calculating index for each
            # character
            position = ord(input[i]) - ord('A')
            output = output + arr[position]
    # output sequence
    return output


# Driver code
str = ["2", "22", "222",
       "3", "33", "333",
       "4", "44", "444",
       "5", "55", "555",
       "6", "66", "666",
       "7", "77", "777", "7777",
       "8", "88", "888",
       "9", "99", "999", "9999"]

input = "GEEKSFORGEEKS"
print(printSequence(str, input))
