// SXGASUserWidget.h

#pragma once

#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "SXGASUserWidget.generated.h"

class UAbilitySystemComponent;

/**
 *
 */
UCLASS()
class SHOOTERX_API USXGASUserWidget
	: public UUserWidget
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> ASC;

};
