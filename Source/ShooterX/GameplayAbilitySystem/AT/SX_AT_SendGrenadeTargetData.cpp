// SX_AT_SendGrenadeTargetData.cpp

#include "GameplayAbilitySystem/AT/SX_AT_SendGrenadeTargetData.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include "GameplayAbilitySystem/TargetData/SXGASGrenadeTargetData.h"

USX_AT_SendGrenadeTargetData* USX_AT_SendGrenadeTargetData::SendGrenadeTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FSXGASGrenadeThrowData& LocalThrowData)
{
	USX_AT_SendGrenadeTargetData* Task = NewAbilityTask<USX_AT_SendGrenadeTargetData>(OwningAbility, TaskInstanceName);
	if (IsValid(Task))
	{
		Task->LocalThrowData = LocalThrowData;
	}

	return Task;
}

FGameplayAbilityTargetDataHandle USX_AT_SendGrenadeTargetData::MakeTargetDataHandle() const
{
	auto* NewTargetData = new FSXGameplayAbilityTargetData_GrenadeThrow();
	NewTargetData->StartLocation = LocalThrowData.StartLocation;
	NewTargetData->LaunchVelocity =	LocalThrowData.LaunchVelocity;

	return FGameplayAbilityTargetDataHandle(NewTargetData);
}

void USX_AT_SendGrenadeTargetData::Activate()
{
	Super::Activate();

	if (IsValid(Ability) == false)
	{
		EndTask();
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (IsValid(ASC) == false || ActorInfo == nullptr)
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

		EndTask();
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
	const bool bLocallyControlled = ActorInfo->IsLocallyControlled();
	const bool bAuthority =	ActorInfo->IsNetAuthority();
	if (bLocallyControlled)
	{
		// 로컬 플레이어 또는 리슨 서버 호스트.

		if (LocalThrowData.IsValid() == false)
		{
			Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

			EndTask();
			return;
		}

		FGameplayAbilityTargetDataHandle TargetData = MakeTargetDataHandle();

		// 리모트 클라이언트일 때만 RPC로 서버에 전달.
		if (bAuthority == false)
		{
			FScopedPredictionWindow ScopedPrediction(ASC, true);

			ASC->ServerSetReplicatedTargetData(
				SpecHandle,
				ActivationPredictionKey,
				TargetData,
				FGameplayTag(),
				ASC->ScopedPredictionKey);
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(TargetData);
		}

		EndTask();
		return;
	}

	if (bAuthority)
	{
		// 리모트 클라이언트를 담당하는 서버 실행.

		TargetDataDelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
				this,
				&ThisClass::
				OnTargetDataReplicated);

		const bool bTargetDataAlreadyReceived =	ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (bTargetDataAlreadyReceived == false)
		{
			SetWaitingOnRemotePlayerData();
		}

		return;
	}

	EndTask();
}

void USX_AT_SendGrenadeTargetData::OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ActivationTag)
{
	if (IsValid(Ability) == false)
	{
		EndTask();
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (IsValid(ASC) == false)
	{
		EndTask();
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

	const FGameplayAbilityTargetDataHandle CopiedTargetData = TargetData;
		// Consume 전에 복사해 Task 콜백에서 사용한다.

	ASC->ConsumeClientReplicatedTargetData(SpecHandle, ActivationPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(CopiedTargetData);
	}

	EndTask();
}

void USX_AT_SendGrenadeTargetData::OnDestroy(bool bAbilityEnded)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	if (IsValid(ASC) &&	IsValid(Ability) &&	TargetDataDelegateHandle.IsValid())
	{
		ASC->AbilityTargetDataSetDelegate(
			Ability->GetCurrentAbilitySpecHandle(),
			Ability
			->GetCurrentActivationInfo()
			.GetActivationPredictionKey())
		.Remove(TargetDataDelegateHandle);
	}

	Super::OnDestroy(bAbilityEnded);
}
