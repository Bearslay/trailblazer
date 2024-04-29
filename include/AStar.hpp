#ifndef ASTAR
#define ASTAR

#include <utility>
#include <vector>
#include <cmath>

#include <iostream>

#define CARDINAL_DISTANCE 1
#define DIAGONAL_DISTANCE 1.41421356

#define ASTAR_MOVE_CARDINAL 0
#define ASTAR_MOVE_COMPASS 1
#define ASTAR_MOVE_ANY 2

double dHeightMax = 1.0;

unsigned char moveStyle = ASTAR_MOVE_COMPASS;

std::vector<std::vector<double>> Grid = {
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0},
    {1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0},
    {1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
    {1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0},
    {1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0},
    {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0},
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}
};
std::pair<unsigned long int, unsigned long int> start = {1, 1}, goal = {1, 14};

struct Cell {
    int parentRow, parentCol;

    double f, g, h;
};

// Return true if the coordinate is within the grid, false if not
bool isValid(const int &row, const int &col) {return row >= 0 && row < Grid.size() && col >= 0 && col < Grid[0].size();}

// Return true if the coordinate can be used (is not a wall)
bool isUnblocked(const int &row, const int &col) {return Grid[row][col] < 1.0;}

// Return true if the coordinate matches the goal's coordinate
bool isGoal(const int &row, const int &col) {return row == goal.first && col == goal.second;}

double calcHVal(const int &row, const int &col) {
    switch (moveStyle) {
        case ASTAR_MOVE_CARDINAL:
            return (double)std::fabs(row - goal.first) + std::fabs(col - goal.second);
        case ASTAR_MOVE_COMPASS:
            return std::sqrt((row - goal.first) * (row - goal.first) + (col - goal.second) * (col - goal.second));
    }

    const double dx = std::fabs(row - goal.first);
    const double dy = std::fabs(col - goal.second);
    return CARDINAL_DISTANCE * (dx + dy) + (DIAGONAL_DISTANCE - 2 * CARDINAL_DISTANCE) * std::min(dx, dy);
}

void aStar() {
    if (!isValid(start.first, start.second) || !isValid(goal.first, goal.second)) {
        std::cout << "[Error] Invalid start or goal\n";
        return;
    }
    if (!isUnblocked(start.first, start.second) || !isUnblocked(goal.first, goal.second)) {
        std::cout << "[Error] Blocked start or goal\n";
        return;
    }
    if (isGoal(start.first, start.second)) {
        std::cout << "[Success] Started at goal\n";
        return;
    }

    std::vector<std::vector<bool>> closedList;
    std::vector<std::vector<Cell>> cellDetails;

    for (unsigned long int i = 0; i < Grid.size(); i++) {
        closedList.emplace_back();
        cellDetails.emplace_back();
        for (unsigned long int j = 0; j < Grid[i].size(); j++) {
            closedList[i].emplace_back(false);

            cellDetails[i].emplace_back();
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].f = __FLT_MAX__;
            cellDetails[i][j].g = __FLT_MAX__;
            cellDetails[i][j].h = __FLT_MAX__;
        }
    }

    int row = start.first, col = start.second;
    cellDetails[row][col].f = cellDetails[row][col].g = cellDetails[row][col].h = 0.0;
    cellDetails[row][col].parentRow = row;
    cellDetails[row][col].parentCol = col;

    std::vector<std::pair<double, std::pair<int, int>>> openList = {{0.0, {row, col}}};

    bool foundGoal = false;

    while (openList.size() != 0) {
        std::pair<double, std::pair<int, int>> p = *openList.begin();
        openList.erase(openList.begin());

        row = p.second.first;
        col = p.second.second;
        closedList[row][col] = true;


    }

    if (!foundGoal) {std::cout << "[Error] No path to goal\n";}

    return;
}

#endif /* ASTAR */
