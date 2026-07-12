// SXGASWidgetComponent.cpp


#include "SXGASWidgetComponent.h"
#include "UI/SXGASUserWidget.h"
#include "Character/SXGASPlayerCharacter.h"
#include "Character/SXGASNonPlayerCharacter.h"
#include "Game/SXGASPlayerState.h"

void USXGASWidgetComponent::InitWidget()
{
	Super::InitWidget();

	TrySetAbilitySystemComponent();
}

void USXGASWidgetComponent::TrySetAbilitySystemComponent()
{
	USXGASUserWidget* SXGASUserWidget = Cast<USXGASUserWidget>(GetWidget());
	if (IsValid(SXGASUserWidget) == false)
	{
		return;
	}

	ASXGASPlayerCharacter* OwnerCharacter = Cast<ASXGASPlayerCharacter>(GetOwner());
	if (IsValid(OwnerCharacter) == false)
	{
		ASXGASNonPlayerCharacter* OwnerNPC = Cast<ASXGASNonPlayerCharacter>(GetOwner());
		if (IsValid(OwnerNPC) == true)
		{
			SXGASUserWidget->SetAbilitySystemComponent(OwnerNPC);
		}

		return;
	}

	ASXGASPlayerState* SXGASPlayerState = OwnerCharacter->GetPlayerState<ASXGASPlayerState>();
	if (IsValid(SXGASPlayerState) == false)
	{
		UWorld* World = GetWorld();
		if (IsValid(World) == true)
		{
			World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::TrySetAbilitySystemComponent);
		}

		return;
	}

	SXGASUserWidget->SetAbilitySystemComponent(SXGASPlayerState);
}
