// Renderer.cpp
// Renderer 클래스의 ncurses 설정, 맵/Snake 출력, 셀 기호와 색상 변환을 구현한다.

#include "Renderer.hpp"

#include <ncurses.h>

namespace {
constexpr short ColorWall = 1;
constexpr short ColorImmuneWall = 2;
constexpr short ColorSnakeHead = 3;
constexpr short ColorSnakeBody = 4;
constexpr short ColorGrowthItem = 5;
constexpr short ColorPoisonItem = 6;
constexpr short ColorGate = 7;
}

void Renderer::init() {
    // ncurses를 키 입력을 즉시 받고 커서를 숨기는 게임 화면 모드로 설정한다.
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    if (has_colors()) {
        // 셀 종류별로 사용할 색상 pair를 등록한다.
        start_color();
        init_pair(ColorWall, COLOR_WHITE, COLOR_BLACK);
        init_pair(ColorImmuneWall, COLOR_CYAN, COLOR_BLACK);
        init_pair(ColorSnakeHead, COLOR_GREEN, COLOR_BLACK);
        init_pair(ColorSnakeBody, COLOR_YELLOW, COLOR_BLACK);
        init_pair(ColorGrowthItem, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(ColorPoisonItem, COLOR_RED, COLOR_BLACK);
        init_pair(ColorGate, COLOR_BLUE, COLOR_BLACK);
    }
}

void Renderer::shutdown() {
    endwin();
}

void Renderer::draw(const Map& map, const Snake& snake, bool gameOver, const std::string& status) const {
    // 이전 프레임을 지운 뒤 맵 전체를 다시 그려 화면 상태를 단순하게 유지한다.
    erase();

    for (int row = 0; row < map.rows(); ++row) {
        for (int col = 0; col < map.cols(); ++col) {
            CellType cell = map.at(row, col);
            const Position position{row, col};

            // 맵 셀보다 Snake 위치를 우선해서 머리와 몸통이 화면에 표시되도록 한다.
            if (snake.occupies(position)) {
                cell = position == snake.head() ? CellType::SnakeHead : CellType::SnakeBody;
            }

            // 가로 폭을 맞추기 위해 한 칸을 문자 하나와 공백 하나로 출력한다.
            const short color = colorFor(cell);
            if (has_colors() && color != 0) {
                attron(COLOR_PAIR(color));
            }
            mvaddch(row, col * 2, symbolFor(cell));
            mvaddch(row, col * 2 + 1, ' ');
            if (has_colors() && color != 0) {
                attroff(COLOR_PAIR(color));
            }
        }
    }

    // 맵 오른쪽 여백에 조작법과 현재 게임 상태를 표시한다.
    const int infoCol = map.cols() * 2 + 3;
    mvprintw(1, infoCol, "Snake Game");
    mvprintw(3, infoCol, "Arrow keys: move");
    mvprintw(4, infoCol, "q: quit");
    mvprintw(6, infoCol, "Status: %s", status.c_str());

    if (gameOver) {
        mvprintw(8, infoCol, "GAME OVER");
        mvprintw(9, infoCol, "r: restart  q: quit");
    }

    refresh();
}

char Renderer::symbolFor(CellType cell) {
    // 맵 파일의 CellType 값을 터미널에서 볼 수 있는 문자로 바꾼다.
    switch (cell) {
    case CellType::Empty:
        return ' ';
    case CellType::Wall:
        return '#';
    case CellType::ImmuneWall:
        return 'X';
    case CellType::SnakeHead:
        return '@';
    case CellType::SnakeBody:
        return 'o';
    case CellType::GrowthItem:
        return '+';
    case CellType::PoisonItem:
        return '-';
    case CellType::Gate:
        return 'G';
    }
    return '?';
}

short Renderer::colorFor(CellType cell) {
    // 색상이 필요한 셀만 등록된 color pair 번호를 반환하고, 나머지는 기본 색상을 사용한다.
    switch (cell) {
    case CellType::Wall:
        return ColorWall;
    case CellType::ImmuneWall:
        return ColorImmuneWall;
    case CellType::SnakeHead:
        return ColorSnakeHead;
    case CellType::SnakeBody:
        return ColorSnakeBody;
    case CellType::GrowthItem:
        return ColorGrowthItem;
    case CellType::PoisonItem:
        return ColorPoisonItem;
    case CellType::Gate:
        return ColorGate;
    default:
        return 0;
    }
}
