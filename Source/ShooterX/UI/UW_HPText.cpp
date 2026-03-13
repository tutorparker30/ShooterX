#include "UI/UW_HPText.h"
#include "Component/SXStatusComponent.h"
#include "Components/TextBlock.h"
#include "Character/SXNonPlayerCharacter.h"
#include "UW_HPBar.h"

UUW_HPText::UUW_HPText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_HPText::NativeConstruct()
{
	Super::NativeConstruct();

	ASXNonPlayerCharacter* OwningCharacter = Cast<ASXNonPlayerCharacter>(OwningActor);
	if (IsValid(OwningCharacter) == true)
	{
		OwningCharacter->SetHPTextWidget(this);
	}

}

void UUW_HPText::InitializeHPTextWidget(USXStatusComponent* InStatusComponent)
{
	OnMaxHPChange(InStatusComponent->GetMaxHP());
	OnCurrentHPChange(InStatusComponent->GetCurrentHP());
}

void UUW_HPText::OnMaxHPChange(float InMaxHP)
{
	MaxHPText->SetText(FText::FromString(FString::SanitizeFloat(InMaxHP)));
}

void UUW_HPText::OnCurrentHPChange(float InCurrentHP)
{
	CurrentHPText->SetText(FText::FromString(FString::SanitizeFloat(InCurrentHP)));
}
