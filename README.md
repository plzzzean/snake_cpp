# Snake Game

C++과 `ncurses`를 사용해 만든 터미널 기반 Snake Game 팀 프로젝트입니다.

## 프로젝트 구조

```text
.
├── Makefile
├── README.md
├── REPORT.md
├── build/              # 빌드 시 생성되는 산출물, git 제외
├── maps/
│   ├── stage1.txt      # 27x27
│   ├── ...
│   └── stage10.txt     # 21x21
├── src/
│   ├── Game.cpp / Game.hpp
│   ├── Map.cpp / Map.hpp
│   ├── Renderer.cpp / Renderer.hpp
│   ├── Snake.cpp / Snake.hpp
│   ├── Types.hpp
│   └── main.cpp
└── tests/
    └── test_logic.cpp
```

## 주요 파일 설명

- `src/Game.*`: 게임 루프, 입력 처리, Tick 기반 이동, 스테이지별 맵 선택, 게임 오버 상태를 관리합니다.
- `src/Map.*`: 채점 기준의 board 역할로, 맵 파일 로드, 기본 맵 생성, 벽 충돌 판정을 담당합니다.
- `src/Snake.*`: Snake 몸 좌표, 방향 변경, 이동 결과를 관리합니다.
- `src/Renderer.*`: `ncurses` 초기화와 화면 출력을 담당합니다.
- `src/Types.hpp`: 맵 셀 종류, 방향, 좌표 타입을 정의합니다.
- `tests/test_logic.cpp`: 맵 로드와 Snake 이동 규칙을 확인하는 간단한 테스트입니다.
- `.gitignore`: `build/`, 실행 파일, object 파일 등 빌드 산출물을 제외합니다.

현재 코드의 실제 담당 범위는 `Map`, `Snake`, `Game`, `Renderer`입니다.
채점 기준의 food, poison, gate 기능은 팀 내 별도 담당 영역으로 분리되어 있어 이 수정본에서는 새 동작을 추가하지 않았습니다.

## 빌드 방법

Ubuntu에서는 먼저 `ncurses` 개발 패키지를 설치합니다.

```bash
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev
```

프로젝트를 빌드하고 실행합니다.

```bash
make
make run
```

현재 기본 설정은 `GameConfig.stageLevel = 10`이므로 `make run`은 `maps/stage10.txt`를 로드합니다.
다른 스테이지를 실행하려면 `src/Game.hpp`의 `stageLevel` 기본값을 바꿉니다.

테스트를 실행합니다.

```bash
make test
```

빌드 산출물을 삭제합니다.

```bash
make clean
```

`make`는 루트에 실행 파일 `snake`를 만들고, object 파일은 `build/` 아래에 생성합니다.
`make test`는 `build/snake_tests`를 생성해 실행합니다.
제출 zip에는 `snake`, `build/`, `*.o` 같은 빌드 산출물을 포함하지 않습니다.

## 조작 방법

- 방향키: Snake 이동 방향 변경
- `q`: 게임 종료

## 맵 파일 형식

`maps/stage1.txt`부터 `maps/stage10.txt`까지는 공백으로 구분된 숫자 값으로 맵을 표현합니다.
현재 파서는 숫자 값만 읽으므로 맵 파일에는 주석을 넣지 않습니다.

- `0`: 빈 칸
- `1`: Wall
- `2`: Immune Wall
- `3`: Snake Head
- `4`: Snake Body
- `5`: Growth Item
- `6`: Poison Item
- `7`: Gate

현재 렌더러는 파일에서 읽은 맵 위에 실제 Snake 상태를 덮어 그립니다.
`Growth Item`, `Poison Item`, `Gate` 셀 값은 팀 전체 통합 시 공통 맵 형식을 맞추기 위해 타입에 남겨 두었습니다.

## 스테이지 맵

맵 파일명은 `maps/stageN.txt` 형식입니다. 현재는 `stage1`부터 `stage10`까지 준비되어 있습니다.
스테이지가 올라갈수록 맵 크기가 줄어들며, 최소 크기는 21x21입니다.
모든 맵은 서로 다른 내부 벽 구조를 가지고 있고, 시작 Snake 위치인 중앙 가로 구간은 비워 두었습니다.

- `stage1`, `stage2`: 27x27
- `stage3`, `stage4`: 25x25
- `stage5`, `stage6`, `stage7`: 23x23
- `stage8`, `stage9`, `stage10`: 21x21

## 현재 게임 흐름

1. `main.cpp`에서 `Game` 객체를 생성합니다.
2. `Game::run()`이 현재 `stageLevel`에 맞는 맵을 로드하고 Snake 초기 위치를 만듭니다.
3. `Renderer::init()`이 터미널을 `ncurses` 모드로 전환합니다.
4. 매 프레임 키 입력을 확인하고, Tick 시간이 지나면 Snake를 이동시킵니다.
5. 충돌이 발생하면 게임 오버 상태로 전환하고, `q` 입력을 기다립니다.
6. 종료 시 `Renderer::shutdown()`이 터미널 상태를 복구합니다.
