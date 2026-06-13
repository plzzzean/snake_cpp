// Food.cpp
// Food 클래스의 Growth Item 배치와 10초 재생성 정책을 구현한다.

#include "Food.hpp"

#include <chrono>

void Food::spawn(Map& map, const std::vector<Position>& occupied, std::mt19937& rng) {
    // Growth Item은 기존 위치를 정리한 뒤 빈 칸에 다시 배치해 Snake, 벽, Gate와 겹치지 않게 한다.
    map.clearCells(CellType::GrowthItem);
    for (int count = 0; count < MaxCount; ++count) {
        map.placeRandomItem(CellType::GrowthItem, occupied, rng);
    }

    createdAt_ = std::chrono::steady_clock::now();
    spawned_ = true;
}

void Food::refreshIfExpired(Map& map, const std::vector<Position>& occupied, std::mt19937& rng) {
    const auto now = std::chrono::steady_clock::now();
    if (!spawned_ || now - createdAt_ >= Lifetime) {
        spawn(map, occupied, rng);
    }
}
