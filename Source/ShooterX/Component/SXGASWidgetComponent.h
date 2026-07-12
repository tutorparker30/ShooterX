// SXGASWidgetComponent.h

#pragma once

#include "Components/WidgetComponent.h"
#include "SXGASWidgetComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERX_API USXGASWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;

private:
	void TrySetAbilitySystemComponent();

};
