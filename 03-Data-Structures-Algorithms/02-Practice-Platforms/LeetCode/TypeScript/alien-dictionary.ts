/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 269 - Alien Dictionary
 * Difficulty: Hard
 * Language: TypeScript
 *
 * Performance Analysis:
 * - Time Complexity: O(C) where C is the total length of all words.
 * - Space Complexity: O(1) auxiliary space bounded by alphabet size 26.
 */

export function alienOrder(words: string[]): string {
    if (!words || words.length === 0) {
        return '';
    }

    const present: boolean[] = new Array(26).fill(false);
    const inDegree: number[] = new Array(26).fill(0);
    const adj: number[][] = Array.from({ length: 26 }, () => []);
    let uniqueChars = 0;

    for (const w of words) {
        for (let i = 0; i < w.length; ++i) {
            const idx = w.charCodeAt(i) - 97;
            if (!present[idx]) {
                present[idx] = true;
                uniqueChars++;
            }
        }
    }

    for (let i = 0; i < words.length - 1; ++i) {
        const w1 = words[i];
        const w2 = words[i + 1];

        // Prefix violation check: ["abc", "ab"]
        if (w1.length > w2.length && w1.startsWith(w2)) {
            return '';
        }

        const minLen = Math.min(w1.length, w2.length);
        for (let j = 0; j < minLen; ++j) {
            const c1 = w1.charCodeAt(j) - 97;
            const c2 = w2.charCodeAt(j) - 97;
            if (c1 !== c2) {
                adj[c1].push(c2);
                inDegree[c2]++;
                break;
            }
        }
    }

    const queue: number[] = [];
    for (let i = 0; i < 26; ++i) {
        if (present[i] && inDegree[i] === 0) {
            queue.push(i);
        }
    }

    const result: string[] = [];
    let head = 0;
    while (head < queue.length) {
        const u = queue[head++];
        result.push(String.fromCharCode(97 + u));

        for (const v of adj[u]) {
            inDegree[v]--;
            if (inDegree[v] === 0) {
                queue.push(v);
            }
        }
    }

    if (result.length < uniqueChars) {
        return '';
    }

    return result.join('');
}

// Standalone execution test
if (typeof require !== 'undefined' && require.main === module) {
    console.log('Order 1:', alienOrder(['wrt', 'wrf', 'er', 'ett', 'rftt'])); // "wertf"
    console.log('Order 2:', alienOrder(['z', 'x'])); // "zx"
    console.log('Order 3:', alienOrder(['z', 'x', 'z'])); // ""
}
