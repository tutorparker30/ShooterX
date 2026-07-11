// SXGASPlayerState.cpp


#include "Game/SXGASPlayerState.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

ASXGASPlayerState::ASXGASPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USX_AS_Character>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ASXGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
