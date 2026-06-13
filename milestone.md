# Snake Game 마일스톤 정리

이 문서는 현재 프로젝트에서 구현된 1단계, 2단계, 3단계 기능과 이후 마일스톤에서 구현해야 할 항목을 정리한 개발 가이드입니다.

## 현재 전체 상태

- 1단계 Map 표시, 2단계 Snake 이동, 3단계 Item 기능이 구현되어 있습니다.
- `maps/stage1.txt`부터 `maps/stage10.txt`까지 총 10개 스테이지 맵이 준비되어 있습니다.
- 현재 기본 실행 설정은 `GameConfig.stageLevel = 10`이며, `mapPath`가 비어 있으면 `maps/stage10.txt`를 로드합니다.
- 빌드 산출물은 `build/` 디렉터리와 루트 실행 파일 `snake`로 생성되며, `.gitignore`로 제외됩니다.
- Gate, Score Board, Mission Board, Stage 클리어/전환 로직과 Dynamic Wall 추가 기능이 구현되어 있습니다.

## 1단계: Map 표시

### 구현 완료

- `maps/stage1.txt`부터 `maps/stage10.txt`까지 스테이지별 숫자 맵을 읽어옵니다.
- 맵 파일은 공백으로 구분된 숫자 값으로 구성됩니다.
- `Map` 클래스가 맵 데이터를 `CellType` 2차원 배열로 관리합니다.
- 맵 크기가 최소 21x21인지 검사합니다.
- 모든 행의 길이가 같은 직사각형 맵인지 검사합니다.
- 잘못된 숫자 값이 들어오면 맵 로드 실패로 처리합니다.
- 맵 파일을 읽지 못하면 `loadFallbackMap()`으로 현재 스테이지 구간에 맞는 기본 맵을 생성합니다.
- 파일 로드 검증이 성공한 뒤에만 `Map`의 내부 grid를 교체합니다.
- Wall과 Immune Wall을 구분합니다.
- 화면 출력은 `Renderer` 클래스가 담당합니다.
- `ncurses`를 사용해 터미널 화면에 맵을 그립니다.
- 각 칸은 가로 2칸씩 사용해 터미널에서 더 보기 좋게 표시합니다.

### 관련 파일

- `src/Types.hpp`: `CellType`, `Direction`, `Position` 정의
- `src/Map.hpp`, `src/Map.cpp`: 맵 로드, 검증, 셀 조회
- `src/Renderer.hpp`, `src/Renderer.cpp`: 맵과 게임 상태 출력
- `maps/stage1.txt` ~ `maps/stage10.txt`: 스테이지별 맵 데이터
- `.gitignore`: 빌드 산출물 제외 설정

### 현재 셀 값

- `0`: 빈 칸
- `1`: Wall
- `2`: Immune Wall
- `3`: Snake Head
- `4`: Snake Body
- `5`: Growth Item
- `6`: Poison Item
- `7`: Gate

현재 Snake Head와 Snake Body는 맵 파일 값이 아니라 실제 `Snake` 객체의 위치를 기준으로 화면에 덮어 그립니다.

### 스테이지별 맵 크기

- 1단계 ~ 2단계: 27x27
- 3단계 ~ 4단계: 25x25
- 5단계 ~ 7단계: 23x23
- 8단계 ~ 10단계: 21x21

모든 맵은 최소 21x21 이상이며, 내부 벽 구조가 서로 다르게 구성되어 있습니다.
맵 크기는 `Game::currentMapSize()`의 fallback 크기와 테스트의 `expectedMapSize()` 기준과 맞춰져 있습니다.

## 2단계: Snake 이동

### 구현 완료

- `Snake` 클래스가 Snake의 몸 좌표와 현재 이동 방향을 관리합니다.
- Snake는 시작 위치 기준 길이 3으로 생성됩니다.
- 기본 시작 위치는 현재 맵의 중앙입니다.
- 기본 이동 방향은 오른쪽입니다.
- 방향키 입력으로 Snake의 이동 방향을 변경합니다.
- 같은 방향 입력은 그대로 허용합니다.
- 현재 방향의 정반대 입력은 실패로 처리합니다.
- 고정 Tick마다 Snake가 한 칸씩 이동합니다.
- 현재 설정 기준 기본 Tick은 500ms입니다.
- Stage Level에 따라 Tick을 줄일 수 있는 구조가 준비되어 있습니다. (level당 tick 45ms 감소)
- 최소 Tick은 180ms로 제한됩니다.
- 기본 `stageLevel`은 10이므로 기본 실행 Tick은 최소값 180ms까지 줄어든 상태입니다.
- 다음 위치가 Wall 또는 Immune Wall이면 게임 오버가 됩니다.
- 다음 위치가 자기 몸통이면 게임 오버가 됩니다.
- Snake가 이동할 때 머리를 새 위치에 추가하고 꼬리를 제거해 현재 길이를 유지합니다.
- 게임 오버 후에는 이동을 멈추고 `r` 입력으로 재시작하거나 `q` 입력으로 종료할 수 있습니다.

### 관련 파일

- `src/Snake.hpp`, `src/Snake.cpp`: 방향 전환, 이동, 몸통 충돌 판정
- `src/Game.hpp`, `src/Game.cpp`: 게임 루프, 입력 처리, Tick 제어, 게임 오버 처리
- `src/main.cpp`: 게임 실행 진입점
- `tests/test_logic.cpp`: 맵 로드와 Snake 이동 규칙 테스트
- `build/`: `make`와 `make test` 실행 시 생성되는 object/test 산출물 디렉터리

### 현재 조작법

- 방향키: 이동 방향 변경
- `r`: 게임 오버 후 재시작
- `q`: 게임 종료

## 3단계: Item

### 구현 완료

- `Food` 클래스가 Growth Item의 생성과 5초 재생성을 관리합니다.
- `Poison` 클래스가 Poison Item의 생성과 5초 재생성을 관리합니다.
- Growth Item은 화면에 `+`로 표시됩니다.
- Poison Item은 화면에 `-`로 표시됩니다.
- Growth Item을 획득하면 Snake 길이가 1 증가합니다.
- Poison Item을 획득하면 Snake 길이가 1 감소합니다.
- Poison Item 획득 후 Snake 길이가 3보다 작아지면 Game Over가 됩니다.
- 전체 Item 수는 최대 3개입니다.
- 현재 생성 정책은 Growth Item 1개 + Poison Item 1개 + Shield Item 1개입니다.
- Item은 Wall, Immune Wall, Snake 몸통, Gate 위치와 겹치지 않는 빈 칸에 생성됩니다.
- Item은 5초마다 기존 위치에서 사라지고 다른 빈 위치에 재생성됩니다.

### 관련 파일

- `src/Food.hpp`, `src/Food.cpp`: Growth Item 생성 개수와 재생성 주기 관리
- `src/Poison.hpp`, `src/Poison.cpp`: Poison Item 생성 개수와 재생성 주기 관리
- `src/Game.hpp`, `src/Game.cpp`: Item 초기 배치, 획득 후 재생성, 5초 재생성 호출
- `src/Map.hpp`, `src/Map.cpp`: Item을 배치할 수 있는 빈 칸 확인과 랜덤 배치
- `src/Snake.hpp`, `src/Snake.cpp`: Growth/Poison 획득 효과와 길이 3 미만 실패 결과 처리
- `src/Renderer.hpp`, `src/Renderer.cpp`: Growth Item `+`, Poison Item `-` 출력

## 현재 테스트 범위

`tests/test_logic.cpp`에서 다음 동작을 확인합니다.

- `maps/stage1.txt`부터 `maps/stage10.txt`까지 정상 로드되는지 확인
- 각 스테이지 맵이 설계된 크기와 일치하는지 확인
- 주요 Wall, Immune Wall, Empty 값이 올바른지 확인
- 정반대 방향 입력이 거부되는지 확인
- Snake가 정상적으로 한 칸 전진하는지 확인
- Snake가 벽과 충돌했을 때 `HitWall`을 반환하는지 확인
- Growth Item 획득 시 Snake 길이가 1 증가하는지 확인
- Poison Item 획득 시 Snake 길이가 1 감소하는지 확인
- Poison Item 획득 후 길이가 3보다 작아지면 `TooShort` 결과가 반환되는지 확인
- Item이 Snake 몸통 위치와 Gate 위치에 생성되지 않는지 확인
- Growth Item 1개 + Poison Item 1개 + Shield Item 1개로 전체 Item 수가 3개인지 확인
- Item 재생성 정책 호출 후 개수 정책이 유지되는지 확인
- `make test` 실행 시 테스트 바이너리는 `build/snake_tests`로 생성되는지 확인

테스트 실행 명령은 다음과 같습니다.

```bash
make test
```

## 빌드/정리 상태

- `make`: `build/*.o`를 만든 뒤 루트에 `snake` 실행 파일을 생성합니다.
- `make run`: `snake`를 빌드한 뒤 실행합니다.
- `make test`: `build/snake_tests`를 빌드하고 테스트를 실행합니다.
- `make clean`: `build/`, `snake`, 과거 호환용 `snake_tests`를 삭제합니다.
- 제출 시 `build/`, `snake`, `*.o`는 포함하지 않습니다.

## 4단계: Gate 구현

### 구현 완료

- `Gate` 클래스가 Gate 쌍의 생성, 진출 방향 계산, 활성 상태 추적을 담당합니다.
- ImmuneWall을 제외한 일반 Wall 셀 중 Snake가 점유하지 않은 임의의 두 위치에 Gate 쌍을 생성합니다.
- Gate는 한 번에 한 쌍만 활성 상태를 유지합니다.
- 게임 시작 후 10초가 지나면 Gate 쌍이 출현합니다.
- Snake 머리가 Gate 위치에 도달하면 반대편 Gate 출구로 순간이동합니다.
- Snake 몸통이 입구 Gate를 벗어나면 Gate를 제거하며, 이후 10초가 지나면 새 Gate 쌍이 다시 출현합니다.
- Snake가 Gate에 진입 중일 때는 Gate가 사라지지 않으며, 새 Gate도 생성되지 않습니다.
- 출구 Gate가 가장자리 Wall에 있으면 항상 맵 안쪽 방향으로 진출합니다.
  - 상단 벽 → 아래 방향
  - 하단 벽 → 위 방향
  - 좌측 벽 → 오른쪽 방향
  - 우측 벽 → 왼쪽 방향
- 출구 Gate가 내부 Wall에 있으면 진입 방향 → 시계방향 → 역시계방향 → 반대 방향 순으로 진출 가능한 방향을 결정합니다.
- Gate 출구 후보 방향이 모두 막혀 있으면 반대 방향으로 진출합니다.
- Gate 출구에서 자기 몸통과 충돌하면 Game Over로 처리합니다.
- Gate 사용 횟수를 `gateUseCount_`로 추적합니다.
- Item 배치 후보에서 Gate 위치를 제외합니다.
- Snake 몸통이 입구 Gate를 완전히 벗어날 때까지 Gate 쌍을 유지합니다.
- Dynamic Wall은 8초마다 Snake, Item, Gate를 피해 내부 빈 칸으로 이동합니다.
- Dynamic Wall이 Gate가 된 경우 통과 완료 후 원래 Dynamic Wall로 복원합니다.

### 관련 파일

- `src/Gate.hpp`, `src/Gate.cpp`: Gate 생성, 출구 방향 계산, 활성 상태 관리
- `src/DynamicWall.hpp`, `src/DynamicWall.cpp`: 이동 Wall 초기화, 주기 확인, 안전 재배치
- `src/Game.hpp`, `src/Game.cpp`: Gate 생성 대기, 진입 감지, 순간이동, 통과 후 제거
- `src/Snake.hpp`, `src/Snake.cpp`: `teleportHead()` — Gate 통과 시 머리 위치와 방향 갱신
- `src/Renderer.hpp`, `src/Renderer.cpp`: Gate 셀을 `G`(파란색)로 표시

## 5단계: Score, Mission, Stage 클리어와 다음 Stage 전환

### 구현 완료

- 우측에 게임 점수를 표시하는 화면을 구성한다.
- Score Board에 현재 길이 / 최대 길이, Growth Item 수, Poison Item 수, Gate 사용 횟수, 게임 시간을 표시한다.
- Mission을 표시하고 달성 여부를 갱신한다.
- Mission 달성 시 다음 스테이지로 진행하거나 종료할 수 있다.
- 10 스테이지를 클리어하면 10 스테이지를 다시 하거나 종료할 수 있다.

### 관련 파일

- `src/Game.hpp`, `src/Game.cpp`: 미션 처리, 스테이지 클리어 처리
- `src/Snake.hpp`, `src/Snake.cpp`: 최대 길이, 게임 시간
- `src/Renderer.hpp`, `src/Renderer.cpp`: 스코어보드, 미션 표시
