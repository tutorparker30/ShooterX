// SXOnlineSessionSubsystem.cpp

#include "Game/SXOnlineSessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

void USXOnlineSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		// 현재 로드된 서브시스템 이름을 로그로 확인하는 습관이 중요
		UE_LOG(LogTemp, Log, TEXT("Online Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());

		SessionManager = OnlineSubsystem->GetSessionInterface();
	}
}

void USXOnlineSessionSubsystem::CreateSession(int32 MaxPlayers)
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	// 이미 세션이 존재하면 먼저 파괴 후 재생성 (17.8 Ex170503 참고)
	if (SessionManager->GetNamedSession(NAME_GameSession) != nullptr)
	{
		DestroySession();
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.bShouldAdvertise = true;  // 세션 서버에 광고 (검색 허용)
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;

	// [리슨/데디 분기] dedicated/Lobby 플래그가 갈린다.
	const bool bIsDedicated = IsRunningDedicatedServer();
	SessionSettings.bIsDedicated = bIsDedicated;
	SessionSettings.bUsesPresence = (bIsDedicated == false); // 데디서버는 로그인 유저가 없어 presence 불가
	SessionSettings.bUseLobbiesIfAvailable = (bIsDedicated == false); // Steam 로비도 presence 기반이므로 데디는 off

	// CreateUObject로 단일 객체 재사용 (AddNewObject 사용 시 중복 호출 위험)
	FOnCreateSessionCompleteDelegate CreateDelegate =
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	CreateCompleteDelegateHandle = SessionManager->AddOnCreateSessionCompleteDelegate_Handle(CreateDelegate);

	if (bIsDedicated == true)
	{
		// [데디서버] 로컬 플레이어가 없으므로 UniqueNetId 오버로드를 쓸 수 없다.
		// HostingPlayerNum(=0) 오버로드로 서버 자신이 세션을 생성한다.
		SessionManager->CreateSession(0, NAME_GameSession, SessionSettings);
	}
	else
	{
		// [리슨서버] 방을 만든 로컬 플레이어의 UniqueNetId로 세션을 생성한다.
		const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
		SessionManager->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
	}
}

void USXOnlineSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 완료 후 반드시 등록 해제 (메모리 누수 및 중복 호출 방지)
	SessionManager->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);

	if (bWasSuccessful == false)
	{
		return;
	}

	if (IsRunningDedicatedServer() == true)
	{
		// [데디서버] 이미 ServerDefaultMap(Lobby)을 실행 중이므로 별도 이동이 불필요하다.
		// 특히 "listen" 옵션을 붙이면 데디서버가 리슨서버로 승격되어 버리므로 절대 붙이지 않는다.
		UE_LOG(LogTemp, Log, TEXT("Dedicated session created. Waiting for players..."));
	}
	else
	{
		// [리슨서버] 방을 만든 클라이언트가 곧 호스트가 되도록 "listen" 옵션으로 맵을 연다.
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Lobby")), true, FString(TEXT("listen")));
	}
}

void USXOnlineSessionSubsystem::DestroySession()
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	FOnDestroySessionCompleteDelegate DestroyDelegate =
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	DestroyCompleteDelegateHandle = SessionManager->AddOnDestroySessionCompleteDelegate_Handle(DestroyDelegate);

	SessionManager->DestroySession(NAME_GameSession);
}

void USXOnlineSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionManager->ClearOnDestroySessionCompleteDelegate_Handle(DestroyCompleteDelegateHandle);
}