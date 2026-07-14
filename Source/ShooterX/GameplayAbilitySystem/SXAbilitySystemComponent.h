// SXAbilitySystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SXAbilitySystemComponent.generated.h"

UCLASS()
class SHOOTERX_API USXAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& AbilitySpec) override;

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& AbilitySpec) override;
	
};
