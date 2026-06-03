// Snake.cpp
// Snake 클래스의 방향 전환, 전진 이동, 벽/몸통 충돌 판정 로직을 구현한다.

#include "Snake.hpp"

#include <algorithm>
#include <iterator>

Snake::Snake(Position head, Direction direction)
    : direction_(direction) {
    // 시작 시 머리를 먼저 넣고, 진행 방향 기준 왼쪽에 몸통 2칸을 붙여 길이 3을 만든다.
    body_.push_back(head);
    body_.push_back({head.row, head.col - 1});
    body_.push_back({head.row, head.col - 2});
}

Direction Snake::direction() const {
    return direction_;
}

bool Snake::setDirection(Direction next) {
    // 같은 방향 입력은 상태 변화가 없지만 정상 입력으로 처리한다.
    if (next == direction_) {
        return true;
    }

    // 반대 방향으로 즉시 꺾으면 몸통과 충돌하는 입력이므로 거부한다.
    if (isOpposite(direction_, next)) {
        return false;
    }

    direction_ = next;
    return true;
}

MoveResult Snake::move(Map& map) {
    // 현재 머리와 방향을 기준으로 다음 Tick에서 도착할 좌표를 먼저 계산한다.
    const Position next = nextPosition(head(), direction_);
    const CellType nextCell = map.at(next.row, next.col);

    // 맵의 벽 또는 범위 밖 좌표에 닿으면 이동하지 않고 벽 충돌을 반환한다.
    if (map.isBlocked(next.row, next.col)) {
        return MoveResult::HitWall;
    }

    // 꼬리는 이번 이동에서 빠질 칸이므로 자기 충돌 검사 대상에서 제외한다.
    const bool hitsBody = std::any_of(body_.begin(), std::prev(body_.end()), [next](Position part) {
        return part == next;
    });

    if (hitsBody) {
        return MoveResult::HitSelf;
    }

    // 새 머리를 앞에 추가하고 아이템 종류에 따라 길이와 맵 상태를 갱신한다.
    body_.push_front(next);

    if (nextCell == CellType::GrowthItem) {
        map.setCell(next.row, next.col, CellType::Empty);
        if ((int)body_.size() > MAX_LENGTH) {
            body_.pop_back();
        }
        return MoveResult::AteGrowth;
    }

    body_.pop_back();
    if (nextCell == CellType::PoisonItem) {
        map.setCell(next.row, next.col, CellType::Empty);
        body_.pop_back();
        if (body_.size() < 3) {
            return MoveResult::TooShort;
        }
        return MoveResult::AtePoison;
    }

    return MoveResult::Moved;
}

bool Snake::occupies(Position position) const {
    return std::find(body_.begin(), body_.end(), position) != body_.end();
}

const std::deque<Position>& Snake::body() const {
    return body_;
}

Position Snake::head() const {
    return body_.front();
}

void Snake::teleportHead(Position newHead, Direction newDir) {
    // 머리 좌표와 이동 방향을 Gate 출구 기준으로 교체한다.
    body_.front() = newHead;
    direction_ = newDir;
}

Position Snake::nextPosition(Position current, Direction direction) {
    // 좌표계는 row가 세로, col이 가로이므로 위/아래는 row를, 좌/우는 col을 바꾼다.
    switch (direction) {
    case Direction::Up:
        return {current.row - 1, current.col};
    case Direction::Down:
        return {current.row + 1, current.col};
    case Direction::Left:
        return {current.row, current.col - 1};
    case Direction::Right:
        return {current.row, current.col + 1};
    }
    return current;
}

bool Snake::isOpposite(Direction current, Direction next) {
    // 현재 방향과 입력 방향이 같은 축에서 서로 반대인지 확인한다.
    return (current == Direction::Up && next == Direction::Down)
        || (current == Direction::Down && next == Direction::Up)
        || (current == Direction::Left && next == Direction::Right)
        || (current == Direction::Right && next == Direction::Left);
}
