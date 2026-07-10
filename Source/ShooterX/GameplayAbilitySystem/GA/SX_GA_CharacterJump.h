// SX_GA_CharacterJump.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SX_GA_CharacterJump.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERX_API USX_GA_CharacterJump : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USX_GA_CharacterJump();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

};
