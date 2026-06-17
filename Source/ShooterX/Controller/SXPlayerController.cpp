// SXPlayerController.cpp


#include "Controller/SXPlayerController.h"
#include "UI/SXHUD.h"
#include "Game/SXPlayerState.h"
#include "Character/SXPlayerCharacter.h"
#include "Component/SXStatusComponent.h"

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

	if (IsValid(HUDWidgetClass) == true)
	{
		HUDWidget = CreateWidget<USXHUD>(this, HUDWidgetClass);
		if (IsValid(HUDWidget) == true)
		{
			HUDWidget->AddToViewport();

			ASXPlayerState* SXPlayerState = GetPlayerState<ASXPlayerState>();
			if (IsValid(SXPlayerState) == true)
			{
				HUDWidget->BindPlayerState(SXPlayerState);
			}

			ASXPlayerCharacter* PC = GetPawn<ASXPlayerCharacter>();
			if (IsValid(PC) == true)
			{
				USXStatusComponent* StatusComponent = PC->GetStatusComponent();
				if (IsValid(StatusComponent) == true)
				{
					HUDWidget->BindStatusComponent(StatusComponent);
				}
			}
		}
	}
}
