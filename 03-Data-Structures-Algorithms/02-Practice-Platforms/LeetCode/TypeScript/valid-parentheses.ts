/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

function isValid(s: string): boolean {
    if ((s.length & 1) !== 0) {
        return false;
    }
    const stack: string[] = [];
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === '(') {
            stack.push(')');
        } else if (c === '{') {
            stack.push('}');
        } else if (c === '[') {
            stack.push(']');
        } else if (stack.length === 0 || stack.pop() !== c) {
            return false;
        }
    }
    return stack.length === 0;
}
