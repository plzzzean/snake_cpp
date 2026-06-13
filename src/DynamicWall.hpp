// DynamicWall.hpp
// 일정 주기마다 안전한 빈 칸으로 이동하는 특수 Wall 클래스를 선언한다.

#ifndef DYNAMIC_WALL_HPP
#define DYNAMIC_WALL_HPP

#include <chrono>
#include <random>
#include <vector>

#include "Map.hpp"
#include "Types.hpp"

// 내부 Wall 하나를 움직이는 Wall로 바꿔 스테이지 구조에 변화를 주는 추가 기능이다.
class DynamicWall {
public:
    // 외곽과 Immune Wall을 제외한 내부 Wall 하나를 Dynamic Wall로 지정한다.
    bool initialize(Map& map, std::mt19937& rng);

    // 이동 주기가 지나면 Snake와 모든 객체를 피해 Dynamic Wall을 재배치한다.
    bool update(Map& map, const std::vector<Position>& occupied, std::mt19937& rng,
                std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now());

    bool isActive() const;
    Position position() const;

    static constexpr std::chrono::seconds MoveInterval{8};

private:
    Position position_{-1, -1};
    bool active_ = false;
    std::chrono::steady_clock::time_point lastMovedAt_{};
};

#endif
