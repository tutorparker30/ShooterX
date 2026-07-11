// SXGASNonPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SXGASNonPlayerCharacter.generated.h"

class UAbilitySystemComponent;
class USX_AS_Character;

UCLASS()
class SHOOTERX_API ASXGASNonPlayerCharacter 
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASXGASNonPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USX_AS_Character> AttributeSet;

};
