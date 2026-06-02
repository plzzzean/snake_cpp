// Snake.hpp
// Snake의 몸 좌표, 이동 방향, 이동 결과를 관리하는 Snake 클래스를 선언한다.

#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>

#include "Map.hpp"
#include "Types.hpp"

// 한 번 이동했을 때 발생할 수 있는 결과다.
enum class MoveResult {
    Moved,
    HitWall,
    HitSelf
};

// Snake의 몸 좌표와 이동 방향을 관리한다.
class Snake {
public:
    // 시작 머리 위치를 기준으로 왼쪽에 몸통 2칸을 붙여 길이 3으로 시작한다.
    Snake(Position head, Direction direction);

    Direction direction() const;

    // 반대 방향으로 즉시 회전하려는 입력은 실패로 반환한다.
    bool setDirection(Direction next);

    // 현재 방향으로 한 칸 전진하고 벽/몸통 충돌 여부를 반환한다.
    MoveResult move(const Map& map);

    // 렌더러가 특정 좌표에 Snake가 있는지 확인할 때 사용한다.
    bool occupies(Position position) const;

    const std::deque<Position>& body() const;
    Position head() const;

private:
    // 현재 좌표와 방향으로 다음 머리 좌표를 계산한다.
    static Position nextPosition(Position current, Direction direction);

    // 현재 방향과 입력 방향이 정반대인지 확인한다.
    static bool isOpposite(Direction current, Direction next);

    std::deque<Position> body_;
    Direction direction_;
};

#endif
