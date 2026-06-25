// SXGM_Lobby.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SXGM_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXGM_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;	
	
};
