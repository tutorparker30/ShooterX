#include "Controller/SXPlayerController.h"
#include "UI/SXHUD.h"
#include "Game/SXPlayerState.h"
#include "Character/SXPlayerCharacter.h"
#include "Component/SXStatusComponent.h"

ASXPlayerController::ASXPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);

	if (IsValid(HUDWidgetClass))
	{
		HUDWidget = CreateWidget<USXHUD>(this, HUDWidgetClass);
		if (IsValid(HUDWidget))
		{
			HUDWidget->AddToViewport();

			ASXPlayerState* SXPlayerState = GetPlayerState<ASXPlayerState>();
			if (IsValid(SXPlayerState))
			{
				HUDWidget->BindPlayerState(SXPlayerState);
			}

			ASXPlayerCharacter* PC = GetPawn<ASXPlayerCharacter>();
			if (IsValid(PC))
			{
				USXStatusComponent* StatusComponent = PC->GetStatusComponent();
				if (IsValid(StatusComponent))
				{
					HUDWidget->BindStatusComponent(StatusComponent);
				}
			}
		}
	}

	if (IsValid(CrosshairWidgetClass) == true)
	{
		UUserWidget* CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
		if (IsValid(CrosshairWidgetInstance) == true)
		{
			CrosshairWidgetInstance->AddToViewport(1);

			CrosshairWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
