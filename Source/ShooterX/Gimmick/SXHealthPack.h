// SXHealthPack.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXHealthPack.generated.h"



UCLASS()
class SHOOTERX_API ASXHealthPack : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	float HealAmount;

};
