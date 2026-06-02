# Snake 프로젝트 - 팀원 공유용 ( 제출하지 마세용 )

## 과제 개요

- C++ 프로그래밍 언어와 `ncurses` 라이브러리를 사용해 Snake Game을 구현한다.
- 팀 프로젝트이며 팀원은 같은 반 학생 기준 최대 4명이다.
- 최종 제출물은 프로젝트 보고서, 프로그램 소스코드, Makefile이다.
- 제출은 e-Campus에 팀장 1명만 수행한다.
- 소스 코드와 보고서를 하나의 zip 파일로 압축해 제출한다.
- 실행 파일은 제출하지 않는다.
- 제출 파일명 형식은 `x조팀장성명.zip`이다. 예: `1조홍길동.zip`
- 프로젝트 진행 및 실행 데모 결과를 동영상으로 제작한다.
- 팀장은 전체 프로젝트 진행 및 결과물 데모 영상을 제작한다.
- 조원은 개인별 담당 부분 설명 영상을 각각 제작한다.
- 팀장과 조원 모두 영상을 YouTube에 업로드하고, 링크를 최종 보고서 2.2.5절 및 조편성 스프레드시트에 입력한다.
- 프로젝트 기간은 6월 14일 일요일 24시까지다.
- 평가 시점에 소스코드가 열람 가능하고 동작 가능해야 한다.
- 프로그램 평가는 동작 여부가 기준이며, 보고서 평가는 항목별 문서 작성, 표현, 합리성, 프로젝트 이해도, 개인별 역할 평가를 포함한다.

## 기본 게임 구성

Snake Game 화면은 다음 요소로 구성한다.

- Snake Head
- Snake Body
- Wall
- Immune Wall
- Growth Item
- Poison Item
- Gate A / Gate B
- Score Board
- Mission Board

Score Board에는 다음 값을 표시한다.

- `B`: 현재 길이 / 게임 중 최대 길이
- `+`: 획득한 Growth Item 수
- `-`: 획득한 Poison Item 수
- `G`: Gate 사용 횟수
- 게임 시간: 초 단위

Mission Board에는 각 항목별 목표치와 달성 여부를 표시한다.

- `B`: 목표 길이 달성 여부
- `+`: Growth Item 목표 획득 수 달성 여부
- `-`: Poison Item 목표 획득 수 달성 여부
- `G`: Gate 목표 사용 횟수 달성 여부

## Game Rule #1: Snake 이동

- Snake는 Head 방향으로 일정 시간마다 이동한다.
- Tick 예시는 0.5초 또는 1초이며, 팀에서 직접 정한다.
- 방향키 입력은 직접 정한다.
- 현재 진행 방향과 같은 방향키 입력은 무시한다.
- 현재 진행 방향의 반대 방향, 즉 Tail 방향으로 이동하려 하면 실패 처리한다.
- Snake는 자신의 Body를 통과할 수 없다.
- Snake는 Wall을 통과할 수 없다.
- Snake Head가 자신의 Body 또는 Wall과 충돌하면 Game Over다.

## Game Rule #2: Growth Item / Poison Item

- Snake가 이동 방향에 놓인 Item을 만나면 해당 Item을 획득한다.
- Growth Item은 화면에 `+`로 표시한다.
- Growth Item을 획득하면 Snake의 몸 길이가 1 증가한다.
- Growth Item은 최대 2개까지 생성한다.
- Poison Item은 화면에 `-`로 표시한다.
- Poison Item을 획득하면 Snake의 몸 길이가 1 감소한다.
- Poison Item으로 몸 길이가 3보다 작아지면 Game Over로 처리한다.
- Poison Item은 최대 1개까지 생성한다.
- 전체 Item 수는 최대 3개로 제한한다. 현재 구현은 Growth Item 2개 + Poison Item 1개 구조다.
- Growth Item과 Poison Item은 Snake Body가 없는 임의의 빈 위치에 출현한다.
- Item은 Wall, Immune Wall, Snake 몸통, Gate 위치와 겹치지 않는다.
- Item은 출현 후 5초가 지나면 기존 위치에서 사라지고 다른 빈 위치에 다시 나타난다.
- Map Data에서 Growth Item과 Poison Item은 서로 다른 값으로 구분한다. 예: Growth Item `5`, Poison Item `6`
- 화면에서는 색상과 기호로 두 Item을 구분한다.

## Game Rule #3: Gate 기본 규칙

- Gate는 두 개가 한 쌍이다.
- Gate 두 개는 서로 겹치지 않는다.
- Gate는 임의의 Wall 위치에 나타난다.
- Gate는 한 번에 한 쌍만 나타난다.
- Snake가 한 Gate로 진입하면 다른 Gate로 진출한다.
- Snake가 Gate에 진입 중일 때는 Gate가 사라지지 않는다.
- Snake가 Gate에 진입 중일 때는 다른 위치에 새 Gate가 나타나지 않는다.

## Game Rule #4: Gate 진출 방향

Gate가 가장자리 Wall에 있을 때는 항상 Map 안쪽 방향으로 진출한다.

- 상단 벽 Gate: 아래 방향
- 하단 벽 Gate: 위 방향
- 좌측 벽 Gate: 오른쪽 방향
- 우측 벽 Gate: 왼쪽 방향

Gate가 Map 가운데 Wall에 있을 때는 다음 우선순위로 진출 방향을 정한다.

1. 진입 방향과 일치하는 방향
2. 진입 방향의 시계방향
3. 진입 방향의 반시계방향
4. 진입 방향과 반대 방향

진출 후보 방향이 Wall, Immune Wall, Snake Body 등으로 막혀 있으면 다음 우선순위 방향을 선택한다.

## Game Rule #5: Wall / Immune Wall

- Wall은 Gate로 변할 수 있다.
- Immune Wall은 Gate로 변할 수 없다.
- 모든 Wall은 Snake가 통과할 수 없다.
- 모든 Wall은 Snake Head와 충돌하면 실패 처리한다.
- Gate 출현 조건은 팀에서 정한다.
- 예시 조건: 게임 시작 후 일정 시간 뒤 출현
- 예시 조건: Snake의 최대 길이가 10을 초과하면 출현

## Game Rule #6: 점수와 Mission

- 게임 중 Snake의 최대 길이를 계산한다.
- 현재 길이와 최대 길이를 `B: 현재 길이 / 최대 길이` 형식으로 표시한다.
- 게임 중 획득한 Growth Item 수를 계산한다.
- 게임 중 획득한 Poison Item 수를 계산한다.
- Gate 사용 횟수를 계산한다.
- 게임 시간을 초 단위로 계산한다.
- 게임 방법은 주어진 Mission을 달성하는 것이다.
- Mission의 각 점수 항목 목표치에 도달하면 목표 달성 여부를 표시한다.
- Mission을 달성하면 다음 Map으로 진행한다.
- Stage는 최소 4개로 구성한다.
- 각 Stage의 Map은 서로 달라야 한다.
- Mission 값은 Map 정의에 고정값으로 넣거나, 매 게임마다 임의 값으로 생성할 수 있다.

## 구현 마일스톤

### 1단계: Map 표시

- `ncurses` 함수로 2차원 배열 Snake Map을 게임 화면에 표시한다.
- Map은 최소 21x21 크기다.
- Map 구성 요소를 정한다.
- Wall과 Immune Wall을 반드시 구분한다.
- 예시 값:
  - Wall: `1`
  - Immune Wall: `2`
  - Snake Head: `3`
  - Snake Body: `4`
- Map Data 처리 방법, Import/Export 여부를 설계한다.
- Map과 Snake, Item 등 다른 객체의 상호작용 방식을 설계한다.

### 2단계: Snake 이동

- 1단계 Map 위에 Snake를 표시한다.
- 방향키 입력을 받아 Snake가 움직이도록 한다.
- Game Rule #1을 준수한다.
- 키 입력 처리 방식을 설계한다.
- Tick 변화 주기를 설계한다.
- Tick 관련 추가 아이디어를 제안하고 구현한다.

### 3단계: Item 구현

- 2단계 프로그램에서 Growth Item과 Poison Item이 Map 위에 출현하도록 수정한다.
- Game Rule #2를 준수한다.
- Map 배열에서 Growth Item과 Poison Item 값을 구분한다.
- 화면에서 색상 또는 기호로 Item 종류를 구분한다.
- Snake와 Item의 상호작용을 처리한다.
- 추가 Item 아이디어를 제안하고 구현한다.

#### 현재 구현 완료 내용

- `src/Food.hpp`, `src/Food.cpp`를 추가해 Growth Item의 생성과 5초 재생성 정책을 분리했다.
- `src/Poison.hpp`, `src/Poison.cpp`를 추가해 Poison Item의 생성과 5초 재생성 정책을 분리했다.
- `Food`는 Growth Item을 최대 2개까지 유지한다.
- `Poison`은 Poison Item을 최대 1개까지 유지한다.
- `Game`은 게임 시작 시 Item을 초기 배치하고, 매 프레임 5초 재생성 조건을 확인한다.
- `Map`은 `isEmpty()` 기준으로 배치 가능한 빈 칸만 후보로 사용한다.
- `Map::placeRandomItem()`은 Snake가 차지한 좌표를 제외하고 Item을 배치한다.
- `Snake::move()`는 Growth Item 획득 시 길이를 1 증가시키고, Poison Item 획득 시 길이를 1 감소시킨다.
- Poison Item 획득 후 Snake 길이가 3보다 작아지면 `TooShort` 결과를 반환하고, `Game`이 이를 Game Over로 처리한다.
- `Renderer`는 Growth Item을 `+`, Poison Item을 `-`로 표시한다.

### 4단계: Gate 구현

- 3단계 프로그램에서 Wall의 임의 위치에 한 쌍의 Gate가 출현하도록 수정한다.
- Snake가 Gate를 통과할 수 있도록 구현한다.
- Game Rule #3, #4, #5를 준수한다.
- Wall, Immune Wall, Gate를 Map 배열에서 구분한다.
- 예시 값:
  - Gate: `7`
- 화면에서 Gate가 Wall과 구분되도록 표시한다.
- Snake와 Wall의 상호작용을 처리한다.
- Snake와 Gate의 상호작용을 처리한다.
- Wall에 대한 추가 동작 아이디어를 제안하고 구현한다.

#### 현재 구현 완료 내용

- `src/Gate.hpp`, `src/Gate.cpp`를 추가해 Gate 쌍의 생성, 진출 방향 계산, 활성 상태 추적을 분리했다.
- ImmuneWall을 제외한 일반 Wall 셀 중 Snake가 점유하지 않은 임의의 두 위치에 Gate 쌍을 생성한다.
- Gate는 한 번에 한 쌍만 활성 상태를 유지한다.
- 게임 시작 후 10초가 지나면 Gate 쌍이 출현하고, 통과 후 10초 후에 새 Gate 쌍이 다시 출현한다.
- Snake 머리가 Gate 위치에 도달하면 `Snake::teleportHead()`로 반대편 Gate 출구로 순간이동시킨다.
- 출구 Gate가 가장자리 Wall에 있으면 항상 맵 안쪽 방향(상→아래, 하→위, 좌→오른쪽, 우→왼쪽)으로 진출한다.
- 출구 Gate가 내부 Wall에 있으면 진입 방향 → 시계방향 → 역시계방향 → 반대 방향 순으로 진출 가능한 방향을 결정한다.
- Gate 통과 후 자기 몸통과 충돌하면 Game Over로 처리한다.
- Gate 사용 횟수를 `Game::gateUseCount_`로 추적한다.
- `Renderer`는 Gate 셀을 `G` 기호와 파란색으로 표시한다.
- Item 배치 후보에서 Gate 위치를 제외한다.

### 5단계: Score Board / Mission / Stage

- 4단계 프로그램에서 우측에 게임 점수를 표시하는 화면을 구성한다.
- Game Rule #6을 준수한다.
- Score Board에 현재 길이, 최대 길이, Growth Item 수, Poison Item 수, Gate 사용 횟수, 시간을 표시한다.
- Mission을 표시하고 달성 여부를 갱신한다.
- Mission 달성 시 다음 Map으로 진행한다.
- 최소 4개 Stage를 구현한다.
- 각 Stage의 Map은 서로 달라야 한다.
- Score Board에 대한 추가 상호작용 또는 표시 요소를 제안하고 구현한다.

## 보고서에 반드시 포함할 내용

- 구현된 단계까지의 구현 내용
- 소스 코드 설명
- 각 단계별 실행 화면
- Map 설계 사항
- Snake 이동 설계 사항
- Snake와 Item 상호작용 설계 사항
- Snake와 Wall/Gate 상호작용 설계 사항
- Score Board 설계 사항
- 추가한 기능이나 요소에 대한 설명
- 팀원별 역할
- 팀장 및 팀원별 YouTube 영상 링크

## 개발 환경 및 빌드

- Ubuntu 기준 ncurses 설치:

```bash
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev
```

- 컴파일 예시:

```bash
g++ -o snake snake.cpp rule.cpp -lncurses
```

- Makefile을 사용하는 경우 Makefile을 제출물에 포함한다.
- 현재 로컬 프로젝트는 `make`, `make run`, `make clean` 구조를 갖고 있으며 `-lncurses`로 링크한다.
- Stage3 Item 정리 후 `make clean`, `make`, `make test`가 통과한 상태다.
