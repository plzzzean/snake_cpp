#include "Snake.hpp"

#include <algorithm>
#include <iterator>

Snake::Snake(Position head, Direction direction)
    : direction_(direction) {
    body_.push_back(head);
    body_.push_back({head.row, head.col - 1});
    body_.push_back({head.row, head.col - 2});
}

Direction Snake::direction() const {
    return direction_;
}

bool Snake::setDirection(Direction next) {
    if (next == direction_) {
        return true;
    }

    if (isOpposite(direction_, next)) {
        return false;
    }

    direction_ = next;
    return true;
}

MoveResult Snake::move(Map& map) {
    const Position next = nextPosition(head(), direction_);
    const CellType nextCell = map.at(next.row, next.col);

    if (map.isBlocked(next.row, next.col)) {
        return MoveResult::HitWall;
    }

    const bool hitsBody = std::any_of(body_.begin(), std::prev(body_.end()), [next](Position part) {
        return part == next;
    });

    if (hitsBody) {
        return MoveResult::HitSelf;
    }

    body_.push_front(next);

    if (nextCell == CellType::GrowthItem) {
        map.setCell(next.row, next.col, CellType::Empty);
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

Position Snake::nextPosition(Position current, Direction direction) {
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
    return (current == Direction::Up && next == Direction::Down)
        || (current == Direction::Down && next == Direction::Up)
        || (current == Direction::Left && next == Direction::Right)
        || (current == Direction::Right && next == Direction::Left);
}
