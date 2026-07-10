// SXGASPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SXGASPlayerState.generated.h"

UCLASS()
class SHOOTERX_API ASXGASPlayerState 
	: public APlayerState
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASXGASPlayerState();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "ASXGASPlayerState|Component")
	TObjectPtr<UAbilitySystemComponent> ASC;

};
