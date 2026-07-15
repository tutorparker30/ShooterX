// SX_GA_ThrowGrenade.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Item/SXGASGrenadeTypes.h"
#include "SX_GA_ThrowGrenade.generated.h"

class UAnimMontage;
class ASXGASGrenade;
class ASX_TA_GrenadeTrajectory;

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

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	bool ValidateMontage() const;

	void FinishAbility(bool bWasCancelled);

	UFUNCTION()
	void OnInputReleased(float InTimeHeld);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	bool TryCommitThrow();

	bool ValidateThrowConfiguration();

	bool StartTrajectoryPreview();

	void StopTrajectoryPreview();

	bool ValidateAndSanitizeThrowTargetData(
		const FGameplayAbilityTargetDataHandle&
		TargetDataHandle,
		FSXGASGrenadeThrowData&
		OutSanitizedThrowData) const;

	ASXGASGrenade* BeginDeferredGrenadeSpawn(const FSXGASGrenadeThrowData& ThrowData) const;

	UFUNCTION()
	void OnThrowTargetDataReady(FGameplayAbilityTargetDataHandle TargetDataHandle);

	UFUNCTION()
	void OnThrowTargetDataCancelled(FGameplayAbilityTargetDataHandle TargetDataHandle);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GrenadeThrowMontage;

	UPROPERTY(EditDefaultsOnly)
	FName AimStartSectionName;

	UPROPERTY(EditDefaultsOnly)
	FName AimLoopSectionName;

	UPROPERTY(EditDefaultsOnly)
	FName ThrowSectionName;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASXGASGrenade> GrenadeClass;

	UPROPERTY(EditDefaultsOnly)
	FName GrenadeThrowSocketName;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASX_TA_GrenadeTrajectory> TrajectoryTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0.0"))
	float MaxStartLocationError;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0.0"))
	float LaunchSpeedTolerance;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float MinimumAimDirectionDot;

private:
	bool bThrowConfirmed;

	bool bFinishRequested;

	UPROPERTY(Transient)
	TObjectPtr<ASX_TA_GrenadeTrajectory> TrajectoryTargetActor;

	bool bThrowTargetDataRequested;

};
