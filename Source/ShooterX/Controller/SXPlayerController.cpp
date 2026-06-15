// SXPlayerController.cpp


#include "Controller/SXPlayerController.h"

ASXPlayerController::ASXPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	// 컨트롤러 액터의 Tick이 돌아야 입력도 처리할 수 있음.
}

void ASXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}
