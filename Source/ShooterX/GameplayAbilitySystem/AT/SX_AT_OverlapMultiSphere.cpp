// SX_AT_OverlapMultiSphere.cpp

#include "GameplayAbilitySystem/AT/SX_AT_OverlapMultiSphere.h"

#include "Abilities/GameplayAbility.h"
#include "Engine/World.h"
#include "GameplayAbilitySystem/TA/SX_TA_OverlapMultiSphere.h"

USX_AT_OverlapMultiSphere*
USX_AT_OverlapMultiSphere::OverlapMultiSphere(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TSubclassOf<ASX_TA_OverlapMultiSphere> TargetActorClass,
	FVector Origin,
	float Radius,
	AActor* IgnoredActor)
{
	USX_AT_OverlapMultiSphere* Task = NewAbilityTask<USX_AT_OverlapMultiSphere>(OwningAbility, TaskInstanceName);
	if (IsValid(Task))
	{
		Task->TargetActorClass = TargetActorClass;
		Task->Origin = Origin;
		Task->Radius = Radius;
		Task->IgnoredActor = IgnoredActor;
	}

	return Task;
}

void USX_AT_OverlapMultiSphere::Activate()
{
	Super::Activate();

	if (IsValid(Ability) == false ||
		Ability->GetCurrentActorInfo() == nullptr ||
		Ability->GetCurrentActorInfo()
		->IsNetAuthority() == false ||
		IsValid(TargetActorClass) == false ||
		Radius <= 0.0f ||
		Origin.ContainsNaN())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

		EndTask();
		return;
	}

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

		EndTask();
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Ability->GetAvatarActorFromActorInfo();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedTargetActor = World->SpawnActor<ASX_TA_OverlapMultiSphere>(
				TargetActorClass,
				Origin,
				FRotator::ZeroRotator,
				SpawnParameters);

	if (IsValid(SpawnedTargetActor) == false)
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

		EndTask();
		return;
	}

	SpawnedTargetActor->ConfigureOverlap(Origin, Radius, IgnoredActor);
	SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &ThisClass::OnTargetDataReady);
	SpawnedTargetActor->CanceledDelegate.AddUObject(this, &ThisClass::OnTargetDataCancelled);
	SpawnedTargetActor->StartTargeting(Ability);
	SpawnedTargetActor->ConfirmTargetingAndContinue();
}

void USX_AT_OverlapMultiSphere::OnDestroy(bool bAbilityEnded)
{
	if (IsValid(SpawnedTargetActor))
	{
		SpawnedTargetActor->TargetDataReadyDelegate.RemoveAll(this);
		SpawnedTargetActor->CanceledDelegate.RemoveAll(this);
		SpawnedTargetActor->Destroy();
		SpawnedTargetActor = nullptr;
	}

	Super::OnDestroy(bAbilityEnded);
}

void USX_AT_OverlapMultiSphere::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(TargetDataHandle);
	}

	EndTask();
}

void USX_AT_OverlapMultiSphere::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(TargetDataHandle);
	}

	EndTask();
}
