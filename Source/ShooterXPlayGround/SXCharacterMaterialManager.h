// SXCharacterMaterialManager.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SXCharacterMaterialManager.generated.h"

/**
 *
 */
UCLASS(config = PlayerCharacterMeshMaterialPaths)
class SHOOTERXPLAYGROUND_API USXCharacterMaterialManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerCharacterMeshMaterialPaths;

};
