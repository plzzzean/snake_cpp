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
    void init() const;

    // ncurses 모드를 종료해 터미널을 원래 상태로 되돌린다.
    void shutdown() const;

    // 매 프레임 맵, Snake, 상태 문구를 한 화면에 다시 그린다.
    void draw(const Map& map, const Snake& snake, bool stageCleared, bool gameOver, const std::string& status, int stage, int missionLength, int growth, int missionGrowth, int poison, int missionPoison, int gateUseCount, int missionGate, bool hasShield, std::chrono::steady_clock::time_point runStartTime, std::chrono::steady_clock::time_point levelStartTime, std::chrono::steady_clock::time_point gameStartTime, std::chrono::steady_clock::time_point now, int levelDeaths, int totalDeaths) const;

private:
    static constexpr short ColorWall = 1;
    static constexpr short ColorImmuneWall = 2;
    static constexpr short ColorSnakeHead = 3;
    static constexpr short ColorSnakeBody = 4;
    static constexpr short ColorGrowthItem = 5;
    static constexpr short ColorPoisonItem = 6;
    static constexpr short ColorGate = 7;
    static constexpr short ColorShieldItem = 8;
    static constexpr short ColorDynamicWall = 9;

    // CellType을 터미널에 찍을 문자로 변환한다.
    static char symbolFor(CellType cell);

    // CellType을 ncurses 색상 pair 번호로 변환한다.
    static short colorFor(CellType cell);
};

#endif
