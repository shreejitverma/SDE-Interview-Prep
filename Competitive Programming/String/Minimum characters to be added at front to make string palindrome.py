'''https://www.geeksforgeeks.org/minimum-characters-added-front-make-string-palindrome/

Given a string str we need to tell minimum characters to be added at front of string to make string palindrome.
Examples: 
 

Input  : str = "ABC"
Output : 2
We can make above string palindrome as "CBABC"
by adding 'B' and 'C' at front.

Input  : str = "AACECAAAA";
Output : 2
We can make above string palindrome as AAAACECAAAA
by adding two A's at front of string.
 
In case you wish to attend live classes with experts, please refer DSA Live Classes for Working Professionals and Competitive Programming Live for Students.

Naive approach: Start checking the string each time if it is a palindrome and if not, then delete the last character and check again. When the string gets reduced to wither a palindrome or empty then the number of characters deleted from the end till now will be the answer as those characters could have been inserted at the beginning of the original string in the order which will will make the string a palindrome.
Below is the implementation of the above approach:'''

# Python 3 program for getting minimum character
# to be added at front to make string palindrome

# function for checking string is
# palindrome or not


def ispalindrome(s):

    l = len(s)

    i = 0
    j = l - 1
    while i <= j:

        if(s[i] != s[j]):
            return False
        i += 1
        j -= 1

    return True


# Driver code
if __name__ == "__main__":

    s = "BABABAA"
    cnt = 0
    flag = 0

    while(len(s) > 0):

        # if string becomes palindrome then break
        if(ispalindrome(s)):
            flag = 1
            break

        else:
            cnt += 1

            # erase the last element of the string
            s = s[:-1]

    # print the number of insertion at front
    if(flag):
        print(cnt)
