// SX_GA_Rotate.cpp


#include "GameplayAbilitySystem/GA/SX_GA_Rotate.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "SXGameplayTags.h"

USX_GA_Rotate::USX_GA_Rotate()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

	FGameplayTagContainer InAssetTags;
	InAssetTags.AddTag(SXGameplayTags::Ability_ID_Action_Movement_Rotate);
	InAssetTags.AddTag(SXGameplayTags::Ability_Type_Action_Movement_Rotate);
	SetAssetTags(InAssetTags);
		// UE5부터 AbilityTags 대신에 AssetTags를 사용하게끔 권장함.

	ActivationOwnedTags.AddTag(SXGameplayTags::State_Action_Movement_Rotating);
}

void USX_GA_Rotate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		URotatingMovementComponent* RotatingMovementComponent = Cast<URotatingMovementComponent>(AvatarActor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
		if (IsValid(RotatingMovementComponent) == true)
		{
			RotatingMovementComponent->RotationRate = FRotator(0.f, 180.f, 0.f);
		}
	}
}

void USX_GA_Rotate::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		URotatingMovementComponent* RotatingMovementComponent = Cast<URotatingMovementComponent>(AvatarActor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
		if (IsValid(RotatingMovementComponent) == true)
		{
			RotatingMovementComponent->RotationRate = FRotator(0.f, 0.f, 0.f);
		}
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
