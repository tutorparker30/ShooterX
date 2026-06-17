// SXPlayerState.cpp


#include "Game/SXPlayerState.h"

#include "Character/SXPlayerCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

ASXPlayerState::ASXPlayerState()
{
}

void ASXPlayerState::AddCurrentKillCount(int32 InCurrentKillCount)
{
	CurrentKillCount = FMath::Clamp(CurrentKillCount + InCurrentKillCount, 0, MaxKillCount);

	OnCurrentKillCountChanged.Broadcast(CurrentKillCount);

	ASXPlayerCharacter* PlayerCharacter = Cast<ASXPlayerCharacter>(GetPawn());
	checkf(IsValid(PlayerCharacter) == true, TEXT("Invalid PlayerCharacter"));
	PlayerCharacter->GetNiagaraComponent()->Activate(true);
}
