// SXGASGrenadeTypes.h

#pragma once

#include "CoreMinimal.h"
#include "SXGASGrenadeTypes.generated.h"

/**
 * 수류탄 예상 궤적과 실제 투척에서 공유할 데이터.
 */
USTRUCT(BlueprintType)
struct SHOOTERX_API FSXGASGrenadeThrowData
{
	GENERATED_BODY()

public:
	bool IsValid() const
	{
		return StartLocation.ContainsNaN() == false &&
			AimPoint.ContainsNaN() == false &&
			LaunchVelocity.ContainsNaN() == false &&
			LaunchVelocity.IsNearlyZero() == false &&
			FMath::IsFinite(GravityZ);
	}

public:
	UPROPERTY(BlueprintReadOnly)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector AimPoint = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector LaunchVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float GravityZ = 0.0f;
	
};
