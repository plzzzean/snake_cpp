// Gate.hpp
// Gate 쌍의 생성, 진출 위치 계산, 통과 상태 추적을 담당하는 Gate 클래스를 선언한다.

#ifndef GATE_HPP
#define GATE_HPP

#include <random>
#include <utility>
#include <vector>

#include "Map.hpp"
#include "Snake.hpp"
#include "Types.hpp"

// Wall 셀 두 곳에 쌍으로 나타나 Snake를 순간이동시키는 출입구 클래스다.
class Gate {
public:
    // ImmuneWall을 제외한 Wall 셀 중 임의의 두 위치에 Gate 쌍을 생성한다.
    bool spawn(Map& map, const Snake& snake, std::mt19937& rng);

    // 맵에서 Gate 셀을 제거하고 원래 Wall로 복원한 뒤 비활성 상태로 전환한다.
    void clear(Map& map);

    // 주어진 위치가 현재 활성 Gate 위치인지 확인한다.
    bool isGatePosition(Position pos) const;

    // entryGate에 진입한 방향을 바탕으로 반대편 Gate의 출구 위치와 진출 방향을 반환한다.
    std::pair<Position, Direction> calcExit(Position entryGate, Direction entryDir, const Map& map) const;

    // Gate가 활성 상태인지 반환한다.
    bool isActive() const;

    int useCount() const;

private:
    // 해당 Gate 위치가 맵 가장자리 벽인지 확인한다.
    bool isBorderWall(Position pos, const Map& map) const;

    // 가장자리 Gate의 고정 진출 방향(항상 맵 안쪽)을 반환한다.
    Direction borderExitDir(Position gatePos, const Map& map) const;

    // 내부 벽 Gate의 우선순위 규칙(동→시계→역시계→반대)에 따라 진출 방향을 결정한다.
    Direction interiorExitDir(Direction entryDir, Position exitGatePos, const Map& map) const;

    // exitGatePos에서 dir 방향으로 이동한 칸이 진출 가능한지 확인한다.
    bool canExitTo(Position exitGatePos, Direction dir, const Map& map) const;

    // 진입 방향의 시계방향을 반환한다.
    static Direction clockwise(Direction dir);

    // 진입 방향의 역시계방향을 반환한다.
    static Direction counterClockwise(Direction dir);

    // 진입 방향의 반대 방향을 반환한다.
    static Direction opposite(Direction dir);

    Position gateA_{};
    Position gateB_{};
    bool active_ = false;
    int useCount_ = 0;
};

#endif
