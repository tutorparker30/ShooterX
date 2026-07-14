// SX_GA_ThrowGrenade.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SX_GA_ThrowGrenade.generated.h"

class UAnimMontage;

/**
 * 입력을 누르는 동안 수류탄 조준 상태를 유지하고,
 * 입력을 해제하면 투척을 확정하는 어빌리티.
 */
UCLASS()
class SHOOTERX_API USX_GA_ThrowGrenade
	: public UGameplayAbility
{
	GENERATED_BODY()

public:
	USX_GA_ThrowGrenade();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	bool ValidateMontage() const;

	void FinishAbility(bool bWasCancelled);

private:
	UFUNCTION()
	void OnInputReleased(float InTimeHeld);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GrenadeThrowMontage;

	UPROPERTY(EditDefaultsOnly)
	FName AimStartSectionName;

	UPROPERTY(EditDefaultsOnly)
	FName AimLoopSectionName;

	UPROPERTY(EditDefaultsOnly)
	FName ThrowSectionName;

private:
	bool bThrowConfirmed;
		// 이번 어빌리티 활성에서 우클릭 해제가 정상적으로 수신되었는지

	bool bFinishRequested;
		// 여러 몽타주 콜백이 연속으로 발생하더라도 EndAbility() 함수가 중복 호출되지 않도록 방지함.

};