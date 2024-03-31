# https://www.geeksforgeeks.org/a-program-to-check-if-strings-are-rotations-of-each-other/

# Python program to check if strings are rotations of
# each other or not

# Function checks if passed strings (str1 and str2)
# are rotations of each other
def areRotations(string1, string2):
    size1 = len(string1)
    size2 = len(string2)
    temp = ''

    # Check if sizes of two strings are same
    if size1 != size2:
        return 0

    # Create a temp string with value str1.str1
    temp = string1 + string1

    # Now check if str2 is a substring of temp
    # string.count returns the number of occurrences of
    # the second string in temp
    if (temp.count(string2) > 0):
        return 1
    else:
        return 0


# Driver program to test the above function
string1 = "AACD"
string2 = "ACDA"

if areRotations(string1, string2):
    print("Strings are rotations of each other")
else:
    print("Strings are not rotations of each other")


def check_rotation(s, goal):

    if (len(s) != len(goal)):
        skip

    q1 = []
    for i in range(len(s)):
        q1.insert(0, s[i])

    q2 = []
    for i in range(len(goal)):
        q2.insert(0, goal[i])

    k = len(goal)
    while (k > 0):
        ch = q2[0]
        q2.pop(0)
        q2.insert(0, ch)
        if (q2 == q1):
            return True

        k -= 1

    return False


if __name__ == "__main__":

    s1 = "ABCD"
    s2 = "CDAB"
    if (check_rotation(s1, s2)):
        print(s2, " is a rotated form of ", s1)

    else:
        print(s2, " is not a rotated form of ", s1)

    s3 = "ACBD"
    if (check_rotation(s1, s3)):
        print(s3, " is a rotated form of ", s1)

    else:
        print(s3, " is not a rotated form of ", s1)
