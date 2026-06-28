// SXStatusComponent.cpp


#include "Component/SXStatusComponent.h"

#include "Net/UnrealNetwork.h"
#include "ShooterX.h"

USXStatusComponent::USXStatusComponent()
	: MaxHP(100.f)
	, CurrentHP(100.f)
	, bIsDead(false)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

float USXStatusComponent::ApplyDamage(float InDamage)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return 0.f;
	}

	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void USXStatusComponent::SetMaxHP(float InMaxHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}

	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
}

void USXStatusComponent::SetCurrentHP(float InCurrentHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}

	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();

		bIsDead = true;
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void USXStatusComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentHP);
	//DOREPLIFETIME(ThisClass, MaxHP);
	DOREPLIFETIME_CONDITION(ThisClass, MaxHP, COND_OwnerOnly);
}

void USXStatusComponent::TakeBuff(float InBuffValue)
{
	SetCurrentHP(GetCurrentHP() + InBuffValue);
	SetMaxHP(GetMaxHP() + InBuffValue);
}

void USXStatusComponent::OnRep_CurrentHP()
{
	SX_LOG_SUBOBJECT_NET_ROLE(LogSXNet, Log, TEXT("CurrentHP: %.1f"), CurrentHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void USXStatusComponent::OnRep_MaxHP()
{
	SX_LOG_SUBOBJECT_NET_ROLE(LogSXNet, Log, TEXT("MaxHP: %.1f"), MaxHP);
	OnMaxHPChanged.Broadcast(MaxHP);
}
