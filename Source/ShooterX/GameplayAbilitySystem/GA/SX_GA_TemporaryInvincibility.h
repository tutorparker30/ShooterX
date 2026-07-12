// SX_GA_TemporaryInvincibility.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SX_GA_TemporaryInvincibility.generated.h"

class UGameplayEffect;

UCLASS()
class SHOOTERX_API USX_GA_TemporaryInvincibility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USX_GA_TemporaryInvincibility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> InvincibleEffect;

};
