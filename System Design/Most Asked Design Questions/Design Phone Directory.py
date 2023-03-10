"""
https://leetcode.com/problems/design-phone-directory/

379. Design Phone Directory
Medium

263

367

Add to List

Share
Design a phone directory that initially has maxNumbers empty slots that can store numbers. The directory should store numbers, check if a certain slot is empty or not, and empty a given slot.

Implement the PhoneDirectory class:

PhoneDirectory(int maxNumbers) Initializes the phone directory with the number of available slots maxNumbers.
int get() Provides a number that is not assigned to anyone. Returns -1 if no number is available.
bool check(int number) Returns true if the slot number is available and false otherwise.
void release(int number) Recycles or releases the slot number.
 

Example 1:

Input
["PhoneDirectory", "get", "get", "check", "get", "check", "release", "check"]
[[3], [], [], [2], [], [2], [2], [2]]
Output
[null, 0, 1, true, 2, false, null, true]

Explanation
PhoneDirectory phoneDirectory = new PhoneDirectory(3);
phoneDirectory.get();      // It can return any available phone number. Here we assume it returns 0.
phoneDirectory.get();      // Assume it returns 1.
phoneDirectory.check(2);   // The number 2 is available, so return true.
phoneDirectory.get();      // It returns 2, the only number that is left.
phoneDirectory.check(2);   // The number 2 is no longer available, so return false.
phoneDirectory.release(2); // Release number 2 back to the pool.
phoneDirectory.check(2);   // Number 2 is available again, return true.
 

Constraints:

1 <= maxNumbers <= 104
0 <= number < maxNumbers
At most 2 * 104 calls will be made to get, check, and release.

Design a Phone Directory which supports the following operations:
get: Provide a number which is not assigned to anyone.
check: Check if a number is available or not.
release: Recycle or release a number.
Example:
// Init a phone directory containing a total of 3 numbers: 0, 1, and 2.
PhoneDirectory directory = new PhoneDirectory(3);
// It can return any available phone number. Here we assume it returns 0.
directory.get();
// Assume it returns 1.
directory.get();
// The number 2 is available, so return true.
directory.check(2);
// It returns 2, the only number that is left.
directory.get();
// The number 2 is no longer available, so return false.
directory.check(2);
// Release number 2 back to the pool.
directory.release(2);
// Number 2 is available again, return true.
directory.check(2);
Solution:
1.List/Queue/Stack
2.Set
"""


# List
# faster than 20%
class PhoneDirectory:
    # Time: O(N), where N means maxNumbers
    def __init__(self, maxNumbers: int):
        """
        Initialize your data structure here
        @param maxNumbers - The maximum numbers that can be stored in the phone directory.
        """
        self.d = [x for x in range(maxNumbers)]

    # Time: O(1)
    def get(self) -> int:
        """
        Provide a number which is not assigned to anyone.
        @return - Return an available number. Return -1 if none is available.
        """
        if len(self.d) == 0:
            return -1
        return self.d.pop(0)

    # Time: O(N), where N is the length of self.d
    def check(self, number: int) -> bool:
        """
        Check if a number is available or not.
        """
        if number in self.d:
            return True
        else:
            return False

    # Time: O(1)
    def release(self, number: int) -> None:
        """
        Recycle or release a number.
        """
        if number not in self.d:
            self.d.append(number)


# Your PhoneDirectory object will be instantiated and called as such:
# obj = PhoneDirectory(maxNumbers)
# param_1 = obj.get()
# param_2 = obj.check(number)
# obj.release(number)


# Set
# faster than 50%
class PhoneDirectory:
    # Time: O(N), where N == maxNumbers
    def __init__(self, maxNumbers: int):
        """
        Initialize your data structure here
        @param maxNumbers - The maximum numbers that can be stored in the phone directory.
        """
        self.d = set([x for x in range(maxNumbers)])

    # Time: O(1)
    def get(self) -> int:
        """
        Provide a number which is not assigned to anyone.
        @return - Return an available number. Return -1 if none is available.
        """
        if len(self.d) == 0:
            return -1
        return self.d.pop()

    # Time: O(1), since Python set is implemented as a hash table, lookup/insert/delete in O(1) average.
    def check(self, number: int) -> bool:
        """
        Check if a number is available or not.
        """
        if number in self.d:
            return True
        else:
            return False

    # Time: O(1)
    def release(self, number: int) -> None:
        """
        Recycle or release a number.
        """
        if number not in self.d:
            self.d.add(number)
