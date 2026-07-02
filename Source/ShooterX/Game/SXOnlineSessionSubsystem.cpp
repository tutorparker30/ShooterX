// SXOnlineSessionSubsystem.cpp

#include "Game/SXOnlineSessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

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
	SessionSettings.bIsLANMatch = true;
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

	UE_LOG(LogTemp, Log, TEXT("Dedicated session created. Waiting for players..."));
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

void USXOnlineSessionSubsystem::FindSessions()
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	CombinedSearchResults.Empty();

	// [통합 검색] 리슨서버(Lobby) 세션부터 1차로 검색을 시작한다.
	// 완료 콜백(OnFindSessionsComplete)에서 이어서 데디서버(dedicated) 세션도 검색하여
	// 두 결과를 하나의 목록으로 합친다.
	StartSessionSearch(ESessionSearchPass::Lobby);
}

void USXOnlineSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& InSearchResult)
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	FOnJoinSessionCompleteDelegate JoinDelegate =
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
	JoinCompleteDelegateHandle = SessionManager->AddOnJoinSessionCompleteDelegate_Handle(JoinDelegate);

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	SessionManager->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, InSearchResult);
}

void USXOnlineSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionManager->ClearOnDestroySessionCompleteDelegate_Handle(DestroyCompleteDelegateHandle);
}

void USXOnlineSessionSubsystem::StartSessionSearch(ESessionSearchPass InPass)
{
	CurrentSearchPass = InPass;

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->bIsLanQuery = true;

	// [검색 대상에 따라 쿼리가 갈린다]
	// - 리슨서버 세션: Lobby 기반으로 광고되므로 SEARCH_LOBBIES 로 찾는다.
	// - 데디서버 세션: Lobby 를 쓰지 않으므로 SEARCH_LOBBIES 로는 검색되지 않는다.
	//   대신 SEARCH_DEDICATED_ONLY 로 데디 세션만 조회한다.
	if (InPass == ESessionSearchPass::Lobby)
	{
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}
	else
	{
		SessionSearch->QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
	}

	FOnFindSessionsCompleteDelegate FindDelegate =
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	FindCompleteDelegateHandle = SessionManager->AddOnFindSessionsCompleteDelegate_Handle(FindDelegate);

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	SessionManager->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void USXOnlineSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	SessionManager->ClearOnFindSessionsCompleteDelegate_Handle(FindCompleteDelegateHandle);

	if (bWasSuccessful == true && SessionSearch.IsValid() == true)
	{
		for (const FOnlineSessionSearchResult& NewResult : SessionSearch->SearchResults)
		{
			// Null OSS는 Lobby/Dedicated 두 패스를 구분하지 않고 같은 세션을 두 번 응답할 수 있어
			// SessionId 기준으로 중복을 걸러낸다.
			const bool bAlreadyCollected = CombinedSearchResults.ContainsByPredicate(
				[&NewResult](const FOnlineSessionSearchResult& Existing)
				{
					return Existing.Session.SessionInfo->GetSessionId() == NewResult.Session.SessionInfo->GetSessionId();
				});

			if (bAlreadyCollected == false)
			{
				CombinedSearchResults.Add(NewResult);
			}
		}
	}

	if (CurrentSearchPass == ESessionSearchPass::Lobby)
	{
		// 1차(Lobby) 검색이 끝나면, 곧바로 2차(dedicated) 검색을 이어서 시작한다.
		StartSessionSearch(ESessionSearchPass::Dedicated);
		return;
	}

	// 2차(dedicated)까지 끝난 시점에만 결과를 확정하여 UI에 넘긴다.
	CurrentSearchPass = ESessionSearchPass::None;

	SessionResults = CombinedSearchResults;
	CombinedSearchResults.Empty();

	OnSessionSearchComplete.Broadcast();
}

void USXOnlineSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	SessionManager->ClearOnJoinSessionCompleteDelegate_Handle(JoinCompleteDelegateHandle);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
		if (IsValid(PC) == true)
		{
			FString TravelURL;
			if (SessionManager->GetResolvedConnectString(NAME_GameSession, TravelURL) == true)
			{
				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		// 접속 실패 시 로컬 세션 정리 (재시도 가능하도록)
		DestroySession();
	}
}