#ifndef ASTAR
#define ASTAR

#include <cmath>
#include <vector>

#define ASTAR_MOVE_NOBOUND 0
#define ASTAR_MOVE_NOPHASE 1
#define ASTAR_MOVE_NOTOUCH 2

class AStar_Grid {
    private:
        struct Cell {
            unsigned long int parentRow = __INT64_MAX__, parentCol = __INT64_MAX__;
            double totalCost = 0.0, fromCost = 0.0, toCost = 0.0;
        };

        static bool isValid(const std::vector<std::vector<double>> &grid, const unsigned long int &row, const unsigned long int &col) {return row < grid.size() && col < grid.at(row).size();}
        static bool isDestination(const std::pair<unsigned long int, unsigned long int> &dst, const unsigned long int &row, const unsigned long int &col) {return row == dst.first && col == dst.second;}

        static bool isUnblocked(const std::vector<std::vector<double>> &grid, const unsigned long int &row, const unsigned long int &col, const double &height, const double &maxAscend, const double &maxDescend) {return grid.at(row).at(col) < height ? height - grid.at(row).at(col) <= maxDescend : grid.at(row).at(col) - height <= maxAscend;}
        static double manhattan(const std::pair<unsigned long int, unsigned long int> &dst, const double &dstHeight, const unsigned long int &row, const unsigned long int &col, const double &height) {return (double)(std::fabs(row - dst.first) + std::fabs(col - dst.second)) + std::fabs(height - dstHeight);}
        static double diagonal(const std::pair<unsigned long int, unsigned long int> &dst, const double &dstHeight, const unsigned long int &row, const unsigned long int &col, const double &height, const double &cardinalDistance, const double &diagonalDistance) {
            const double dx = (double)std::fabs(row - dst.first);
            const double dy = (double)std::fabs(col - dst.second);
            return cardinalDistance * (dx + dy) + (diagonalDistance - 2 * cardinalDistance) * std::min(dx, dy) + std::fabs(height - dstHeight);
        }
        static double euclidean(const std::pair<unsigned long int, unsigned long int> &dst, const double &dstHeight, const unsigned long int &row, const unsigned long int &col, const double &height) {return std::sqrt((row - dst.first) * (row - dst.first) + (col - dst.second) * (col - dst.second) + (height - dstHeight) * (height - dstHeight));}

        static double cellCost(const int &rowStep, const int &colStep, const double &cardinalDistance, const double &diagonalDistance) {
            if (rowStep == 0 || colStep == 0) {return cardinalDistance;}
            if (std::abs(rowStep) == 1 && std::abs(colStep) == 1) {return diagonalDistance;}
            return std::sqrt(rowStep * rowStep * cardinalDistance + colStep * colStep * cardinalDistance);
        }

        static std::vector<std::pair<unsigned long int, unsigned long int>> getPath(const std::pair<unsigned long int, unsigned long int> &dst, const std::vector<std::vector<AStar_Grid::Cell>> &cellDetails) {
            unsigned long int row = dst.first, col = dst.second;
            std::vector<std::pair<unsigned long int, unsigned long int>> output;

            while (!(cellDetails.at(row).at(col).parentRow == row && cellDetails.at(row).at(col).parentCol == col)) {
                output.insert(output.begin(), std::make_pair(row, col));
                const int rowTemp = cellDetails.at(row).at(col).parentRow;
                const int colTemp = cellDetails.at(row).at(col).parentCol;
                row = rowTemp;
                col = colTemp;
            }
            output.insert(output.begin(), std::make_pair(row, col));

            for (unsigned long int i = 0; i < output.size() / 2; i++) {
                const std::pair<unsigned long int, unsigned long int> temp = output.at(i);
                output[i] = output.at(output.size() - 1 - i);
                output[output.size() - 1 - i] = temp;
            }
            return output;
        }

        static bool successorManhattan(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &dst, const std::vector<std::vector<bool>> &closedList, std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> &openList, std::vector<std::vector<AStar_Grid::Cell>> &cellDetails, const int &row, const int &col, const int &rowStep, const int &colStep, const double &maxAscend, const double &maxDescend, const double &cardinalDistance, const double &diagonalDistance) {
            if (AStar_Grid::isValid(grid, row + rowStep, col + colStep)) {
                if (AStar_Grid::isDestination(dst, row + rowStep, col + colStep)) {
                    cellDetails[row + rowStep][col + colStep].parentRow = row;
                    cellDetails[row + rowStep][col + colStep].parentCol = col;
                    
                    return true;
                } else if (!closedList.at(row + rowStep).at(col + colStep) && AStar_Grid::isUnblocked(grid, row + rowStep, col + colStep, grid.at(row).at(col), maxAscend, maxDescend)) {
                    const double fromCost = cellDetails.at(row).at(col).fromCost + AStar_Grid::cellCost(rowStep, colStep, cardinalDistance, diagonalDistance) + std::fabs(grid.at(row).at(col) - grid.at(row + rowStep).at(col + colStep));
                    const double toCost = AStar_Grid::manhattan(dst, grid.at(dst.first).at(dst.second), row + rowStep, col + colStep, grid.at(row).at(col));
                    const double totalCost = fromCost + toCost;

                    if (cellDetails.at(row + rowStep).at(col + colStep).totalCost == __FLT_MAX__ || cellDetails.at(row + rowStep).at(col + colStep).totalCost > totalCost) {
                        openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + rowStep, col + colStep)));

                        cellDetails[row + rowStep][col + colStep].totalCost = totalCost;
                        cellDetails[row + rowStep][col + colStep].fromCost  = fromCost;
                        cellDetails[row + rowStep][col + colStep].toCost    = toCost;
                        cellDetails[row + rowStep][col + colStep].parentRow = row;
                        cellDetails[row + rowStep][col + colStep].parentCol = col;
                    }
                }
            }
            return false;
        }
        static bool successorDiagonal(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &dst, const std::vector<std::vector<bool>> &closedList, std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> &openList, std::vector<std::vector<AStar_Grid::Cell>> &cellDetails, const int &row, const int &col, const int &rowStep, const int &colStep, const double &maxAscend, const double &maxDescend, const double &cardinalDistance, const double &diagonalDistance, const unsigned char &moveType) {
            if (AStar_Grid::isValid(grid, row + rowStep, col + colStep)) {
                if (moveType == ASTAR_MOVE_NOPHASE && !(AStar_Grid::isUnblocked(grid, row + rowStep, col, grid.at(row).at(col), maxAscend, maxDescend) || AStar_Grid::isUnblocked(grid, row, col + colStep, grid.at(row).at(col), maxAscend, maxDescend))) {return false;}
                if (moveType == ASTAR_MOVE_NOTOUCH && !(AStar_Grid::isUnblocked(grid, row + rowStep, col, grid.at(row).at(col), maxAscend, maxDescend) && AStar_Grid::isUnblocked(grid, row, col + colStep, grid.at(row).at(col), maxAscend, maxDescend))) {return false;}

                if (AStar_Grid::isDestination(dst, row + rowStep, col + colStep)) {
                    cellDetails[row + rowStep][col + colStep].parentRow = row;
                    cellDetails[row + rowStep][col + colStep].parentCol = col;
                    
                    return true;
                } else if (!closedList.at(row + rowStep).at(col + colStep) && AStar_Grid::isUnblocked(grid, row + rowStep, col + colStep, grid.at(row).at(col), maxAscend, maxDescend)) {
                    const double fromCost = cellDetails.at(row).at(col).fromCost + AStar_Grid::cellCost(rowStep, colStep, cardinalDistance, diagonalDistance) + std::fabs(grid.at(row).at(col) - grid.at(row + rowStep).at(col + colStep));
                    const double toCost = AStar_Grid::diagonal(dst, grid.at(dst.first).at(dst.second), row + rowStep, col + colStep, grid.at(row).at(col), cardinalDistance, diagonalDistance);
                    const double totalCost = fromCost + toCost;

                    if (cellDetails.at(row + rowStep).at(col + colStep).totalCost == __FLT_MAX__ || cellDetails.at(row + rowStep).at(col + colStep).totalCost > totalCost) {
                        openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + rowStep, col + colStep)));

                        cellDetails[row + rowStep][col + colStep].totalCost = totalCost;
                        cellDetails[row + rowStep][col + colStep].fromCost  = fromCost;
                        cellDetails[row + rowStep][col + colStep].toCost    = toCost;
                        cellDetails[row + rowStep][col + colStep].parentRow = row;
                        cellDetails[row + rowStep][col + colStep].parentCol = col;
                    }
                }
            }
            return false;
        }
        static bool successorEuclidean(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &dst, const std::vector<std::vector<bool>> &closedList, std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> &openList, std::vector<std::vector<AStar_Grid::Cell>> &cellDetails, const int &row, const int &col, const int &rowStep, const int &colStep, const double &maxAscend, const double &maxDescend, const double &cardinalDistance, const double &diagonalDistance, const unsigned char &moveType) {
            if (AStar_Grid::isValid(grid, row + rowStep, col + colStep)) {
                if (moveType == ASTAR_MOVE_NOPHASE && !(AStar_Grid::isUnblocked(grid, row + rowStep, col, grid.at(row).at(col), maxAscend, maxDescend) || AStar_Grid::isUnblocked(grid, row, col + colStep, grid.at(row).at(col), maxAscend, maxDescend))) {return false;}
                if (moveType == ASTAR_MOVE_NOTOUCH && !(AStar_Grid::isUnblocked(grid, row + rowStep, col, grid.at(row).at(col), maxAscend, maxDescend) && AStar_Grid::isUnblocked(grid, row, col + colStep, grid.at(row).at(col), maxAscend, maxDescend))) {return false;}

                if (AStar_Grid::isDestination(dst, row + rowStep, col + colStep)) {
                    cellDetails[row + rowStep][col + colStep].parentRow = row;
                    cellDetails[row + rowStep][col + colStep].parentCol = col;
                    
                    return true;
                } else if (!closedList.at(row + rowStep).at(col + colStep) && AStar_Grid::isUnblocked(grid, row + rowStep, col + colStep, grid.at(row).at(col), maxAscend, maxDescend)) {
                    const double fromCost = cellDetails.at(row).at(col).fromCost + AStar_Grid::cellCost(rowStep, colStep, cardinalDistance, diagonalDistance) + std::fabs(grid.at(row).at(col) - grid.at(row + rowStep).at(col + colStep));
                    const double toCost = AStar_Grid::euclidean(dst, grid.at(dst.first).at(dst.second), row + rowStep, col + colStep, grid.at(row).at(col));
                    const double totalCost = fromCost + toCost;

                    if (cellDetails.at(row + rowStep).at(col + colStep).totalCost == __FLT_MAX__ || cellDetails.at(row + rowStep).at(col + colStep).totalCost > totalCost) {
                        openList.emplace_back(std::make_pair(totalCost, std::make_pair(row + rowStep, col + colStep)));

                        cellDetails[row + rowStep][col + colStep].totalCost = totalCost;
                        cellDetails[row + rowStep][col + colStep].fromCost  = fromCost;
                        cellDetails[row + rowStep][col + colStep].toCost    = toCost;
                        cellDetails[row + rowStep][col + colStep].parentRow = row;
                        cellDetails[row + rowStep][col + colStep].parentCol = col;
                    }
                }
            }
            return false;
        }
    
    public:
        static std::vector<std::pair<unsigned long int, unsigned long int>> cardinal(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &src, const std::pair<unsigned long int, unsigned long int> &dst, const double &maxAscend, const double &maxDescend, const double &cardinalDistance = 1.0, const double &diagonalDistance = 1.41421356237309504880) {
            if (!AStar_Grid::isValid(grid, src.first, src.second) || !AStar_Grid::isValid(grid, dst.first, dst.second)) {return {src};}
            if (AStar_Grid::isDestination(dst, src.first, src.second)) {return {src};}

            std::vector<std::vector<bool>> closedList;
            std::vector<std::vector<AStar_Grid::Cell>> cellDetails;

            for (unsigned long int i = 0; i < grid.size(); i++) {
                closedList.emplace_back();
                cellDetails.emplace_back();
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    closedList[i].emplace_back(false);
                    
                    cellDetails[i].emplace_back();
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].totalCost = __FLT_MAX__;
                    cellDetails[i][j].fromCost  = __FLT_MAX__;
                    cellDetails[i][j].toCost    = __FLT_MAX__;
                }
            }

            unsigned long int row = src.first, col = src.second;
            cellDetails[row][col].totalCost = 0.0;
            cellDetails[row][col].fromCost  = 0.0;
            cellDetails[row][col].toCost    = 0.0;
            cellDetails[row][col].parentRow = row;
            cellDetails[row][col].parentCol = col;

            std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};

            while (openList.size() > 0) {
                row = openList.at(0).second.first;
                col = openList.at(0).second.second;
                openList.erase(openList.begin());

                closedList[row][col] = true;

                if (AStar_Grid::successorManhattan(grid, dst, closedList, openList, cellDetails, row, col, -1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorManhattan(grid, dst, closedList, openList, cellDetails, row, col,  1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorManhattan(grid, dst, closedList, openList, cellDetails, row, col,  0,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorManhattan(grid, dst, closedList, openList, cellDetails, row, col,  0, -1, maxAscend, maxDescend, cardinalDistance, diagonalDistance)) {return AStar_Grid::getPath(dst, cellDetails);}
            }

            return {src};
        }
        static std::vector<std::pair<unsigned long int, unsigned long int>> diagonal(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &src, const std::pair<unsigned long int, unsigned long int> &dst, const double &maxAscend, const double &maxDescend, const unsigned char &moveType = ASTAR_MOVE_NOBOUND, const double &cardinalDistance = 1.0, const double &diagonalDistance = 1.41421356237309504880) {
            if (!AStar_Grid::isValid(grid, src.first, src.second) || !AStar_Grid::isValid(grid, dst.first, dst.second)) {return {src};}
            if (AStar_Grid::isDestination(dst, src.first, src.second)) {return {src};}

            std::vector<std::vector<bool>> closedList;
            std::vector<std::vector<AStar_Grid::Cell>> cellDetails;

            for (unsigned long int i = 0; i < grid.size(); i++) {
                closedList.emplace_back();
                cellDetails.emplace_back();
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    closedList[i].emplace_back(false);
                    
                    cellDetails[i].emplace_back();
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].totalCost = __FLT_MAX__;
                    cellDetails[i][j].fromCost  = __FLT_MAX__;
                    cellDetails[i][j].toCost    = __FLT_MAX__;
                }
            }

            unsigned long int row = src.first, col = src.second;
            cellDetails[row][col].totalCost = 0.0;
            cellDetails[row][col].fromCost  = 0.0;
            cellDetails[row][col].toCost    = 0.0;
            cellDetails[row][col].parentRow = row;
            cellDetails[row][col].parentCol = col;

            std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};

            while (openList.size() > 0) {
                row = openList.at(0).second.first;
                col = openList.at(0).second.second;
                openList.erase(openList.begin());

                closedList[row][col] = true;

                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col, -1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col,  1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col,  0,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col,  0, -1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col, -1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col,  1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col, -1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorDiagonal(grid, dst, closedList, openList, cellDetails, row, col,  1, -1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
            }

            return {src};
        }
        static std::vector<std::pair<unsigned long int, unsigned long int>> euclidean(const std::vector<std::vector<double>> &grid, const std::pair<unsigned long int, unsigned long int> &src, const std::pair<unsigned long int, unsigned long int> &dst, const double &maxAscend, const double &maxDescend, const unsigned char &moveType = ASTAR_MOVE_NOBOUND, const double &cardinalDistance = 1.0, const double &diagonalDistance = 1.41421356237309504880) {
            if (!AStar_Grid::isValid(grid, src.first, src.second) || !AStar_Grid::isValid(grid, dst.first, dst.second)) {return {src};}
            if (AStar_Grid::isDestination(dst, src.first, src.second)) {return {src};}

            std::vector<std::vector<bool>> closedList;
            std::vector<std::vector<AStar_Grid::Cell>> cellDetails;

            for (unsigned long int i = 0; i < grid.size(); i++) {
                closedList.emplace_back();
                cellDetails.emplace_back();
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    closedList[i].emplace_back(false);
                    
                    cellDetails[i].emplace_back();
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].parentRow = __INT64_MAX__;
                    cellDetails[i][j].totalCost = __FLT_MAX__;
                    cellDetails[i][j].fromCost  = __FLT_MAX__;
                    cellDetails[i][j].toCost    = __FLT_MAX__;
                }
            }

            unsigned long int row = src.first, col = src.second;
            cellDetails[row][col].totalCost = 0.0;
            cellDetails[row][col].fromCost  = 0.0;
            cellDetails[row][col].toCost    = 0.0;
            cellDetails[row][col].parentRow = row;
            cellDetails[row][col].parentCol = col;

            std::vector<std::pair<double, std::pair<unsigned long int, unsigned long int>>> openList = {std::make_pair(0.0, std::make_pair(row, col))};

            while (openList.size() > 0) {
                row = openList.at(0).second.first;
                col = openList.at(0).second.second;
                openList.erase(openList.begin());

                closedList[row][col] = true;

                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col, -1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col,  1,  0, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col,  0,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col,  0, -1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, ASTAR_MOVE_NOBOUND)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col, -1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col,  1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col, -1,  1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
                if (AStar_Grid::successorEuclidean(grid, dst, closedList, openList, cellDetails, row, col,  1, -1, maxAscend, maxDescend, cardinalDistance, diagonalDistance, moveType)) {return AStar_Grid::getPath(dst, cellDetails);}
            }

            return {src};
        }
} AStar;

#endif /* ASTAR */
