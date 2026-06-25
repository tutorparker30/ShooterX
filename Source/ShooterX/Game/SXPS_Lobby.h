// SXPS_Lobby.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SXPS_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXPS_Lobby : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASXPS_Lobby();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated)
	FString PlayerLobbyNameString;

};
