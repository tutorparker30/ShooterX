// SX_GA_CheckHit.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SX_GA_CheckHit.generated.h"

class UGameplayEffect;

UCLASS()
class SHOOTERX_API USX_GA_CheckHit : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USX_GA_CheckHit();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnSweepSingleCapsuleResultReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	float CurrentLevel = 1.0f;

};