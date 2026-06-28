// SXPlayerController.cpp


#include "Controller/SXPlayerController.h"
#include "UI/SXHUD.h"
#include "Game/SXPlayerState.h"
#include "Character/SXPlayerCharacter.h"
#include "Component/SXStatusComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Game/SXGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UW_GameResult.h"
#include "Components/TextBlock.h"

ASXPlayerController::ASXPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	// 컨트롤러 액터의 Tick이 돌아야 입력도 처리할 수 있음.
}

void ASXPlayerController::ToggleInGameMenu()
{
	checkf(IsValid(InGameMenuInstance) == true, TEXT("Invalid InGameMenuInstance"));

	if (false == bIsInGameMenuOn)
	{
		InGameMenuInstance->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(InGameMenuInstance->GetCachedWidget());
		SetInputMode(Mode);

		// SetPause(true); 만약 게임 일시 정지를 원한다면.
		//					그리고 InputAction 애셋의 TriggerWhenPaused 속성을 true로 지정해야 Pause 상태에서도 해당 입력 액션이 동작함.
		bShowMouseCursor = true;
	}
	else
	{
		InGameMenuInstance->SetVisibility(ESlateVisibility::Collapsed);

		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);

		// SetPause(false); 만약 게임 일시 정지를 원한다면 이 코드가 필요함.
		//					그리고 InputAction 애셋의 TriggerWhenPaused 속성을 true로 지정해야 Pause 상태에서도 해당 입력 액션이 동작함.
		bShowMouseCursor = false;
	}

	bIsInGameMenuOn = !bIsInGameMenuOn;
}

void ASXPlayerController::OnCharacterDead()
{
	ASXGameModeBase* GameMode = Cast<ASXGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (HasAuthority() == true && IsValid(GameMode) == true)
	{
		GameMode->OnCharacterDead(this);
	}
}

void ASXPlayerController::ClientRPCShowGameResultWidget_Implementation(int32 InRanking)
{
	if (IsLocalController() == true)
	{
		if (IsValid(GameResultUIClass) == true)
		{
			UUW_GameResult* GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
			if (IsValid(GameResultUI) == true)
			{
				GameResultUI->AddToViewport(3);

				FString GameResultString = FString::Printf(TEXT("%s"), InRanking == 1 ? TEXT("Winner Winner!") : TEXT("Looser..."));
				GameResultUI->ResultText->SetText(FText::FromString(GameResultString));

				FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
				GameResultUI->RankingText->SetText(FText::FromString(RankingString));

				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(GameResultUI->GetCachedWidget());
				SetInputMode(Mode);

				bShowMouseCursor = true;
			}
		}
	}
}

void ASXPlayerController::ClientRPCReturnToTitle_Implementation()
{
	if (IsLocalController() == true)
	{ // 서버의 레벨이 변경되는걸 원치 않음. 클라이언트가 이동해야하므로 if() 처리.
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Title")), true);
	}
}

void ASXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

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

	if (IsValid(CrosshairWidgetClass) == true)
	{
		UUserWidget* CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
		if (IsValid(CrosshairWidgetInstance) == true)
		{
			CrosshairWidgetInstance->AddToViewport(1);

			CrosshairWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (IsValid(InGameMenuClass) == true)
	{
		InGameMenuInstance = CreateWidget<UUserWidget>(this, InGameMenuClass);
		if (IsValid(InGameMenuInstance) == true)
		{
			InGameMenuInstance->AddToViewport(3); // 상위에 띄움.

			InGameMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (IsValid(NotificationTextUIClass) == true)
	{
		UUserWidget* NotificationTextUI = CreateWidget<UUserWidget>(this, NotificationTextUIClass);
		if (IsValid(NotificationTextUI) == true)
		{
			NotificationTextUI->AddToViewport(1);

			NotificationTextUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ASXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}
