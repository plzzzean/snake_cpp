// Map.hpp
// 숫자 기반 스테이지 맵을 로드하고 셀 조회/충돌 판정을 제공하는 Map 클래스를 선언한다.

#ifndef MAP_HPP
#define MAP_HPP

#include <random>
#include <string>
#include <vector>

#include "Types.hpp"

// 맵 파일을 읽고, 벽/빈 공간 등 셀 정보를 조회하는 클래스다.
class Map {
public:
    // 공백으로 구분된 숫자 맵 파일을 읽어 grid_에 저장한다.
    bool loadFromFile(const std::string& path);

    // 맵 파일을 읽지 못했을 때 사용하는 기본 정사각형 맵을 생성한다.
    void loadFallbackMap(int size = 21);

    // 범위를 벗어난 좌표는 벽으로 취급해 충돌 판정을 안전하게 만든다.
    CellType at(int row, int col) const;
    bool isBlocked(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool contains(int row, int col) const;

    // 맵 안의 특정 칸을 원하는 CellType으로 바꾼다.
    bool setCell(int row, int col, CellType cell);

    // 맵에서 특정 CellType에 해당하는 칸을 모두 빈 칸으로 되돌린다.
    void clearCells(CellType cell);

    // Snake가 차지하지 않는 빈 칸 중 하나에 아이템을 무작위로 배치한다.
    bool placeRandomItem(CellType item, const std::vector<Position>& occupied, std::mt19937& rng);

    // 테스트와 아이템 정책 확인을 위해 특정 셀 종류의 개수를 센다.
    int countCells(CellType cell) const;

    // 현재 로드된 맵의 크기와 오류 메시지를 외부에 알려준다.
    int rows() const;
    int cols() const;
    const std::string& errorMessage() const;

private:
    // 플레이 가능한 최소 크기와 직사각형 형태인지 확인한다.
    bool validateGrid(const std::vector<std::vector<CellType>>& grid);

    // 맵 파일에 허용된 CellType 숫자만 들어왔는지 검사한다.
    static bool isKnownCellValue(int value);

    std::vector<std::vector<CellType>> grid_;
    std::string errorMessage_;
};

#endif
