/*
https://leetcode.com/problems/flood-fill/
733. Flood Fill
Easy

2918

305

Add to List

Share
An image is represented by an m x n integer grid image where image[i][j] represents the pixel value of the image.

You are also given three integers sr, sc, and newColor. You should perform a flood fill on the image starting from the pixel image[sr][sc].

To perform a flood fill, consider the starting pixel, plus any pixels connected 4-directionally to the starting pixel of the same color as the starting pixel, plus any pixels connected 4-directionally to those pixels (also with the same color), and so on. Replace the color of all of the aforementioned pixels with newColor.

Return the modified image after performing the flood fill.

 

Example 1:


Input: image = [[1,1,1],[1,1,0],[1,0,1]], sr = 1, sc = 1, newColor = 2
Output: [[2,2,2],[2,2,0],[2,0,1]]
Explanation: From the center of the image with position (sr, sc) = (1, 1) (i.e., the red pixel), all pixels connected by a path of the same color as the starting pixel (i.e., the blue pixels) are colored with the new color.
Note the bottom corner is not colored 2, because it is not 4-directionally connected to the starting pixel.
Example 2:

Input: image = [[0,0,0],[0,0,0]], sr = 0, sc = 0, newColor = 2
Output: [[2,2,2],[2,2,2]]
 

Constraints:

m == image.length
n == image[i].length
1 <= m, n <= 50
0 <= image[i][j], newColor < 216
0 <= sr < m
0 <= sc < n
*/

// Time:  O(m * n)
// Space: O(m * n)

class Solution
{
public:
    vector<vector<int> > floodFill(vector<vector<int> > &image, int sr, int sc, int newColor)
    {
        int color = image[sr][sc];
        if (color == newColor)
            return image;
        dfs(&image, sr, sc, newColor, color);
        return image;
    }

private:
    void dfs(vector<vector<int> > *image, int r, int c, int newColor, int color)
    {
        static const vector<pair<int, int> > directions{{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
        if (r < 0 || r >= image->size() ||
            c < 0 || c >= (*image)[0].size() ||
            (*image)[r][c] != color)
        {
            return;
        }
        (*image)[r][c] = newColor;
        for (const auto &d : directions)
        {
            dfs(image, r + d.first, c + d.second, newColor, color);
        }
    }
};
