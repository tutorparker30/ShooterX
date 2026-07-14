// SXAbilitySystemComponent.cpp

#include "GameplayAbilitySystem/SXAbilitySystemComponent.h"

#include "Abilities/GameplayAbility.h"

void USXAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& AbilitySpec)
{
	Super::AbilitySpecInputPressed(AbilitySpec);

	if (AbilitySpec.IsActive() == false)
	{
		return;
	}

	UGameplayAbility* AbilityInstance =	AbilitySpec.GetPrimaryInstance();
	if (IsValid(AbilityInstance) == false)
	{
		return;
	}

	const FPredictionKey ActivationPredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, ActivationPredictionKey);
}

void USXAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& AbilitySpec)
{
	Super::AbilitySpecInputReleased(AbilitySpec);

	if (AbilitySpec.IsActive() == false)
	{
		return;
	}

	UGameplayAbility* AbilityInstance =	AbilitySpec.GetPrimaryInstance();
	if (IsValid(AbilityInstance) == false)
	{
		return;
	}

	const FPredictionKey ActivationPredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, ActivationPredictionKey);
}