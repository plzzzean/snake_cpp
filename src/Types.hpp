// Types.hpp
// 맵 셀 종류, 이동 방향, 좌표처럼 여러 클래스가 공유하는 공통 타입을 정의한다.

#ifndef TYPES_HPP
#define TYPES_HPP

// 맵 파일과 렌더러가 공유하는 칸 종류다.
// 숫자 값은 maps/stage1.txt의 셀 값과 직접 대응한다.
enum class CellType {
    Empty = 0,
    Wall = 1,
    ImmuneWall = 2,
    SnakeHead = 3,
    SnakeBody = 4,
    GrowthItem = 5,
    PoisonItem = 6,
    Gate = 7,
    ShieldItem = 8,
    DynamicWall = 9
};

// Snake가 현재 이동 중인 방향이다.
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

// 맵 위의 좌표다. row는 세로 위치, col은 가로 위치를 의미한다.
struct Position {
    int row;
    int col;

    // 테스트와 충돌 판정에서 좌표를 간단히 비교하기 위한 연산자다.
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

struct Mission {
    int target_length; // B: 목표 길이
    int target_growth; // +: 목표 개수
    int target_poison; // -: 목표 개수
    int target_gate;   // G: 목표 사용 횟수
};

#endif
