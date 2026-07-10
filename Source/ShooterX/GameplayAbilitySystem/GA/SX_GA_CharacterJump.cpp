// SX_GA_CharacterJump.cpp


#include "SX_GA_CharacterJump.h"

#include "GameFramework/Character.h"

USX_GA_CharacterJump::USX_GA_CharacterJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void USX_GA_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character) == true)
	{
		Character->Jump();
		// ACharacter::Jump()는 CharacterMovementComponent를 통해 로컬/서버 양쪽에서
		// 동일하게 이동을 처리하므로, 여기서는 추가 복제 로직 없이 그대로 호출.
	}
}

void USX_GA_CharacterJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character) == true)
	{
		Character->StopJumping();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
