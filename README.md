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
│   ├── Food.cpp / Food.hpp
│   ├── DynamicWall.cpp / DynamicWall.hpp
│   ├── Game.cpp / Game.hpp
│   ├── Gate.cpp / Gate.hpp
│   ├── Map.cpp / Map.hpp
│   ├── Poison.cpp / Poison.hpp
│   ├── Renderer.cpp / Renderer.hpp
│   ├── Snake.cpp / Snake.hpp
│   ├── Types.hpp
│   ├── Shield.cpp / Shield.hpp
│   └── main.cpp
└── tests/
    └── test_logic.cpp
```

## 주요 파일 설명

- `src/Food.*`: Growth Item의 생성 개수와 5초 재생성 정책을 관리합니다.
- `src/DynamicWall.*`: 내부 Wall 하나를 8초마다 안전한 빈 칸으로 이동시키는 추가 기능을 관리합니다.
- `src/Game.*`: 게임 루프, 입력 처리, Tick 기반 이동, 스테이지별 맵 선택, 게임 오버 상태, 아이템 초기 배치와 재생성 호출, Gate 생성 대기와 진입 처리를 관리합니다.
- `src/Gate.*`: Gate 쌍의 생성, 진출 방향 계산, 활성 상태 추적을 담당합니다.
- `src/Map.*`: 채점 기준의 board 역할로, 맵 파일 로드, 기본 맵 생성, 벽 충돌 판정을 담당합니다.
- `src/Poison.*`: Poison Item의 생성 개수와 5초 재생성 정책을 관리합니다.
- `src/Snake.*`: Snake 몸 좌표, 방향 변경, 이동 결과, Gate 통과 후 위치 갱신을 관리합니다.
- `src/Renderer.*`: `ncurses` 초기화와 화면 출력을 담당합니다.
- `src/Types.hpp`: 맵 셀 종류, 방향, 좌표 타입을 정의합니다.
- `src/Shield.*`: Shield Item의 생성 개수와 5초 재생성 정책, 보호막 획득 상태 처리를 관리합니다.
- `tests/test_logic.cpp`: 맵 로드와 Snake 이동 규칙을 확인하는 간단한 테스트입니다.
- `.gitignore`: `build/`, 실행 파일, object 파일 등 빌드 산출물을 제외합니다.

현재 코드의 실제 담당 범위는 `Map`, `Snake`, `Game`, `Renderer`, `Food`, `Poison`, `Shield`, `Gate`입니다.
Stage3 Item 기능, Stage4 Gate 기능, Stage5 Score/Mission 기능이 구현되어 있으며, 추가 Item인 Shield 기능이 확장 구현되었습니다.

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

게임은 `stage1`에서 시작하며 Mission을 달성하면 다음 스테이지로 진행합니다.
`GameConfig.stageLevel`은 마지막 스테이지 번호를 정합니다.

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
- `r`: 게임 오버 후 재시작
- `q`: 게임 종료

## Item 규칙

- Growth Item은 `+`로 표시되며, 획득하면 Snake 길이가 1 증가합니다.
- Poison Item은 `-`로 표시되며, 획득하면 Snake 길이가 1 감소합니다.
- Poison Item 획득 후 Snake 길이가 3보다 작아지면 Game Over입니다.
- 전체 Item은 최대 3개이며, 현재 구현은 Growth Item 1개 + Poison Item 1개 + Shield Item 1개 구조입니다.
- Shield Item은 `S`로 표시되며, 획득하면 보호막 상태가 됩니다.
- 보호막 상태에서는 Wall 또는 Snake 몸통 충돌을 1회 무효화하며, 사용 후 Shield 상태가 해제됩니다.
- Item은 Wall, Immune Wall, Snake 몸통, Gate 위치와 겹치지 않는 빈 칸에 생성됩니다.
- Item은 5초마다 기존 위치에서 사라지고 새 빈 칸에 재생성됩니다.

## Gate 규칙

- 게임 시작 후 10초가 지나면 ImmuneWall을 제외한 일반 Wall 위치 두 곳에 Gate 쌍이 출현합니다.
- Gate는 화면에 `G`(파란색)로 표시되며 Wall과 구분됩니다.
- Snake 머리가 Gate에 진입하면 반대편 Gate 출구로 순간이동합니다.
- 출구 Gate가 가장자리 Wall에 있으면 항상 맵 안쪽 방향으로 진출합니다.
  - 상단 벽 → 아래 방향 / 하단 벽 → 위 방향 / 좌측 벽 → 오른쪽 / 우측 벽 → 왼쪽
- 출구 Gate가 내부 Wall에 있으면 진입 방향 → 시계방향 → 역시계방향 → 반대 방향 순으로 진출 가능한 방향을 결정합니다.
- Gate 통과 후 자기 몸통과 충돌하면 Game Over입니다.
- Snake 몸통이 입구 Gate를 완전히 빠져나갈 때까지 Gate 쌍을 유지합니다.
- 통과 완료 후 Gate는 원래 Wall 종류로 복원되며, 10초 후 새 Gate 쌍이 나타납니다.

## Dynamic Wall 추가 기능

- 내부 일반 Wall 하나를 `D`(자홍색)로 표시되는 Dynamic Wall로 지정합니다.
- Dynamic Wall은 8초마다 내부의 다른 빈 칸으로 이동해 맵의 이동 경로를 변화시킵니다.
- Snake 몸통, Item, Gate, 외곽 Wall, Immune Wall과 겹치지 않습니다.
- 일반 Wall과 동일하게 충돌 시 Game Over이며, Gate 위치 후보로도 사용할 수 있습니다.
- Dynamic Wall이 Gate가 되면 통과가 끝날 때까지 이동하지 않고, Gate 제거 후 `D`로 정확히 복원됩니다.

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
- `8`: Shield Item
- `9`: Dynamic Wall
현재 렌더러는 파일에서 읽은 맵 위에 실제 Snake 상태를 덮어 그립니다.
`Growth Item`, `Poison Item`, `Shield Item`은 게임 중 `Food`, `Poison`, `Shield` 클래스가 빈 칸에 배치합니다. `Gate` 셀 값은 Gate 출현 및 통과 처리를 위해 사용됩니다.

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
4. Food, Poison, Shield가 Growth Item 1개, Poison Item 1개, Shield Item 1개를 빈 칸에 배치합니다.
5. 매 프레임 키 입력을 확인하고, Tick 시간이 지나면 Snake를 이동시킵니다.
6. Snake가 Item을 획득하면 길이 증가, 길이 감소, 보호막 획득 효과를 적용하고 해당 Item을 재생성합니다.
7. 5초가 지난 Item은 새 빈 위치에 재생성됩니다.
8. 게임 시작 후 10초가 지나면 `Gate::spawn()`이 Wall 위치 두 곳에 Gate 쌍을 생성합니다.
9. Dynamic Wall은 8초마다 Snake와 다른 객체를 피해 새로운 빈 칸으로 이동합니다.
10. Snake 머리가 Gate 위치에 도달하면 출구 방향을 계산해 순간이동시킵니다.
11. Snake 몸통이 입구 Gate를 벗어나면 Gate를 원래 Wall로 복원합니다.
12. 충돌 또는 길이 3 미만 상태가 발생하면 게임 오버 상태로 전환합니다.
13. 종료 시 `Renderer::shutdown()`이 터미널 상태를 복구합니다.
