// test_logic.cpp
// Map 로드와 Snake 이동 규칙을 assert로 확인하는 간단한 로직 테스트다.

#include <cassert>
#include <string>

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
}

void testMapLoadsStageFile() {
    // stage1 맵이 27x27로 로드되고 주요 셀 값이 맞는지 확인한다.
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
    // 오른쪽으로 이동 중인 Snake는 왼쪽으로 즉시 방향 전환할 수 없다.
    Snake snake({10, 10}, Direction::Right);
    assert(!snake.setDirection(Direction::Left));
    assert(snake.direction() == Direction::Right);
}

void testSnakeMovesForward() {
    // 열린 공간에서는 현재 방향으로 한 칸 이동해야 한다.
    Map map;
    map.loadFallbackMap();

    Snake snake({10, 10}, Direction::Right);
    assert(snake.move(map) == MoveResult::Moved);
    assert((snake.head() == Position{10, 11}));
}

void testSnakeHitsWall() {
    // 벽에 도달하면 MoveResult::HitWall을 반환해야 한다.
    Map map;
    map.loadFallbackMap();

    Snake snake({1, 18}, Direction::Right);
    assert(snake.move(map) == MoveResult::Moved);
    assert(snake.move(map) == MoveResult::HitWall);
}

int main() {
    // 간단한 assert 기반 테스트를 순서대로 실행한다.
    testMapLoadsStageFile();
    testAllStageMapsAreValid();
    testSnakeRejectsReverseDirection();
    testSnakeMovesForward();
    testSnakeHitsWall();
    return 0;
}
