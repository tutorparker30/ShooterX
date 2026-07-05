// SXOnlineSessionSubsystem.cpp

#include "Game/SXOnlineSessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/NetDriver.h"

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

void USXOnlineSessionSubsystem::CreateSession(int32 MaxPlayers, FString InSessionName)
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	UNetDriver* NetDriver = GetWorld() ? GetWorld()->GetNetDriver() : nullptr;
	bool bNetDriverPortReady = false;
	if (NetDriver != nullptr && NetDriver->GetNetMode() < NM_Client)
	{
		const FString AddressStr = NetDriver->LowLevelGetNetworkNumber();
		const int32 Colon = AddressStr.Find(TEXT(":"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		const FString PortStr = (Colon != INDEX_NONE) ? AddressStr.Mid(Colon + 1) : FString();
		bNetDriverPortReady = PortStr.IsEmpty() == false && PortStr != TEXT("0");
	}

	if (bNetDriverPortReady == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Session] NetDriver port not ready yet. Retrying CreateSession(%d) next tick."), MaxPlayers);
		//GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::CreateSession, MaxPlayers));
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::CreateSession, MaxPlayers, InSessionName));
		return;
	}

	if (SessionManager->GetNamedSession(NAME_GameSession) != nullptr)
	{
		if (IsRunningDedicatedServer() == true)
		{
			UE_LOG(LogTemp, Log, TEXT("Session already exists. Skipping CreateSession (dedicated server)."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Session] Existing session found (non-dedicated). Destroy & recreate with MaxPlayers=%d"),
				MaxPlayers);
			PendingCreateSessionMaxPlayers = MaxPlayers;
			DestroySession();
		}
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true; 
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;

	const bool bIsDedicated = IsRunningDedicatedServer();
	SessionSettings.bIsDedicated = bIsDedicated;
	SessionSettings.bUsesPresence = (bIsDedicated == false);
	SessionSettings.bUseLobbiesIfAvailable = (bIsDedicated == false);

	SessionSettings.Set(
		FName(TEXT("SessionName")),
		InSessionName,
		EOnlineDataAdvertisementType::ViaOnlineService
	);

	FOnCreateSessionCompleteDelegate CreateDelegate =
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	CreateCompleteDelegateHandle = SessionManager->AddOnCreateSessionCompleteDelegate_Handle(CreateDelegate);

	if (bIsDedicated == true)
	{
		SessionManager->CreateSession(0, NAME_GameSession, SessionSettings);
	}
	else
	{
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
		UE_LOG(LogTemp, Error, TEXT("[Session] CreateSession FAILED: %s"), *SessionName.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Session created. Waiting for players..."));
}

void USXOnlineSessionSubsystem::DestroySession()
{
	UE_LOG(LogTemp, Warning, TEXT("[Session] DestroySession() called."));

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
	UE_LOG(LogTemp, Warning, TEXT("[Session] JoinSession() called."));

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

void USXOnlineSessionSubsystem::UpdateSession(const FString& InMapName)
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	// 현재 세션 설정을 가져와 기존 데이터를 유지한 채 맵 이름만 덮어씀
	FOnlineSessionSettings* ExistingSettings = SessionManager->GetSessionSettings(NAME_GameSession);
	if (ExistingSettings == nullptr)
	{
		return;
	}

	ExistingSettings->Set(
		FName(TEXT("Map_Name")),
		InMapName,
		EOnlineDataAdvertisementType::ViaOnlineService
	);

	// 세션 갱신 중에도 방 검색이 유지되도록 반드시 true로 설정
	ExistingSettings->bAllowJoinInProgress = true;

	FOnUpdateSessionCompleteDelegate UpdateDelegate =
		FOnUpdateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnUpdateSessionComplete);
	UpdateCompleteDelegateHandle = SessionManager->AddOnUpdateSessionCompleteDelegate_Handle(UpdateDelegate);

	SessionManager->UpdateSession(NAME_GameSession, *ExistingSettings, true);
}

void USXOnlineSessionSubsystem::StartSession()
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	FOnlineSessionSettings* ExistingSettings = SessionManager->GetSessionSettings(NAME_GameSession);
	if (ExistingSettings == nullptr)
	{
		return;
	}

	ExistingSettings->bAllowJoinInProgress = false;

	FOnStartSessionCompleteDelegate StartDelegate =
		FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete);
	StartCompleteDelegateHandle = SessionManager->AddOnStartSessionCompleteDelegate_Handle(StartDelegate);

	SessionManager->StartSession(NAME_GameSession);
}

void USXOnlineSessionSubsystem::EndSession()
{
	if (SessionManager.IsValid() == false)
	{
		return;
	}

	FOnlineSessionSettings* ExistingSettings = SessionManager->GetSessionSettings(NAME_GameSession);
	if (ExistingSettings == nullptr)
	{
		return;
	}

	ExistingSettings->bAllowJoinInProgress = true;

	FOnEndSessionCompleteDelegate EndDelegate =
		FOnEndSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnEndSessionComplete);
	EndCompleteDelegateHandle = SessionManager->AddOnEndSessionCompleteDelegate_Handle(EndDelegate);

	SessionManager->EndSession(NAME_GameSession);
}

void USXOnlineSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("[Session] DestroySession complete. Success=%d, PendingRecreate=%d"), bWasSuccessful,
		PendingCreateSessionMaxPlayers);

	SessionManager->ClearOnDestroySessionCompleteDelegate_Handle(DestroyCompleteDelegateHandle);

	if (0 < PendingCreateSessionMaxPlayers)
	{
		const int32 MaxPlayers = PendingCreateSessionMaxPlayers;
		PendingCreateSessionMaxPlayers = -1;
		CreateSession(MaxPlayers);
		return;
	}
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

	UE_LOG(LogTemp, Warning, TEXT("[Session] JoinSession complete. Result=%d"), (int32)Result);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString TravelURL;
		const bool bHasValidConnectString =
			SessionManager->GetResolvedConnectString(NAME_GameSession, TravelURL) == true &&
			TravelURL.EndsWith(TEXT(":0")) == false;

		if (bHasValidConnectString == true)
		{
			APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
			if (IsValid(PC) == true)
			{
				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			// 호스트의 GameNetDriver가 준비되기 전에 FindSessions()가 캐싱해버린
			// 포트 0짜리 좀비 검색 결과. 이대로 ClientTravel하면 20초 타임아웃까지
			// Join 버튼이 막히므로 여기서 즉시 실패로 처리해 버튼을 바로 풀어준다.
			UE_LOG(LogTemp, Warning, TEXT("[Session] JoinSession resolved port=0. TravelURL=%s"), *TravelURL);
			DestroySession();
			OnJoinSessionResult.Broadcast(false);
			return;
		}
	}
	else
	{
		// 접속 실패 시 로컬 세션 정리 (재시도 가능하도록)
		DestroySession();
	}

	OnJoinSessionResult.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
}

void USXOnlineSessionSubsystem::OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionManager->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateCompleteDelegateHandle);
}

void USXOnlineSessionSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionManager->ClearOnStartSessionCompleteDelegate_Handle(StartCompleteDelegateHandle);
}

void USXOnlineSessionSubsystem::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionManager->ClearOnEndSessionCompleteDelegate_Handle(EndCompleteDelegateHandle);
}
