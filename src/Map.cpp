#include "Map.hpp"

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>

namespace {
constexpr int MinPlayableMapSize = 21;
}

bool Map::loadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        errorMessage_ = "Cannot open map file: " + path;
        return false;
    }

    std::vector<std::vector<CellType>> nextGrid;
    std::string line;
    int lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }

        std::istringstream stream(line);
        std::vector<CellType> row;
        int value = 0;

        while (stream >> value) {
            if (!isKnownCellValue(value)) {
                errorMessage_ = "Unknown map value at line " + std::to_string(lineNumber);
                return false;
            }
            row.push_back(static_cast<CellType>(value));
        }

        if (row.empty()) {
            errorMessage_ = "Map line has no cell values at line " + std::to_string(lineNumber);
            return false;
        }
        nextGrid.push_back(row);
    }

    if (!validateGrid(nextGrid)) {
        return false;
    }

    grid_ = nextGrid;
    errorMessage_.clear();
    return true;
}

void Map::loadFallbackMap(int size) {
    const int mapSize = std::max(MinPlayableMapSize, size);
    grid_.assign(mapSize, std::vector<CellType>(mapSize, CellType::Empty));

    for (int r = 0; r < mapSize; ++r) {
        for (int c = 0; c < mapSize; ++c) {
            if (r == 0 || r == mapSize - 1 || c == 0 || c == mapSize - 1) {
                grid_[r][c] = CellType::Wall;
            }
        }
    }

    grid_[0][0] = CellType::ImmuneWall;
    grid_[0][mapSize - 1] = CellType::ImmuneWall;
    grid_[mapSize - 1][0] = CellType::ImmuneWall;
    grid_[mapSize - 1][mapSize - 1] = CellType::ImmuneWall;
    errorMessage_.clear();
}

CellType Map::at(int row, int col) const {
    if (!contains(row, col)) {
        return CellType::Wall;
    }
    return grid_[row][col];
}

bool Map::isBlocked(int row, int col) const {
    const CellType cell = at(row, col);
    return cell == CellType::Wall || cell == CellType::ImmuneWall;
}

bool Map::isEmpty(int row, int col) const {
    return at(row, col) == CellType::Empty;
}

bool Map::contains(int row, int col) const {
    return row >= 0 && row < rows() && col >= 0 && col < cols();
}

bool Map::setCell(int row, int col, CellType cell) {
    if (!contains(row, col)) {
        return false;
    }
    grid_[row][col] = cell;
    return true;
}

void Map::clearCells(CellType cell) {
    for (auto& row : grid_) {
        for (CellType& current : row) {
            if (current == cell) {
                current = CellType::Empty;
            }
        }
    }
}

bool Map::placeRandomItem(CellType item, const std::vector<Position>& occupied, std::mt19937& rng) {
    if (item != CellType::GrowthItem && item != CellType::PoisonItem) {
        return false;
    }

    std::vector<Position> candidates;
    for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < cols(); ++col) {
            const Position position{row, col};
            const bool snakeOccupies = std::find(occupied.begin(), occupied.end(), position) != occupied.end();
            if (isEmpty(row, col) && !snakeOccupies) {
                candidates.push_back(position);
            }
        }
    }

    if (candidates.empty()) {
        return false;
    }

    std::uniform_int_distribution<std::size_t> dist(0, candidates.size() - 1);
    const Position selected = candidates[dist(rng)];
    return setCell(selected.row, selected.col, item);
}

int Map::rows() const {
    return static_cast<int>(grid_.size());
}

int Map::cols() const {
    if (grid_.empty()) {
        return 0;
    }
    return static_cast<int>(grid_.front().size());
}

const std::string& Map::errorMessage() const {
    return errorMessage_;
}

bool Map::validateGrid(const std::vector<std::vector<CellType>>& grid) {
    if (grid.empty() || static_cast<int>(grid.size()) < MinPlayableMapSize) {
        errorMessage_ = "Map must be at least 21x21";
        return false;
    }

    const int expectedCols = static_cast<int>(grid.front().size());
    if (expectedCols < MinPlayableMapSize) {
        errorMessage_ = "Map must be at least 21x21";
        return false;
    }

    for (const auto& row : grid) {
        if (static_cast<int>(row.size()) != expectedCols) {
            errorMessage_ = "Map must be rectangular";
            return false;
        }
    }

    return true;
}

bool Map::isKnownCellValue(int value) {
    return value >= static_cast<int>(CellType::Empty)
        && value <= static_cast<int>(CellType::Gate);
}
