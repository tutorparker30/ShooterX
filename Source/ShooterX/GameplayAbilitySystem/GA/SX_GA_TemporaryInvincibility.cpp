// SX_GA_TemporaryInvincibility.cpp

#include "GameplayAbilitySystem/GA/SX_GA_TemporaryInvincibility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SXGameplayTags.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

USX_GA_TemporaryInvincibility::USX_GA_TemporaryInvincibility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void USX_GA_TemporaryInvincibility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&CurrentActivationInfo) == false || IsValid(InvincibleEffect) == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		return;
	}

	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, InvincibleEffect, GetAbilityLevel(Handle, ActorInfo));

	if (EffectSpecHandle.IsValid() == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		return;
	}

	const FActiveGameplayEffectHandle ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
	const bool bEffectApplied = ActiveEffectHandle.IsValid();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, bEffectApplied == false);
}
