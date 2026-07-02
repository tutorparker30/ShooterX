// SXOnlineSessionSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "SXOnlineSessionSubsystem.generated.h"

// [데디서버] 리슨서버(Lobby) 세션과 데디서버(dedicated) 세션은
// 검색 쿼리 자체가 달라 한 번의 FindSessions 호출로는 둘 다 찾을 수 없다.
// 두 종류를 순차 검색한 뒤 결과를 합쳐 하나의 목록으로 만들기 위한 상태값들.
enum class ESessionSearchPass : uint8
{
	None,
	Lobby,   // 1차: 리슨서버(Lobby) 검색
	Dedicated,  // 2차: 데디서버(dedicated) 검색
};

DECLARE_MULTICAST_DELEGATE(FOnSessionSearchComplete);

/**
 *
 */
UCLASS()
class SHOOTERX_API USXOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateSession(int32 MaxPlayers);

	void DestroySession();

	void FindSessions();

	void JoinSession(const FOnlineSessionSearchResult& InSearchResult);

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void StartSessionSearch(ESessionSearchPass InPass);

	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

public:
	// UW_SessionList가 읽어갈 검색 결과 배열
	TArray<FOnlineSessionSearchResult> SessionResults;

	FOnSessionSearchComplete OnSessionSearchComplete;

private:
	IOnlineSessionPtr SessionManager;

	FDelegateHandle CreateCompleteDelegateHandle;

	FDelegateHandle DestroyCompleteDelegateHandle;

	ESessionSearchPass CurrentSearchPass = ESessionSearchPass::None;

	TArray<FOnlineSessionSearchResult> CombinedSearchResults;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FDelegateHandle FindCompleteDelegateHandle;

	FDelegateHandle JoinCompleteDelegateHandle;

};