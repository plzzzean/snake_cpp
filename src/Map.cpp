// Map.cpp
// Map 클래스의 파일 파싱, 기본 맵 생성, 범위 검사, 맵 검증 로직을 구현한다.

#include "Map.hpp"

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>

namespace {
constexpr int MinPlayableMapSize = 21;
}

bool Map::loadFromFile(const std::string& path) {
    // 맵 파일은 공백으로 구분된 숫자를 한 줄씩 읽어 CellType 그리드로 변환한다.
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
        // 빈 줄은 맵 데이터로 보지 않고 건너뛴다.
        if (line.empty()) {
            continue;
        }

        std::istringstream stream(line);
        std::vector<CellType> row;
        int value = 0;

        while (stream >> value) {
            // 정의되지 않은 숫자가 들어오면 잘못된 맵으로 보고 즉시 실패한다.
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

    // 파일 파싱이 끝난 뒤 크기와 직사각형 여부를 검증한 다음 실제 grid_를 교체한다.
    if (!validateGrid(nextGrid)) {
        return false;
    }

    grid_ = nextGrid;
    errorMessage_.clear();
    return true;
}

void Map::loadFallbackMap(int size) {
    // 요청 크기가 너무 작아도 최소 플레이 가능 크기 이상으로 보정한다.
    const int mapSize = std::max(MinPlayableMapSize, size);
    grid_.assign(mapSize, std::vector<CellType>(mapSize, CellType::Empty));

    // 가장자리는 일반 벽으로 채워 Snake가 맵 밖으로 나가지 못하게 한다.
    for (int r = 0; r < mapSize; ++r) {
        for (int c = 0; c < mapSize; ++c) {
            if (r == 0 || r == mapSize - 1 || c == 0 || c == mapSize - 1) {
                grid_[r][c] = CellType::Wall;
            }
        }
    }

    // 모서리는 과제 맵 형식에 맞춰 ImmuneWall로 표시한다.
    grid_[0][0] = CellType::ImmuneWall;
    grid_[0][mapSize - 1] = CellType::ImmuneWall;
    grid_[mapSize - 1][0] = CellType::ImmuneWall;
    grid_[mapSize - 1][mapSize - 1] = CellType::ImmuneWall;
    errorMessage_.clear();
}

CellType Map::at(int row, int col) const {
    // 범위 밖 좌표를 벽으로 처리하면 호출자가 매번 범위 검사를 하지 않아도 안전하다.
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
            // isEmpty() 조건으로 Wall, Immune Wall, Gate, 다른 Item 칸은 후보에서 제외한다.
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

int Map::countCells(CellType cell) const {
    int count = 0;
    for (const auto& row : grid_) {
        count += static_cast<int>(std::count(row.begin(), row.end(), cell));
    }
    return count;
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
    // Snake가 시작하고 이동할 수 있는 최소 크기보다 작으면 유효하지 않은 맵이다.
    if (grid.empty() || static_cast<int>(grid.size()) < MinPlayableMapSize) {
        errorMessage_ = "Map must be at least 21x21";
        return false;
    }

    const int expectedCols = static_cast<int>(grid.front().size());
    if (expectedCols < MinPlayableMapSize) {
        errorMessage_ = "Map must be at least 21x21";
        return false;
    }

    // 모든 행의 길이가 같아야 row/col 인덱싱을 단순하고 안전하게 유지할 수 있다.
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
