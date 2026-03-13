#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SXHPTextWidgetComponent.generated.h"

UCLASS()
class SHOOTERX_API USXHPTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	USXHPTextWidgetComponent();
	virtual void InitWidget() override;

};
