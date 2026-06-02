// main.cpp
// Game 객체를 생성해 Snake Game 프로그램을 시작하는 진입점이다.

#include "Game.hpp"

int main() {
    // 기본 설정으로 게임 객체를 만들고 전체 게임 루프를 시작한다.
    Game game;
    game.run();
    return 0;
}
