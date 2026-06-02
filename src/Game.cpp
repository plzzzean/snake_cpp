// Game.cpp
// Game 클래스의 맵 로드, 입력 처리, Tick 이동, 렌더링 호출 로직을 구현한다.

#include "Game.hpp"

#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <utility>

#include <ncurses.h>

namespace {
constexpr std::chrono::milliseconds FrameDelay{16};
}

Game::Game(GameConfig config)
    : config_(std::move(config)),
      rng_(std::random_device{}()) {
}

void Game::run() {
    // 게임 시작 전에 맵과 Snake를 먼저 준비하고 ncurses 화면을 초기화한다.
    loadMap();
    Snake snake = createInitialSnake();
    spawnItems(snake);

    renderer_.init();
    auto lastTick = std::chrono::steady_clock::now();

    while (!shouldQuit_) {
        // nodelay 모드라 입력이 없으면 ERR이 반환되고 게임 루프는 계속 진행된다.
        const int input = getch();
        if (input != ERR) {
            shouldQuit_ = handleInput(input, snake);
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        // 설정된 Tick 간격이 지났을 때만 Snake를 한 칸 이동시켜 게임 속도를 제어한다.
        if (!gameOver_ && elapsed.count() >= currentTickMs()) {
            const MoveResult result = snake.move(map_);
            if (result == MoveResult::HitWall) {
                gameOver_ = true;
                status_ = "Hit wall";
            } else if (result == MoveResult::HitSelf) {
                gameOver_ = true;
                status_ = "Hit body";
            } else if (result == MoveResult::TooShort) {
                // Poison Item 획득으로 길이가 3 미만이 되면 과제 규칙상 실패 처리한다.
                gameOver_ = true;
                status_ = "Snake too short";
            } else if (result == MoveResult::AteGrowth) {
                status_ = "Ate growth item";
                food_.spawn(map_, occupiedPositions(snake), rng_);
            } else if (result == MoveResult::AtePoison) {
                status_ = "Ate poison item";
                poison_.spawn(map_, occupiedPositions(snake), rng_);
            }
            lastTick = now;
        }

        // 이동 여부와 관계없이 화면은 계속 갱신해서 입력과 상태 문구가 바로 반영되게 한다.
        if (!gameOver_) {
            refreshExpiredItems(snake);
        }
        renderer_.draw(map_, snake, gameOver_, status_);
        std::this_thread::sleep_for(FrameDelay);
    }

    renderer_.shutdown();
}

int Game::currentTickMs() const {
    // 스테이지가 높을수록 Tick을 줄이되, 너무 빨라지지 않도록 minTickMs로 하한을 둔다.
    const int stagePenalty = std::max(0, config_.stageLevel - 1) * config_.stageSpeedStepMs;
    return std::max(config_.minTickMs, config_.baseTickMs - stagePenalty);
}

bool Game::handleInput(int input, Snake& snake) {
    // 종료 키는 게임 오버 여부와 관계없이 즉시 루프 종료 요청으로 처리한다.
    if (input == 'q' || input == 'Q') {
        return true;
    }

    // 게임 오버 뒤에는 방향 입력을 무시하고 q 입력만 기다린다.
    if (gameOver_) {
        return false;
    }

    Direction next = Direction::Right;
    bool directionKey = true;

    switch (input) {
    case KEY_UP:
        next = Direction::Up;
        break;
    case KEY_DOWN:
        next = Direction::Down;
        break;
    case KEY_LEFT:
        next = Direction::Left;
        break;
    case KEY_RIGHT:
        next = Direction::Right;
        break;
    default:
        directionKey = false;
        break;
    }

    if (directionKey) {
        // Snake 규칙상 정반대 방향 입력은 자기 몸으로 되돌아가는 입력이므로 게임 오버로 처리한다.
        if (!snake.setDirection(next)) {
            gameOver_ = true;
            status_ = "Reverse direction";
        }
    }

    return false;
}

void Game::loadMap() {
    // 스테이지 맵 로드에 실패해도 실행은 계속할 수 있도록 fallback 맵을 생성한다.
    const std::string mapPath = currentMapPath();
    if (!map_.loadFromFile(mapPath)) {
        map_.loadFallbackMap(currentMapSize());
        status_ = "Using fallback map";
    }
}

std::string Game::currentMapPath() const {
    // 사용자가 직접 mapPath를 지정했으면 스테이지 번호보다 해당 경로를 우선한다.
    if (!config_.mapPath.empty()) {
        return config_.mapPath;
    }

    // 준비된 스테이지 파일 범위를 벗어나지 않도록 1~10 사이로 제한한다.
    constexpr int MaxStageLevel = 10;
    const int stageLevel = std::clamp(config_.stageLevel, 1, MaxStageLevel);
    return "maps/stage" + std::to_string(stageLevel) + ".txt";
}

int Game::currentMapSize() const {
    // fallback 맵도 스테이지가 높을수록 작아지게 맞춰 실제 스테이지 난이도 흐름을 따라간다.
    const int stageLevel = std::max(1, config_.stageLevel);

    if (stageLevel <= 2) {
        return 27;
    }
    if (stageLevel <= 4) {
        return 25;
    }
    if (stageLevel <= 7) {
        return 23;
    }
    return 21;
}

Snake Game::createInitialSnake() const {
    // 초기 Snake는 맵 중앙에서 오른쪽으로 이동하며, 생성자에서 몸통을 왼쪽으로 붙인다.
    return Snake({map_.rows() / 2, map_.cols() / 2}, Direction::Right);
}

void Game::spawnItems(const Snake& snake) {
    const std::vector<Position> occupied = occupiedPositions(snake);
    food_.spawn(map_, occupied, rng_);
    poison_.spawn(map_, occupied, rng_);
}

void Game::refreshExpiredItems(const Snake& snake) {
    const std::vector<Position> occupied = occupiedPositions(snake);
    food_.refreshIfExpired(map_, occupied, rng_);
    poison_.refreshIfExpired(map_, occupied, rng_);
}

std::vector<Position> Game::occupiedPositions(const Snake& snake) const {
    return std::vector<Position>(snake.body().begin(), snake.body().end());
}
