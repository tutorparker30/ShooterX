// SX_GA_TestInput.cpp


#include "GameplayAbilitySystem/GA/SX_GA_TestInput.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "SXGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

USX_GA_TestInput::USX_GA_TestInput()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void USX_GA_TestInput::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_TestInput::InputPressed()")));
}

void USX_GA_TestInput::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_TestInput::InputReleased()")));
}

void USX_GA_TestInput::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_TestInput::ActivateAbility()")));

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void USX_GA_TestInput::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("USX_GA_TestInput::CancelAbility()")));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
