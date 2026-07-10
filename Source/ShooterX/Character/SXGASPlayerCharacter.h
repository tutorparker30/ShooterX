// SXGASPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySystem/SXGameplayAbilitySet.h"
#include "SXGASPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UGameplayAbility;
class UAnimMontage;

/**
 *
 */
UCLASS()
class SHOOTERX_API ASXGASPlayerCharacter 
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ASXGASPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

#pragma endregion

#pragma region Component

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;

#pragma endregion

#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASXGASPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

#pragma endregion

#pragma region GAS

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	void InitializeGASCharacterInfo();

	void HandleGameplayAbilityInputPressed(FGameplayTag InputTag);

	void HandleGameplayAbilityInputReleased(FGameplayTag InputTag);

public:
	UPROPERTY(EditDefaultsOnly, Category = "ASXGASPlayerCharacter|GameplayAbilitySystem")
	TObjectPtr<UAnimMontage> TestMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ASXGASPlayerCharacter|GameplayAbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "ASXGASPlayerCharacter|GameplayAbilitySystem")
	TArray<FSXGameplayAbilityGrant> GrantedInputAbilities;

#pragma endregion

};
