// SXGameplayAbilitySet.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "InputActionValue.h"
#include "SXGameplayAbilitySet.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FSXGameplayAbilityGrant
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InputAction = nullptr;

};
