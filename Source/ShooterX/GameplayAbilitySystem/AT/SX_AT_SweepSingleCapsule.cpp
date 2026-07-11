// SX_AT_SweepSingleCapsule.cpp

#include "GameplayAbilitySystem/AT/SX_AT_SweepSingleCapsule.h"

#include "GameplayAbilitySystem/TA/SX_TA_SweepSingleCapsule.h"
#include "AbilitySystemComponent.h"

USX_AT_SweepSingleCapsule::USX_AT_SweepSingleCapsule()
{
}

USX_AT_SweepSingleCapsule* USX_AT_SweepSingleCapsule::CreateTask(UGameplayAbility* OwningAbility,
	TSubclassOf<ASX_TA_SweepSingleCapsule> TargetActorClass)
{
	USX_AT_SweepSingleCapsule* NewTask = NewAbilityTask<USX_AT_SweepSingleCapsule>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void USX_AT_SweepSingleCapsule::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void USX_AT_SweepSingleCapsule::SpawnAndInitializeTargetActor()
{
	TargetActorInstance = Cast<ASX_TA_SweepSingleCapsule>(
		Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (IsValid(TargetActorInstance) == true)
	{
		TargetActorInstance->TargetDataReadyDelegate.AddUObject(this, &ThisClass::OnTargetDataReady);
	}
}

void USX_AT_SweepSingleCapsule::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC == nullptr)
	{
		return;
	}

	const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
	TargetActorInstance->FinishSpawning(SpawnTransform);

	ASC->SpawnedTargetActors.Push(TargetActorInstance);
	TargetActorInstance->StartTargeting(Ability);
	TargetActorInstance->ConfirmTargeting();
}

void USX_AT_SweepSingleCapsule::OnDestroy(bool AbilityEnded)
{
	if (IsValid(TargetActorInstance) == true)
	{
		TargetActorInstance->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void USX_AT_SweepSingleCapsule::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates() == true)
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}