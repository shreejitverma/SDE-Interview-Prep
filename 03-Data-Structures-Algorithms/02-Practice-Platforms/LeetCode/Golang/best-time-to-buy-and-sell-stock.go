/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

import "math"

func maxProfit(prices []int) int {
    minPrice := math.MaxInt32
    maxProfit := 0
    for _, price := range prices {
        if price < minPrice {
            minPrice = price
        } else if price-minPrice > maxProfit {
            maxProfit = price - minPrice
        }
    }
    return maxProfit
}
