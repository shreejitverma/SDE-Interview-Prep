/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 853 - Car Fleet
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N log N)
 * - Space: O(N)
 */

function carFleet(target: number, position: number[], speed: number[]): number {
    const n = position.length;
    if (n === 0) return 0;

    const cars: [number, number][] = [];
    for (let i = 0; i < n; i++) {
        cars.push([position[i], (target - position[i]) / speed[i]]);
    }

    cars.sort((a, b) => a[0] - b[0]);

    let fleets = 0;
    let maxTime = 0;

    for (let i = n - 1; i >= 0; i--) {
        if (cars[i][1] > maxTime) {
            maxTime = cars[i][1];
            fleets++;
        }
    }

    return fleets;
}
