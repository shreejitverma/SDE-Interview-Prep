'''https://leetcode.com/problems/design-tic-tac-toe/

348. Design Tic-Tac-Toe
Medium

1414

82

Add to List

Share
Assume the following rules are for the tic-tac-toe game on an n x n board between two players:

A move is guaranteed to be valid and is placed on an empty block.
Once a winning condition is reached, no more moves are allowed.
A player who succeeds in placing n of their marks in a horizontal, vertical, or diagonal row wins the game.
Implement the TicTacToe class:

TicTacToe(int n) Initializes the object the size of the board n.
int move(int row, int col, int player) Indicates that the player with id player plays at the cell (row, col) of the board. The move is guaranteed to be a valid move.
 

Example 1:

Input
["TicTacToe", "move", "move", "move", "move", "move", "move", "move"]
[[3], [0, 0, 1], [0, 2, 2], [2, 2, 1], [1, 1, 2], [2, 0, 1], [1, 0, 2], [2, 1, 1]]
Output
[null, 0, 0, 0, 0, 0, 0, 1]

Explanation
TicTacToe ticTacToe = new TicTacToe(3);
Assume that player 1 is "X" and player 2 is "O" in the board.
ticTacToe.move(0, 0, 1); // return 0 (no one wins)
|X| | |
| | | |    // Player 1 makes a move at (0, 0).
| | | |

ticTacToe.move(0, 2, 2); // return 0 (no one wins)
|X| |O|
| | | |    // Player 2 makes a move at (0, 2).
| | | |

ticTacToe.move(2, 2, 1); // return 0 (no one wins)
|X| |O|
| | | |    // Player 1 makes a move at (2, 2).
| | |X|

ticTacToe.move(1, 1, 2); // return 0 (no one wins)
|X| |O|
| |O| |    // Player 2 makes a move at (1, 1).
| | |X|

ticTacToe.move(2, 0, 1); // return 0 (no one wins)
|X| |O|
| |O| |    // Player 1 makes a move at (2, 0).
|X| |X|

ticTacToe.move(1, 0, 2); // return 0 (no one wins)
|X| |O|
|O|O| |    // Player 2 makes a move at (1, 0).
|X| |X|

ticTacToe.move(2, 1, 1); // return 1 (player 1 wins)
|X| |O|
|O|O| |    // Player 1 makes a move at (2, 1).
|X|X|X|
 

Constraints:

2 <= n <= 100
player is 1 or 2.
0 <= row, col < n
(row, col) are unique for each different call to move.
At most n2 calls will be made to move.
 

Follow-up: Could you do better than O(n2) per move() operation?'''

# Time:  O(1), per move.
# Space: O(n^2)


class TicTacToe(object):

    def __init__(self, n):
        self.n = n      # total number
        self.row = [0] * n
        self.col = [0] * n
        self.diag = 0
        self.anti_diag = 0

    def move(self, row, col, player):
        '''
        assign the value of 1 for player 1 and -1 for player 2
        '''
        player_val = 1 if player == 1 else -1
        self.row[row] += player_val
        self.col[col] += player_val

        if row == col:
            self.diag += player_val
            if abs(self.diag) == self.n:
                return player      # win case 1

        if (self.n - 1 - row) == col:
            self.anti_diag += player_val
            if abs(self.anti_diag) == self.n:
                return player  # win case 2

        if abs(self.row[row]) == self.n or \
           abs(self.col[col]) == self.n:
            return player      # win case3, 4

        return 0
