// SXOnlineSessionSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "SXOnlineSessionSubsystem.generated.h"

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

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionManager;

	FDelegateHandle CreateCompleteDelegateHandle;

	FDelegateHandle DestroyCompleteDelegateHandle;

};