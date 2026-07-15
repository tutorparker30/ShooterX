// SX_AT_SendGrenadeTargetData.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Item/SXGASGrenadeTypes.h"
#include "SX_AT_SendGrenadeTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSXGASGrenadeTargetDataTaskDelegate, FGameplayAbilityTargetDataHandle, TargetData);

/**
 * 로컬 수류탄 투척 데이터를 서버에 전달하고
 * 서버에서 동일한 TargetData를 수신하는 AbilityTask.
 */
UCLASS()
class SHOOTERX_API USX_AT_SendGrenadeTargetData
	: public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSXGASGrenadeTargetDataTaskDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FSXGASGrenadeTargetDataTaskDelegate Cancelled;

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility",	DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USX_AT_SendGrenadeTargetData* SendGrenadeTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FSXGASGrenadeThrowData& LocalThrowData);

	virtual void Activate() override;

protected:
	virtual void OnDestroy(bool bAbilityEnded) override;

private:
	void OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ActivationTag);

	FGameplayAbilityTargetDataHandle MakeTargetDataHandle() const;

private:
	FSXGASGrenadeThrowData LocalThrowData;

	FDelegateHandle TargetDataDelegateHandle;

};
