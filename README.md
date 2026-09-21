# 츠라메쵸키 (Tsuramechoki)

**100% x86-64 Assembly로 작성하는 2D 횡스크롤 액션 RPG 제작 도구.**

츠라메쵸키는 범용 엔진이 아니라 메이플스토리형 플랫폼 액션 RPG를 코딩 없이 조립하는 전용 제작툴을 목표로 합니다. 에디터와 게임 런타임 모두 NASM x86-64 Assembly이며 C/C++ 런타임을 사용하지 않습니다.

## 현재 구현

### 프로젝트
- TSRP v2 프로젝트 파일 (`project.tsrp`)
- 프로젝트 이름 메타데이터
- 해상도 설정
- 시작 맵 설정
- 1~3개 저장 슬롯 설정
- F5 즉시 테스트
- F9 독립 게임 폴더 빌드
- `Build/Game.exe + Build/project.tsrp + Build/Assets`

### 맵 에디터
- 최대 16개 맵
- Ctrl+M 맵 추가
- PgUp/PgDn 맵 전환
- 발판 생성/선택/이동/삭제
- 플레이어 시작점
- 몬스터 / NPC / 포탈
- 사다리 / 로프
- 체크포인트
- 장식 오브젝트
- 이벤트 트리거
- 맵별 배경색과 기본 스폰 좌표
- 월드 X축 카메라 이동

### 플레이어 / 전투
- 이동속도
- 점프력
- 최대 HP / MP
- 기본 공격력
- 중력 / 일방향 발판 충돌
- 스킬 MP 소모 / 쿨타임 / 사거리 / 피해량
- 공격 애니메이션 연결
- 장비 공격력 / 방어력
- 피격 무적시간 / 사망 / 체크포인트 부활

### 데이터베이스
- 애니메이션
- 스킬
- 몬스터
- 아이템/장비
- 퀘스트
- 변수/플래그
- 이벤트
- BMP 스프라이트 시트

F6으로 데이터베이스 종류를 순환하고 Tab으로 필드, +/-로 값을 변경하며 Insert로 새 레코드를 추가합니다.

### 스프라이트 / 애니메이션 / 히트박스
- F7 BMP 스프라이트 시트 가져오기
- 프로젝트 `Assets` 폴더로 자동 복사
- 애니메이션별 프레임 수 / 프레임 시간
- 공격 활성 프레임 시작/종료
- 공격 판정 위치/크기 데이터
- 루프 여부
- 애니메이션 ↔ 스프라이트 연결
- 런타임에서 가로형 스프라이트 시트를 프레임 단위로 재생

현재 네이티브 GDI 렌더러는 BMP를 사용합니다. PNG/WIC 자산 계층은 이후 단계입니다.

### 몬스터
- 최대 HP
- 공격력
- 이동속도 데이터
- 경험치 보상
- 드랍 아이템 / 수량
- AI 타입
- 애니메이션 연결
- 기본 추적 AI / 접촉 공격
- 사망 시 경험치·아이템·퀘스트 진행 반영

### 아이템 / 장비 / UI
- 소비 / 무기 / 방어구 / 재료 / 퀘스트 아이템
- 회복 아이템
- 무기 공격력 보너스
- 방어구 방어력
- 인벤토리 수량
- 장비 적용
- HP/MP, 레벨, 경험치, 돈 HUD
- 인벤토리 / 스킬 / 퀘스트 / 스탯 패널

### NPC / 퀘스트 / 이벤트
- 처치 / 대화 / 수집 / 지역 도달 퀘스트 데이터
- NPC에 퀘스트 ID 연결
- 이벤트 트리거에 이벤트 ID 연결
- 이벤트 명령:
  - 대화
  - 변수/플래그 변경
  - 아이템 지급
  - 돈 지급
  - 맵 이동
  - 퀘스트 시작
  - 변수 조건 분기
  - 2지선다 선택지
- 퀘스트·이벤트 이름을 런타임 대화창에 표시

### 저장
- TSAV0001 런타임 세이브
- 최대 3개 슬롯
- 맵/좌표/HP/MP/경험치/레벨/돈
- 퀘스트 상태와 진행도
- 변수
- 인벤토리

## 주요 조작

에디터:
- Q 선택 / P 발판
- 1 플레이어, 2 몬스터, 3 NPC, 4 포탈
- 5 사다리, 6 로프, 7 체크포인트, 8 장식, 9 이벤트
- 방향키 선택 이동 / Delete 삭제
- 휠 맵 가로 이동 / Home 카메라 초기화
- Ctrl+N 새 프로젝트 / Ctrl+S 저장 / Ctrl+O 열기
- Ctrl+M 새 맵 / PgUp·PgDn 맵 전환
- F1 프로젝트 설정 / F2 플레이어 설정
- F6 DB 순환 / F7 BMP 스프라이트 가져오기
- Tab 필드 이동 / +/- 값 변경 / Insert 레코드 추가
- F5 테스트 / F9 게임 빌드

런타임:
- A/D 또는 ←/→ 이동
- Space 점프
- 사다리/로프 근처 ↑/↓
- X 공격 / E 상호작용
- C 소비 아이템
- I 인벤토리 / K 스킬 / J 퀘스트 / T 스탯
- V 첫 보유 장비 장착
- F2 저장 / F3 불러오기 / F4 저장 슬롯 변경
- 선택지에서 1/2
- Esc 종료

## 빌드

Visual Studio Developer Command Prompt + NASM:

    nasm -f win64 -Isrc/ src\editor.asm -o editor.obj
    link /entry:mainCRTStartup /subsystem:windows /machine:x64 /LARGEADDRESSAWARE:NO editor.obj user32.lib gdi32.lib kernel32.lib comdlg32.lib /out:Tsuramechoki.exe

    nasm -f win64 -Isrc/ src\runtime.asm -o runtime.obj
    link /entry:mainCRTStartup /subsystem:windows /machine:x64 /LARGEADDRESSAWARE:NO runtime.obj user32.lib gdi32.lib kernel32.lib /out:TsuramechokiRuntime.exe

## 코드 구조
- `src/editor.asm` — Win32 에디터
- `src/editor_systems.inc` — 프로젝트/맵/DB/스프라이트/내보내기
- `src/runtime.asm` — 게임 런타임/물리/전투
- `src/runtime_systems.inc` — RPG/세이브/퀘스트/이벤트/자산 시스템
- `src/project.inc` — TSRP 포맷
- `src/data.inc` — 공용 RPG 데이터 모델

프로그램 로직은 계속 **Assembly only**를 유지합니다.
