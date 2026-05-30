#include <cassert>
#include <random>
#include <string>
#include <vector>

#include "Map.hpp"
#include "Snake.hpp"

namespace {
constexpr int StageCount = 10;

bool isOuterWall(CellType cell) {
    return cell == CellType::Wall || cell == CellType::ImmuneWall;
}

int expectedMapSize(int stage) {
    if (stage <= 2) {
        return 27;
    }
    if (stage <= 4) {
        return 25;
    }
    if (stage <= 7) {
        return 23;
    }
    return 21;
}

int countCells(const Map& map, CellType target) {
    int count = 0;
    for (int row = 0; row < map.rows(); ++row) {
        for (int col = 0; col < map.cols(); ++col) {
            if (map.at(row, col) == target) {
                ++count;
            }
        }
    }
    return count;
}
}

void testMapLoadsStageFile() {
    // stage1 맵이 27x27로 로드되고 주요 셀 값이 맞는지 확인합니다.
    Map map;
    assert(map.loadFromFile("maps/stage1.txt"));
    assert(map.rows() == 27);
    assert(map.cols() == 27);
    assert(map.at(0, 0) == CellType::ImmuneWall);
    assert(map.at(0, 1) == CellType::Wall);
    assert(map.at(1, 1) == CellType::Empty);
}

void testAllStageMapsAreValid() {
    for (int stage = 1; stage <= StageCount; ++stage) {
        Map map;
        const std::string path = "maps/stage" + std::to_string(stage) + ".txt";
        const int mapSize = expectedMapSize(stage);

        assert(map.loadFromFile(path));
        assert(map.rows() == mapSize);
        assert(map.cols() == mapSize);

        assert(map.at(0, 0) == CellType::ImmuneWall);
        assert(map.at(0, mapSize - 1) == CellType::ImmuneWall);
        assert(map.at(mapSize - 1, 0) == CellType::ImmuneWall);
        assert(map.at(mapSize - 1, mapSize - 1) == CellType::ImmuneWall);

        for (int index = 0; index < mapSize; ++index) {
            assert(isOuterWall(map.at(0, index)));
            assert(isOuterWall(map.at(mapSize - 1, index)));
            assert(isOuterWall(map.at(index, 0)));
            assert(isOuterWall(map.at(index, mapSize - 1)));
        }

        const int middle = mapSize / 2;
        assert(map.at(middle, middle - 2) == CellType::Empty);
        assert(map.at(middle, middle - 1) == CellType::Empty);
        assert(map.at(middle, middle) == CellType::Empty);
        assert(map.at(middle, middle + 1) == CellType::Empty);
    }
}

void testSnakeRejectsReverseDirection() {
    // 오른쪽으로 이동 중인 Snake는 왼쪽으로 즉시 방향 전환할 수 없습니다.
    Snake snake({10, 10}, Direction::Right);
    assert(!snake.setDirection(Direction::Left));
    assert(snake.direction() == Direction::Right);
}

void testSnakeMovesForward() {
    // 열린 공간에서는 현재 방향으로 한 칸 이동해야 합니다.
    Map map;
    map.loadFallbackMap();

    Snake snake({10, 10}, Direction::Right);
    assert(snake.move(map) == MoveResult::Moved);
    assert((snake.head() == Position{10, 11}));
}

void testSnakeHitsWall() {
    // 벽에 도달하면 MoveResult::HitWall을 반환해야 합니다.
    Map map;
    map.loadFallbackMap();

    Snake snake({1, 18}, Direction::Right);
    assert(snake.move(map) == MoveResult::Moved);
    assert(snake.move(map) == MoveResult::HitWall);
}

void testGrowthItemIncreasesLength() {
    Map map;
    map.loadFallbackMap();
    map.setCell(10, 11, CellType::GrowthItem);

    Snake snake({10, 10}, Direction::Right);
    assert(snake.body().size() == 3);
    assert(snake.move(map) == MoveResult::AteGrowth);
    assert(snake.body().size() == 4);
    assert(map.at(10, 11) == CellType::Empty);
}

void testPoisonItemDecreasesLength() {
    Map map;
    map.loadFallbackMap();

    Snake snake({10, 10}, Direction::Right);
    map.setCell(10, 11, CellType::GrowthItem);
    assert(snake.move(map) == MoveResult::AteGrowth);
    assert(snake.body().size() == 4);

    map.setCell(10, 12, CellType::PoisonItem);
    assert(snake.move(map) == MoveResult::AtePoison);
    assert(snake.body().size() == 3);
    assert(map.at(10, 12) == CellType::Empty);
}

void testPoisonItemTooShortGameOverResult() {
    Map map;
    map.loadFallbackMap();
    map.setCell(10, 11, CellType::PoisonItem);

    Snake snake({10, 10}, Direction::Right);
    assert(snake.body().size() == 3);
    assert(snake.move(map) == MoveResult::TooShort);
    assert(snake.body().size() == 2);
}

void testItemSpawnsOnlyOnEmptyCell() {
    Map map;
    map.loadFallbackMap();
    Snake snake({10, 10}, Direction::Right);
    std::vector<Position> occupied(snake.body().begin(), snake.body().end());
    std::mt19937 rng(1);

    assert(map.placeRandomItem(CellType::GrowthItem, occupied, rng));
    assert(map.placeRandomItem(CellType::PoisonItem, occupied, rng));
    assert(countCells(map, CellType::GrowthItem) == 1);
    assert(countCells(map, CellType::PoisonItem) == 1);

    for (const Position& position : occupied) {
        assert(map.at(position.row, position.col) != CellType::GrowthItem);
        assert(map.at(position.row, position.col) != CellType::PoisonItem);
    }

    for (int row = 0; row < map.rows(); ++row) {
        assert(map.at(row, 0) != CellType::GrowthItem);
        assert(map.at(row, 0) != CellType::PoisonItem);
        assert(map.at(row, map.cols() - 1) != CellType::GrowthItem);
        assert(map.at(row, map.cols() - 1) != CellType::PoisonItem);
    }

    for (int col = 0; col < map.cols(); ++col) {
        assert(map.at(0, col) != CellType::GrowthItem);
        assert(map.at(0, col) != CellType::PoisonItem);
        assert(map.at(map.rows() - 1, col) != CellType::GrowthItem);
        assert(map.at(map.rows() - 1, col) != CellType::PoisonItem);
    }
}

int main() {
    // 간단한 assert 기반 테스트를 순서대로 실행합니다.
    testMapLoadsStageFile();
    testAllStageMapsAreValid();
    testSnakeRejectsReverseDirection();
    testSnakeMovesForward();
    testSnakeHitsWall();
    testGrowthItemIncreasesLength();
    testPoisonItemDecreasesLength();
    testPoisonItemTooShortGameOverResult();
    testItemSpawnsOnlyOnEmptyCell();
    return 0;
}
