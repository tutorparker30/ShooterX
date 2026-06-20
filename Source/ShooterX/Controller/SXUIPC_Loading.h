// SXUIPC_Loading.h

#pragma once

#include "CoreMinimal.h"
#include "Controller/SXUIPlayerControllerBase.h"
#include "SXUIPC_Loading.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERX_API ASXUIPC_Loading : public ASXUIPlayerControllerBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

};
