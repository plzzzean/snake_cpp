// DynamicWall.cpp
// Dynamic Wall의 초기 선택과 주기적 안전 재배치 로직을 구현한다.

#include "DynamicWall.hpp"

#include <algorithm>

bool DynamicWall::initialize(Map& map, std::mt19937& rng) {
    std::vector<Position> candidates;
    for (int row = 1; row < map.rows() - 1; ++row) {
        for (int col = 1; col < map.cols() - 1; ++col) {
            if (map.at(row, col) == CellType::Wall) {
                candidates.push_back({row, col});
            }
        }
    }

    if (candidates.empty()) {
        return false;
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    position_ = candidates.front();
    active_ = map.setCell(position_.row, position_.col, CellType::DynamicWall);
    lastMovedAt_ = std::chrono::steady_clock::now();
    return active_;
}

bool DynamicWall::update(Map& map, const std::vector<Position>& occupied, std::mt19937& rng,
                         std::chrono::steady_clock::time_point now) {
    if (!active_) {
        return initialize(map, rng);
    }

    if (now - lastMovedAt_ < MoveInterval) {
        return false;
    }

    std::vector<Position> candidates;
    for (int row = 1; row < map.rows() - 1; ++row) {
        for (int col = 1; col < map.cols() - 1; ++col) {
            const Position candidate{row, col};
            const bool isOccupied = std::find(occupied.begin(), occupied.end(), candidate) != occupied.end();
            const bool nextToHead = !occupied.empty()
                && ((candidate.row == occupied.front().row
                        && (candidate.col == occupied.front().col - 1 || candidate.col == occupied.front().col + 1))
                    || (candidate.col == occupied.front().col
                        && (candidate.row == occupied.front().row - 1 || candidate.row == occupied.front().row + 1)));
            if (map.isEmpty(row, col) && !isOccupied && !nextToHead) {
                candidates.push_back(candidate);
            }
        }
    }

    if (candidates.empty()) {
        lastMovedAt_ = now;
        return false;
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    const Position next = candidates.front();
    map.setCell(position_.row, position_.col, CellType::Empty);
    map.setCell(next.row, next.col, CellType::DynamicWall);
    position_ = next;
    lastMovedAt_ = now;
    return true;
}

bool DynamicWall::isActive() const {
    return active_;
}

Position DynamicWall::position() const {
    return position_;
}
