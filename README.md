# 츠라메쵸키 (Tsuramechoki)

**100% 어셈블리어로 만드는 2D 횡스크롤 액션 RPG 제작 도구.**

츠라메쵸키는 RPG Maker처럼 코딩을 거의 하지 않고 게임 콘텐츠를 조립하는 전용 제작툴입니다. 목표 장르는 메이플스토리형 플랫폼 RPG와 던전앤파이터형 벨트스크롤 액션 RPG입니다.

## 절대 원칙

- 실행 코드의 소스는 **100% x86-64 Assembly**
- C, C++, C#, Rust, JavaScript, Python 등 다른 구현 언어를 섞지 않음
- C/C++ 런타임에 의존하지 않음
- Windows API를 어셈블리어에서 직접 호출
- 문서와 설정 파일을 제외한 프로그램 로직은 모두 `.asm` / `.inc`

## 현재 구현

### 에디터

- 네이티브 Win32 제작툴 창
- 좌측 맵/도구 패널, 중앙 캔버스, 우측 속성 패널
- 32px 그리드 / 16px 스냅
- 마우스 드래그로 횡스크롤 발판 생성
- 발판 선택, 방향키 이동, Delete 삭제
- 플레이어 시작점 배치
- 몬스터 배치
- NPC 배치
- 포탈 배치
- 엔티티 선택, 방향키 이동, Delete 삭제
- 마우스 휠로 긴 맵 가로 이동
- Home으로 편집 카메라 원점 복귀
- Ctrl+N 새 프로젝트
- Ctrl+S 저장
- Ctrl+O 불러오기
- F5 테스트 플레이 실행
- 종료 시 자동 저장
- `project.tsrp` 바이너리 프로젝트 포맷

### 테스트 런타임

- 프로젝트 파일 직접 로드
- A/D 또는 방향키 이동
- 중력
- Space 점프
- 일방향 발판 착지
- 플레이어 추적 카메라
- X 근접 공격
- 몬스터 3회 피격 사망
- 기본 몬스터 추적 AI
- 몬스터 접촉 피해
- 플레이어 HP / 피격 무적시간
- 사망/낙하 시 시작점 부활
- E로 NPC 상호작용
- E로 포탈 상호작용
- NPC 대화 패널
- 플레이어 시작점, 몬스터, NPC, 포탈의 런타임 표시

현재 그래픽은 제작 시스템 검증을 위한 GDI 플레이스홀더입니다. 스프라이트/애니메이션 계층은 다음 단계에서 붙입니다.

## 조작

에디터:

- `Q` 선택 도구
- `P` 발판 도구
- `1` 플레이어 시작점
- `2` 몬스터
- `3` NPC
- `4` 포탈
- `방향키` 선택 오브젝트 이동
- `Delete` 선택 삭제
- `마우스 휠` 맵 좌우 이동
- `Home` 카메라 원점
- `Ctrl+N` 새 프로젝트
- `Ctrl+S` 저장
- `Ctrl+O` 불러오기
- `F5` 테스트 실행

런타임:

- `A/D` 또는 `←/→` 이동
- `Space` 점프
- `X` 공격
- `E` 상호작용
- `Esc` 종료

## 빌드

필요 도구:

- NASM
- Microsoft Visual C++ Build Tools의 `link.exe`
- Windows SDK 라이브러리

Visual Studio Developer Command Prompt에서 저장소 루트를 기준으로:

    nasm -f win64 -Isrc/ src\editor.asm -o editor.obj
    link /entry:mainCRTStartup /subsystem:windows /machine:x64 editor.obj user32.lib gdi32.lib kernel32.lib /out:Tsuramechoki.exe

    nasm -f win64 -Isrc/ src\runtime.asm -o runtime.obj
    link /entry:mainCRTStartup /subsystem:windows /machine:x64 runtime.obj user32.lib gdi32.lib kernel32.lib /out:TsuramechokiRuntime.exe

`Tsuramechoki.exe`와 `TsuramechokiRuntime.exe`를 같은 폴더에서 실행합니다. F5를 누르면 에디터가 프로젝트를 저장한 뒤 런타임을 실행합니다.

## 소스

- `src/editor.asm` — 에디터
- `src/runtime.asm` — 테스트 게임 런타임
- `src/project.inc` — 공용 프로젝트 포맷 정의

## 다음 큰 단계

현재는 **플레이 가능한 제작툴 MVP**까지 연결된 상태입니다. 다음 핵심 개발은 다음과 같습니다.

1. PNG/스프라이트 자산 계층
2. 애니메이션 프레임 편집기
3. 공격/피격 판정 타임라인
4. 캐릭터/몬스터 데이터베이스
5. NPC 대화/퀘스트 이벤트 편집기
6. 다중 맵과 포탈 연결
7. 타일/배경/오브젝트 레이어
8. 실행 취소/다시 실행
9. 프로젝트별 게임 빌드
10. 던파형 Y축 이동 모드

## 프로젝트 철학

> 새 프로젝트를 만들면 이미 움직이고 공격할 수 있는 작은 게임이 열린다.

범용 엔진이 아니라 **2D 횡스크롤 액션 RPG 제작에 필요한 기능을 처음부터 제공하는 도구**를 지향합니다.
