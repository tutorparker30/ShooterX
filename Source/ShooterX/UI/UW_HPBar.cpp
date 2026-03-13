#include "UI/UW_HPBar.h"
#include "Component/SXStatusComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UUW_HPBar::UUW_HPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_HPBar::NativeConstruct()
{
}

void UUW_HPBar::InitializerHPBarWidget(USXStatusComponent* InStatusComponent)
{
	OnMaxHPChange(InStatusComponent->GetMaxHP());
	OnCurrentHPChange(InStatusComponent->GetCurrentHP());
}

void UUW_HPBar::UpdateProgressBar()
{
	if (IsValid(HPProgressBar))
	{
		if (KINDA_SMALL_NUMBER < LastUpdatedMaxHP)
		{
			HPProgressBar->SetPercent(LastUpdatedCurrentHP / LastUpdatedMaxHP);
		}
		else
		{
			HPProgressBar->SetPercent(0.f);
		}
	}
}

void UUW_HPBar::OnMaxHPChange(float InMaxHP)
{
	if (LastUpdatedMaxHP == InMaxHP)
	{
		return;
	}

	LastUpdatedMaxHP = InMaxHP;
	MaxHPText->SetText(FText::FromString(FString::SanitizeFloat(InMaxHP)));
	UpdateProgressBar();

}

void UUW_HPBar::OnCurrentHPChange(float InCurrentHP)
{
	if (LastUpdatedCurrentHP == InCurrentHP)
	{
		return;
	}

	LastUpdatedCurrentHP = InCurrentHP;
	CurrentHPText->SetText(FText::FromString(FString::SanitizeFloat(InCurrentHP)));
	UpdateProgressBar();

}