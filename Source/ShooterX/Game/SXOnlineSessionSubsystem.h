// SXOnlineSessionSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "SXOnlineSessionSubsystem.generated.h"

enum class ESessionSearchPass : uint8
{
	None,
	Lobby,
	Dedicated,
};

DECLARE_MULTICAST_DELEGATE(FOnSessionSearchComplete);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionResult, bool /*bWasSuccessful*/);

/**
 *
 */
UCLASS()
class SHOOTERX_API USXOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateSession(int32 MaxPlayers, FString InSessionName = TEXT("None"));

	void DestroySession();

	void FindSessions();

	void JoinSession(const FOnlineSessionSearchResult& InSearchResult);

	void UpdateSession(const FString& InMapName);

	void StartSession();

	void EndSession();

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void StartSessionSearch(ESessionSearchPass InPass);

	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

public:
	TArray<FOnlineSessionSearchResult> SessionResults;

	FOnSessionSearchComplete OnSessionSearchComplete;

	FOnJoinSessionResult OnJoinSessionResult;

private:
	IOnlineSessionPtr SessionManager;

	FDelegateHandle CreateCompleteDelegateHandle;

	FDelegateHandle DestroyCompleteDelegateHandle;

	int32 PendingCreateSessionMaxPlayers = -1;

	ESessionSearchPass CurrentSearchPass = ESessionSearchPass::None;

	TArray<FOnlineSessionSearchResult> CombinedSearchResults;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FDelegateHandle FindCompleteDelegateHandle;

	FDelegateHandle JoinCompleteDelegateHandle;

	FDelegateHandle UpdateCompleteDelegateHandle;

	FDelegateHandle StartCompleteDelegateHandle;

	FDelegateHandle EndCompleteDelegateHandle;

};
