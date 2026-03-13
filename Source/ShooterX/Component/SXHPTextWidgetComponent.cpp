#include "Component/SXHPTextWidgetComponent.h"
#include "UI/UW_HPText.h"

USXHPTextWidgetComponent::USXHPTextWidgetComponent()
{
}

void USXHPTextWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UUW_HPText* HPTextWidgetInstance = Cast<UUW_HPText>(GetWidget());
	if (IsValid(HPTextWidgetInstance) == true && IsValid(GetOwner()) == true)
	{
		HPTextWidgetInstance->SetOwningActor(GetOwner());
	}

}
