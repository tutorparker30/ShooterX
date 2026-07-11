// SX_GE_Damage.cpp

#include "GameplayAbilitySystem/GE/SX_GE_Damage.h"

#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

USX_GE_Damage::USX_GE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = USX_AS_Character::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-10.f));

	Modifiers.Add(HealthModifier);
}
