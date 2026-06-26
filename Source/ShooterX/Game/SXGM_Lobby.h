// SXGM_Lobby.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SXGM_Lobby.generated.h"

class ASXUIPC_Lobby;

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXGM_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASXGM_Lobby();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void OnPostLogin(AController* NewPlayer) override;	
	
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Logout(AController* Exiting) override;

public:
	TArray<TWeakObjectPtr<ASXUIPC_Lobby>> AllPlayerControllers;

	UPROPERTY(EditAnywhere)
	int32 RequiredPlayerCount = 2;

	UPROPERTY(EditAnywhere)
	float InitialRemainTimeForPlaying = 5.f;

	UPROPERTY(VisibleAnywhere)
	float RemainTimeForPlaying = 5.f;

private:
	bool bIsTravelling = false;

};
