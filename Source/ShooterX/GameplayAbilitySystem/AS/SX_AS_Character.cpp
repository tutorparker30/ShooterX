// SX_AS_Character.cpp

#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

#include "Net/UnrealNetwork.h"

USX_AS_Character::USX_AS_Character()
	: AttackRange(100.f)
	, AttackRadius(50.f)
	, AttackDamage(10.f)
	, Health(100.f)
	, MaxHealth(100.f)
{
	InitHealth(GetMaxHealth());
}

void USX_AS_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
}

void USX_AS_Character::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void USX_AS_Character::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("Health : %.1f -> %.1f"), OldValue, NewValue);
	}
}

void USX_AS_Character::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void USX_AS_Character::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}
