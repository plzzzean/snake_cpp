#include "Game.hpp"

#include <algorithm>
#include <string>
#include <thread>
#include <utility>

#include <ncurses.h>

Game::Game(GameConfig config)
    : config_(std::move(config)) {
}

void Game::run() {
    loadMap();
    Snake snake = createInitialSnake();

    renderer_.init();
    auto lastTick = std::chrono::steady_clock::now();

    while (!shouldQuit_) {
        const int input = getch();
        if (input != ERR) {
            shouldQuit_ = handleInput(input, snake);
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        if (!gameOver_ && elapsed.count() >= currentTickMs()) {
            const MoveResult result = snake.move(map_);
            if (result == MoveResult::HitWall) {
                gameOver_ = true;
                status_ = "Hit wall";
            } else if (result == MoveResult::HitSelf) {
                gameOver_ = true;
                status_ = "Hit body";
            }
            lastTick = now;
        }

        renderer_.draw(map_, snake, gameOver_, status_);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    renderer_.shutdown();
}

int Game::currentTickMs() const {
    const int stagePenalty = std::max(0, config_.stageLevel - 1) * config_.stageSpeedStepMs;
    return std::max(config_.minTickMs, config_.baseTickMs - stagePenalty);
}

bool Game::handleInput(int input, Snake& snake) {
    if (input == 'q' || input == 'Q') {
        return true;
    }

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
        if (!snake.setDirection(next)) {
            gameOver_ = true;
            status_ = "Reverse direction";
        }
    }

    return false;
}

void Game::loadMap() {
    const std::string mapPath = currentMapPath();
    if (!map_.loadFromFile(mapPath)) {
        map_.loadFallbackMap(currentMapSize());
        status_ = "Using fallback map";
    }
}

std::string Game::currentMapPath() const {
    if (!config_.mapPath.empty()) {
        return config_.mapPath;
    }

    constexpr int MaxStageLevel = 10;
    const int stageLevel = std::clamp(config_.stageLevel, 1, MaxStageLevel);
    return "maps/stage" + std::to_string(stageLevel) + ".txt";
}

int Game::currentMapSize() const {
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
    return Snake({map_.rows() / 2, map_.cols() / 2}, Direction::Right);
}
