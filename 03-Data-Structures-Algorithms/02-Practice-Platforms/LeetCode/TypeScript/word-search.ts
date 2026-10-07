/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N * 4^L) worst case, heavily pruned in practice
// Space: O(L) recursion stack

function exist(board: string[][], word: string): boolean {
    const m = board.length;
    const n = board[0].length;
    const len = word.length;
    if (m * n < len) {
        return false;
    }

    const boardFreq: Map<string, number> = new Map();
    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            const ch = board[r][c];
            boardFreq.set(ch, (boardFreq.get(ch) || 0) + 1);
        }
    }

    const wordFreq: Map<string, number> = new Map();
    for (const ch of word) {
        wordFreq.set(ch, (wordFreq.get(ch) || 0) + 1);
        if ((wordFreq.get(ch) || 0) > (boardFreq.get(ch) || 0)) {
            return false;
        }
    }

    // Optimization: Start from end if prefix has higher frequency on board
    let target = word;
    if ((boardFreq.get(word[0]) || 0) > (boardFreq.get(word[len - 1]) || 0)) {
        target = word.split('').reverse().join('');
    }

    function dfs(r: number, c: number, idx: number): boolean {
        if (idx === target.length) {
            return true;
        }
        if (r < 0 || r >= m || c < 0 || c >= n || board[r][c] !== target[idx]) {
            return false;
        }

        const temp = board[r][c];
        board[r][c] = '#'; // Mark visited

        const found = dfs(r + 1, c, idx + 1)
                   || dfs(r - 1, c, idx + 1)
                   || dfs(r, c + 1, idx + 1)
                   || dfs(r, c - 1, idx + 1);

        board[r][c] = temp; // Backtrack
        return found;
    }

    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (board[r][c] === target[0] && dfs(r, c, 0)) {
                return true;
            }
        }
    }

    return false;
}
