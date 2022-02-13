#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <stdlib.h>

namespace maze
{
    class MazeGenerator
    {
    public:
        int height;
        int width;
        std::vector<std::vector<bool>> maze;
        std::vector<std::pair<int, int>> skew;

        MazeGenerator(int height, int width)
        {
            this->height = height;
            this->width = width;
            maze = std::vector<std::vector<bool>>(height, std::vector<bool>(width, 1));

            skew.push_back(std::make_pair(1, 0));
            skew.push_back(std::make_pair(0, -1));

        }

        std::vector<std::vector<bool>> generateMaze() {
            maze[1][0] = 0;

            for (int i = 1; i < this->height; i += 2) {
                for (int j = 1; j < this->width; j += 2) {
                    maze[i][j] = 0;
                    std::pair<int, int> chosenPair = getNeighbour(i, j);
                    maze[chosenPair.first][chosenPair.second] = 0;
                }
            }

            return maze;
        }

        ~MazeGenerator()
        { }

    private:
        std::pair<int, int> getNeighbour(int currRow, int currCol) {
            std::vector<std::pair<int, int>> neighbours;

            for (int i = 0; i < skew.size(); i++) {
                int skewRow = skew[i].first;
                int skewCol = skew[i].second;

                int neighbourRow = currRow + skewRow;
                int neighbourCol = currCol + skewCol;
                if (neighbourRow > 0 && neighbourRow < this->height - 1) {
                    if (neighbourCol > 0 && neighbourCol < this->width - 1) {
                        neighbours.push_back(std::make_pair(neighbourRow, neighbourCol));
                    }
                }
            }

            if (neighbours.size() == 0) {
                return std::make_pair(currRow, currCol);
            }
            else {
                return neighbours[rand() % neighbours.size()];
            }
        }
    };
}   // namespace maze