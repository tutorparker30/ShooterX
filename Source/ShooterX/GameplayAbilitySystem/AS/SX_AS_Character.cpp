// SX_AS_Character.cpp

#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

USX_AS_Character::USX_AS_Character()
	: AttackRange(100.f)
	, AttackRadius(50.f)
	, AttackDamage(10.f)
	, Health(100.f)
	, MaxHealth(100.f)
	, MetaDamage(0.f)
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

	if (Attribute == GetMetaDamageAttribute())
	{
		NewValue = (NewValue < 0.f) ? 0.f : NewValue;
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

void USX_AS_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float MinimumHealth = 0.f;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMetaDamageAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("[Server] MetaDamage : %.1f"), GetMetaDamage());

		// 이 위치에서 FinalDamage = FMath::Max(GetMetaDamage() - GetDefense(), 0.f); 과 같은 코드로
		// 메타 데미지에서 방어력을 제하고 적용한다던지 할 수 있음.

		SetHealth(FMath::Clamp(GetHealth() - GetMetaDamage(), MinimumHealth, GetMaxHealth()));
		SetMetaDamage(0.f);
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
