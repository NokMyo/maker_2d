# 르엘라 프렐류드 (Leella Prelude)

**100% 어셈블리어로 만드는 2D 횡스크롤 액션 RPG 제작 도구.**

르엘라 프렐류드는 RPG Maker처럼 코딩을 거의 하지 않고 게임 콘텐츠를 조립할 수 있게 하는 전용 제작툴입니다. 목표 장르는 메이플스토리형 플랫폼 RPG와 던전앤파이터형 벨트스크롤 액션 RPG입니다.

## 절대 원칙

- 실행 코드의 소스는 **100% x86-64 Assembly**
- C, C++, C#, Rust, JavaScript, Python 등 다른 구현 언어를 섞지 않음
- C/C++ 런타임에 의존하지 않음
- Windows API를 어셈블리어에서 직접 호출
- 문서와 설정 파일을 제외한 프로그램 로직은 모두 `.asm`

## 현재 구현

첫 번째 에디터 프로토타입은 Windows x64용입니다.

- 네이티브 Win32 창 생성
- 제작툴형 3패널 UI
- 중앙 맵 캔버스
- 32px 편집 그리드
- 16px 스냅
- 마우스 드래그로 발판 생성
- 생성한 발판 즉시 렌더링
- 맵/도구/속성 패널 뼈대

## 빌드

필요 도구:

- NASM
- Microsoft Visual C++ Build Tools의 `link.exe`
- Windows SDK 라이브러리

Developer Command Prompt for VS에서:

    nasm -f win64 src\prelude.asm -o prelude.obj
    link /entry:mainCRTStartup /subsystem:windows /machine:x64 prelude.obj user32.lib gdi32.lib kernel32.lib /out:LeellaPrelude.exe

## 다음 구현 순서

1. 발판 선택/이동/삭제
2. 맵 저장/불러오기
3. 플레이어 시작점 배치
4. NPC/몬스터/포탈 엔티티
5. 스프라이트 및 애니메이션 편집기
6. 공격 판정/피격 판정 타임라인
7. 실제 테스트 플레이 런타임
8. 퀘스트/이벤트 명령 시스템
9. 메이플형 프로젝트 템플릿
10. 던파형 벨트스크롤 프로젝트 모드

## 프로젝트 철학

> 새 프로젝트를 만들면 이미 움직이고 공격할 수 있는 작은 게임이 열린다.

범용 엔진이 아니라 **2D 횡스크롤 액션 RPG 제작에 필요한 기능을 처음부터 제공하는 도구**를 지향합니다.
