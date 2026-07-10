// SXGASPlayerState.cpp


#include "Game/SXGASPlayerState.h"

#include "AbilitySystemComponent.h"

ASXGASPlayerState::ASXGASPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* ASXGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
