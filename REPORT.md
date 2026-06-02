# Snake Game 프로젝트 보고서

## 1. 개발 목표

본 프로젝트는 C++과 `ncurses`를 사용해 터미널에서 실행되는 Snake Game을 구현하는 것을 목표로 한다. 이 수정본의 담당 범위는 맵 로드, Snake 이동, 충돌 처리, 스테이지 맵 선택, 화면 출력, Stage3 Item 기능, 빌드 및 테스트 구조이다.

## 2. 구현 기능

- `maps/stage1.txt`부터 `maps/stage10.txt`까지 스테이지 맵을 파일로 분리하였다.
- `Map` 클래스가 채점 기준의 board 역할을 담당하며, 맵 파일 로드와 벽/Immune Wall 충돌 판정을 처리한다.
- `Snake` 클래스가 몸 좌표와 현재 방향을 관리하고, 반대 방향 입력과 벽/몸통 충돌을 판정한다.
- `Food` 클래스가 Growth Item의 생성 개수와 5초 재생성 정책을 관리한다.
- `Poison` 클래스가 Poison Item의 생성 개수와 5초 재생성 정책을 관리한다.
- `Game` 클래스가 입력 처리, Tick 기반 이동, 게임 오버 상태, 스테이지별 맵 선택, Item 초기 배치와 재생성 호출을 관리한다.
- `Renderer` 클래스가 `ncurses` 초기화, 종료, 맵과 Snake 출력을 담당한다.
- Growth Item은 `+`로 표시하며, 획득 시 Snake 길이가 1 증가한다.
- Poison Item은 `-`로 표시하며, 획득 시 Snake 길이가 1 감소한다.
- Poison Item 획득 후 Snake 길이가 3보다 작아지면 Game Over로 처리한다.
- 전체 Item 수는 Growth Item 2개 + Poison Item 1개, 최대 3개로 제한한다.
- Item은 Wall, Immune Wall, Snake 몸통, Gate 위치와 겹치지 않는 빈 칸에 생성하며 5초마다 재생성한다.
- `tests/test_logic.cpp`에서 맵 로드, 스테이지 맵 검증, Snake 이동 규칙, Item 획득 효과와 생성 정책을 assert 기반으로 테스트한다.

## 3. 클래스 구조

| 클래스 | 헤더/구현 파일 | 역할 |
|---|---|---|
| `Game` | `src/Game.hpp`, `src/Game.cpp` | 전체 게임 루프, 입력, Tick, 상태 관리 |
| `Food` | `src/Food.hpp`, `src/Food.cpp` | Growth Item 생성, 개수 제한, 5초 재생성 관리 |
| `Map` | `src/Map.hpp`, `src/Map.cpp` | board 역할, 맵 로드, 맵 검증, 충돌 판정 |
| `Poison` | `src/Poison.hpp`, `src/Poison.cpp` | Poison Item 생성, 개수 제한, 5초 재생성 관리 |
| `Snake` | `src/Snake.hpp`, `src/Snake.cpp` | Snake 몸 좌표, 방향, 이동 결과 관리 |
| `Renderer` | `src/Renderer.hpp`, `src/Renderer.cpp` | ncurses 화면 초기화와 출력 |

공통 타입인 `CellType`, `Direction`, `Position`은 `src/Types.hpp`에 정의했다. 모든 주요 클래스는 헤더 파일과 멤버 함수 구현 파일을 별도로 작성했다.

## 4. 개인 담당 범위

| 담당 영역 | 실제 구현 내용 |
|---|---|
| 맵/board | `Map` 클래스, 스테이지 맵 파일 10개, 맵 검증 |
| Snake 로직 | 방향 변경, Tick 이동, 벽 충돌, 몸통 충돌 |
| Item 로직 | `Food`, `Poison` 클래스, Growth 2개 + Poison 1개 생성, 5초 재생성 |
| 게임 흐름 | `Game` 클래스, 입력 처리, 게임 오버 상태, Item 초기 배치와 재생성 호출 |
| 화면 출력 | `Renderer` 클래스, ncurses 기반 맵/Snake 표시 |
| 품질 관리 | Makefile, 테스트 코드, 파일별 역할 주석, `const` 점검 |

Stage3의 Food, Poison 동작은 구현되었다. Gate, Score Board, Mission Board는 이후 팀 통합 단계에서 연결할 영역이며, `CellType`에는 해당 셀 값을 유지했다.

## 5. 실행 및 테스트

빌드:

```bash
make
```

실행:

```bash
make run
```

테스트:

```bash
make test
```

정리:

```bash
make clean
```

Stage3 Item 정리 후 다음 명령을 순서대로 실행해 통과를 확인했다.

```bash
make clean
make
make test
```

## 6. 발표 영상 데모 체크리스트

- `make run`으로 실제 게임 실행 장면을 보여준다.
- 방향키 입력으로 Snake가 이동하는 장면을 보여준다.
- Growth Item `+` 획득 후 Snake 길이가 증가하는 장면을 보여준다.
- Poison Item `-` 획득 후 Snake 길이가 감소하는 장면을 보여준다.
- Poison Item으로 길이가 3보다 작아질 때 Game Over가 되는 장면을 보여준다.
- 반대 방향 입력 또는 벽 충돌로 Game Over가 되는 장면을 보여준다.
- `Map`, `Snake`, `Game`, `Renderer`, `Food`, `Poison`이 각각 어떤 역할인지 코드 파일을 열어 설명한다.
- 본인 담당 범위와 Gate/Score/Mission 담당 범위가 분리되어 있음을 보고서 기준으로 설명한다.

## 7. 어려웠던 점과 개선점

맵 파일을 여러 크기의 스테이지로 관리하면서도 Snake 시작 위치와 충돌 판정이 안정적으로 동작하도록 검증하는 부분이 중요했다. 이를 위해 모든 스테이지 맵의 크기, 외곽 벽, 중앙 시작 구간을 테스트로 확인했다.

Stage3 Item 구현에서는 기존 `Game`에 몰려 있던 생성/재생성 책임을 `Food`, `Poison` 클래스로 나눠 유지보수성을 높였다. 추후 팀 통합 단계에서는 Gate, Score Board, Mission Board를 `Game` 루프와 `Renderer` 출력에 연결하면 전체 과제 요구사항을 완성할 수 있다.
