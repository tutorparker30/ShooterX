#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SXPlayerCharacterMaterialManager.generated.h"

UCLASS(Config = PlayerCharacterMeshMaterialPaths)
class SHOOTERXPLAYGROUND_API USXPlayerCharacterMaterialManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	TArray<FSoftObjectPath> PlayerCharacterMeshMaterialPaths;

};
