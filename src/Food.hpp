// Food.hpp
// Growth Item의 생성 개수, 재생성 주기, 맵 배치 요청을 관리하는 Food 클래스를 선언한다.

#ifndef FOOD_HPP
#define FOOD_HPP

#include <chrono>
#include <random>
#include <vector>

#include "Map.hpp"
#include "Types.hpp"

// Food는 Snake 길이를 증가시키는 Growth Item의 출현 정책을 담당한다.
class Food {
public:
    // Shield Item 추가 후 총 아이템 3개 제한을 맞추기 위해 Growth Item은 1개만 유지한다.
    static constexpr int MaxCount = 1;

    // Snake와 벽, Gate가 없는 빈 칸에 Growth Item을 다시 배치한다.
    void spawn(Map& map, const std::vector<Position>& occupied, std::mt19937& rng);

    // 마지막 생성 후 5초가 지났으면 Growth Item 위치를 새로 뽑는다.
    void refreshIfExpired(Map& map, const std::vector<Position>& occupied, std::mt19937& rng);

private:
    static constexpr std::chrono::seconds Lifetime{5};

    std::chrono::steady_clock::time_point createdAt_{};
    bool spawned_ = false;
};

#endif
