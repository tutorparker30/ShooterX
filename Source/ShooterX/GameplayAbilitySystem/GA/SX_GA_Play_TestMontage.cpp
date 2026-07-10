// SX_GA_Play_TestMontage.cpp


#include "GameplayAbilitySystem/GA/SX_GA_Play_TestMontage.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "SXGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/SXGASPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USX_GA_Play_TestMontage::USX_GA_Play_TestMontage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void USX_GA_Play_TestMontage::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_Play_TestMontage::InputPressed()")));
}

void USX_GA_Play_TestMontage::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_Play_TestMontage::InputReleased()")));
}

void USX_GA_Play_TestMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ASXGASPlayerCharacter* AvatarCharacter = CastChecked<ASXGASPlayerCharacter>(ActorInfo->AvatarActor.Get());
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("PlayAttack"), AvatarCharacter->TestMontage, 1.f);
		PlayAttackTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAttackTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAttackTask->ReadyForActivation();
	}
}

void USX_GA_Play_TestMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
		return;
	}

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_Play_TestMontage::EndAbility()")));

	ASXGASPlayerCharacter* AvatarCharacter = CastChecked<ASXGASPlayerCharacter>(ActorInfo->AvatarActor.Get());
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USX_GA_Play_TestMontage::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USX_GA_Play_TestMontage::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
