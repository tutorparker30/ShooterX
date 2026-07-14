// SX_AS_Grenade.cpp

#include "GameplayAbilitySystem/AS/SX_AS_Grenade.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

USX_AS_Grenade::USX_AS_Grenade()
	: ExplosionRadius(500.0f)
	, ExplosionDamage(100.0f)
	, CurrentGrenadeCount(2.0f)
	, MaxGrenadeCount(3.0f)
{
}

void USX_AS_Grenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExplosionRadius, COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExplosionDamage, COND_OwnerOnly, REPNOTIFY_Always);
		// 폭발 이펙트에 필요한 위치와 범위 정보는 나중에 GameplayCue Parameters를 통해 각 클라이언트에 전달한다. 
		// 따라서 다른 플레이어의 ExplosionRadius와 ExplosionDamage Attribute를 계속 복제할 필요가 없다.

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentGrenadeCount, COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxGrenadeCount, COND_OwnerOnly, REPNOTIFY_Always);
		// GrenadeCount와 MaxGrenadeCount는 인벤토리 성격의 정보이므로 COND_OwnerOnly로 복제한다. 
		// 서버와 해당 캐릭터의 소유 클라이언트만 정확한 수량을 알면 되기 때문.
}

void USX_AS_Grenade::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetExplosionRadiusAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetExplosionDamageAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetCurrentGrenadeCountAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxGrenadeCount());
	}
	else if (Attribute == GetMaxGrenadeCountAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void USX_AS_Grenade::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentGrenadeCountAttribute())
	{
		SetCurrentGrenadeCount(FMath::Clamp(GetCurrentGrenadeCount(), 0.0f, GetMaxGrenadeCount()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxGrenadeCountAttribute())
	{
		SetMaxGrenadeCount(FMath::Max(GetMaxGrenadeCount(), 0.0f));

		SetCurrentGrenadeCount(FMath::Clamp(GetCurrentGrenadeCount(), 0.0f, GetMaxGrenadeCount()));
	}
	else if (Data.EvaluatedData.Attribute == GetExplosionRadiusAttribute())
	{
		SetExplosionRadius(FMath::Max(GetExplosionRadius(), 0.0f));
	}
	else if (Data.EvaluatedData.Attribute == GetExplosionDamageAttribute())
	{
		SetExplosionDamage(FMath::Max(GetExplosionDamage(), 0.0f));
	}
}

void USX_AS_Grenade::OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USX_AS_Grenade, ExplosionRadius, OldValue);
}

void USX_AS_Grenade::OnRep_ExplosionDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USX_AS_Grenade, ExplosionDamage, OldValue);
}

void USX_AS_Grenade::OnRep_CurrentGrenadeCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USX_AS_Grenade, CurrentGrenadeCount, OldValue);
}

void USX_AS_Grenade::OnRep_MaxGrenadeCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USX_AS_Grenade, MaxGrenadeCount, OldValue);
}