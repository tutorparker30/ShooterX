#include "Component/SXStatusComponent.h"

USXStatusComponent::USXStatusComponent()
	: MaxHP(100.f)
	, CurrentHP(100.f)
	, bIsDead(false)
{
	PrimaryComponentTick.bCanEverTick = false;

}

float USXStatusComponent::ApplyDamage(float InDamage)
{
	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void USXStatusComponent::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
}

void USXStatusComponent::SetCurrentHP(float InCurrentHP)
{
	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();

		bIsDead = true;
	}

	OnCurrentHPChanged.Broadcast(CurrentHP);
}