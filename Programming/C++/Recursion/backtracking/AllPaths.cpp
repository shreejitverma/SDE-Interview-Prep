
#include <iostream>
#include <string>
using namespace std;
void allPath(string p, bool maze[][3], int r, int c)
{
    if (r == 3 - 1 && c == 3 - 1)
    {
        cout << p << endl;
        return;
    }

    if (!maze[r][c])
    {
        return;
    }

    // i am considering this block in my path
    maze[r][c] = false;

    if (r < maze.size() - 1)
    {
        allPath(p + 'D', maze, r + 1, c);
    }

    if (c < maze[0].size() - 1)
    {
        allPath(p + 'R', maze, r, c + 1);
    }

    if (r > 0)
    {
        allPath(p + 'U', maze, r - 1, c);
    }

    if (c > 0)
    {
        allPath(p + 'L', maze, r, c - 1);
    }

    // this line is where the function will be over
    // so before the function gets removed, also remove the changes that were made by that function
    maze[r][c] = true;
}

void allPathPrint(string p, bool maze[][3], int r, int c, int path[][3], int step)
{
    if (r == 3 - 1 && c == sizeof(maze[0]) - 1)
    {
        path[r][c] = step;
        // for (int i = 0; i < 3; i++)
        // {
        //     cout << to_string('path[i]);
        // }
        // cout << p << endl;
        // cout << endl;
        return;
    }

    if (!maze[r][c])
    {
        return;
    }

    // i am considering this block in my path
    maze[r][c] = false;
    path[r][c] = step;
    if (r < 3 - 1)
    {
        allPathPrint(p + to_string('D'), maze, r + 1, c, path, step + 1);
    }

    if (c < sizeof(maze[0]) - 1)
    {
        allPathPrint(p + to_string('R'), maze, r, c + 1, path, step + 1);
    }

    if (r > 0)
    {
        allPathPrint(p + to_string('U'), maze, r - 1, c, path, step + 1);
    }

    if (c > 0)
    {
        allPathPrint(p + to_string('L'), maze, r, c - 1, path, step + 1);
    }

    // this line is where the function will be over
    // so before the function gets removed, also remove the changes that were made by that function
    maze[r][c] = true;
    path[r][c] = 0;
}
int main()
{
    bool board[3][3] = {
        {true, true, true},
        {true, true, true},
        {true, true, true}};
    int path[3][3];
    allPath("", board, 3, 3);
}
