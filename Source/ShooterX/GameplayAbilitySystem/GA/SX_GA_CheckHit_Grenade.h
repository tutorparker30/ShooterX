// SX_GA_CheckHit_Grenade.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SX_GA_CheckHit_Grenade.generated.h"

class ASX_TA_OverlapMultiSphere;
class UGameplayEffect;

UCLASS()
class SHOOTERX_API USX_GA_CheckHit_Grenade : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USX_GA_CheckHit_Grenade();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnOverlapTargetDataReady(FGameplayAbilityTargetDataHandle TargetDataHandle);

	UFUNCTION()
	void OnOverlapTargetDataCancelled(FGameplayAbilityTargetDataHandle TargetDataHandle);

	bool ExtractExplosionData(const FGameplayEventData*	TriggerEventData);

	void FinishCheckHit(bool bWasCancelled);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASX_TA_OverlapMultiSphere> OverlapTargetActorClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GrenadeDamageEffectClass;

	UPROPERTY(EditDefaultsOnly)
	bool bIgnoreSourceActor;

private:
	FVector ExplosionLocation;

	float ExplosionRadius;

	float ExplosionDamage;

	bool bFinishRequested;
	
};
