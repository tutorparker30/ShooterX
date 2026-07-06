# ShooterX - 언리얼 엔진 멀티플레이어 게임 프로젝트 분석

> 최종 업데이트: 2026-06-30  
> 엔진 버전: Unreal Engine 5.7  
> 브랜치: `1704` (main: `0203`)

---

## 목차

1. [프로젝트 개요](#1-프로젝트-개요)
2. [빌드 설정](#2-빌드-설정)
3. [프로젝트 구조](#3-프로젝트-구조)
4. [클래스 목록 및 역할](#4-클래스-목록-및-역할)
5. [클래스 상속 관계](#5-클래스-상속-관계)
6. [게임플레이 아키텍처](#6-게임플레이-아키텍처)
7. [멀티플레이어 / 네트워킹](#7-멀티플레이어--네트워킹)
8. [컴포넌트 구조](#8-컴포넌트-구조)
9. [애니메이션 시스템](#9-애니메이션-시스템)
10. [무기 / 전투 시스템](#10-무기--전투-시스템)
11. [AI 시스템](#11-ai-시스템)
12. [UI 시스템](#12-ui-시스템)
13. [Config 설정](#13-config-설정)
14. [Content 폴더 구조](#14-content-폴더-구조)
15. [플러그인 및 모듈 의존성](#15-플러그인-및-모듈-의존성)
16. [Git 커밋 히스토리](#16-git-커밋-히스토리)
17. [기능 완성도 요약](#17-기능-완성도-요약)
18. [협업 방식 (Claude 작업 규칙)](#18-협업-방식-claude-작업-규칙)
19. [TODO: 세션 이름 / 다음 레벨 정보 전달 흐름 개선](#19-todo-세션-이름--다음-레벨-정보-전달-흐름-개선)
20. [TODO: Online Subsystem Steam 적용 (진행 중)](#20-todo-online-subsystem-steam-적용-진행-중)

---

## 1. 프로젝트 개요

ShooterX는 언리얼 엔진 5.7 기반의 멀티플레이어 3인칭 슈터 게임 프로젝트다.  
네트워킹, 게임플레이 프레임워크, AI, UI, 애니메이션 동기화 등 AAA급 게임에 필요한 핵심 기술을 실전 구현하는 데 초점을 맞춘 교육/실습 프로젝트다.

**게임 흐름:**
```
Title Level → Lobby Level → L_Expanse Level(게임) → Lobby 복귀
```

**핵심 특징:**
- 최대 4인 멀티플레이 (ListenServer / DedicatedServer 지원)
- 근접 콤보 공격 + 원거리 총기 사격
- 행동 트리(BehaviorTree) 기반 NPC AI
- Unreal Replication을 사용한 상태 동기화
- EnhancedInput 입력 시스템

---

## 2. 빌드 설정

### 2.1 ShooterX.uproject

```json
{
  "EngineAssociation": "5.7",
  "Modules": [
    { "Name": "ShooterXPlayGround", "Type": "Runtime", "LoadingPhase": "PreDefault" },
    { "Name": "ShooterX",           "Type": "Runtime", "LoadingPhase": "Default",
      "AdditionalDependencies": ["Engine","AIModule","UMG","ShooterXPlayGround"] }
  ],
  "Plugins": [
    { "Name": "ModelingToolsEditorMode", "Enabled": true, "TargetAllowList": ["Editor"] },
    { "Name": "EnhancedInput",           "Enabled": true }
  ]
}
```

### 2.2 Target 파일

| 파일 | 타겟 타입 |
|------|-----------|
| ShooterX.Target.cs | Game |
| ShooterXEditor.Target.cs | Editor |
| ShooterXClient.Target.cs | Client |
| ShooterXServer.Target.cs | Server |

### 2.3 모듈 의존성

**ShooterX.Build.cs (Public):**
```
Core, CoreUObject, Engine, InputCore, EnhancedInput,
Json, JsonUtilities, Niagara, AIModule, NavigationSystem,
GameplayTasks, UMG, Slate, SlateCore
```
**ShooterX.Build.cs (Private):** `ShooterXPlayGround`

**ShooterXPlayGround.Build.cs (Public):** `Core, CoreUObject, Engine, InputCore`

---

## 3. 프로젝트 구조

```
ShooterX/
├── Source/
│   ├── ShooterX/                  # 메인 게임 모듈
│   │   ├── Animation/             # AnimInstance, AnimNotify
│   │   ├── Character/             # 플레이어/NPC 캐릭터
│   │   ├── AI/                    # AIController, BT Task/Decorator/Service
│   │   ├── Component/             # StatusComponent, PickupComponent, HPTextWidgetComponent
│   │   ├── Game/                  # GameMode, GameState, PlayerController, PlayerState, GameInstance
│   │   ├── Gimmick/               # LandMine, HealthPack
│   │   ├── Input/                 # InputConfig DataAsset
│   │   ├── UI/                    # HUD, Widget 클래스
│   │   ├── Weapon/                # Weapon 클래스
│   │   └── ShooterXFunctionLibrary # 유틸 함수 라이브러리
│   └── ShooterXPlayGround/        # 보조 모듈 (머티리얼 관리)
├── Content/
│   ├── ShooterX/                  # 게임 에셋
│   ├── LyraResource/              # 라이라 프로젝트 리소스 (캐릭터, 무기 메시, 이펙트)
│   └── ZombieAnimationPack/       # NPC 애니메이션 팩
├── Config/                        # 프로젝트 설정
├── Binaries/, Intermediate/, Saved/, DerivedDataCache/
└── ShooterX.uproject
```

---

## 4. 클래스 목록 및 역할

### 4.1 캐릭터 (Character/)

#### ASXCharacterBase
- **상속:** ACharacter
- **역할:** 모든 캐릭터의 기본 클래스. HP 관리 컴포넌트 보유, 근접 공격 및 콤보 시스템 담당.
- **주요 컴포넌트:** `USXStatusComponent`
- **핵심 함수:**
  - `HandleOnCheckHit()` — Sphere Sweep으로 근접 공격 판정 (사거리 50cm, 반경 20cm)
  - `HandleOnCheckInputAttack()` — 콤보 입력 체크
  - `BeginAttack() / EndAttack()` — 공격 상태 관리
- **최대 콤보 수:** 3

#### ASXPlayerCharacter
- **상속:** ASXCharacterBase
- **역할:** 플레이어가 조종하는 캐릭터. 입력 처리, 카메라, 총기 사격, 착지뢰 스폰.
- **주요 컴포넌트:** `USpringArmComponent`, `UCameraComponent`, `UNiagaraComponent(킬 이펙트)`
- **복제 속성:** `CurrentAimPitch` (조준 피치각)
- **입력 액션:** Move, Look, Jump, AttackMelee, AttackRanged, Menu, SpawnLandMine
- **Server RPC:** `ServerRPCFire`, `ServerRPCSpawnLandMine`, `ServerRPCMeleeAttack`, `ServerRPCUpdateAimValue`
- **Multicast RPC:** `MulticastRPCFire`, `MulticastRPCMeleeAttack`
- **머티리얼:** PlayerColor에 따라 동적 머티리얼 인스턴스 적용 (Black/Blue/Red/White)

#### ASXNonPlayerCharacter
- **상속:** ASXCharacterBase
- **역할:** AI가 제어하는 NPC. 체력 텍스트 위젯 3D 표시. 사망 시 킬한 플레이어의 킬 카운트 증가.
- **주요 컴포넌트:** `USXHPTextWidgetComponent`
- **AI 컨트롤러:** `ASXAIController` 자동 소유

#### ASXPlayerPawn
- **상속:** APawn
- **역할:** Lobby 화면에서 캐릭터 프리뷰 표시용 Pawn.
- **컴포넌트:** `UFloatingPawnMovement`, `USkeletalMeshComponent`

#### ASXLobbyPlayerCharacter
- **상속:** ASXPlayerPawn
- **역할:** Lobby에서 실제 플레이어가 제어하는 캐릭터.

---

### 4.2 게임플레이 프레임워크 (Game/)

#### ASXGameModeBase
- **상속:** AGameModeBase
- **역할:** 메인 게임 모드. 게임 상태 전환, 플레이어 로그인/로그아웃 관리.
- **상수:**
  - `MinimumPlayerCountForPlaying = 2`
  - 대기 시간: 15초
  - 종료 대기: 15초
  - 타이머 주기: 0.1초
- **상태 전환:**
  - `Waiting → Playing` : 2명 이상 + 15초 경과
  - `Playing → Ending` : 생존자 1명 이하
  - `Ending → Lobby` : 15초 후 ServerTravel

#### ASXGameStateBase
- **상속:** AGameStateBase
- **복제 속성:** `AlivePlayerControllerCount`, `MatchState`
- **MatchState enum:**
  ```cpp
  enum EMatchState { None, Waiting, Playing, Ending, End };
  ```

#### ASXPlayerState
- **상속:** APlayerState
- **속성:** `MaxKillCount(99)`, `CurrentKillCount`, `PlayerColor(Black/Blue/Red/White)`
- **기능:** `PlayerInfo.txt`에서 JSON으로 플레이어 정보 로드. 킬 카운트 변경 시 델리게이트 브로드캐스트.

#### ASXGM_Lobby
- **상속:** AGameModeBase
- **역할:** 로비 게임 모드. 2명 이상이 Ready 시 5초 후 L_Expanse로 ServerTravel.

#### ASXGS_Lobby
- **상속:** AGameStateBase
- **기능:** 플레이어 로그인/로그아웃 메시지를 `MulticastRPC`로 전체 브로드캐스트.

#### USXGameInstance
- **상속:** UGameInstance
- **데이터 테이블:** `MaxHPDataTable` (레벨별 최대 HP 관리)
- **구조체:** `FSXMaxHPTableRow`

---

### 4.3 플레이어 컨트롤러 (Game/)

#### ASXPlayerController
- **상속:** APlayerController
- **역할:** HUD 생성/바인딩, 게임 내 메뉴 토글, 게임 결과 UI 표시.
- **복제 속성:** `NotificationText` (게임 상태 메시지)
- **Client RPC:**
  - `ClientRPCShowGameResultWidget(InRanking)` — 결과 화면 표시
  - `ClientRPCReturnToTitle()` — 타이틀 레벨 복귀

#### ASXAIController
- **상속:** AAIController
- **역할:** NPC의 BehaviorTree 실행 및 Blackboard 관리.
- **순찰 반경:** `PatrolRadius = 500cm`
- **Blackboard 키:** `StartPatrolPosition`, `EndPatrolPosition`, `TargetCharacter`
- **함수:** `BeginAI() / EndAI()`

#### ASXUIPlayerControllerBase
- **상속:** APlayerController
- **역할:** BeginPlay에서 UIWidgetClass를 뷰포트에 추가하는 기본 UI 컨트롤러.
- **파생 클래스:** `ASXUIPC_Loading`, `ASXUIPC_Lobby`

#### ASXUIPC_Lobby
- **상속:** ASXUIPlayerControllerBase
- **기능:** 채팅 입력 처리. `ServerRPCPrintChatMessageString → ClientRPCPrintChatMessageString` 흐름.

---

### 4.4 컴포넌트 (Component/)

#### USXStatusComponent
- **상속:** UActorComponent
- **역할:** HP 관리 (ApplyDamage, SetCurrentHP, SetMaxHP, TakeBuff)
- **복제 속성:**
  - `CurrentHP` — `OnRep_CurrentHP` 콜백
  - `MaxHP` — `COND_OwnerOnly`, `OnRep_MaxHP` 콜백
- **델리게이트:** `OnMaxHPChanged`, `OnCurrentHPChanged`, `OnOutOfCurrentHP`

#### USXPickupComponent
- **상속:** UStaticMeshComponent
- **역할:** 플레이어 충돌 감지 → `OnPickUp` 델리게이트 브로드캐스트.

#### USXHPTextWidgetComponent
- **상속:** UWidgetComponent
- **역할:** 3D 월드 공간에 NPC 체력 텍스트 표시.

---

### 4.5 무기 / 아이템 (Weapon/, Gimmick/)

#### ASXWeapon
- **상속:** AActor
- **루트 컴포넌트:** `USXPickupComponent`
- **속성:** `AttackMontage`, `MaxAttackRange(25000cm)`
- **기능:** 픽업 시 플레이어 `hand_rSocket`에 AttachActorToComponent.

#### ASXHealthPack
- **상속:** AActor
- **컴포넌트:** `UBoxComponent`, `UStaticMeshComponent`, `URotatingMovementComponent`, `UNiagaraComponent`
- **기능:** 부유 애니메이션 + 충돌 시 플레이어 HP 회복.
- **네트워크:** `ServerRotationYaw` 복제 (`OnRep_ServerRotationYaw`)

#### ASXLandMine
- **상속:** AActor
- **기능:** 플레이어 충돌 시 폭발, Multicast RPC로 모든 클라이언트에 이펙트 표시 및 대미지 적용.
- **복제 속성:** `bIsExploded : 1` (bitfield, `OnRep_IsExploded`)
- **Multicast RPC:** `MulticastRPCSpawnEffect` (Unreliable)

---

### 4.6 AI (AI/)

#### UBTTask_Attack
- **역할:** NPC가 `AttackMontage`를 재생하는 BT 태스크.

#### UBTDecorator_IsInAttackRange
- **역할:** 대상이 공격 범위(50cm) 내에 있는지 체크하는 BT 데코레이터.

#### UBTService_DetectPlayerCharacter
- **역할:** 300cm 반경에서 플레이어 감지, Blackboard의 `TargetCharacter` 업데이트.
- **주기:** 1초

#### UBTTask_GetEndPatrolPosition
- **역할:** NavigationSystem을 사용해 순찰 종료 위치(시작 위치 기준 PatrolRadius 내 랜덤)를 계산.

#### UBTTask_TurnToTarget
- **역할:** NPC를 목표 방향으로 회전시키는 BT 태스크.

---

### 4.7 애니메이션 (Animation/)

#### USXAnimInstance
- **상속:** UAnimInstance
- **프로퍼티:**
  ```cpp
  FVector Velocity
  float GroundSpeed
  bool bShouldMove
  bool bIsFalling
  bool bIsDead
  bool bIsUnarmed
  float NormalizedCurrentPitch  // 조준각 (-90~90 → 0~1)
  ```
- **델리게이트:** `OnCheckHit`, `OnPostDead`

#### UAN_CheckHit
- **상속:** UAnimNotify
- **역할:** 몽타주 재생 중 근접 공격 히트 체크 트리거.

#### UAN_CheckInput_Attack
- **상속:** UAnimNotify
- **역할:** 몽타주 재생 중 콤보 입력 체크 트리거.

---

### 4.8 UI (UI/)

#### USXHUD
- **상속:** UUserWidget
- **바인딩 위젯:** `KillCountText`, `PlayerNameText`, `HPBar(UUW_HPBar)`
- **기능:** PlayerState의 킬 카운트 및 StatusComponent의 HP에 바인딩.

#### UUW_HPBar
- **바인딩 위젯:** `HPProgressBar`, `CurrentHPText`, `MaxHPText`

#### UUW_InGameMenu
- **버튼:** `ResumeButton` (재개), `ReturnTitleButton` (타이틀 복귀), `ExitButton` (종료)

#### UUW_GameResult
- **표시:** `ResultText` (Winner/Loser), `RankingText` (#01, #02...)
- **버튼:** `ReturnToTitleButton`

#### UUW_LobbyLayout
- **버튼:** `PreviousButton / NextButton` (색상 변경), `PlayButton` (게임 시작)
- **입력:** `EditPlayerName` (플레이어 이름 입력)

#### USXChatInput
- **역할:** 로비 채팅 입력 UI.

---

### 4.9 입력 (Input/)

#### USXInputConfig
- **상속:** UDataAsset
- **입력 액션 목록:** `Move`, `Look`, `Jump`, `AttackMelee`, `AttackRanged`, `Menu`, `SpawnLandMine`

---

### 4.10 ShooterXPlayGround 모듈

#### USXCharacterMaterialManager
- **상속:** UObject
- **역할:** `DefaultPlayerCharacterMeshMaterialPaths.ini`에서 색상별(Black/Blue/Red/White) 머티리얼 경로를 로드하여 제공.

---

### 4.11 유틸리티

#### ShooterXFunctionLibrary
- `MyPrintString()` — 화면/로그 디버그 출력
- `GetNetModeString()` — 네트 모드 문자열화
- `GetRoleString()` — 네트 롤 문자열화
- 커스텀 로깅 매크로

---

## 5. 클래스 상속 관계

```
AActor
├── ASXWeapon
├── ASXHealthPack
└── ASXLandMine

ACharacter
├── ASXCharacterBase
│   ├── ASXPlayerCharacter
│   └── ASXNonPlayerCharacter
└── APawn
    └── ASXPlayerPawn
        └── ASXLobbyPlayerCharacter

AGameModeBase
├── ASXGameModeBase
└── ASXGM_Lobby

AGameStateBase
├── ASXGameStateBase
└── ASXGS_Lobby

APlayerController
├── ASXPlayerController
├── ASXUIPlayerControllerBase
│   ├── ASXUIPC_Loading
│   └── ASXUIPC_Lobby
└── AAIController
    └── ASXAIController

APlayerState
├── ASXPlayerState
└── ASXPS_Lobby

UActorComponent
├── USXStatusComponent
├── USXHPTextWidgetComponent
└── USXPickupComponent (UStaticMeshComponent 상속)

UAnimInstance
└── USXAnimInstance

UAnimNotify
├── UAN_CheckHit
└── UAN_CheckInput_Attack

UBTTaskNode
├── UBTTask_Attack
├── UBTTask_GetEndPatrolPosition
└── UBTTask_TurnToTarget

UBTDecorator
└── UBTDecorator_IsInAttackRange

UBTService
└── UBTService_DetectPlayerCharacter

UUserWidget
├── USXHUD
├── UUW_HPBar
├── UUW_HPText
├── UUW_InGameMenu
├── UUW_GameResult
├── UUW_LobbyLayout
└── USXChatInput

UGameInstance
└── USXGameInstance

UDataAsset
└── USXInputConfig

UObject
└── USXCharacterMaterialManager
```

---

## 6. 게임플레이 아키텍처

### 6.1 게임 상태 머신

```
WAITING
  └─ 조건: AlivePlayerCount >= 2 && 대기시간(15초) 경과
      ↓
PLAYING
  └─ 조건: AlivePlayerCount <= 1
      ↓
ENDING
  └─ 조건: 15초 경과
      ↓
Lobby (ServerTravel)
```

### 6.2 플레이어 입력 흐름

```
EnhancedInput 액션
    ↓
ASXPlayerCharacter::SetupPlayerInputComponent()
    ├── Move         → InputMove()
    ├── Look         → InputLook()
    ├── Jump         → Jump() / StopJumping()
    ├── AttackMelee  → InputAttackMelee() → ServerRPCMeleeAttack()
    ├── AttackRanged → InputAttackRanged() → Fire() → ServerRPCFire()
    ├── Menu         → InputMenu() → ToggleInGameMenu()
    └── SpawnLandMine → InputSpawnLandMine() → ServerRPCSpawnLandMine()
```

### 6.3 근접 공격 / 콤보 흐름

```
InputAttackMelee()
  └─ 낙하 중 금지
  └─ BeginAttack() → AttackMeleeMontage 재생
      ├── AN_CheckInput_Attack → HandleOnCheckInputAttack()
      │     └─ bIsAttackKeyPressed = true → 섹션 전환 (Attack01→02→03)
      └── AN_CheckHit → HandleOnCheckHit()
            └─ Sphere Sweep(사거리 50cm, 반경 20cm, ECC_ATTACK)
                  └─ ApplyDamage(10)
```

### 6.4 원거리 공격 흐름

```
InputAttackRanged()
  └─ CanFire() 체크 (이동 중 금지, 무기 보유 필요, 애니 미재생)
  └─ Fire()
      ├─ GetAimLocation() : 카메라에서 ECC_CAMERAAIM 채널 트레이싱
      ├─ TraceBullet()    : 총구→조준점 ECC_ATTACK 라인 트레이싱
      ├─ ApplyHitDamage() : 헤드샷 100 / 기타 10
      └─ ServerRPCFire() → MulticastRPCFire() (애니메이션 동기화)
```

### 6.5 HP / 대미지 흐름

```
Actor::TakeDamage(DamageAmount)
  └─ MatchState == Playing 체크 (서버 권한)
  └─ StatusComponent::ApplyDamage()
      ├─ CurrentHP -= DamageAmount
      └─ CurrentHP <= 0:
          ├─ bIsDead = true
          ├─ 충돌 비활성화
          ├─ 이동 중단
          └─ AnimInstance::OnPostDead → Character 소멸
```

### 6.6 킬 / 승리 조건

- NPC 사망 시 → `AlivePlayerControllerCount` 감소
- `AlivePlayerControllerCount <= 1` → `ENDING` 상태로 전환
- 마지막 생존자가 승자 (`ClientRPCShowGameResultWidget(Ranking)`)

---

## 7. 멀티플레이어 / 네트워킹

### 7.1 복제 속성 (DOREPLIFETIME)

| 클래스 | 속성 | 조건 |
|--------|------|------|
| ASXPlayerCharacter | CurrentAimPitch | 기본 |
| USXStatusComponent | CurrentHP | 기본 |
| USXStatusComponent | MaxHP | COND_OwnerOnly |
| ASXGameStateBase | AlivePlayerControllerCount | 기본 |
| ASXGameStateBase | MatchState | 기본 |
| ASXPlayerController | NotificationText | 기본 |
| ASXPS_Lobby | PlayerLobbyNameString | 기본 |
| ASXHealthPack | ServerRotationYaw | OnRep |
| ASXLandMine | bIsExploded (bitfield) | OnRep |

### 7.2 RPC 목록

#### ASXPlayerCharacter
| RPC | 타입 | Reliable | 설명 |
|-----|------|----------|------|
| ServerRPCUpdateAimValue | Server | No (Unreliable) | 조준 피치 동기화 |
| ServerRPCFire | Server | Yes | 총기 발사 서버 처리 |
| ServerRPCSpawnLandMine | Server | Yes + Validation | 착지뢰 스폰 |
| ServerRPCMeleeAttack | Server | Yes | 근접 공격 서버 처리 |
| MulticastRPCFire | Multicast | Yes | 총기 발사 애니메이션 동기화 |
| MulticastRPCMeleeAttack | Multicast | Yes | 근접 공격 애니메이션 동기화 |

#### ASXPlayerController
| RPC | 타입 | Reliable | 설명 |
|-----|------|----------|------|
| ClientRPCShowGameResultWidget | Client | Yes | 결과 화면 표시 |
| ClientRPCReturnToTitle | Client | Yes | 타이틀 복귀 |

#### ASXUIPC_Lobby
| RPC | 타입 | Reliable | 설명 |
|-----|------|----------|------|
| ServerRPCPrintChatMessageString | Server | Yes | 채팅 서버 전달 |
| ClientRPCPrintChatMessageString | Client | Yes | 채팅 수신 |

#### ASXGS_Lobby
| RPC | 타입 | Reliable | 설명 |
|-----|------|----------|------|
| MulticastRPCBroadcastLoginMessage | Multicast | Yes | 로그인/로그아웃 알림 |

#### ASXLandMine
| RPC | 타입 | Reliable | 설명 |
|-----|------|----------|------|
| MulticastRPCSpawnEffect | Multicast | No (Unreliable) | 폭발 이펙트 동기화 |

### 7.3 서버 권한 패턴

```cpp
// 서버에서만 대미지 적용
if (GetOwner()->HasAuthority()) {
    StatusComponent->ApplyDamage(FinalDamageAmount);
}

// 클라이언트에서 서버로 요청
if (IsLocallyControlled()) {
    ServerRPCSpawnLandMine();
}
```

### 7.4 네트워크 최적화

- `ASXHealthPack::NetCullDistance = 1000.f` + `IsNetRelevantFor()` 오버라이드 (거리 기반 필터링)
- `ASXLandMine::NetCullDistance` 설정 (원거리 클라이언트에 미표시)

---

## 8. 컴포넌트 구조

### 8.1 ASXCharacterBase
```
Capsule (루트)
├── SkeletalMesh
│   └── 소켓: hand_rSocket (무기 부착점)
└── USXStatusComponent
```

### 8.2 ASXPlayerCharacter (추가)
```
Capsule (루트)
├── USpringArmComponent (TargetArmLength: 300)
│   └── UCameraComponent
└── UNiagaraComponent (킬 이펙트)
```

### 8.3 ASXNonPlayerCharacter (추가)
```
Capsule (루트)
└── USXHPTextWidgetComponent (3D HP 표시)
```

### 8.4 ASXHealthPack
```
SceneComponent (루트)
├── UBoxComponent (충돌)
├── UStaticMeshComponent
├── URotatingMovementComponent
└── UNiagaraComponent
```

### 8.5 ASXLandMine
```
SceneComponent (루트)
├── UBoxComponent (충돌)
├── UStaticMeshComponent
└── UNiagaraComponent (폭발 이펙트)
```

### 8.6 ASXWeapon
```
USXPickupComponent (루트, StaticMesh + 충돌)
```

---

## 9. 애니메이션 시스템

### 9.1 애니메이션 블루프린트

| 파일명 | 대상 |
|--------|------|
| ABP_PlayerCharacter | 플레이어 캐릭터 |
| ABP_NPC | NPC |
| ABP_PlayerPawn | 로비 Pawn |

### 9.2 몽타주

| 몽타주 | 클래스 | 섹션 |
|--------|--------|------|
| AttackMeleeMontage | ASXCharacterBase | Attack01, Attack02, Attack03 |
| AttackMontage | ASXWeapon | 총기 공격 |

### 9.3 AnimInstance 주요 변수

```cpp
FVector  Velocity                // 이동 속도 벡터
float    GroundSpeed             // 지상 속도
bool     bShouldMove             // 이동 중
bool     bIsFalling              // 낙하 중
bool     bIsDead                 // 사망 여부
bool     bIsUnarmed              // 비무장
float    NormalizedCurrentPitch  // 카메라 피치 (0~1 정규화)
```

### 9.4 AnimNotify

| 클래스 | 연결 함수 |
|--------|-----------|
| UAN_CheckHit | ASXCharacterBase::HandleOnCheckHit() |
| UAN_CheckInput_Attack | ASXCharacterBase::HandleOnCheckInputAttack() |

---

## 10. 무기 / 전투 시스템

### 10.1 근접 공격

- **공격 판정:** Sphere Sweep (ECC_GameTraceChannel2 = ECC_ATTACK)
- **사거리:** 50cm, **반경:** 20cm
- **대미지:** 10
- **콤보:** 최대 3단 (Montage 섹션: Attack01 → Attack02 → Attack03)

### 10.2 원거리 공격 (총기)

- **조준:** 카메라에서 ECC_CAMERAAIM 채널로 Ray Cast → 조준점 좌표 획득
- **발사:** 총구에서 조준점까지 ECC_ATTACK 채널 Line Trace
- **대미지:**
  - 헤드샷: **100**
  - 일반: **10**
- **최대 사거리:** 25,000cm

### 10.3 착지뢰 (ASXLandMine)

- 플레이어가 `ServerRPCSpawnLandMine`으로 서버에 스폰 요청
- 충돌한 캐릭터에 대미지 적용
- `MulticastRPCSpawnEffect`로 모든 클라이언트에 폭발 이펙트 동기화
- `bIsExploded` 복제로 늦게 접속한 클라이언트도 동기화

### 10.4 힐팩 (ASXHealthPack)

- `URotatingMovementComponent`로 회전 + Tick에서 상하 부유
- 충돌 시 `StatusComponent::TakeBuff()`로 HP 회복

---

## 11. AI 시스템

### 11.1 BehaviorTree 구조

```
BT_NPC
├── Selector
│   ├── [Decorator: IsInAttackRange] Sequence
│   │   └── Task: BTTask_Attack
│   └── Sequence (순찰)
│       ├── Task: BTTask_GetEndPatrolPosition
│       ├── Task: MoveTo (EndPatrolPosition)
│       └── Task: BTTask_TurnToTarget
└── Service: BTService_DetectPlayerCharacter (1초 주기)
```

### 11.2 Blackboard 키

| 키 | 타입 | 설명 |
|----|------|------|
| StartPatrolPosition | Vector | 순찰 시작 위치 |
| EndPatrolPosition | Vector | 순찰 목표 위치 |
| TargetCharacter | Object | 감지된 플레이어 |

### 11.3 감지 시스템

- `BTService_DetectPlayerCharacter`: 1초마다 300cm 반경 내 플레이어 감지
- 감지 성공 → `TargetCharacter` 블랙보드 키 설정
- 공격 조건: 대상이 50cm 이내 (`BTDecorator_IsInAttackRange`)

---

## 12. UI 시스템

### 12.1 HUD 구조 (z-order)

```
z=0  WBP_HUD         → KillCountText, PlayerNameText, HPBar
z=1  WBP_Crosshair   → 크로스헤어
z=1  WBP_Notification → 게임 상태 텍스트 (NotificationText)
z=3  WBP_InGameMenu  → 재개/타이틀/종료 버튼 (기본 숨김)
```

### 12.2 게임 결과 UI 흐름

```
GameMode: Ending 상태
  └─ PlayerController::ClientRPCShowGameResultWidget(Ranking)
      └─ WBP_GameResult 표시
          ├─ ResultText: "Winner" / "Loser"
          └─ RankingText: "#01", "#02"...
```

### 12.3 로비 UI 흐름

```
UUW_LobbyLayout
  ├─ EditPlayerName → PlayerInfo.txt 저장 (JSON)
  ├─ Previous/NextButton → PlayerColor 변경 (머티리얼 교체)
  └─ PlayButton → Ready 상태 → ASXGM_Lobby가 ServerTravel
```

### 12.4 채팅 흐름 (로비)

```
USXChatInput 입력
  └─ ASXUIPC_Lobby::ServerRPCPrintChatMessageString()
      └─ 모든 클라이언트에 ClientRPCPrintChatMessageString()
```

---

## 13. Config 설정

### DefaultEngine.ini 핵심 설정

```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/ShooterX/Map/Title
TransitionMap=/Game/ShooterX/Map/Loading
GlobalDefaultGameMode=BP_GameModeBase_C
ServerDefaultMap=/Game/ShooterX/Map/Lobby
```

### DefaultPlayerCharacterMeshMaterialPaths.ini

```ini
[ShooterXCharacterMaterialManager]
BlackMaterialPath=...
BlueMaterialPath=...
RedMaterialPath=...
WhiteMaterialPath=...
```

---

## 14. Content 폴더 구조

```
Content/
├── ShooterX/
│   ├── AI/                   # BlackboardData, BehaviorTree
│   ├── Animation/
│   │   ├── AnimationBlueprint/ (ABP_PlayerCharacter, ABP_NPC, ABP_PlayerPawn)
│   │   └── AnimMontages/      (근접/원거리 공격 몽타주)
│   ├── Blueprint/
│   │   ├── Character/         (BP_PlayerCharacter, BP_NPC, BP_LobbyPlayerCharacter)
│   │   ├── Game/              (BP_GameModeBase, BP_GameInstance, BP_GM_Lobby 등)
│   │   ├── Item/              (BP_Weapon, BP_HealthPack, BP_LandMine)
│   │   └── Controller/        (BP_PlayerController, BP_AIController 등)
│   ├── Data/                  # MaxHP DataTable
│   ├── Font/                  # UI 폰트
│   ├── Input/                 # InputConfig, InputMappingContext, InputActions
│   ├── Map/
│   │   ├── Title.umap         # 타이틀 레벨
│   │   ├── Loading.umap       # 트랜지션 레벨
│   │   ├── Lobby.umap         # 로비 레벨
│   │   └── L_Expanse.umap     # 메인 게임 맵
│   ├── Material/              # M_PlayerCharacter, 색상별 인스턴스(4종)
│   ├── Texture/
│   └── UI/                    # WBP_HUD, WBP_HPBar, WBP_InGameMenu, WBP_GameResult 등
├── LyraResource/              # 라이라 샘플 리소스 (캐릭터 메시, 무기, 이펙트)
└── ZombieAnimationPack/       # NPC 애니메이션
```

---

## 15. 플러그인 및 모듈 의존성

### 플러그인

| 플러그인 | 활성화 범위 |
|----------|-------------|
| EnhancedInput | 모든 타겟 |
| ModelingToolsEditorMode | Editor만 |

### 엔진 모듈

| 모듈 | 용도 |
|------|------|
| AIModule | BehaviorTree, Blackboard |
| NavigationSystem | AI 경로 탐색 |
| GameplayTasks | AI 태스크 |
| UMG, Slate, SlateCore | UI |
| Niagara | 파티클 이펙트 |
| Json, JsonUtilities | PlayerInfo.txt 파싱 |
| EnhancedInput | 입력 처리 |

---

## 16. Git 커밋 히스토리

| 해시 | 메시지 | 내용 요약 |
|------|--------|-----------|
| 95959ff | package | 패키징 설정 |
| 14d79f0 | 게임 종료 | 종료 조건/흐름 구현 |
| c9724be | plus | 기능 추가 |
| 1863e30 | 액터 컴포넌트 동기화 | USXStatusComponent 복제 |
| 75aae2f | 애니메이션 동기화 | 멀티플레이 애니메이션 동기화 |
| 0892317 | Property Replication을 이용한 이펙트 재생 | Niagara 이펙트 복제 |
| 9b298e7 | 1403 | 체크포인트 |
| 24ca1e8 | Relevancy와 NetCullDistance | 네트워크 최적화 |
| 69f443c | npc | NPC 구현 |
| a9dd3d9 | Travel | ServerTravel 구현 |
| b172ea2 | 게임플레이 프레임워크와 멀티플레이 | 멀티플레이 핵심 |
| 84d80b6 | Property Replication 실습 | 프로퍼티 복제 |
| b480621 | Remote Procedure Call 실습 | RPC 구현 |
| dd65d5a | 플레이어 스타트 추가 | 스폰 포인트 |
| 448275d | 서버로의 접속 | 네트워크 접속 |
| 04dde78 | NetRole | 네트워크 롤 이해 |
| ae9cf3d | Lobby Level | 로비 레벨 |
| 9f6b3ae | In-Game Menu | 메뉴 UI |
| c063988 | Loading Level | 로딩 화면 |
| 7a5492c | Title Level | 타이틀 화면 |
| 9a02fd5 | 사격 & 피격 리팩토링 | 총기 시스템 정리 |
| 163a647 | 피격 | 피격 시스템 |
| 6c933db | 모듈의 활용 | ShooterXPlayGround 모듈 |
| 7acb6c8 | HUD | 게임 HUD |
| db41cd7 | PlayerState | 플레이어 상태 |
| d9a8b34 | Simple Parallel - 공격 | 병렬 AI 공격 |
| a7e86d5 | Behavior Tree and Blackboard | AI 시스템 |
| b91e72c | AIController 생성 | AI 컨트롤러 |
| cd6763a | 지뢰와 힐팩 | 아이템 시스템 |
| dff9fd6 | 충돌 감지와 디버그 드로잉 | 초기 기반 작업 |

**개발 흐름 요약:**
1. **기반 구축**: 충돌, 디버그, 아이템, AI 기본
2. **전투 시스템**: 공격, 피격, 무기
3. **네트워킹**: NetRole → RPC → Property Replication → 최적화
4. **UI/UX**: HUD, 메뉴, 로비, 타이틀, 로딩
5. **마무리**: 게임 종료 흐름, 패키징

---

## 17. 기능 완성도 요약

| 시스템 | 담당 클래스 | 상태 |
|--------|-------------|------|
| 플레이어 이동/점프 | ASXPlayerCharacter | 완성 |
| 근접 공격 (3단 콤보) | ASXCharacterBase | 완성 |
| 원거리 공격 (총기) | ASXPlayerCharacter, ASXWeapon | 완성 |
| HP 관리 (복제 포함) | USXStatusComponent | 완성 |
| 대미지 / 사망 처리 | ASXCharacterBase | 완성 |
| NPC AI (순찰 + 공격) | ASXAIController + BT | 완성 |
| 4인 멀티플레이 동기화 | Replication + RPC | 완성 |
| 킬 카운트 시스템 | ASXPlayerState | 완성 |
| 게임 상태 관리 | ASXGameModeBase | 완성 |
| 로비 시스템 | ASXGM_Lobby, ASXUIPC_Lobby | 완성 |
| 플레이어 색상 선택 | USXCharacterMaterialManager | 완성 |
| 게임 HUD | USXHUD, UUW_HPBar | 완성 |
| 인게임 메뉴 | UUW_InGameMenu | 완성 |
| 게임 결과 화면 | UUW_GameResult | 완성 |
| 로비 채팅 | USXChatInput, ASXUIPC_Lobby | 완성 |
| 착지뢰 | ASXLandMine | 완성 |
| 힐팩 | ASXHealthPack | 완성 |
| 네트워크 최적화 | Relevancy, NetCullDistance | 완성 |
| 패키징 | Build Settings | 완성 |

---

## 18. 협업 방식 (Claude 작업 규칙)

> 2026-07-04부터 적용

- 사용자가 앞으로 해야 할 작업을 설명하면, Claude는 **코드를 직접 수정하지 않는다.**
- 대신 작업 방법을 **스텝바이스텝**으로 안내한다.
- 안내 시 반드시 다음을 포함한다:
  - **어떤 파일**을 수정해야 하는지
  - **어떤 클래스**를 수정해야 하는지
  - **어떤 함수**를 수정해야 하는지
  - **Before / After** 코드 비교 (수정 전 코드 / 수정 후 코드)
- 사용자가 명시적으로 "직접 구현해줘" 등으로 요청하는 경우에는 이 규칙에서 예외로 하고 직접 구현한다.

---

## 19. TODO: 세션 이름 / 다음 레벨 정보 전달 흐름 개선

> 2026-07-04 작성. `UW_SessionSetupMenu`에서 세션 생성 시 정한 "세션 이름"과 "다음에 이동할 레벨 이름"을,
> `Lobby` 레벨의 `ASXGM_Lobby`까지 전달하기 위한 작업 계획. 기존 `Saved/PlayerInfo.txt` JSON 저장/로드 패턴
> (`UW_LobbyLayout.cpp` / `SXPlayerState.cpp`)을 그대로 따라간다.

### 설계 요약

- `Saved/SessionSetup.txt`에 `{sessionname, mapname}` JSON을 새로 저장 (기존 `PlayerInfo.txt`와 동일한 방식)
- `UW_SessionSetupMenu`에서 직접 `CreateSession()`을 호출하던 걸 걷어내고, 파일 저장 + `OpenLevel(Lobby, listen)`으로 변경
- `ASXGM_Lobby::BeginPlay()`에서 그 파일을 읽어 세션 이름으로 `CreateSession()` 호출 + 다음에 이동할 레벨 이름을 멤버 변수에 저장
- `ASXGM_Lobby::Tick()`의 하드코딩된 `"L_Expanse"` 대신 그 멤버 변수로 `ServerTravel()`

### Step 1. `Source/ShooterX/UI/UW_SessionSetupMenu.cpp` — `UUW_SessionSetupMenu::OnCreateSessionButtonClicked()`

**Before**
```cpp
void UUW_SessionSetupMenu::OnCreateSessionButtonClicked()
{
	CreateSessionButton->SetIsEnabled(false);

	FString SessionName = SessionNameEditableText->GetText().ToString();
	FString MapName = MapComboBox->GetSelectedOption();

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem))
	{
		Subsystem->CreateSession(MaxPlayers, SessionName, MapName);
	}

	RemoveFromParent();
}
```
> 지금 이 코드는 `Subsystem->CreateSession(MaxPlayers, SessionName, MapName)` 처럼 인자를 3개 넘기는데, `SXOnlineSessionSubsystem.h`의 선언은 `CreateSession(int32 MaxPlayers, FString InSessionName = TEXT("None"))` 로 2개만 받기 때문에 **현재 컴파일이 안 되는 상태**임.

**After**
```cpp
void UUW_SessionSetupMenu::OnCreateSessionButtonClicked()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false || World->GetNetMode() != NM_Standalone)
	{
		return;
	}

	CreateSessionButton->SetIsEnabled(false);

	const FString SessionName = SessionNameEditableText->GetText().ToString();
	const FString MapName = MapComboBox->GetSelectedOption();

	const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	const FString SavedFileName(TEXT("SessionSetup.txt"));
	FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
	FPaths::MakeStandardFilename(AbsoluteFilePath);

	TSharedRef<FJsonObject> SessionSetupJsonObject = MakeShared<FJsonObject>();
	SessionSetupJsonObject->SetStringField(TEXT("sessionname"), SessionName);
	SessionSetupJsonObject->SetStringField(TEXT("mapname"), MapName);

	FString SessionSetupJsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&SessionSetupJsonString);
	if (FJsonSerializer::Serialize(SessionSetupJsonObject, JsonWriterArchive) == true)
	{
		FFileHelper::SaveStringToFile(SessionSetupJsonString, *AbsoluteFilePath);
	}

	const FName LevelName(TEXT("Lobby"));
	const FString Options(TEXT("listen"));
	UGameplayStatics::OpenLevel(World, LevelName, true, Options);
}
```
- `Subsystem->CreateSession(...)` 호출 자체를 제거 (세션 생성은 Lobby 레벨의 `ASXGM_Lobby::BeginPlay()`로 이관)
- 파일 상단 include에 `#include "Dom/JsonObject.h"`, `#include "Serialization/JsonWriter.h"`, `#include "Misc/FileHelper.h"`, `#include "Kismet/GameplayStatics.h"` 필요 여부 확인 (없으면 추가)
- `MaxPlayers` 프로퍼티는 이 함수에서 더 이상 쓰이지 않게 됨 — 나중에 세션 인원수를 UI에서 반영하고 싶으면 이 값도 파일에 같이 저장해서 `ASXGM_Lobby`가 읽게 만들어야 함 (지금 계획 범위 밖이라 일단 그대로 둠)

### Step 2. `Source/ShooterX/Game/SXGM_Lobby.h` — `ASXGM_Lobby` 클래스에 멤버 변수 추가

**Before**
```cpp
	UPROPERTY(EditDefaultsOnly)
	int32 MaxSessionPlayers = 4;

private:
	bool bIsTravelling = false;
```

**After**
```cpp
	UPROPERTY(EditDefaultsOnly)
	int32 MaxSessionPlayers = 4;

private:
	bool bIsTravelling = false;

	UPROPERTY(VisibleAnywhere)
	FString TargetLevelName = TEXT("L_Expanse");
```
- `TargetLevelName`을 기본값 `"L_Expanse"`로 둬서, `SessionSetup.txt`가 없을 때(예: 에디터에서 Lobby 맵 단독 실행)도 안전하게 동작하도록 함

### Step 3. `Source/ShooterX/Game/SXGM_Lobby.cpp` — `ASXGM_Lobby::BeginPlay()`

**Before**
```cpp
void ASXGM_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
		if (IsValid(Subsystem) == true)
		{
			Subsystem->CreateSession(MaxSessionPlayers);
		}
	}
}
```

**After**
```cpp
void ASXGM_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
		const FString SavedFileName(TEXT("SessionSetup.txt"));
		FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
		FPaths::MakeStandardFilename(AbsoluteFilePath);

		FString SessionName = TEXT("None");

		FString SessionSetupJsonString;
		if (FFileHelper::LoadFileToString(SessionSetupJsonString, *AbsoluteFilePath) == true)
		{
			TSharedRef<TJsonReader<TCHAR>> JsonReaderArchive = TJsonReaderFactory<TCHAR>::Create(SessionSetupJsonString);
			TSharedPtr<FJsonObject> SessionSetupJsonObject = nullptr;
			if (FJsonSerializer::Deserialize(JsonReaderArchive, SessionSetupJsonObject) == true)
			{
				SessionName = SessionSetupJsonObject->GetStringField(TEXT("sessionname"));
				TargetLevelName = SessionSetupJsonObject->GetStringField(TEXT("mapname"));
			}
		}

		USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
		if (IsValid(Subsystem) == true)
		{
			Subsystem->CreateSession(MaxSessionPlayers, SessionName);
		}
	}
}
```
- `TargetLevelName`은 파일이 없거나 파싱 실패 시 Step 2에서 설정한 기본값(`"L_Expanse"`)을 그대로 유지
- 파일 상단 include에 `#include "Dom/JsonObject.h"`, `#include "Serialization/JsonReader.h"`, `#include "Misc/FileHelper.h"` 필요 여부 확인

### Step 4. `Source/ShooterX/Game/SXGM_Lobby.cpp` — `ASXGM_Lobby::Tick(float DeltaSeconds)`

**Before**
```cpp
	if (RemainTimeForPlaying <= 0.f)
	{
		bIsTravelling = true;

		UWorld* World = GetWorld();
		if (IsValid(World) == true)
		{
			World->ServerTravel(TEXT("L_Expanse"));
		}
	}
```

**After**
```cpp
	if (RemainTimeForPlaying <= 0.f)
	{
		bIsTravelling = true;

		UWorld* World = GetWorld();
		if (IsValid(World) == true)
		{
			World->ServerTravel(TargetLevelName);
		}
	}
```

### 참고: 실제 검증 순서

1. Title 레벨에서 `UW_SessionSetupMenu`로 세션 이름/맵 선택 → `SessionSetup.txt` 저장 → `Lobby`를 listen 서버로 오픈
2. `ASXGM_Lobby::BeginPlay()`가 그 파일을 읽어 세션 이름으로 `CreateSession()` 호출, 동시에 `TargetLevelName` 세팅
3. 인원 충족 후 `Tick()`에서 `TargetLevelName`(예: `L_Convolution_Blockout` 또는 `L_Expanse`)으로 `ServerTravel()`

이대로 진행하면 기존에 있던 3-인자 `CreateSession()` 컴파일 에러도 자연히 해소됨.

---

## 20. TODO: Online Subsystem Steam 적용 (진행 중)

> 2026-07-06 작성/진행 중. Steam 친구 초대·매치메이킹 등을 위해 Online Subsystem Steam을 적용하는 작업.
> Advanced Sessions Plugin은 `IOnlineSession`/`IOnlineSubsystem` C++ API를 BP에서 쓰게 해주는 래퍼일 뿐이라
> (이 프로젝트처럼 C++로만 직접 구현하는 경우) 불필요하다고 판단, 언리얼 공식 문서(Online Subsystem Steam) 절차를 그대로 따라 진행.
> 엔진은 소스 빌드(`C:/UnrealProject/UnrealEngine-5.7.4-release`, `.uproject`의 EngineAssociation이 GUID로 등록된 커스텀 엔진).

### 완료된 작업 (체크리스트)

- **Step 1.** Steam 클라이언트 설치 및 로그인 확인
- **Step 2.** Steamworks SDK(v1.64) 다운로드 → 엔진 `Engine/Source/ThirdParty/Steamworks/Steamv164/sdk`에 배치.
  `Engine/Source/ThirdParty/Steamworks/Steamworks.build.cs`의 `SteamVersionNumber = 1.64`와 폴더명(`Steamv164`) 일치 확인.
- **Step 3.** `steam_api64.dll`을 `Engine/Source/ThirdParty/Steamworks/Steamv164/sdk/redistributable_bin/win64/`에서
  `Engine/Binaries/ThirdParty/Steamworks/Steamv164/Win64/`로 복사 (이 프로젝트는 `-force_steamclient_link`를 안 써서 `steamclient64.dll` 등은 불필요).
- **Step 4.** 정식 Steamworks 파트너 등록($100, 서류, 30일 대기) 대신 **테스트 App ID `480`**으로 개발 진행하기로 결정.
- **Step 5.** `Source/ShooterX/ShooterX.Build.cs` 수정
  - `PublicDependencyModuleNames`에서 `"OnlineSubsystemNull"` 제거(주석 처리), `"OnlineSubsystem"`/`"OnlineSubsystemUtils"`는 유지
  - `DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull")`, `.Add("OnlineSubsystemSteam")` 추가
  - 이유: `OnlineSubsystem`/`OnlineSubsystemUtils`는 코드(`SXOnlineSessionSubsystem.cpp`)에서 `#include`로 직접 참조하는 범용 인터페이스라 컴파일 타임 링크(Public)가 맞고, `OnlineSubsystemNull`/`OnlineSubsystemSteam`은 코드에서 직접 참조하지 않는 "런타임에 ini 설정으로 선택되는 구현체"라 `DynamicallyLoadedModuleNames`가 맞음. Null도 남겨둬서 ini만 바꾸면 언제든 LAN 테스트로 되돌릴 수 있게 함.
- **Step 6.** `Config/DefaultEngine.ini`에 아래 블록 추가:
  ```ini
  [/Script/Engine.GameEngine]
  +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

  [OnlineSubsystem]
  DefaultPlatformService=Steam

  [OnlineSubsystemSteam]
  bEnabled=true
  SteamDevAppId=480
  bInitServerOnClient=true

  [/Script/OnlineSubsystemSteam.SteamNetDriver]
  NetConnectionClassName="OnlineSubsystemSteam.SteamNetConnection"
  ```
  - `bInitServerOnClient=true`에 대해 처음엔 "데디서버가 Session 방식(`bUseLobbiesIfAvailable=false`)이라 필요하다"고 설명했으나, 실제 엔진 소스(`OnlineSubsystemSteam.cpp:335-357`)를 확인해보니 틀린 설명이었음. **데디서버는 `IsRunningDedicatedServer()==true`라서 이 플래그와 무관하게 GameServer API가 자동 초기화됨.** 이 플래그가 실제로 영향을 주는 대상은 **리슨서버**(Client 빌드 안에서 호스팅되므로 기본적으론 GameServer API가 안 켜짐) — 다만 지금 리슨서버 경로는 Lobby 방식(`bUseLobbiesIfAvailable=true`)이라 엄밀히는 필수는 아니고, `-server` 커맨드라인 테스트 등을 대비한 안전장치로 켜둔 상태.
- **Step 7.** 프로젝트 빌드 + 패키징까지 완료.

### 현재 막힌 지점

패키징된 빌드를 실행해서 로그에서 "Online Subsystem"을 검색하면 여전히 **`Online Subsystem: Null`**로 출력됨 (`SXOnlineSessionSubsystem::Initialize()`가 찍는 로그). `[OnlineSubsystem] DefaultPlatformService=Steam`으로 설정했음에도 Steam이 아니라 Null이 로드되고 있음.

### 원인 파악 완료

`Engine/Plugins/Online/OnlineSubsystemSteam/OnlineSubsystemSteam.uplugin`:
```json
"EnabledByDefault" : false,
```
비교: `Engine/Plugins/Online/OnlineSubsystemNull/OnlineSubsystemNull.uplugin`:
```json
"EnabledByDefault" : true,
```

`OnlineSubsystemSteam`은 엔진 **플러그인**인데 기본적으로 비활성화 상태이고, `ShooterX.uproject`의 `"Plugins"` 배열에도 등록돼 있지 않음(현재 `ModelingToolsEditorMode`, `EnhancedInput`만 있음). `ShooterX.Build.cs`에 `DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam")`을 추가해도 **플러그인 자체가 비활성 상태라 그 모듈이 빌드/패키징에 아예 포함되지 않고**, 런타임에 Steam 구현체가 존재하지 않아 결국 Null로 폴백된 것으로 보임. (`OnlineSubsystemNull`은 `EnabledByDefault: true`라 지금까지 별도 설정 없이도 항상 동작했던 것.)

### 다음에 할 일 (다음 세션에서 이어갈 스텝)

1. `ShooterX.uproject`의 `"Plugins"` 배열에 아래 항목 추가 (또는 에디터 `Edit > Plugins`에서 "Steam" 검색 후 체크 — 그러면 uproject에 자동 반영됨):
   ```json
   {
       "Name": "OnlineSubsystemSteam",
       "Enabled": true
   }
   ```
2. Visual Studio 프로젝트 파일 재생성 → 다시 빌드 (이번엔 `OnlineSubsystemSteam` 모듈이 실제로 컴파일 대상에 포함되어야 정상)
3. 에디터 실행 후 Output Log에서 `Online Subsystem: Steam`으로 찍히는지 재확인
4. 확인되면 다시 패키징해서 패키징된 빌드에서도 동일하게 찍히는지 확인
5. 그 다음 실제 호스트/조인 테스트 — 접속이 이상하면 `[OnlineSubsystemSteam]`에 `bUseSteamNetworking=false` 추가 검토 (Steam P2P 대신 기존 IP 소켓 방식 유지)

### 이어서 진행하는 방법

다음에 "이어서하자"라고만 말하면, 이 섹션 "다음에 할 일"의 1번(`ShooterX.uproject`에 `OnlineSubsystemSteam` 플러그인 활성화)부터 지금까지와 동일하게 스텝바이스텝으로 안내 + 검수하며 계속 진행.
