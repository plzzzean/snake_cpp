// Gate.cpp
// Gate 클래스의 Wall 기반 생성, 진출 방향 계산, 활성 상태 관리 로직을 구현한다.

#include "Gate.hpp"

#include <algorithm>

bool Gate::spawn(Map& map, const Snake& snake, std::mt19937& rng) {
    // 활성 Gate가 있으면 새 쌍을 만들지 않아 한 번에 한 쌍만 유지한다.
    if (active_) {
        return false;
    }

    // Immune Wall을 제외하고 실제 출구가 확보된 Wall만 후보로 모은다.
    std::vector<Position> candidates;
    for (int row = 0; row < map.rows(); ++row) {
        for (int col = 0; col < map.cols(); ++col) {
            const CellType cell = map.at(row, col);
            if ((cell == CellType::Wall || cell == CellType::DynamicWall)
                && !snake.occupies({row, col}) && hasUsableExit({row, col}, map)) {
                candidates.push_back({row, col});
            }
        }
    }

    if (static_cast<int>(candidates.size()) < 2) {
        return false;
    }

    // 후보 목록을 섞어 앞 두 위치를 Gate A와 Gate B로 선택한다.
    std::shuffle(candidates.begin(), candidates.end(), rng);
    gateA_ = candidates[0];
    gateB_ = candidates[1];
    gateAOriginal_ = map.at(gateA_.row, gateA_.col);
    gateBOriginal_ = map.at(gateB_.row, gateB_.col);
    active_ = true;

    map.setCell(gateA_.row, gateA_.col, CellType::Gate);
    map.setCell(gateB_.row, gateB_.col, CellType::Gate);
    return true;
}

void Gate::clear(Map& map) {
    if (!active_) {
        return;
    }
    // Gate 생성 전에 저장한 셀 종류로 복원해 Dynamic Wall도 보존한다.
    map.setCell(gateA_.row, gateA_.col, gateAOriginal_);
    map.setCell(gateB_.row, gateB_.col, gateBOriginal_);
    active_ = false;
}

bool Gate::isGatePosition(Position pos) const {
    return active_ && (pos == gateA_ || pos == gateB_);
}

std::pair<Position, Direction> Gate::calcExit(Position entryGate, Direction entryDir, const Map& map) const {
    // 진입한 Gate의 반대편 Gate가 출구다.
    const Position exitGate = (entryGate == gateA_) ? gateB_ : gateA_;

    const Direction exitDir = isBorderWall(exitGate, map)
        ? borderExitDir(exitGate, map)
        : interiorExitDir(entryDir, exitGate, map);

    // 출구 위치는 출구 Gate에서 진출 방향으로 한 칸 이동한 칸이다.
    Position exitPos = exitGate;
    switch (exitDir) {
    case Direction::Up:    exitPos.row -= 1; break;
    case Direction::Down:  exitPos.row += 1; break;
    case Direction::Left:  exitPos.col -= 1; break;
    case Direction::Right: exitPos.col += 1; break;
    }

    return {exitPos, exitDir};
}

bool Gate::isActive() const {
    return active_;
}

bool Gate::isBorderWall(Position pos, const Map& map) const {
    return pos.row == 0 || pos.row == map.rows() - 1
        || pos.col == 0 || pos.col == map.cols() - 1;
}

Direction Gate::borderExitDir(Position gatePos, const Map& map) const {
    // 가장자리 벽은 항상 맵 안쪽 방향으로 진출한다.
    if (gatePos.row == 0)                return Direction::Down;
    if (gatePos.row == map.rows() - 1)   return Direction::Up;
    if (gatePos.col == 0)                return Direction::Right;
    return Direction::Left;
}

Direction Gate::interiorExitDir(Direction entryDir, Position exitGatePos, const Map& map) const {
    // 진입 방향 → 시계방향 → 역시계방향 → 반대방향 순으로 진출 가능한 방향을 찾는다.
    const Direction priority[] = {
        entryDir,
        clockwise(entryDir),
        counterClockwise(entryDir),
        opposite(entryDir)
    };

    for (const Direction dir : priority) {
        if (canExitTo(exitGatePos, dir, map)) {
            return dir;
        }
    }

    // 모든 방향이 막혀 있으면 마지막 우선순위인 반대 방향을 반환한다.
    return opposite(entryDir);
}

bool Gate::canExitTo(Position exitGatePos, Direction dir, const Map& map) const {
    Position next = exitGatePos;
    switch (dir) {
    case Direction::Up:    next.row -= 1; break;
    case Direction::Down:  next.row += 1; break;
    case Direction::Left:  next.col -= 1; break;
    case Direction::Right: next.col += 1; break;
    }
    return !map.isBlocked(next.row, next.col);
}

bool Gate::hasUsableExit(Position wallPosition, const Map& map) const {
    if (isBorderWall(wallPosition, map)) {
        return canExitTo(wallPosition, borderExitDir(wallPosition, map), map);
    }

    constexpr Direction Directions[] = {
        Direction::Up, Direction::Down, Direction::Left, Direction::Right
    };
    return std::any_of(std::begin(Directions), std::end(Directions),
        [this, wallPosition, &map](Direction direction) {
            return canExitTo(wallPosition, direction, map);
        });
}

Direction Gate::clockwise(Direction dir) {
    switch (dir) {
    case Direction::Up:    return Direction::Right;
    case Direction::Right: return Direction::Down;
    case Direction::Down:  return Direction::Left;
    case Direction::Left:  return Direction::Up;
    }
    return dir;
}

Direction Gate::counterClockwise(Direction dir) {
    switch (dir) {
    case Direction::Up:    return Direction::Left;
    case Direction::Left:  return Direction::Down;
    case Direction::Down:  return Direction::Right;
    case Direction::Right: return Direction::Up;
    }
    return dir;
}

Direction Gate::opposite(Direction dir) {
    switch (dir) {
    case Direction::Up:    return Direction::Down;
    case Direction::Down:  return Direction::Up;
    case Direction::Left:  return Direction::Right;
    case Direction::Right: return Direction::Left;
    }
    return dir;
}
