#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>

#include "Types.hpp"

// 맵 파일을 읽고, 벽/빈 공간 등 셀 정보를 조회하는 클래스입니다.
class Map {
public:
    // 공백으로 구분된 숫자 맵 파일을 읽어 grid_에 저장합니다.
    bool loadFromFile(const std::string& path);

    // 맵 파일을 읽지 못했을 때 사용하는 기본 정사각형 맵을 생성합니다.
    void loadFallbackMap(int size = 21);

    // 범위를 벗어난 좌표는 벽으로 취급해 충돌 판정을 안전하게 만듭니다.
    CellType at(int row, int col) const;
    bool isBlocked(int row, int col) const;
    bool contains(int row, int col) const;

    // 현재 로드된 맵의 크기와 오류 메시지를 외부에 알려줍니다.
    int rows() const;
    int cols() const;
    const std::string& errorMessage() const;

private:
    // 플레이 가능한 최소 크기와 직사각형 형태인지 확인합니다.
    bool validateGrid(const std::vector<std::vector<CellType>>& grid);

    // 맵 파일에 허용된 CellType 숫자만 들어왔는지 검사합니다.
    static bool isKnownCellValue(int value);

    std::vector<std::vector<CellType>> grid_;
    std::string errorMessage_;
};

#endif
