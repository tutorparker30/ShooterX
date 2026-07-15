// SX_AT_OverlapMultiSphere.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SX_AT_OverlapMultiSphere.generated.h"

class ASX_TA_OverlapMultiSphere;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSXOverlapMultiSphereTaskDelegate,
	FGameplayAbilityTargetDataHandle,
	TargetData);

UCLASS()
class SHOOTERX_API USX_AT_OverlapMultiSphere
	: public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category = "Ability|Tasks",
		Meta = (
			HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "true"))
	static USX_AT_OverlapMultiSphere* OverlapMultiSphere(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			TSubclassOf<ASX_TA_OverlapMultiSphere> TargetActorClass,
			FVector Origin,
			float Radius,
			AActor* IgnoredActor);

	virtual void Activate() override;

protected:
	virtual void OnDestroy(bool bAbilityEnded) override;

private:
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FSXOverlapMultiSphereTaskDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FSXOverlapMultiSphereTaskDelegate Cancelled;
	
private:
	UPROPERTY()
	TSubclassOf<ASX_TA_OverlapMultiSphere> TargetActorClass;

	UPROPERTY(Transient)
	TObjectPtr<ASX_TA_OverlapMultiSphere> SpawnedTargetActor;

	UPROPERTY(Transient)
	TObjectPtr<AActor> IgnoredActor;

	FVector Origin;

	float Radius;
	
};