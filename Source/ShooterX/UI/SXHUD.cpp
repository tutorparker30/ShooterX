#include "UI/SXHUD.h"
#include "Component/SXStatusComponent.h"
#include "Game/SXPlayerState.h"
#include "UI/UW_HPBar.h"
#include "Components/TextBlock.h"
#include "Game/SXGameInstance.h"

void USXHUD::BindStatusComponent(USXStatusComponent* InStatusComponent)
{
	if (IsValid(InStatusComponent))
	{
		USXGameInstance* GameInstance = Cast<USXGameInstance>(GetWorld()->GetGameInstance());
		if (IsValid(GameInstance))
		{
			if (GameInstance->GetMaxHPDataTable() != nullptr || GameInstance->GetMaxHPDataTableRow(1) != nullptr)
			{
				float MaxHP = GameInstance->GetMaxHPDataTableRow(1)->MaxHP;
				HPBar->OnCurrentHPChange(MaxHP);
				HPBar->OnMaxHPChange(MaxHP);
			}
		}

		StatusComponent = InStatusComponent;
		HPBar->InitializerHPBarWidget(StatusComponent);
		StatusComponent->OnCurrentHPChanged.AddUObject(HPBar, &UUW_HPBar::OnCurrentHPChange);
		StatusComponent->OnMaxHPChanged.AddUObject(HPBar, &UUW_HPBar::OnMaxHPChange);
	}
}

void USXHUD::BindPlayerState(ASXPlayerState* InPlayerState)
{
	if (IsValid(InPlayerState))
	{
		PlayerState = InPlayerState;
		PlayerState->OnCurrentKillCountChanged.AddDynamic(this, &ThisClass::OnKillCountChanged);

		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
		OnKillCountChanged(PlayerState->GetCurrentKillCount());
	}
}

void USXHUD::OnKillCountChanged(int32 InKillCount)
{
	FString KillCountString = FString::Printf(TEXT("%d"), InKillCount);
	KillCountText->SetText(FText::FromString(KillCountString));
}
