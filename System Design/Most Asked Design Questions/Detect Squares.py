'''https://leetcode.com/problems/detect-squares/
2013. Detect Squares
Medium

134

63

Add to List

Share
You are given a stream of points on the X-Y plane. Design an algorithm that:

Adds new points from the stream into a data structure. Duplicate points are allowed and should be treated as different points.
Given a query point, counts the number of ways to choose three points from the data structure such that the three points and the query point form an axis-aligned square with positive area.
An axis-aligned square is a square whose edges are all the same length and are either parallel or perpendicular to the x-axis and y-axis.

Implement the DetectSquares class:

DetectSquares() Initializes the object with an empty data structure.
void add(int[] point) Adds a new point point = [x, y] to the data structure.
int count(int[] point) Counts the number of ways to form axis-aligned squares with point point = [x, y] as described above.
 

Example 1:


Input
["DetectSquares", "add", "add", "add", "count", "count", "add", "count"]
[[], [[3, 10]], [[11, 2]], [[3, 2]], [[11, 10]], [[14, 8]], [[11, 2]], [[11, 10]]]
Output
[null, null, null, null, 1, 0, null, 2]

Explanation
DetectSquares detectSquares = new DetectSquares();
detectSquares.add([3, 10]);
detectSquares.add([11, 2]);
detectSquares.add([3, 2]);
detectSquares.count([11, 10]); // return 1. You can choose:
                               //   - The first, second, and third points
detectSquares.count([14, 8]);  // return 0. The query point cannot form a square with any points in the data structure.
detectSquares.add([11, 2]);    // Adding duplicate points is allowed.
detectSquares.count([11, 10]); // return 2. You can choose:
                               //   - The first, second, and third points
                               //   - The first, third, and fourth points
 

Constraints:

point.length == 2
0 <= x, y <= 1000
At most 3000 calls in total will be made to add and count.'''

# Time:  ctor:  O(1)
#        add:   O(1)
#        count: O(n)
# Space: O(n)

import collections


class DetectSquares(object):

    def __init__(self):
        self.__x_to_ys = collections.defaultdict(set)
        self.__point_counts = collections.defaultdict(int)

    def add(self, point):
        """
        :type point: List[int]
        :rtype: None
        """
        self.__x_to_ys[point[0]].add(point[1])
        self.__point_counts[tuple(point)] += 1

    def count(self, point):
        """
        :type point: List[int]
        :rtype: int
        """
        result = 0
        for y in self.__x_to_ys[point[0]]:
            if y == point[1]:
                continue
            dy = y-point[1]
            result += self.__point_counts[(point[0], y)]*self.__point_counts[(
                point[0]+dy, point[1])]*self.__point_counts[(point[0]+dy, y)]
            result += self.__point_counts[(point[0], y)]*self.__point_counts[(
                point[0]-dy, point[1])]*self.__point_counts[(point[0]-dy, y)]
        return result


# Time:  ctor:  O(1)
#        add:   O(1)
#        count: O(n)
# Space: O(n)


class DetectSquares2(object):

    def __init__(self):
        self.__points = []
        self.__point_counts = collections.defaultdict(int)

    def add(self, point):
        """
        :type point: List[int]
        :rtype: None
        """
        self.__points.append(point)
        self.__point_counts[tuple(point)] += 1

    def count(self, point):
        """
        :type point: List[int]
        :rtype: int
        """
        result = 0
        for x, y in self.__points:
            if not (point[0] != x and point[1] != y and (abs(point[0]-x) == abs(point[1]-y))):
                continue
            result += self.__point_counts[(point[0], y)] * \
                self.__point_counts[(x, point[1])]
        return result
