'''https://leetcode.com/problems/stock-price-fluctuation/
2034. Stock Price Fluctuation
Medium

174

16

Add to List

Share
You are given a stream of records about a particular stock. Each record contains a timestamp and the corresponding price of the stock at that timestamp.

Unfortunately due to the volatile nature of the stock market, the records do not come in order. Even worse, some records may be incorrect. Another record with the same timestamp may appear later in the stream correcting the price of the previous wrong record.

Design an algorithm that:

Updates the price of the stock at a particular timestamp, correcting the price from any previous records at the timestamp.
Finds the latest price of the stock based on the current records. The latest price is the price at the latest timestamp recorded.
Finds the maximum price the stock has been based on the current records.
Finds the minimum price the stock has been based on the current records.
Implement the StockPrice class:

StockPrice() Initializes the object with no price records.
void update(int timestamp, int price) Updates the price of the stock at the given timestamp.
int current() Returns the latest price of the stock.
int maximum() Returns the maximum price of the stock.
int minimum() Returns the minimum price of the stock.
 

Example 1:

Input
["StockPrice", "update", "update", "current", "maximum", "update", "maximum", "update", "minimum"]
[[], [1, 10], [2, 5], [], [], [1, 3], [], [4, 2], []]
Output
[null, null, null, 5, 10, null, 5, null, 2]

Explanation
StockPrice stockPrice = new StockPrice();
stockPrice.update(1, 10); // Timestamps are [1] with corresponding prices [10].
stockPrice.update(2, 5);  // Timestamps are [1,2] with corresponding prices [10,5].
stockPrice.current();     // return 5, the latest timestamp is 2 with the price being 5.
stockPrice.maximum();     // return 10, the maximum price is 10 at timestamp 1.
stockPrice.update(1, 3);  // The previous timestamp 1 had the wrong price, so it is updated to 3.
                          // Timestamps are [1,2] with corresponding prices [3,5].
stockPrice.maximum();     // return 5, the maximum price is 5 after the correction.
stockPrice.update(4, 2);  // Timestamps are [1,2,4] with corresponding prices [3,5,2].
stockPrice.minimum();     // return 2, the minimum price is 2 at timestamp 4.
 

Constraints:

1 <= timestamp, price <= 109
At most 105 calls will be made in total to update, current, maximum, and minimum.
current, maximum, and minimum will be called only after update has been called at least once.'''

# Time:  ctor:    O(1)
#        update:  O(logn)
#        current: O(1)
#        max:     O(1)
#        min:     O(1)
# Space: O(n)

import heapq
from sortedcontainers import SortedList


class StockPrice(object):

    def __init__(self):
        self.__curr = 0
        self.__lookup = {}
        self.__sl_by_price = SortedList()

    def update(self, timestamp, price):
        """
        :type timestamp: int
        :type price: int
        :rtype: None
        """
        if timestamp > self.__curr:
            self.__curr = timestamp
        if timestamp in self.__lookup:
            self.__sl_by_price.remove(self.__lookup[timestamp])
        self.__lookup[timestamp] = price
        self.__sl_by_price.add(price)

    def current(self):
        """
        :rtype: int
        """
        return self.__lookup[self.__curr]

    def maximum(self):
        """
        :rtype: int
        """
        return next(reversed(self.__sl_by_price))

    def minimum(self):
        """
        :rtype: int
        """
        return next(iter(self.__sl_by_price))


# Time:  ctor:    O(1)
#        update:  O(logn)
#        current: O(1)
#        max:     O(logn) on average
#        min:     O(logn) on average
# Space: O(n)


class StockPrice2(object):

    def __init__(self):
        self.__curr = 0
        self.__lookup = {}
        self.__min_heap = []
        self.__max_heap = []

    def update(self, timestamp, price):
        """
        :type timestamp: int
        :type price: int
        :rtype: None
        """
        def full_delete(heap, sign):  # Time: O(n), Space: O(n)
            heap[:] = [x for x in set(heap) if sign *
                       x[0] == self.__lookup[x[1]]]
            heapq.heapify(heap)

        if timestamp > self.__curr:
            self.__curr = timestamp
        self.__lookup[timestamp] = price
        heapq.heappush(self.__min_heap, (price, timestamp))
        heapq.heappush(self.__max_heap, (-price, timestamp))
        # avoid too much expired or duplicated data
        if len(self.__min_heap) > 2*len(self.__lookup):
            full_delete(self.__min_heap, 1)
            full_delete(self.__max_heap, -1)

    def current(self):
        """
        :rtype: int
        """
        return self.__lookup[self.__curr]

    def maximum(self):
        """
        :rtype: int
        """
        while self.__max_heap and self.__lookup[self.__max_heap[0][1]] != -self.__max_heap[0][0]:  # lazy delete
            heapq.heappop(self.__max_heap)
        return -self.__max_heap[0][0]

    def minimum(self):
        """
        :rtype: int
        """
        while self.__min_heap and self.__lookup[self.__min_heap[0][1]] != self.__min_heap[0][0]:  # lazy delete
            heapq.heappop(self.__min_heap)
        return self.__min_heap[0][0]
