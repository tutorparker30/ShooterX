// SXGASGrenadeFunctionLibrary.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Item/SXGASGrenadeTypes.h"
#include "SXGASGrenadeFunctionLibrary.generated.h"

class ACharacter;
class ASXGASGrenade;
struct FPredictProjectilePathResult;

/**
 * 수류탄 조준과 실제 투척이 공유하는 계산 함수.
 */
UCLASS()
class SHOOTERX_API USXGASGrenadeFunctionLibrary
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool CalculateGrenadeThrowData(
		ACharacter* SourceCharacter,
		FName ThrowSocketName,
		TSubclassOf<ASXGASGrenade> GrenadeClass,
		float AimTraceDistance,
		float StartForwardOffset,
		FSXGASGrenadeThrowData& OutThrowData);

	static bool PredictGrenadePath(
		const UObject* WorldContextObject,
		AActor* SourceActor,
		TSubclassOf<ASXGASGrenade> GrenadeClass,
		const FSXGASGrenadeThrowData& ThrowData,
		float MaxSimTime,
		float SimFrequency,
		FPredictProjectilePathResult& OutPathResult);
	
};
