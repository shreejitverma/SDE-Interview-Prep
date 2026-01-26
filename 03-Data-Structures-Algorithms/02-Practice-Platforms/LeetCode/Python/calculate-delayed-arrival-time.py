# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

# Time:  O(1)
# Space: O(1)

# math
class Solution(object):
    def findDelayedArrivalTime(self, arrivalTime, delayedTime):
        """
        :type arrivalTime: int
        :type delayedTime: int
        :rtype: int
        """
        return (arrivalTime + delayedTime)%24
