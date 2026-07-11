// SX_TA_SweepSingleCapsule.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "SX_TA_SweepSingleCapsule.generated.h"

UCLASS()
class SHOOTERX_API ASX_TA_SweepSingleCapsule : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ASX_TA_SweepSingleCapsule();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

};