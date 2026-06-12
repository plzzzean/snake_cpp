// test_logic.cpp
// Map 로드와 Snake 이동 규칙을 assert로 확인하는 간단한 로직 테스트다.

#include <cassert>
#include <random>
#include <string>
#include <vector>

#include "Food.hpp"
#include "Map.hpp"
#include "Poison.hpp"
#include "Shield.hpp"
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

void testGrowthItemIncreasesLength() {
    Map map;
    map.loadFallbackMap();
    map.setCell(10, 11, CellType::GrowthItem);

    Snake snake({10, 10}, Direction::Right);
    assert(snake.body().size() == 3);
    // Growth Item을 밟으면 꼬리를 제거하지 않아 Snake 길이가 1 증가한다.
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
    // Poison Item을 밟으면 일반 이동보다 꼬리를 한 칸 더 제거해 길이가 1 감소한다.
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
    // Poison Item 이후 길이가 3보다 작아지면 Game에서 Game Over로 해석할 결과를 반환한다.
    assert(snake.move(map) == MoveResult::TooShort);
    assert(snake.body().size() == 2);
}

void testShieldItemKeepsLengthAndClearsCell() {
    Map map;
    map.loadFallbackMap();
    map.setCell(10, 11, CellType::ShieldItem);

    Snake snake({10, 10}, Direction::Right);
    assert(snake.body().size() == 3);
    // Shield Item을 밟으면 길이는 유지하고 Shield 획득 결과를 반환한다.
    assert(snake.move(map) == MoveResult::AteShield);
    assert(snake.body().size() == 3);
    assert(map.at(10, 11) == CellType::Empty);
}

void testItemSpawnsOnlyOnEmptyCell() {
    Map map;
    map.loadFallbackMap();
    Snake snake({10, 10}, Direction::Right);
    std::vector<Position> occupied(snake.body().begin(), snake.body().end());
    std::mt19937 rng(1);

    assert(map.placeRandomItem(CellType::GrowthItem, occupied, rng));
    assert(map.placeRandomItem(CellType::PoisonItem, occupied, rng));
    assert(map.placeRandomItem(CellType::ShieldItem, occupied, rng));
    assert(map.countCells(CellType::GrowthItem) == 1);
    assert(map.countCells(CellType::PoisonItem) == 1);
    assert(map.countCells(CellType::ShieldItem) == 1);

    for (const Position& position : occupied) {
        assert(map.at(position.row, position.col) != CellType::GrowthItem);
        assert(map.at(position.row, position.col) != CellType::PoisonItem);
        assert(map.at(position.row, position.col) != CellType::ShieldItem);
    }

    for (int row = 0; row < map.rows(); ++row) {
        assert(map.at(row, 0) != CellType::GrowthItem);
        assert(map.at(row, 0) != CellType::PoisonItem);
        assert(map.at(row, 0) != CellType::ShieldItem);
        assert(map.at(row, map.cols() - 1) != CellType::GrowthItem);
        assert(map.at(row, map.cols() - 1) != CellType::PoisonItem);
        assert(map.at(row, map.cols() - 1) != CellType::ShieldItem);
    }

    for (int col = 0; col < map.cols(); ++col) {
        assert(map.at(0, col) != CellType::GrowthItem);
        assert(map.at(0, col) != CellType::PoisonItem);
        assert(map.at(0, col) != CellType::ShieldItem);
        assert(map.at(map.rows() - 1, col) != CellType::GrowthItem);
        assert(map.at(map.rows() - 1, col) != CellType::PoisonItem);
        assert(map.at(map.rows() - 1, col) != CellType::ShieldItem);
    }
}

void testItemManagersKeepTotalItemLimit() {
    Map map;
    map.loadFallbackMap();
    Snake snake({10, 10}, Direction::Right);
    std::vector<Position> occupied(snake.body().begin(), snake.body().end());
    std::mt19937 rng(2);
    Food food;
    Poison poison;
    Shield shield;

    food.spawn(map, occupied, rng);
    poison.spawn(map, occupied, rng);
    shield.spawn(map, occupied, rng);

    assert(map.countCells(CellType::GrowthItem) == Food::MaxCount);
    assert(map.countCells(CellType::PoisonItem) == Poison::MaxCount);
    assert(map.countCells(CellType::ShieldItem) == Shield::MaxCount);
    assert(map.countCells(CellType::GrowthItem) + map.countCells(CellType::PoisonItem) + map.countCells(CellType::ShieldItem) == 3);
}

void testItemManagersDoNotSpawnOnSnakeOrGate() {
    Map map;
    map.loadFallbackMap();
    map.setCell(5, 5, CellType::Gate);

    Snake snake({10, 10}, Direction::Right);
    std::vector<Position> occupied(snake.body().begin(), snake.body().end());
    std::mt19937 rng(3);
    Food food;
    Poison poison;
    Shield shield;

    food.spawn(map, occupied, rng);
    poison.spawn(map, occupied, rng);
    shield.spawn(map, occupied, rng);

    for (const Position& position : occupied) {
        assert(map.at(position.row, position.col) != CellType::GrowthItem);
        assert(map.at(position.row, position.col) != CellType::PoisonItem);
        assert(map.at(position.row, position.col) != CellType::ShieldItem);
    }
    assert(map.at(5, 5) == CellType::Gate);
}

void testItemManagersRefreshPolicyKeepsCountsBeforeExpiry() {
    Map map;
    map.loadFallbackMap();
    Snake snake({10, 10}, Direction::Right);
    std::vector<Position> occupied(snake.body().begin(), snake.body().end());
    std::mt19937 rng(4);
    Food food;
    Poison poison;
    Shield shield;

    food.spawn(map, occupied, rng);
    poison.spawn(map, occupied, rng);
    shield.spawn(map, occupied, rng);
    food.refreshIfExpired(map, occupied, rng);
    poison.refreshIfExpired(map, occupied, rng);
    shield.refreshIfExpired(map, occupied, rng);

    assert(map.countCells(CellType::GrowthItem) == Food::MaxCount);
    assert(map.countCells(CellType::PoisonItem) == Poison::MaxCount);
    assert(map.countCells(CellType::ShieldItem) == Shield::MaxCount);
}

int main() {
    // 간단한 assert 기반 테스트를 순서대로 실행한다.
    testMapLoadsStageFile();
    testAllStageMapsAreValid();
    testSnakeRejectsReverseDirection();
    testSnakeMovesForward();
    testSnakeHitsWall();
    testGrowthItemIncreasesLength();
    testPoisonItemDecreasesLength();
    testPoisonItemTooShortGameOverResult();
    testShieldItemKeepsLengthAndClearsCell();
    testItemSpawnsOnlyOnEmptyCell();
    testItemManagersKeepTotalItemLimit();
    testItemManagersDoNotSpawnOnSnakeOrGate();
    testItemManagersRefreshPolicyKeepsCountsBeforeExpiry();
    return 0;
}
