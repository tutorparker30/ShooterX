// SXGS_Lobby.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SXGS_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXGS_Lobby : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
	
};
