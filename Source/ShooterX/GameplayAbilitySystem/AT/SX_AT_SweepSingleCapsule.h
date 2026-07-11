// SX_AT_SweepSingleCapsule.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SX_AT_SweepSingleCapsule.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSXTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

class ASX_TA_SweepSingleCapsule;

UCLASS()
class SHOOTERX_API USX_AT_SweepSingleCapsule : public UAbilityTask
{
	GENERATED_BODY()

public:
	USX_AT_SweepSingleCapsule();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (DisplayName = "WaitForSweepingSingleCapsule", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USX_AT_SweepSingleCapsule* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ASX_TA_SweepSingleCapsule> TargetActorClass);

	virtual void Activate() override;

	void SpawnAndInitializeTargetActor();

	void FinalizeTargetActor();

	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FSXTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<ASX_TA_SweepSingleCapsule> TargetActorClass;

	UPROPERTY()
	TObjectPtr<ASX_TA_SweepSingleCapsule> TargetActorInstance;

};