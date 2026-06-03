// Renderer.hpp
// ncurses 초기화와 게임 화면 출력을 담당하는 Renderer 클래스를 선언한다.

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>

#include "Map.hpp"
#include "Snake.hpp"

// ncurses 화면 초기화와 맵/Snake 출력만 담당하는 렌더러다.
class Renderer {
public:
    // ncurses 입력/색상 모드를 설정한다.
    void init();

    // ncurses 모드를 종료해 터미널을 원래 상태로 되돌린다.
    void shutdown();

    // 매 프레임 맵, Snake, 상태 문구를 한 화면에 다시 그린다.
    void draw(const Map& map, const Snake& snake, bool stageCleared, bool gameOver, const std::string& status, int stage, int missionLength, int growth, int missionGrowth, int poison, int missionPoison, int gateUseCount, int missionGate) const;

private:
    // CellType을 터미널에 찍을 문자로 변환한다.
    static char symbolFor(CellType cell);

    // CellType을 ncurses 색상 pair 번호로 변환한다.
    static short colorFor(CellType cell);
};

#endif
