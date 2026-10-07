/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 853 - Car Fleet
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N log N)
 * - Space: O(N)
 */

package main

import "sort"

type car struct {
	pos  int
	time float64
}

func carFleet(target int, position []int, speed []int) int {
	n := len(position)
	if n == 0 {
		return 0
	}

	cars := make([]car, n)
	for i := 0; i < n; i++ {
		cars[i] = car{
			pos:  position[i],
			time: float64(target-position[i]) / float64(speed[i]),
		}
	}

	sort.Slice(cars, func(i, j int) bool {
		return cars[i].pos < cars[j].pos
	})

	fleets := 0
	var maxTime float64 = 0

	for i := n - 1; i >= 0; i-- {
		if cars[i].time > maxTime {
			maxTime = cars[i].time
			fleets++
		}
	}

	return fleets
}
