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

public:
	TArray<TWeakObjectPtr<ASXUIPC_Lobby>> AllPlayerControllers;

};
