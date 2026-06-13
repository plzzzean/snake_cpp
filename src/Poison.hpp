// Poison.hpp
// Poison Item의 생성 개수, 재생성 주기, 맵 배치 요청을 관리하는 Poison 클래스를 선언한다.

#ifndef POISON_HPP
#define POISON_HPP

#include <chrono>
#include <random>
#include <vector>

#include "Map.hpp"
#include "Types.hpp"

// Poison은 Snake 길이를 감소시키는 Poison Item의 출현 정책을 담당한다.
class Poison {
public:
    // Growth, Poison, Shield를 각각 1개씩 두어 전체 Item 수를 3개로 제한한다.
    static constexpr int MaxCount = 1;

    // Snake와 벽, Gate가 없는 빈 칸에 Poison Item을 다시 배치한다.
    void spawn(Map& map, const std::vector<Position>& occupied, std::mt19937& rng);

    // 마지막 생성 후 10초가 지났으면 Poison Item 위치를 새로 뽑는다.
    void refreshIfExpired(Map& map, const std::vector<Position>& occupied, std::mt19937& rng);

private:
    static constexpr std::chrono::seconds Lifetime{10};

    std::chrono::steady_clock::time_point createdAt_{};
    bool spawned_ = false;
};

#endif
