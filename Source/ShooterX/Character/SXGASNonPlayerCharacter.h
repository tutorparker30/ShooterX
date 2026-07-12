// SXGASNonPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SXGASNonPlayerCharacter.generated.h"

class UAbilitySystemComponent;
class USX_AS_Character;
class UGameplayEffect;
class USXGASWidgetComponent;
class UGameplayAbility;

UCLASS()
class SHOOTERX_API ASXGASNonPlayerCharacter 
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASXGASNonPlayerCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	virtual void OnOutOfHealth();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USX_AS_Character> AttributeSet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InitStatEffect;

	UPROPERTY(EditAnywhere)
	float Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Component")
	TObjectPtr<USXGASWidgetComponent> HPBar;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> InvincibilityAbilityClass;

};
