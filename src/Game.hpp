#ifndef GAME_HPP
#define GAME_HPP

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "Map.hpp"
#include "Renderer.hpp"
#include "Snake.hpp"

// 게임 속도, 스테이지, 맵 경로처럼 실행 시 바꿀 수 있는 설정값입니다.
struct GameConfig {
    int baseTickMs = 500;
    int minTickMs = 180;
    int stageSpeedStepMs = 45;
    int stageLevel = 10;
    std::string mapPath;
};

// 입력 처리, 이동 Tick, 렌더링 호출을 묶는 게임 진행 컨트롤러입니다.
class Game {
public:
    explicit Game(GameConfig config = {});

    // 맵 로드부터 ncurses 종료까지 전체 게임 루프를 실행합니다.
    void run();

private:
    // 스테이지가 올라갈수록 Tick을 줄이되, 최소 Tick 아래로는 내려가지 않습니다.
    int currentTickMs() const;

    // 키 입력을 해석해 Snake 방향 전환 또는 종료 요청을 처리합니다.
    bool handleInput(int input, Snake& snake);

    // 지정된 맵 파일을 읽고, 실패하면 기본 맵으로 대체합니다.
    void loadMap();

    // mapPath가 비어 있을 때 stageLevel에 맞는 맵 파일 경로를 만듭니다.
    std::string currentMapPath() const;

    // stageLevel 구간에 맞는 fallback 맵 크기를 계산합니다.
    int currentMapSize() const;

    // 현재 맵 중앙에서 오른쪽 방향으로 출발하는 Snake를 만듭니다.
    Snake createInitialSnake() const;

    void refreshExpiredItems(const Snake& snake);
    void spawnItem(CellType item, const Snake& snake);
    std::vector<Position> occupiedPositions(const Snake& snake) const;

    GameConfig config_;
    Map map_;
    Renderer renderer_;
    std::mt19937 rng_;
    std::chrono::steady_clock::time_point growthItemCreatedAt_;
    std::chrono::steady_clock::time_point poisonItemCreatedAt_;
    bool gameOver_ = false;
    bool shouldQuit_ = false;
    std::string status_ = "Running";
};

#endif
