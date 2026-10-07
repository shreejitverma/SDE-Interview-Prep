/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n * k), where k is the max string length
// Space: O(n * k)

function groupAnagrams(strs: string[]): string[][] {
    const map = new Map<string, string[]>();

    for (const s of strs) {
        const count = new Array(26).fill(0);
        for (let i = 0; i < s.length; i++) {
            count[s.charCodeAt(i) - 97]++;
        }
        const key = count.join("#");

        const group = map.get(key);
        if (group) {
            group.push(s);
        } else {
            map.set(key, [s]);
        }
    }

    return Array.from(map.values());
}
