// SXGASPlayerState.cpp


#include "Game/SXGASPlayerState.h"

#include "GameplayAbilitySystem/SXAbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"
#include "GameplayAbilitySystem/AS/SX_AS_Grenade.h"

ASXGASPlayerState::ASXGASPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ASC = CreateDefaultSubobject<USXAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USX_AS_Character>(TEXT("AttributeSet"));
	GrenadeAttributeSet = CreateDefaultSubobject<USX_AS_Grenade>(TEXT("GrenadeAttributeSet"));
}

UAbilitySystemComponent* ASXGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
