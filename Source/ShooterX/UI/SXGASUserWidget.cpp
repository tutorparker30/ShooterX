// SXGASUserWidget.cpp


#include "SXGASUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"

void USXGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* USXGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
