// SXUIPC_Loading.cpp


#include "Controller/SXUIPC_Loading.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void ASXUIPC_Loading::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (IsValid(GameMode) == true)
	{
		FString NextLevelString = UGameplayStatics::ParseOption(GameMode->OptionsString, FString(TEXT("NextLevel")));
		UGameplayStatics::OpenLevel(GameMode, *NextLevelString, false);
	}
}
