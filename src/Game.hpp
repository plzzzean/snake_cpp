// Game.hpp
// Snake Game의 실행 설정과 전체 게임 루프를 관리하는 Game 클래스를 선언한다.

#ifndef GAME_HPP
#define GAME_HPP

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "Food.hpp"
#include "Gate.hpp"
#include "Map.hpp"
#include "Poison.hpp"
#include "Renderer.hpp"
#include "Snake.hpp"

// 게임 속도, 스테이지, 맵 경로처럼 실행 시 바꿀 수 있는 설정값이다.
struct GameConfig {
    int baseTickMs = 500;
    int minTickMs = 180;
    int stageSpeedStepMs = 45;
    int stageLevel = 10;
    std::string mapPath;
};

// 입력 처리, 이동 Tick, 렌더링 호출을 묶는 게임 진행 컨트롤러다.
class Game {
public:
    explicit Game(GameConfig config = {});

    // 맵 로드부터 ncurses 종료까지 전체 게임 루프를 실행한다.
    void run();

private:
    // 스테이지가 올라갈수록 Tick을 줄이되, 최소 Tick 아래로는 내려가지 않는다.
    int currentTickMs() const;

    // 키 입력을 해석해 Snake 방향 전환 또는 종료 요청을 처리한다.
    bool handleInput(int input, Snake& snake);

    // 지정된 맵 파일을 읽고, 실패하면 기본 맵으로 대체한다.
    void loadMap();

    // mapPath가 비어 있을 때 stageLevel에 맞는 맵 파일 경로를 만든다.
    std::string currentMapPath() const;

    // stageLevel 구간에 맞는 fallback 맵 크기를 계산한다.
    int currentMapSize() const;

    // 현재 맵 중앙에서 오른쪽 방향으로 출발하는 Snake를 만든다.
    Snake createInitialSnake() const;

    // Food와 Poison 객체에 현재 Snake 위치를 전달해 아이템을 초기 배치한다.
    void spawnItems(const Snake& snake);

    // 5초가 지난 아이템을 과제 규칙에 맞게 새 위치로 재생성한다.
    void refreshExpiredItems(const Snake& snake);

    // Gate 생성 대기, Snake 진입 감지, 순간이동, 사용 후 제거를 처리한다.
    void handleGate(Snake& snake);

    std::vector<Position> occupiedPositions(const Snake& snake) const;

    // Gate는 게임 시작 후 일정 시간이 지나면 출현하고, 통과 후 다시 대기한다.
    static constexpr int GateSpawnDelaySeconds = 10;

    GameConfig config_;
    Map map_;
    Food food_;
    Gate gate_;
    Poison poison_;
    Renderer renderer_;
    std::mt19937 rng_;
    std::chrono::steady_clock::time_point gateLastClearedAt_;
    int gateUseCount_ = 0;
    bool gameOver_ = false;
    bool shouldQuit_ = false;
    std::string status_ = "Running";
};

#endif
