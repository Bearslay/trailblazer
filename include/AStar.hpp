#ifndef ASTAR
#define ASTAR

#include <cmath>
#include <vector>

#define CARDINAL_DISTANCE 1
#define DIAGONAL_DISTANCE 1.41421356237309504880

bool isValid(const std::vector<std::vector<double>> &grid, const int &row, const int &col) {return row >= 0 && row < (int)grid.size() && col >= 0 && col < (int)grid.at(row).size();}
bool isUnblocked(const std::vector<std::vector<double>> &grid, const int &row, const int &col) {return grid.at(row).at(col) < 1.0;}
bool isGoal(const std::pair<int, int> &dst, const int &row, const int &col) {return row == dst.first && col == dst.second;}

double calcDist_Cardinal(const std::pair<int, int> &dst, const int &row, const int &col) {return (double)(std::abs(row - dst.first) + std::abs(col - dst.second));}
double calcDist_Compass(const std::pair<int, int> &dst, const int &row, const int &col) {
    const double dx = (double)std::abs(row - dst.first);
    const double dy = (double)std::abs(col - dst.second);
    return CARDINAL_DISTANCE * (dx + dy) + (DIAGONAL_DISTANCE - 2 * CARDINAL_DISTANCE) * std::min(dx, dy);
}
double calcDist_General(const std::pair<int, int> &dst, const int &row, const int &col) {return std::sqrt((row - dst.first) * (row - dst.first) + (col - dst.second) * (col - dst.second));}

struct Cell {
    int parentRow, parentCol;
    double totalCost, fromCost, toCost;
};

std::vector<std::pair<int, int>> aStar_CardinalGrid(const std::vector<std::vector<double>> &grid, const std::pair<int, int> &src, const std::pair<int, int> &dst) {
    if (!isValid(grid, src.first, src.second) || !isValid(grid, dst.first, dst.second)) {return {src};}
    if (!isUnblocked(grid, src.first, src.second) || !isUnblocked(grid, dst.first, dst.second)) {return {src};}
    if (isGoal(dst, src.first, src.second)) {return {src};}

    std::vector<std::vector<bool>> closedList;
    std::vector<std::vector<Cell>> cellDetails;

    for (unsigned long int i = 0; i < grid.size(); i++) {
        closedList.emplace_back();
        cellDetails.emplace_back();
        for (unsigned long int j = 0; j < grid[i].size(); j++) {
            closedList[i].emplace_back(false);

            cellDetails[i].emplace_back();
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].totalCost = __FLT_MAX__;
            cellDetails[i][j].fromCost  = __FLT_MAX__;
            cellDetails[i][j].toCost    = __FLT_MAX__;
        }
    }

    int row = src.first, col = src.second;
    cellDetails[row][col].totalCost = cellDetails[row][col].fromCost = cellDetails[row][col].toCost = 0.0;
    cellDetails[row][col].parentRow = row;
    cellDetails[row][col].parentCol = col;

    std::vector<std::pair<double, std::pair<int, int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};
    bool foundGoal = false;
    double totalCost, fromCost, toCost;
    std::pair<double, std::pair<int, int>> p;

    while (openList.size() > 0) {
        p = *openList.begin();
        openList.erase(openList.begin());

        row = p.second.first;
        col = p.second.second;
        closedList[row][col] = true;

        // North
        if (isValid(grid, row - 1, col)) {
            if (isGoal(dst, row - 1, col)) {
                cellDetails[row - 1][col].parentRow = row;
                cellDetails[row - 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col) && isUnblocked(grid, row - 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Cardinal(dst, row - 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col)));

                    cellDetails[row - 1][col].totalCost = totalCost;
                    cellDetails[row - 1][col].fromCost  = fromCost;
                    cellDetails[row - 1][col].toCost    = toCost;
                    cellDetails[row - 1][col].parentRow = row;
                    cellDetails[row - 1][col].parentCol = col;
                }
            }
        }

        // South
        if (isValid(grid, row + 1, col)) {
            if (isGoal(dst, row + 1, col)) {
                cellDetails[row + 1][col].parentRow = row;
                cellDetails[row + 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col) && isUnblocked(grid, row + 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Cardinal(dst, row + 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col)));

                    cellDetails[row + 1][col].totalCost = totalCost;
                    cellDetails[row + 1][col].fromCost  = fromCost;
                    cellDetails[row + 1][col].toCost    = toCost;
                    cellDetails[row + 1][col].parentRow = row;
                    cellDetails[row + 1][col].parentCol = col;
                }
            }
        }

        // East
        if (isValid(grid, row, col + 1)) {
            if (isGoal(dst, row, col + 1)) {
                cellDetails[row][col + 1].parentRow = row;
                cellDetails[row][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col + 1) && isUnblocked(grid, row, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Cardinal(dst, row, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col + 1)));

                    cellDetails[row][col + 1].totalCost = totalCost;
                    cellDetails[row][col + 1].fromCost  = fromCost;
                    cellDetails[row][col + 1].toCost    = toCost;
                    cellDetails[row][col + 1].parentRow = row;
                    cellDetails[row][col + 1].parentCol = col;
                }
            }
        }

        // West
        if (isValid(grid, row, col - 1)) {
            if (isGoal(dst, row, col - 1)) {
                cellDetails[row][col - 1].parentRow = row;
                cellDetails[row][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col - 1) && isUnblocked(grid, row, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Cardinal(dst, row, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col - 1)));

                    cellDetails[row][col - 1].totalCost = totalCost;
                    cellDetails[row][col - 1].fromCost  = fromCost;
                    cellDetails[row][col - 1].toCost    = toCost;
                    cellDetails[row][col - 1].parentRow = row;
                    cellDetails[row][col - 1].parentCol = col;
                }
            }
        }
    }

    if (!foundGoal) {return {src};}

    row = dst.first, col = dst.second;
    std::vector<std::pair<int, int>> output;

    while (!(cellDetails.at(row).at(col).parentRow == row && cellDetails.at(row).at(col).parentCol == col)) {
        output.insert(output.begin(), std::make_pair(row, col));
        const int rowTemp = cellDetails.at(row).at(col).parentRow, colTemp = cellDetails.at(row).at(col).parentCol;
        row = rowTemp, col = colTemp;
    }
    output.insert(output.begin(), std::make_pair(row, col));

    for (unsigned long int i = 0; i < output.size() / 2; i++) {
        std::pair<int, int> temp = output.at(i);
        output[i] = output.at(output.size() - 1 - i);
        output[output.size() - 1 - i] = temp;
    }
    return output;
}
std::vector<std::pair<int, int>> aStar_CompassGrid(const std::vector<std::vector<double>> &grid, const std::pair<int, int> &src, const std::pair<int, int> &dst) {
    if (!isValid(grid, src.first, src.second) || !isValid(grid, dst.first, dst.second)) {return {src};}
    if (!isUnblocked(grid, src.first, src.second) || !isUnblocked(grid, dst.first, dst.second)) {return {src};}
    if (isGoal(dst, src.first, src.second)) {return {src};}

    std::vector<std::vector<bool>> closedList;
    std::vector<std::vector<Cell>> cellDetails;

    for (unsigned long int i = 0; i < grid.size(); i++) {
        closedList.emplace_back();
        cellDetails.emplace_back();
        for (unsigned long int j = 0; j < grid[i].size(); j++) {
            closedList[i].emplace_back(false);

            cellDetails[i].emplace_back();
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].totalCost = __FLT_MAX__;
            cellDetails[i][j].fromCost  = __FLT_MAX__;
            cellDetails[i][j].toCost    = __FLT_MAX__;
        }
    }

    int row = src.first, col = src.second;
    cellDetails[row][col].totalCost = cellDetails[row][col].fromCost = cellDetails[row][col].toCost = 0.0;
    cellDetails[row][col].parentRow = row;
    cellDetails[row][col].parentCol = col;

    std::vector<std::pair<double, std::pair<int, int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};
    bool foundGoal = false;
    double totalCost, fromCost, toCost;
    std::pair<double, std::pair<int, int>> p;

    while (openList.size() > 0) {
        p = *openList.begin();
        openList.erase(openList.begin());

        row = p.second.first;
        col = p.second.second;
        closedList[row][col] = true;

        // North
        if (isValid(grid, row - 1, col)) {
            if (isGoal(dst, row - 1, col)) {
                cellDetails[row - 1][col].parentRow = row;
                cellDetails[row - 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col) && isUnblocked(grid, row - 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Compass(dst, row - 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col)));

                    cellDetails[row - 1][col].totalCost = totalCost;
                    cellDetails[row - 1][col].fromCost  = fromCost;
                    cellDetails[row - 1][col].toCost    = toCost;
                    cellDetails[row - 1][col].parentRow = row;
                    cellDetails[row - 1][col].parentCol = col;
                }
            }
        }

        // South
        if (isValid(grid, row + 1, col)) {
            if (isGoal(dst, row + 1, col)) {
                cellDetails[row + 1][col].parentRow = row;
                cellDetails[row + 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col) && isUnblocked(grid, row + 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Compass(dst, row + 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col)));

                    cellDetails[row + 1][col].totalCost = totalCost;
                    cellDetails[row + 1][col].fromCost  = fromCost;
                    cellDetails[row + 1][col].toCost    = toCost;
                    cellDetails[row + 1][col].parentRow = row;
                    cellDetails[row + 1][col].parentCol = col;
                }
            }
        }

        // East
        if (isValid(grid, row, col + 1)) {
            if (isGoal(dst, row, col + 1)) {
                cellDetails[row][col + 1].parentRow = row;
                cellDetails[row][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col + 1) && isUnblocked(grid, row, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Compass(dst, row, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col + 1)));

                    cellDetails[row][col + 1].totalCost = totalCost;
                    cellDetails[row][col + 1].fromCost  = fromCost;
                    cellDetails[row][col + 1].toCost    = toCost;
                    cellDetails[row][col + 1].parentRow = row;
                    cellDetails[row][col + 1].parentCol = col;
                }
            }
        }

        // West
        if (isValid(grid, row, col - 1)) {
            if (isGoal(dst, row, col - 1)) {
                cellDetails[row][col - 1].parentRow = row;
                cellDetails[row][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col - 1) && isUnblocked(grid, row, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_Compass(dst, row, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col - 1)));

                    cellDetails[row][col - 1].totalCost = totalCost;
                    cellDetails[row][col - 1].fromCost  = fromCost;
                    cellDetails[row][col - 1].toCost    = toCost;
                    cellDetails[row][col - 1].parentRow = row;
                    cellDetails[row][col - 1].parentCol = col;
                }
            }
        }

        // North-East
        if (isValid(grid, row - 1, col + 1)) {
            if (isGoal(dst, row - 1, col + 1)) {
                cellDetails[row - 1][col + 1].parentRow = row;
                cellDetails[row - 1][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col + 1) && isUnblocked(grid, row - 1, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_Compass(dst, row - 1, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col + 1)));

                    cellDetails[row - 1][col + 1].totalCost = totalCost;
                    cellDetails[row - 1][col + 1].fromCost  = fromCost;
                    cellDetails[row - 1][col + 1].toCost    = toCost;
                    cellDetails[row - 1][col + 1].parentRow = row;
                    cellDetails[row - 1][col + 1].parentCol = col;
                }
            }
        }

        // South-East
        if (isValid(grid, row + 1, col + 1)) {
            if (isGoal(dst, row + 1, col + 1)) {
                cellDetails[row + 1][col + 1].parentRow = row;
                cellDetails[row + 1][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col + 1) && isUnblocked(grid, row + 1, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_Compass(dst, row + 1, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col + 1)));

                    cellDetails[row + 1][col + 1].totalCost = totalCost;
                    cellDetails[row + 1][col + 1].fromCost  = fromCost;
                    cellDetails[row + 1][col + 1].toCost    = toCost;
                    cellDetails[row + 1][col + 1].parentRow = row;
                    cellDetails[row + 1][col + 1].parentCol = col;
                }
            }
        }

        // North-West
        if (isValid(grid, row - 1, col - 1)) {
            if (isGoal(dst, row - 1, col - 1)) {
                cellDetails[row - 1][col - 1].parentRow = row;
                cellDetails[row - 1][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col - 1) && isUnblocked(grid, row - 1, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_Compass(dst, row - 1, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col - 1)));

                    cellDetails[row - 1][col - 1].totalCost = totalCost;
                    cellDetails[row - 1][col - 1].fromCost  = fromCost;
                    cellDetails[row - 1][col - 1].toCost    = toCost;
                    cellDetails[row - 1][col - 1].parentRow = row;
                    cellDetails[row - 1][col - 1].parentCol = col;
                }
            }
        }

        // South-West
        if (isValid(grid, row + 1, col - 1)) {
            if (isGoal(dst, row + 1, col - 1)) {
                cellDetails[row + 1][col - 1].parentRow = row;
                cellDetails[row + 1][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col - 1) && isUnblocked(grid, row + 1, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_Compass(dst, row + 1, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col - 1)));

                    cellDetails[row + 1][col - 1].totalCost = totalCost;
                    cellDetails[row + 1][col - 1].fromCost  = fromCost;
                    cellDetails[row + 1][col - 1].toCost    = toCost;
                    cellDetails[row + 1][col - 1].parentRow = row;
                    cellDetails[row + 1][col - 1].parentCol = col;
                }
            }
        }
    }

    if (!foundGoal) {return {src};}

    row = dst.first, col = dst.second;
    std::vector<std::pair<int, int>> output;

    while (!(cellDetails.at(row).at(col).parentRow == row && cellDetails.at(row).at(col).parentCol == col)) {
        output.insert(output.begin(), std::make_pair(row, col));
        const int rowTemp = cellDetails.at(row).at(col).parentRow, colTemp = cellDetails.at(row).at(col).parentCol;
        row = rowTemp, col = colTemp;
    }
    output.insert(output.begin(), std::make_pair(row, col));

    for (unsigned long int i = 0; i < output.size() / 2; i++) {
        std::pair<int, int> temp = output.at(i);
        output[i] = output.at(output.size() - 1 - i);
        output[output.size() - 1 - i] = temp;
    }
    return output;
}
std::vector<std::pair<int, int>> aStar_GeneralGrid(const std::vector<std::vector<double>> &grid, const std::pair<int, int> &src, const std::pair<int, int> &dst) {
    if (!isValid(grid, src.first, src.second) || !isValid(grid, dst.first, dst.second)) {return {src};}
    if (!isUnblocked(grid, src.first, src.second) || !isUnblocked(grid, dst.first, dst.second)) {return {src};}
    if (isGoal(dst, src.first, src.second)) {return {src};}

    std::vector<std::vector<bool>> closedList;
    std::vector<std::vector<Cell>> cellDetails;

    for (unsigned long int i = 0; i < grid.size(); i++) {
        closedList.emplace_back();
        cellDetails.emplace_back();
        for (unsigned long int j = 0; j < grid[i].size(); j++) {
            closedList[i].emplace_back(false);

            cellDetails[i].emplace_back();
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].parentRow = -1;
            cellDetails[i][j].totalCost = __FLT_MAX__;
            cellDetails[i][j].fromCost  = __FLT_MAX__;
            cellDetails[i][j].toCost    = __FLT_MAX__;
        }
    }

    int row = src.first, col = src.second;
    cellDetails[row][col].totalCost = cellDetails[row][col].fromCost = cellDetails[row][col].toCost = 0.0;
    cellDetails[row][col].parentRow = row;
    cellDetails[row][col].parentCol = col;

    std::vector<std::pair<double, std::pair<int, int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};
    bool foundGoal = false;
    double totalCost, fromCost, toCost;
    std::pair<double, std::pair<int, int>> p;

    while (openList.size() > 0) {
        p = *openList.begin();
        openList.erase(openList.begin());

        row = p.second.first;
        col = p.second.second;
        closedList[row][col] = true;

        // North
        if (isValid(grid, row - 1, col)) {
            if (isGoal(dst, row - 1, col)) {
                cellDetails[row - 1][col].parentRow = row;
                cellDetails[row - 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col) && isUnblocked(grid, row - 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_General(dst, row - 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col)));

                    cellDetails[row - 1][col].totalCost = totalCost;
                    cellDetails[row - 1][col].fromCost  = fromCost;
                    cellDetails[row - 1][col].toCost    = toCost;
                    cellDetails[row - 1][col].parentRow = row;
                    cellDetails[row - 1][col].parentCol = col;
                }
            }
        }

        // South
        if (isValid(grid, row + 1, col)) {
            if (isGoal(dst, row + 1, col)) {
                cellDetails[row + 1][col].parentRow = row;
                cellDetails[row + 1][col].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col) && isUnblocked(grid, row + 1, col)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_General(dst, row + 1, col);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col)));

                    cellDetails[row + 1][col].totalCost = totalCost;
                    cellDetails[row + 1][col].fromCost  = fromCost;
                    cellDetails[row + 1][col].toCost    = toCost;
                    cellDetails[row + 1][col].parentRow = row;
                    cellDetails[row + 1][col].parentCol = col;
                }
            }
        }

        // East
        if (isValid(grid, row, col + 1)) {
            if (isGoal(dst, row, col + 1)) {
                cellDetails[row][col + 1].parentRow = row;
                cellDetails[row][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col + 1) && isUnblocked(grid, row, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_General(dst, row, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col + 1)));

                    cellDetails[row][col + 1].totalCost = totalCost;
                    cellDetails[row][col + 1].fromCost  = fromCost;
                    cellDetails[row][col + 1].toCost    = toCost;
                    cellDetails[row][col + 1].parentRow = row;
                    cellDetails[row][col + 1].parentCol = col;
                }
            }
        }

        // West
        if (isValid(grid, row, col - 1)) {
            if (isGoal(dst, row, col - 1)) {
                cellDetails[row][col - 1].parentRow = row;
                cellDetails[row][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row).at(col - 1) && isUnblocked(grid, row, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + CARDINAL_DISTANCE;
                toCost = calcDist_General(dst, row, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row, col - 1)));

                    cellDetails[row][col - 1].totalCost = totalCost;
                    cellDetails[row][col - 1].fromCost  = fromCost;
                    cellDetails[row][col - 1].toCost    = toCost;
                    cellDetails[row][col - 1].parentRow = row;
                    cellDetails[row][col - 1].parentCol = col;
                }
            }
        }

        // North-East
        if (isValid(grid, row - 1, col + 1)) {
            if (isGoal(dst, row - 1, col + 1)) {
                cellDetails[row - 1][col + 1].parentRow = row;
                cellDetails[row - 1][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col + 1) && isUnblocked(grid, row - 1, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_General(dst, row - 1, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col + 1)));

                    cellDetails[row - 1][col + 1].totalCost = totalCost;
                    cellDetails[row - 1][col + 1].fromCost  = fromCost;
                    cellDetails[row - 1][col + 1].toCost    = toCost;
                    cellDetails[row - 1][col + 1].parentRow = row;
                    cellDetails[row - 1][col + 1].parentCol = col;
                }
            }
        }

        // South-East
        if (isValid(grid, row + 1, col + 1)) {
            if (isGoal(dst, row + 1, col + 1)) {
                cellDetails[row + 1][col + 1].parentRow = row;
                cellDetails[row + 1][col + 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col + 1) && isUnblocked(grid, row + 1, col + 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_General(dst, row + 1, col + 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col + 1).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col + 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col + 1)));

                    cellDetails[row + 1][col + 1].totalCost = totalCost;
                    cellDetails[row + 1][col + 1].fromCost  = fromCost;
                    cellDetails[row + 1][col + 1].toCost    = toCost;
                    cellDetails[row + 1][col + 1].parentRow = row;
                    cellDetails[row + 1][col + 1].parentCol = col;
                }
            }
        }

        // North-West
        if (isValid(grid, row - 1, col - 1)) {
            if (isGoal(dst, row - 1, col - 1)) {
                cellDetails[row - 1][col - 1].parentRow = row;
                cellDetails[row - 1][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row - 1).at(col - 1) && isUnblocked(grid, row - 1, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_General(dst, row - 1, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row - 1).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row - 1).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row - 1, col - 1)));

                    cellDetails[row - 1][col - 1].totalCost = totalCost;
                    cellDetails[row - 1][col - 1].fromCost  = fromCost;
                    cellDetails[row - 1][col - 1].toCost    = toCost;
                    cellDetails[row - 1][col - 1].parentRow = row;
                    cellDetails[row - 1][col - 1].parentCol = col;
                }
            }
        }

        // South-West
        if (isValid(grid, row + 1, col - 1)) {
            if (isGoal(dst, row + 1, col - 1)) {
                cellDetails[row + 1][col - 1].parentRow = row;
                cellDetails[row + 1][col - 1].parentCol = col;
                
                foundGoal = true;
                break;
            } else if (!closedList.at(row + 1).at(col - 1) && isUnblocked(grid, row + 1, col - 1)) {
                fromCost = cellDetails.at(row).at(col).fromCost + DIAGONAL_DISTANCE;
                toCost = calcDist_General(dst, row + 1, col - 1);
                totalCost = fromCost + toCost;

                if (cellDetails.at(row + 1).at(col - 1).totalCost == __FLT_MAX__ || cellDetails.at(row + 1).at(col - 1).totalCost > totalCost) {
                    openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + 1, col - 1)));

                    cellDetails[row + 1][col - 1].totalCost = totalCost;
                    cellDetails[row + 1][col - 1].fromCost  = fromCost;
                    cellDetails[row + 1][col - 1].toCost    = toCost;
                    cellDetails[row + 1][col - 1].parentRow = row;
                    cellDetails[row + 1][col - 1].parentCol = col;
                }
            }
        }
    }

    if (!foundGoal) {return {src};}

    row = dst.first, col = dst.second;
    std::vector<std::pair<int, int>> output;

    while (!(cellDetails.at(row).at(col).parentRow == row && cellDetails.at(row).at(col).parentCol == col)) {
        output.insert(output.begin(), std::make_pair(row, col));
        const int rowTemp = cellDetails.at(row).at(col).parentRow, colTemp = cellDetails.at(row).at(col).parentCol;
        row = rowTemp, col = colTemp;
    }
    output.insert(output.begin(), std::make_pair(row, col));

    for (unsigned long int i = 0; i < output.size() / 2; i++) {
        std::pair<int, int> temp = output.at(i);
        output[i] = output.at(output.size() - 1 - i);
        output[output.size() - 1 - i] = temp;
    }
    return output;
}

#endif /* ASTAR */
