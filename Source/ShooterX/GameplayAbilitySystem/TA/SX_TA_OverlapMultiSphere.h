// SX_TA_OverlapMultiSphere.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "SX_TA_OverlapMultiSphere.generated.h"

UCLASS()
class SHOOTERX_API ASX_TA_OverlapMultiSphere : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ASX_TA_OverlapMultiSphere();

	void ConfigureOverlap(const FVector& InOrigin, float InRadius, AActor* InIgnoredActor);

	virtual void ConfirmTargetingAndContinue() override;

private:
	FGameplayAbilityTargetDataHandle MakeTargetData() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bShowDebug;

private:
	FVector Origin;

	float Radius;

	UPROPERTY(Transient)
	TObjectPtr<AActor> IgnoredActor;
	
};
