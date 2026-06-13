# Snake Game 프로젝트 보고서

## 1. 개발 목표

본 프로젝트는 C++과 `ncurses`를 사용해 터미널에서 실행되는 Snake Game을 구현하는 것을 목표로 한다. 개인 담당 범위는 5단계 중 4단계인 Wall/Gate 상호작용이며, 기본 Gate 규칙과 Wall 변화 추가 기능을 구현했다.

## 2. 구현 기능

- `maps/stage1.txt`부터 `maps/stage10.txt`까지 스테이지 맵을 파일로 분리하였다.
- `Map` 클래스가 채점 기준의 board 역할을 담당하며, 맵 파일 로드와 벽/Immune Wall 충돌 판정을 처리한다.
- `Snake` 클래스가 몸 좌표와 현재 방향을 관리하고, 반대 방향 입력과 벽/몸통 충돌을 판정한다.
- `Food` 클래스가 Growth Item의 생성 개수와 5초 재생성 정책을 관리한다.
- `Poison` 클래스가 Poison Item의 생성 개수와 5초 재생성 정책을 관리한다.
- `Gate` 클래스가 Gate 쌍의 생성, 진출 방향 계산, 활성 상태 추적을 담당한다.
- `DynamicWall` 클래스가 내부 Wall의 초기 선택과 8초 주기 재배치를 담당한다.
- `Game` 클래스가 입력 처리, Tick 기반 이동, 게임 오버 상태, 스테이지별 맵 선택, Item 초기 배치와 재생성 호출, Gate 생성 대기와 진입 처리를 관리한다.
- `Renderer` 클래스가 `ncurses` 초기화, 종료, 맵과 Snake 출력을 담당한다.
- Growth Item은 `+`로 표시하며, 획득 시 Snake 길이가 1 증가한다.
- Poison Item은 `-`로 표시하며, 획득 시 Snake 길이가 1 감소한다.
- Poison Item 획득 후 Snake 길이가 3보다 작아지면 Game Over로 처리한다.
- 전체 Item 수는 Growth Item 1개 + Poison Item 1개 + Shield Item 1개, 최대 3개로 제한한다.
- Item은 Wall, Immune Wall, Snake 몸통, Gate 위치와 겹치지 않는 빈 칸에 생성하며 5초마다 재생성한다.
- Gate는 게임 시작 후 10초가 지나면 ImmuneWall을 제외한 일반 Wall 위치 두 곳에 출현한다.
- Snake 머리가 Gate에 진입하면 반대편 Gate 출구로 순간이동하며, 가장자리 벽은 항상 안쪽 방향, 내부 벽은 진입 방향 우선순위로 진출 방향을 결정한다.
- Gate 통과 후 자기 몸통과 충돌하면 Game Over로 처리한다.
- Snake 몸통이 입구 Gate를 완전히 빠져나갈 때까지 Gate 쌍을 유지하고, 이후 원래 Wall 종류로 복원한다.
- Dynamic Wall은 `D`로 표시되며 8초마다 Snake, Item, Gate를 피한 내부 빈 칸으로 이동한다.
- Dynamic Wall은 일반 Wall처럼 충돌을 처리하며 Gate로 변할 수 있고, Gate 종료 후에도 Dynamic Wall로 복원된다.
- `tests/test_logic.cpp`에서 맵 로드, 스테이지 맵 검증, Snake 이동 규칙, Item 획득 효과와 생성 정책을 assert 기반으로 테스트한다.

## 3. 클래스 구조

| 클래스 | 헤더/구현 파일 | 역할 |
|---|---|---|
| `Game` | `src/Game.hpp`, `src/Game.cpp` | 전체 게임 루프, 입력, Tick, 상태 관리, Gate 진입 처리 |
| `DynamicWall` | `src/DynamicWall.hpp`, `src/DynamicWall.cpp` | 이동 Wall의 생성, 주기 확인, 안전 재배치 |
| `Food` | `src/Food.hpp`, `src/Food.cpp` | Growth Item 생성, 개수 제한, 5초 재생성 관리 |
| `Gate` | `src/Gate.hpp`, `src/Gate.cpp` | Gate 쌍 생성, 진출 방향 계산, 활성 상태 관리 |
| `Map` | `src/Map.hpp`, `src/Map.cpp` | board 역할, 맵 로드, 맵 검증, 충돌 판정 |
| `Poison` | `src/Poison.hpp`, `src/Poison.cpp` | Poison Item 생성, 개수 제한, 5초 재생성 관리 |
| `Snake` | `src/Snake.hpp`, `src/Snake.cpp` | Snake 몸 좌표, 방향, 이동 결과, Gate 통과 후 위치 갱신 |
| `Renderer` | `src/Renderer.hpp`, `src/Renderer.cpp` | ncurses 화면 초기화와 출력 |

공통 타입인 `CellType`, `Direction`, `Position`은 `src/Types.hpp`에 정의했다. 모든 주요 클래스는 헤더 파일과 멤버 함수 구현 파일을 별도로 작성했다.

## 4. 개인 담당 범위

| 담당 영역 | 실제 구현 내용 |
|---|---|
| Gate 생성 | 일반 Wall과 Dynamic Wall 중 두 곳을 선택하고 Immune Wall은 제외 |
| Gate 진출 | 가장자리 안쪽 고정 방향, 내부 벽의 직진/시계/반시계/반대 우선순위 구현 |
| Gate 생명주기 | 생성 대기, 진입 감지, 몸통 통과 중 유지, 통과 완료 후 원래 벽 복원 |
| Wall 추가 기능 | `DynamicWall` 클래스로 8초마다 안전한 빈 칸으로 이동하는 Wall 구현 |
| 충돌/표시 | Wall 충돌 처리, Gate `G`, Dynamic Wall `D` 및 색상 구분 |
| 검증 | Gate 원상복구, Dynamic Wall 이동, 충돌 판정, Gate 통과 몸통 보존 테스트 |

Stage4의 필수 Wall/Gate 동작과 4단계(2)의 Wall 변화 추가 사항을 구현했다. Score Board와 Mission Board도 현재 통합되어 Gate 사용 횟수와 스테이지 진행을 확인할 수 있다.

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

### 5.1 소스코드 품질 점검

- 모든 `.hpp`, `.cpp` 파일 첫 부분에 파일 역할을 설명하는 주석을 작성했다.
- `Gate`, `DynamicWall` 등 주요 클래스는 선언 파일과 구현 파일을 분리했다.
- 상태를 변경하지 않는 멤버 함수에는 `const`를 지정하고, 읽기 전용 객체 인자는 `const` 참조로 전달했다.
- `Map&`, `Snake&`, `std::mt19937&`처럼 실제 상태 변경이 필요한 인자만 비-const 참조로 유지했다.
- 실행 중 변경되지 않는 설정, Mission 표, 시간 간격, 색상 번호는 `const` 또는 `static constexpr`로 선언했다.
- mutable 전역변수와 전역 상수 변수를 사용하지 않고 상태를 각 클래스 멤버로 관리했다.
- 복잡한 Gate 진출 우선순위, Gate 통과 상태, Dynamic Wall 안전 이동 조건에는 동작 이유를 설명하는 주석을 작성했다.
- `-Wall -Wextra -Wconversion -Wshadow -pedantic` 경고 검사와 AddressSanitizer/UndefinedBehaviorSanitizer 검사를 수행했다.

Stage4 Gate 구현 후 다음 명령을 순서대로 실행해 통과를 확인했다.

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
- Game Over 후 `r`을 눌러 재시작하는 장면을 보여준다.
- 게임 시작 후 10초가 지나 Gate 쌍이 출현하는 장면을 보여준다.
- Snake가 Gate에 진입해 반대편 Gate 출구로 순간이동하는 장면을 보여준다.
- 가장자리 벽 Gate에서 안쪽 방향으로 진출하는 장면을 보여준다.
- 내부 벽 Gate가 직진 → 시계 → 반시계 → 반대 방향 우선순위를 따르는 장면을 보여준다.
- Snake 몸통이 입구 Gate를 벗어날 때까지 Gate가 유지되는 장면을 보여준다.
- Dynamic Wall `D`가 8초 후 다른 안전한 빈 칸으로 이동하는 장면을 보여준다.
- Dynamic Wall 충돌 및 Dynamic Wall이 Gate로 변한 뒤 다시 `D`로 복원되는 장면을 보여준다.
- `Gate`, `DynamicWall`, `Game`, `Map`, `Snake`, `Renderer`의 역할과 상호작용을 코드에서 설명한다.
- 개인 담당 범위가 4단계 Wall/Gate 및 Wall 변화 추가 기능임을 구체적으로 설명한다.

### 6.1 발표 영상 링크

- 팀장 전체 프로젝트 및 데모 영상: `[YouTube 링크 입력]`
- 개인 담당 부분 설명 영상: `[YouTube 링크 입력]`

## 7. 어려웠던 점과 개선점

맵 파일을 여러 크기의 스테이지로 관리하면서도 Snake 시작 위치와 충돌 판정이 안정적으로 동작하도록 검증하는 부분이 중요했다. 이를 위해 모든 스테이지 맵의 크기, 외곽 벽, 중앙 시작 구간을 테스트로 확인했다.

Stage4 Gate 구현에서는 가장자리와 내부 벽의 진출 규칙, Gate 생성 후보, 원래 벽 복원, Snake 몸통 통과 상태를 함께 관리해야 했다. 진출 방향과 Gate 상태는 `Gate` 클래스로 분리하고, 게임 진행에 따른 생성과 제거는 `Game`이 조정하도록 책임을 나눴다.

추가 기능은 단순한 색상 변경이 아니라 실제 경로가 변하는 Dynamic Wall로 설계했다. 이동 시 Snake, Item, Gate를 후보에서 제외해 불공정한 즉시 충돌이나 객체 덮어쓰기를 방지했다. Dynamic Wall이 Gate로 선택된 경우 원래 셀 종류를 저장해 통과 완료 후 `D`로 복원하도록 구현했다. 향후에는 이동 직전 시각 경고를 추가해 플레이어가 경로 변화를 더 쉽게 예측하도록 개선할 수 있다.
